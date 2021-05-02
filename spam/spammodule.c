#define PY_SSIZE_T_CLEAN

#include <Python.h>

static PyObject *SpamError;

static PyObject *
spam_system(PyObject *self, PyObject *args) {
    const char *command;
    int status;

    if (!PyArg_ParseTuple(args, "s", &command)) {
        return NULL;
    }
    status = system(command);
    return PyLong_FromLong(status);
}

static PyMethodDef SpamMethod[] = {
        {"system", spam_system, METH_VARARGS, "Execute a shell command."},
        {NULL,     NULL,        0,            NULL},
};

static PyModuleDef spammodule = {
        PyModuleDef_HEAD_INIT,
        "spam",
        NULL,
        -1,
        SpamMethod
};

PyMODINIT_FUNC
PyInit_spam(void) {
    PyObject *m;
    m = PyModule_Create(&spammodule);
    if (m == NULL) {
        return NULL;
    }
    SpamError = PyErr_NewException("spam.error", NULL, NULL);
    Py_XINCREF(SpamError);
    if (PyModule_AddObject(m, "error", SpamError) < 0) {
        Py_XDECREF(SpamError);
        Py_CLEAR(SpamError);
        Py_DECREF(m);
        return NULL;
    }

    return m;
}
