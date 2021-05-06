#define PY_SSIZE_T_CLEAN

#include <Python.h>

typedef struct {
    // PyObject_HEAD を含んでいるので最初に書く
    PyListObject list;
    int state;
} SubListObject;

static PyObject *
SubList_increment(SubListObject *self, PyObject *unused) {
    self->state++;
    return PyLong_FromLong(self->state);
}

static PyMethodDef SubList_methods[] = {
        {"increment", (PyCFunction) SubList_increment, METH_NOARGS, PyDoc_STR("increment state counter")},
        {NULL},
};

static int
SubList_init(SubListObject *self, PyObject *args, PyObject *kwds) {
    // __new__() は自分ではなくベースクラスが呼び出すようにする
    if (PyList_Type.tp_init((PyObject *) self, args, kwds)) {
        return -1;
    }
    self->state = 0;
    return 0;
}

static PyTypeObject SubListType = {
        // 継承時は tp_alloc も継承される
        PyVarObject_HEAD_INIT(NULL, 0)
                .tp_name = "sublist.SubList",
        .tp_doc = "SubList objects",
        .tp_basicsize=sizeof(SubListObject),
        .tp_itemsize=0,
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_init = (initproc) SubList_init,
        .tp_methods = SubList_methods,
};


static PyModuleDef sublistmodule = {
        PyModuleDef_HEAD_INIT,
        .m_name = "sublist",
        .m_doc = "Example module that creates an extension type.",
        .m_size = 1,
};

PyMODINIT_FUNC
PyInit_sublist(void) {
    PyObject *m;

    // PyType_Ready() を呼ぶ前に、型の構造体の tp_base スロットは埋めておく必要がある
    SubListType.tp_base = &PyList_Type;
    if (PyType_Ready(&SubListType) < 0) {
        return NULL;
    }
    m = PyModule_Create(&sublistmodule);
    if (m == NULL) {
        return NULL;
    }
    Py_INCREF(&SubListType);
    if (PyModule_AddObject(m, "SubList", (PyObject * ) & SubListType) < 0) {
        Py_DECREF(&SubListType);
        Py_DECREF(m);
        return NULL;
    }
    return m;
}
