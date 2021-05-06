//
// Created by rhoboro on 2021/05/06.
//

#define PY_SSIZE_T_CLEAN

#include <Python.h>
#include "structmember.h"

typedef struct _CustomObject {
    PyObject ob_base;  // == PyObject_HEAD
    PyObject *first;
    PyObject *last;
    int number;
} CustomObject;

// あとで tp_dealloc に代入する
static void
Custom_dealloc(CustomObject *self) {
    Py_XDECREF(self->first);
    Py_XDECREF(self->last);
    // Py_TYPEが返す型はサブクラスの可能性もある
    Py_TYPE(self)->tp_free((PyObject *) self);
}

// __new__() になる。typeはサブクラスの可能性もあり。
// tp_init (__init__()) の呼び出しはインタプリタが行うのでここで行ってはいけない
static PyObject *
Custom_new(PyTypeObject *type, PyObject *args, PyObject *kwds) {
    CustomObject *self;
    // 失敗時は NULL
    // tp_alloc は PyType_Ready() がセットしている
    self = ((CustomObject *) type->tp_alloc(type, 0));
    if (self != NULL) {
        self->first = PyUnicode_FromString("");
        if (self->first == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->last = PyUnicode_FromString("");
        if (self->last == NULL) {
            Py_DECREF(self);
            return NULL;
        }
        self->number = 0;
    }
    return (PyObject *) self;
}

// __init__() になる。
// 戻り値は成功なら0、失敗なら-1で固定
static int
Custom_init(CustomObject *self, PyObject *args, PyObject *kwds) {
    static char *kwlist[] = {"first", "last", "number", NULL};
    PyObject *first = NULL, *last = NULL, *tmp;
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|OOi", kwlist, &first, &last, &self->number)) {
        return -1;
    }
    // tp_init は複数回呼び出しても良いので実装時には注意が必要
    // 逆に unpickle 時など呼ばれないこともあるのでこちらも注意が必要
    if (first) {
        tmp = self->first;
        Py_INCREF(first);
        self->first = first;
        Py_XDECREF(tmp);
    }
    if (last) {
        tmp = self->last;
        Py_INCREF(last);
        self->last = last;
        Py_XDECREF(tmp);
    }
    return 0;
}

static PyMemberDef Custom_members[] = {
        {"first", T_OBJECT_EX, offsetof(CustomObject, first), 0, "first name"},
        { "last", T_OBJECT_EX, offsetof(CustomObject, last), 0, "last name" },
        { "number", T_INT, offsetof(CustomObject, number), 0, "custom number" },
        { NULL },
};

static PyObject *
Custom_name(CustomObject *self, PyObject *Py_UNUSED(ignored)) {
    if (self->first == NULL) {
        PyErr_SetString(PyExc_AttributeError, "first");
        return NULL;
    }
    if (self->last == NULL) {
        PyErr_SetString(PyExc_AttributeError, "last");
        return NULL;
    }
    return PyUnicode_FromFormat("%S %S", self->first, self->last);
}

static PyMethodDef Custom_methods[] = {
        {"name", (PyCFunction) Custom_name, METH_NOARGS, "Return the name, combining the first and last name"},
        {NULL}
};

// https://docs.python.org/ja/3/c-api/typeobj.html#pytypeobject-slots
static PyTypeObject CustomType = {
        PyVarObject_HEAD_INIT(NULL, 0)  // ob_baseの初期化用ボイラープレート
                .tp_name = "custom2.Custom",
        .tp_doc = "Custom objects",
        .tp_basicsize = sizeof(CustomObject),  // 新しいインスタンス作成時に確保するメモリ量
        .tp_itemsize = 0,  // listやdictなどの可変サイズオブジェクト以外は0
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE,
        .tp_new = Custom_new,
        .tp_init = (initproc) Custom_init,
        .tp_dealloc = (destructor) Custom_dealloc,  // PyObject *を引数に受け取るのでキャストが必要
        .tp_members = Custom_members,
        .tp_methods = Custom_methods,
};

// https://docs.python.org/ja/3/c-api/module.html#c.PyModuleDef
static PyModuleDef custommodule = {
        PyModuleDef_HEAD_INIT,
        .m_name = "custom2",
        .m_doc = "Example module that creates an extension type.",
        .m_size = -1,
};

PyMODINIT_FUNC
PyInit_custom2(void) {
    PyObject *m;
    // いくつかのメンバーを適切なデフォルト値で埋めて、 Custom 型を初期化
    if (PyType_Ready(&CustomType) > 0) {
        return NULL;
    }
    m = PyModule_Create(&custommodule);
    if (m == NULL)
        return NULL;
    Py_INCREF(&CustomType);
    if (PyModule_AddObject(m, "Custom", (PyObject * ) & CustomType) < 0) {
        Py_DECREF(&CustomType);
        Py_DECREF(m);
        return NULL;
    }
    return m;
}

