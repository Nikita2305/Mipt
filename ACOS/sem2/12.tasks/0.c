#include <Python.h>
#include <stdlib.h>

static PyObject* matrix_dot(PyObject *self, PyObject *args) {
    PyObject* a;
    PyObject* b;
    int size;
    if (!PyArg_ParseTuple(args, "iOO", &size, &a, &b)) {
        return NULL;
    }
    int alen = PyList_Size(a), blen = PyList_Size(b);
    if (alen != size || blen != size) {
        PyErr_SetString(PyExc_RuntimeError, "Wrong sizes of matrices");
        return NULL;
    }     
    PyObject* list = PyList_New(size);
    for (int i = 0; i < size; i++) {
        PyObject* p = PyList_GetItem(a, i);
        PyObject* temp = PyList_New(size);
        for (int k = 0; k < size; k++) {
            long ans = 0;
            for (int j = 0; j < size; j++) {
                PyObject* q = PyList_GetItem(b, j);
                ans += PyLong_AsLong(PyList_GetItem(p, j)) * PyLong_AsLong(PyList_GetItem(q, k));
            }
            PyList_SetItem(temp, k, PyLong_FromLong(ans));
        }
        PyList_SetItem(list, i, temp);
    }
    return list;
}

static PyMethodDef MatrixMethods[] = {
    {"dot",  matrix_dot, METH_VARARGS, "Matrix multipying"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef matrixmodule  = {
    PyModuleDef_HEAD_INIT,
    "matrix",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    MatrixMethods
};

PyMODINIT_FUNC PyInit_matrix(void) {
    return PyModule_Create(&matrixmodule);
}
