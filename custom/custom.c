//
// Created by rhoboro on 2021/05/06.
//

#define PY_SSIZE_T_CLEAN

#include <Python.h>

typedef struct _CustomObject {
    PyObject ob_base;  // == PyObject_HEAD
} CustomObject;

// https://docs.python.org/ja/3/c-api/typeobj.html#pytypeobject-slots
static PyTypeObject CustomType = {
    PyVarObject_HEAD_INIT(NULL, 0)  // ob_baseの初期化用ボイラープレート
        .tp_name = "custom.Custom",
        .tp_doc = "Custom objects",
        .tp_basicsize = sizeof(CustomObject),  // 新しいインスタンス作成時に確保するメモリ量
        .tp_itemsize = 0,  // listやdictなどの可変サイズオブジェクト以外は0
        .tp_flags = Py_TPFLAGS_DEFAULT,  // Python 3.3 までに定義されているすべてのメンバを許可する
        .tp_new = PyType_GenericNew,  // __new__ 相当
};

// https://docs.python.org/ja/3/c-api/module.html#c.PyModuleDef
static PyModuleDef custommodule = {
    PyModuleDef_HEAD_INIT,
        .m_name = "custom",
        .m_doc = "Example module that creates an extension type.",
        .m_size = -1,
};

PyMODINIT_FUNC
PyInit_custom(void) {
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

