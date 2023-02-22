#include <Python.h>
#include <stdio.h>
#include <frameobject.h>
#include <string.h>
#include <wchar.h>

int DEBUG = 0;

wchar_t* program = NULL;

int silent(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg) { 
    return 0;
}

void finalize_interpret() {
    PyEval_SetTrace(silent, NULL);
    Py_Finalize();
    PyMem_RawFree(program);
}

int UNDEFINED = -1;
int STEP = 0;
int NEXT = 1;
int CONTINUE = 2;

struct State { 
    int current_status;
    int depth;
    int i;
    char** tasks;
    int tasks_size;
    int mem_allocated;
} state;

void handle_print(PyFrameObject* frame) {
    int len = strlen(state.tasks[state.i]);
    state.tasks[state.i] = realloc(state.tasks[state.i], len + 2);
    state.tasks[state.i][len] = ')';
    state.tasks[state.i][len + 1] = '\0';
    *(strstr(state.tasks[state.i], "print") + 5) = '(';
    // printf("%s\n", state.tasks[state.i]);
    PyRun_SimpleString(state.tasks[state.i]);
}

void quit(int status) {
    finalize_interpret();
    exit(status);
}

void enquire_new_state(PyFrameObject* frame) {
    for (;;state.i++) {
        if (state.i >= state.tasks_size) {
            quit(0);
        }
        char* token = state.tasks[state.i];
        if (strstr(token, "print") == token) {
            handle_print(frame);
            continue;
        } else if (strcmp(token, "step") == 0) {
            state.current_status = STEP;
        } else if (strcmp(token, "next") == 0) {
            state.current_status = NEXT;
            state.depth = 0;
        } else if (strcmp(token, "continue") == 0) {
            state.current_status = CONTINUE;
        } else {
            quit(1);
        }
        if (DEBUG) {
            printf("NEW STATE: %d\n\n", state.current_status);
        }
        state.i += 1;
        break;
    } 
}

int trace(PyObject *obj, PyFrameObject *frame, int what, PyObject *arg)
{ 
    int line = PyFrame_GetLineNumber(frame);
    
    if (DEBUG) {
    if (what == PyTrace_CALL) {
        printf("PyTrace_CALL\n");
    }
    if (what == PyTrace_LINE) {
        printf("PyTrace_LINE\n");
    }
    if (what == PyTrace_RETURN) {
        printf("PyTrace_RETURN\n");
    }
    
    printf("line %d\n", line); 
    }

    if (state.current_status == UNDEFINED) {
        if (what == PyTrace_LINE && line < 500) {
            enquire_new_state(frame);
        }
    } else if (state.current_status == CONTINUE) {
        ;
    } else if (state.current_status == STEP) {
        if (what == PyTrace_LINE) {
            enquire_new_state(frame);
        }
    } else if (state.current_status == NEXT) {
        if (what == PyTrace_CALL) {
            state.depth += 1;
        } else if (what == PyTrace_RETURN) {
            state.depth -= 1;
        } else if (what == PyTrace_LINE && state.depth == 0) {
            enquire_new_state(frame);
        }
    }
    return 0;
}

void init_interpret(char* executable_name, int argc, wchar_t* argv[]) {
    program = Py_DecodeLocale(executable_name, NULL);
    if (program == NULL) {
        fprintf(stderr, "Fatal error: cannot decode argv[0]\n");
        exit(1);
    }
    Py_SetProgramName(program);
    
    Py_Initialize();
 
    PySys_SetArgv(argc, argv); // no tracing for it anyway, but to be sure
    
    PyEval_SetTrace(trace, NULL);
}

void init_dbg_state(char* config_path) {
    FILE* fp = fopen(config_path, "r"); 
    state.current_status = UNDEFINED;
    state.i = 0; 
    state.depth = 0;
    state.tasks_size = 0;
    state.mem_allocated = 0;
    state.tasks = NULL;
    while (1) {
        char* input = NULL;
        ssize_t n = 0;
        int read = getline(&input, &n, fp);
        if (read <= 1) {
            free(input);
            break;
        }
        input[read - 1] = '\0';
        if (state.tasks_size * sizeof(char*) == state.mem_allocated) {
            state.mem_allocated *= 2;
            state.mem_allocated += sizeof(char*);
            state.tasks = realloc(state.tasks, state.mem_allocated);
        }
        state.tasks[state.tasks_size] = input;
        state.tasks_size++;
    }
    fclose(fp);
}

void execute_code(char* filename) {
    FILE* fp = fopen(filename, "r"); 
    PyRun_SimpleFile(fp, filename);
    fclose(fp);
}

int main(int argc, char *argv[])
{
    wchar_t* args[argc - 2];
    for (int i = 2; i < argc; i++) {
        int MAXLEN = 128; 
        args[i - 2] = malloc(MAXLEN);
        swprintf(args[i - 2], MAXLEN, L"%hs", argv[i]);
    }
    init_interpret(argv[0], argc - 2, args);
    
    init_dbg_state(argv[1]); 
 
    execute_code(argv[2]);
    
    finalize_interpret();
 
    return 0;
}

/*
// PyThreadState* _save = PyEval_SaveThread();
// PyEval_RestoreThread(_save);
*/
