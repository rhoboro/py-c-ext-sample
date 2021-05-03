//
// Created by rhoboro on 2021/05/03.
//

#include <Python.h>

long long fibonacci(unsigned int n) {
    if (n < 2) {
        return 1;
    } else {
        return fibonacci(n - 2) + fibonacci(n - 1);
    }
}

static PyObject *
fibonacci_py(PyObject *self, PyObject *args) {
    PyObject *result = NULL;
    long n;

    if (PyArg_ParseTuple(args, "i", &n)) {
        result = Py_BuildValue("L", fibonacci((unsigned int) n));
    }
    return result;
}

static char fibonacci_docs[] = "fibonacci(n): Return nth Fibonacci sequence number computed recursively\n";

static PyMethodDef fibonacci_module_methods[] = {
        /*
         * METH_VARARGS: 位置引数を受け取る
         * METH_KEYWORDS: キーワード引数を受け取る
         * METH_NOARGS: 引数を受け取らない。PyArg_ParseTuple()を呼び出す必要がない
         * METH_0: 引数を1つだけとる。*argsの代わりにその引数がくる
        */
        {"fib", (PyCFunction) fibonacci_py, METH_VARARGS, fibonacci_docs},
        {NULL,  NULL,                       0,            NULL}
};

static struct PyModuleDef fibonacci_module_definition = {
        PyModuleDef_HEAD_INIT,
        "fibonacci",
        "Extension modulethat provides fibonacci sequence function",
        -1,
        fibonacci_module_methods
};

PyMODINIT_FUNC
PyInit_fibonacci(void) {
    Py_Initialize();
    return PyModule_Create(&fibonacci_module_definition);
}