#include <Python.h>
#include <stdio.h>

void getCell(int str, int col, char* s) {
    s[0] = 'A' + col;
    s[1] = '1' + str;
    s[2] = '\0';
} 

int main() {
    Py_Initialize();
    char* input[30];
    int lines = 0;
    PyObject* list = PyList_New(0);
    PyObject* delim = PyUnicode_FromString(";");
    for (int i = 0; i < 30; i++) {
        char* input = NULL;
        ssize_t n = 0;
        int read = getline(&input, &n, stdin);
        if (read <= 1) {
            free(input);
            lines = i;
            break;
        }
        input[read - 1] = '\0';
        PyObject* py_text = PyUnicode_FromString(input);
        PyObject* py_tokens = PyUnicode_Split(py_text, delim, -1);
        PyList_Append(list, py_tokens);
        free(input);
    }
    for (int t = 0; t < 9*26 + 10; t++) { 
        for (int i = 0; i < lines; i++) {
            PyObject* local = PyList_GetItem(list, i);
            for (int j = 0; j < PyList_Size(local); j++) {
                PyObject* ascii = PyUnicode_AsASCIIString(PyList_GetItem(local, j));
                char* s = PyBytes_AsString(ascii);
                char res[100];
                if (strlen(s) == 0) {
                    strcpy(res, "=''");
                } else if (s[0] != '=') {
                    res[0] = '=';
                    strcpy(res + 1, s);
                } else {
                    strcpy(res, s);
                }
                char str[100];
                strcpy(str, "try:\n    ");
                getCell(i, j, str + strlen(str));
                strcpy(str + strlen(str), res);
                strcpy(str + strlen(str), "\nexcept Exception:\n    pass\n");
                // printf("%s\n\n", str);
                PyRun_SimpleString(str);
            }
        }
    }
    
    for (int i = 0; i < lines; i++) {
        PyObject* local = PyList_GetItem(list, i);
        for (int j = 0; j < PyList_Size(local); j++) {
            char str[100];
            strcpy(str, "print(");
            getCell(i, j, str + strlen(str));
            if (j + 1 != PyList_Size(local)) {
                strcpy(str + strlen(str), ", end=';'");
            }
            strcpy(str + strlen(str), ")");
            // printf("%s ", str);
            PyRun_SimpleString(str);     
        }
    }
    Py_Finalize();
}
