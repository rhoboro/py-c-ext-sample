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


static int
Custom_traverse(CustomObject *self, visitproc visit, void *arg) {
    Py_VISIT(self->first);
    // Py_VISIT() はこれと同じなので引数名は必ず visit, arg にする
    //    int vret;
    //    if (self->first) {
    //        vret = visit(self->first, arg);
    //        if (vret != 0)
    //            return vret;
    //    }
    Py_VISIT(self->last);
    return 0;
}

static int
Custom_clear(CustomObject *self) {
    Py_CLEAR(self->first);
    // Py_CLEAR() が行うことはこれと同等
    // PyObject *tmp;
    // tmp = self->first;
    // self->first = NULL;
    // Py_XDECREF(tmp);
    Py_CLEAR(self->last);
    return 0;
}

static void
Custom_dealloc(CustomObject *self) {
    // クリアが参照カウントが1以上を想定しているのでGCからアントラックしておく
    PyObject_GC_UnTrack(self);
    // 循環GC起動してメンバをクリアする。
    Custom_clear(self);
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
    // first, last は文字列のみを許可する
    if (!PyArg_ParseTupleAndKeywords(args, kwds, "|UUi", kwlist, &first, &last, &self->number)) {
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

// getter, setter を定義したのでメンバからは外す
static PyMemberDef Custom_members[] = {
        {"number", T_INT,       offsetof(CustomObject, number), 0, "custom number"},
        {NULL},
};

static PyObject *
Custom_getfirst(CustomObject *self, void *closure) {
    Py_INCREF(self->first);
    return self->first;
}

static int
Custom_setfirst(CustomObject *self, PyObject *value, void *closure) {
    PyObject *tmp;
    // del 時に NULL が渡されてくる
    if (value == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Cannnot delete the first attribute");
        return -1;
    }
    if (!PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_AttributeError, "The first attribute value must be a string");
        return -1;
    }
    tmp = self->first;
    Py_INCREF(value);
    self->first = value;
    Py_DECREF(tmp);
    return 0;
}

static PyObject *
Custom_getlast(CustomObject *self, void *closure) {
    Py_INCREF(self->last);
    return self->last;
}

static int
Custom_setlast(CustomObject *self, PyObject *value, void *closure) {
    PyObject *tmp;
    if (value == NULL) {
        PyErr_SetString(PyExc_AttributeError, "Cannnot delete the last attribute");
        return -1;
    }
    if (!PyUnicode_Check(value)) {
        PyErr_SetString(PyExc_AttributeError, "The last attribute value must be a string");
        return -1;
    }
    tmp = self->last;
    Py_INCREF(value);
    self->last = value;
    Py_DECREF(tmp);
    return 0;
}

static PyGetSetDef Custom_getsetters[] = {
        {"first", (getter) Custom_getfirst, (setter) Custom_setfirst, "first name", NULL},
        {"last",  (getter) Custom_getlast,  (setter) Custom_setlast,  "last name",  NULL},
        {NULL}
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
                .tp_name = "custom4.Custom",
        .tp_doc = "Custom objects",
        .tp_basicsize = sizeof(CustomObject),  // 新しいインスタンス作成時に確保するメモリ量
        .tp_itemsize = 0,  // listやdictなどの可変サイズオブジェクト以外は0
        // https://docs.python.org/ja/3/c-api/typeobj.html#Py_TPFLAGS_HAVE_GC
        .tp_flags = Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE | Py_TPFLAGS_HAVE_GC,
        .tp_new = Custom_new,
        .tp_init = (initproc) Custom_init,
        .tp_dealloc = (destructor) Custom_dealloc,  // PyObject *を引数に受け取るのでキャストが必要
        .tp_traverse = (traverseproc) Custom_traverse,
        .tp_clear = (inquiry) Custom_clear,
        .tp_members = Custom_members,
        .tp_methods = Custom_methods,
        .tp_getset = Custom_getsetters,
};

// https://docs.python.org/ja/3/c-api/module.html#c.PyModuleDef
static PyModuleDef custommodule = {
        PyModuleDef_HEAD_INIT,
        .m_name = "custom4",
        .m_doc = "Example module that creates an extension type.",
        .m_size = -1,
};

PyMODINIT_FUNC
PyInit_custom4(void) {
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

