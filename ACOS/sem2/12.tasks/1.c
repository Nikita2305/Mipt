#include <Python.h>
#include <stdlib.h>

static PyObject* primes_factor(PyObject *self, PyObject *args) {
    int n;
    if (!PyArg_ParseTuple(args, "i", &n)) {
        return NULL;
    }
    if (n <= 0) {
        PyErr_SetString(PyExc_RuntimeError, "Wrong value");
        return NULL;
    }
    PyObject* list = PyList_New(0);
    int N = n;    
    int prime = 2;
    while (prime * prime <= n) {
        if (n % prime == 0) {
            n /= prime;
            PyList_Append(list, PyLong_FromLong(prime));
            continue;
        }
        prime += 1;
    }
    if (N == n) {
        return PyUnicode_FromString("Prime!");
    }
    if (n > 1) {
        PyList_Append(list, PyLong_FromLong(n));
    }
    return list;
}

static PyMethodDef PrimesMethods[] = {
    {"factor_out",  primes_factor, METH_VARARGS, "Factoring"},
    {NULL, NULL, 0, NULL}        /* Sentinel */
};

static struct PyModuleDef primesmodule  = {
    PyModuleDef_HEAD_INIT,
    "primes",   /* name of module */
    NULL, /* module documentation, may be NULL */
    -1,       /* size of per-interpreter state of the module,
                 or -1 if the module keeps state in global variables. */
    PrimesMethods
};

PyMODINIT_FUNC PyInit_primes(void) {
    return PyModule_Create(&primesmodule);
}
