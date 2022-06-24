import json
from threading import *
import time
from pathlib import Path
from vk_parsing.parser import Parser
from vk_parsing.exceptions import StopParsingError
import sys
import os
import logging
import csv
import datetime
import psycopg2
import configparser

output_path = os.path.dirname(__file__) + "/output/"
data_path = os.path.dirname(__file__) + "/data.csv"
db_config_path = os.path.dirname(__file__) + "/db.conf"

def readSQL(query, connection):
    cursor = connection.cursor()
    try:    
        cursor.execute(query)
        return cursor.fetchall()        
    except Exception as error:
        connection.rollback()
        cursor.close()
        raise

def writeSQL(query, connection):
    cursor = connection.cursor()
    try:    
        cursor.execute(query)
        connection.commit()
    except Exception as error:
        connection.rollback()
        cursor.close()
        raise

def save_as_json(obj, filename):
    with open(filename, 'w') as f:
        print(json.dumps(obj, ensure_ascii=False, indent=4), file=f)

def format_string_to_db(string):
    string = ''.join(('\'\'' if e == '\'' else e) for e in string)
    return f"'{string}'"

def append_group_members(response, connection, formatter):
    if (len(response["items"]) == 0):
        return False
    for member in response["items"]:
        if "last_seen" in member:
            member["last_seen"] = member["last_seen"]["time"]
        if "bdate" in member:
            units = member["bdate"].split(".")
            if (len(units) < 3):
                member.pop("bdate")
            else:
                member["bdate"] = f"{units[2]}-{units[1]}-{units[0]}"
        member["group_count"] = 1
        keys = ",".join([str(x) for x in member.keys()])
        values = ",".join([(formatter(x) if isinstance(x, str) else str(x)) for x in member.values()])
        writeSQL(f"INSERT INTO people ({keys}) VALUES ({values}) ON CONFLICT (id) DO UPDATE SET group_count = people.group_count + 1;", connection)
    return True

def append_friends(response, connection, user_id, valid_ids):
    friend_size = len(response["items"])
    writeSQL(f"UPDATE people SET total_friends = {friend_size} WHERE id = {user_id};", connection)
    for friend in response["items"]:
        if (friend in valid_ids):
            writeSQL(f"INSERT INTO friends VALUES ({user_id},{friend});", connection)

def run_parse(vkconfig_path, logger, output_name, connection): 
    logger.debug(f"Starting parsing for {vkconfig_path}")
    try:
        with open(vkconfig_path, "r") as f:
            config = json.load(f)
    except Exception as ex:
        logger.error(f"Problem with {vkconfig_path}, exception {ex}")
        with open(output_name, "w") as f:
            f.write("Wrong config file or doesn't exist")
        return

    logins = []
    passwords = []
    with open(data_path, newline='') as f:
        for arr in csv.reader(f, delimiter=';'):
            if (len(arr) != 2):
                raise Exception("Wrong data.csv")
            logins += [arr[0]]
            passwords += [arr[1]]
    parser = Parser(logins, passwords, logger=logger)
    SAVING_EVERY = 100

    # Downloading from groups
    logger.debug("Starting downloading groups")
    try:
        response = readSQL("SELECT * FROM groups;", connection)
        ready_groups = [x[0] for x in response]
    except Exception as ex:
        logger.error(f"Error while downloading ready groups: {ex}")
        raise
 
    for i in range(len(config["groups"])):
        g_id = config["groups"][i]
        logger.debug("downloading group " + str(g_id))
        if (g_id in ready_groups):
            continue
        j = 0
        while(True):
            logger.debug("Status " + str(j * 1000))
            OK = False
            try:
                OK = parser.direct_call("groups.getMembers",
                                {"group_id": g_id,
                                "offset": j*1000,
                                "count": 1000,
                                "fields": config["fields"]},
                                append_group_members,
                                (connection,format_string_to_db,)
                )
            except StopParsingError as ex:
                logger.critical(f"Critical error while group parsing, stop: {ex}")
                raise
            except Exception as ex:
                logger.error(f"Error while group parsing, ignored: {ex}")
            if (not OK):
                break
            j += 1
        try:
            writeSQL(f"INSERT INTO groups VALUES ({format_string_to_db(g_id)});", connection)
        except Exception as ex:
            logger.error(f"Error while saving progress on the new group: {ex}")

    # Filtering accounts
    logger.debug("Starting filtering groups")
    try:
        template_values = {
            'year': datetime.datetime.now().year,
            'time': time.time()
        }
        
        for query in config["group_filter"]:
           writeSQL(query.format(**template_values), connection)
               
    except Exception as ex:
        logger.error(f"Error while filtering: {ex}")
   
    # Downloading friends
    logger.debug("Starting downloading friends")
    try:
        ids = [x[0] for x in readSQL("SELECT id FROM people;", connection)]
        ids_set = set(ids)
        ready_ids = set([x[0] for x in readSQL("SELECT id FROM people WHERE total_friends IS NOT NULL;", connection)])
    except Exception as ex:
        logger.error(f"Error while getting people id: {ex}")
        raise
 
    for i in range(len(ids)):
        if (i % SAVING_EVERY == 0):
            logger.debug("downloading friends: " + str(i))
        user_id = ids[i]
        if (user_id in ready_ids): # for savings
            continue
        try:
            parser.add_task("friends.get",
                        {"user_id": user_id},
                        append_friends,
                        (connection, user_id, ids_set,)
            )
        except StopParsingError as ex:
            logger.critical(f"Critical error while friends parsing, stop: {ex}")
            raise
        except Exception as ex:
            logger.error(f"Error while friends parsing, ignored: {ex}")
    
    try:
        parser.execute_tasks()
    except StopParsingError as ex:
        logger.critical(f"Critical error while friends parsing, stop: {ex}")
        raise
    except Exception as ex:
        logger.error(f"Error while friends parsing, ignored: {ex}")
    
    # Filtering accounts again
    logger.debug("Parsing is done, now filtering by friends..")
    try:
        template_values = {}
        for query in config["friend_filter"]:
           writeSQL(query.format(**template_values), connection) 
        
        writeSQL("DELETE FROM friends f USING (SELECT friend_id FROM friends a LEFT JOIN people b ON a.friend_id = b.id WHERE b.id IS NULL) d WHERE f.friend_id = d.friend_id;", connection)
        if config["drop_zero_people"]:
            writeSQL("DELETE FROM people p USING (SELECT a.id FROM people a LEFT JOIN friends b ON a.id = b.id WHERE b.id IS NULL) d WHERE p.id = d.id;", connection)
    except Exception as ex:
        logger.error(f"Error while filtering people by friends: {ex}")
        raise
    
    # Merging tables
    try:
        response = readSQL("SELECT array_to_json(array_agg(row_to_json(u))) FROM people u;", connection)
        array = response[0][0]
        if (array is None):
            array = []
        for user in array:
            uid = user["id"]
            user["friends"] = [x[0] for x in readSQL(f"SELECT friend_id FROM friends WHERE id = {uid};", connection)]
        save_as_json(array, output_name)
    except Exception as ex:
        logger.error(f"Error while saving to json: {ex}")
        raise
    logger.debug(f"Formatting is done, FINISH with {vkconfig_path}, waiting for closing")

