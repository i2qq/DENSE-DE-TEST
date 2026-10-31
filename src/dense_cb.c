#include "dense.h"
#include "dense_py.h"
#include "dense_cb.h"

#ifdef DENSE_WITH_PYTHON
#include <Python.h>

static DenseRuntime *ACTIVE_RT = NULL;

void dense_set_active_runtime(DenseRuntime *rt) {
    ACTIVE_RT = rt;
}

static PyObject* py_dense_callback(PyObject *self, PyObject *args) {
    (void)args;
    const char *line = PyCapsule_GetPointer(self, "dense_action");
    if (line && ACTIVE_RT) {
        dense_execute_line(ACTIVE_RT, line);
    }
    Py_RETURN_NONE;
}

static PyMethodDef callback_def = {"dense_cb", (PyCFunction)py_dense_callback, METH_VARARGS, ""};

DenseValue dense_py_create_callback(const char *action_line) {
    dense_py_init();
    char *dup = strdup(action_line);
    PyObject *capsule = PyCapsule_New(dup, "dense_action", NULL);
    PyObject *func = PyCFunction_New(&callback_def, capsule);
    Py_DECREF(capsule);

    // Register callback object
    DenseValue v = dense_val_none();
    if (func) {
        // Return object representation
        extern DenseValue py_to_dense_value_public(PyObject *obj);
        v = py_to_dense_value_public(func);
    }
    return v;
}
#else
void dense_set_active_runtime(DenseRuntime *rt) {
    (void)rt;
}

DenseValue dense_py_create_callback(const char *action_line) {
    (void)action_line;
    return dense_val_none();
}
#endif
