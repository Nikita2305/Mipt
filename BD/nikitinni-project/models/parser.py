import copy

def parse_tuple(string):
    t = string[1:].split('|')
    return [x.strip() for x in t]    

def write_table(table):
    s = "create table if not exists {name} (\n"
    for t in table[1]:
        s += t[0] + " " + t[2] + " " + t[3] + ",\n"
    s = s[:-2]
    s += ");\n\n"
    print(s.format(name=table[0]),end='')

basic_table = ['', [], 0]
with open('physical.md', 'r') as f:
    table = copy.deepcopy(basic_table)
    for line in f:
        if (line[0] == '-'):
            write_table(table)
            table = copy.deepcopy(basic_table)
        if (line[0] == '#'):
            table[0] = line[2:].split('/')[0].strip()
        if (line[0] == '|'):
            table[2] += 1
            if (table[2] >= 3):
                table[1].append(parse_tuple(line))
    if (table[0] != ''):
        write_table(table)