def init_database(logger, table_columns):
    try:
        config = configparser.ConfigParser()
        config.read(db_config_path)
        user = config["Connection"]["user"]
        password = config["Connection"]["password"]
        host = config["Connection"]["host"]
        port = int(config["Connection"]["port"])
        database = config["Connection"]["database"]
    except Exception as ex:
        logger.error(f"Config reading error: {ex}")
        raise

    try:
        connection = psycopg2.connect(user=user,
                                password=password,
                                host=host,
                                port=port,
                                database=database)
        array = [x[0] for x in readSQL("SELECT table_name FROM information_schema.tables;", connection)]
        if ('people' in array):
            logger.debug("People table already exists")
        if ('groups' in array):
            logger.debug("Groups table already exists")
        if ('friends' in array):
            logger.debug("Friends table already exists") 
        writeSQL(f"CREATE TABLE IF NOT EXISTS people (id bigint PRIMARY KEY, first_name text, last_name text, can_access_closed boolean, is_closed boolean, deactivated text, total_friends bigint, group_count bigint{table_columns});", connection)
        writeSQL("CREATE TABLE IF NOT EXISTS friends (id bigint, friend_id bigint);", connection)
        writeSQL("CREATE TABLE IF NOT EXISTS groups (name text);", connection)
        logger.debug("Connected to database and created tables")
    except Exception as ex:
        logger.error(f"Error while connecting to database: {ex}")
        raise
    return connection

def main(vkconfig_path, logger): 
    output_name = output_path + os.path.basename(vkconfig_path).split(".")[0] + ".output"
    out_config_name = output_path + os.path.basename(vkconfig_path)
    if os.path.isfile(out_config_name):
        logger.error(f"There is file with name {out_config_name} already, skipping request")
        new_config_name = f"{output_path}temp_.config"
        os.system(f"cp {out_config_name} {new_config_name}")
        return new_config_name

    try:
        with open(vkconfig_path, "r") as f:
            config = json.load(f)
            table_columns = config["table_columns"]
    except Exception as ex:
        logger.error(f"Problem with {vkconfig_path}, exception {ex}")
        with open(output_name, "w") as f:
            f.write("Wrong config file or doesn't exist")
        os.system(f"cp {vkconfig_path} {output_path}")
 
    connection = init_database(logger, table_columns)
    run_parse(vkconfig_path, logger, output_name, connection) # could throw something
    close_task(vkconfig_path, logger, connection)
    return output_name

def close_task(vkconfig_path, logger, connection):
    try:
        writeSQL("DROP TABLE people;", connection)
        writeSQL("DROP TABLE friends;", connection)
        writeSQL("DROP TABLE groups;", connection)
    except Exception as ex:
        logger.error(f"Error while closing task: {ex}")
        raise
    connection.close()
    os.system(f"cp {vkconfig_path} {output_path}")
    logger.debug("Task was closed successfully")

if __name__ == "__main__":
    logger = logging.getLogger("account_parsing")
    logger.setLevel(logging.DEBUG)
    logger.addHandler(logging.StreamHandler())
    if (len(sys.argv) != 2):
        logger.error("bad args to python file") 
    
    main(sys.argv[1], logger)
