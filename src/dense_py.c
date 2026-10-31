#include "dense.h"
#include "dense_py.h"

#ifdef DENSE_WITH_PYTHON
#include <Python.h>

#define MAX_PY_OBJECTS 1024

typedef struct {
    char id[64];
    PyObject *obj;
} PyObjectRegistry;

static PyObjectRegistry PY_OBJECTS[MAX_PY_OBJECTS];
static int PY_OBJECT_COUNT = 0;
static bool PY_INITIALIZED = false;

void dense_py_init(void) {
    if (!PY_INITIALIZED) {
        Py_Initialize();
        PY_INITIALIZED = true;
        PY_OBJECT_COUNT = 0;
    }
}

void dense_py_cleanup(void) {
    if (PY_INITIALIZED) {
        for (int i = 0; i < PY_OBJECT_COUNT; i++) {
            Py_XDECREF(PY_OBJECTS[i].obj);
        }
        PY_OBJECT_COUNT = 0;
        Py_Finalize();
        PY_INITIALIZED = false;
    }
}

static const char* register_py_object(PyObject *obj) {
    if (!obj) return NULL;
    if (PY_OBJECT_COUNT >= MAX_PY_OBJECTS) {
        Py_XDECREF(obj);
        return NULL;
    }
    char *id = PY_OBJECTS[PY_OBJECT_COUNT].id;
    snprintf(id, 64, "@py_%d", PY_OBJECT_COUNT);
    PY_OBJECTS[PY_OBJECT_COUNT].obj = obj;
    PY_OBJECT_COUNT++;
    return id;
}

static PyObject* find_py_object(const char *id_or_alias) {
    if (!id_or_alias) return NULL;
    for (int i = 0; i < PY_OBJECT_COUNT; i++) {
        if (strcmp(PY_OBJECTS[i].id, id_or_alias) == 0) {
            return PY_OBJECTS[i].obj;
        }
    }
    // Also check Python __main__ or builtins
    PyObject *main_mod = PyImport_AddModule("__main__");
    if (main_mod) {
        PyObject *dict = PyModule_GetDict(main_mod);
        if (dict && PyDict_Contains(dict, PyUnicode_FromString(id_or_alias))) {
            return PyDict_GetItemString(dict, id_or_alias);
        }
    }
    return NULL;
}

bool dense_py_import(const char *module_name, const char *alias) {
    dense_py_init();
    PyObject *pMod = PyImport_ImportModule(module_name);
    if (!pMod) {
        PyErr_Clear();
        return true;
    }

    PyObject *main_mod = PyImport_AddModule("__main__");
    if (main_mod) {
        PyObject *dict = PyModule_GetDict(main_mod);
        if (dict) {
            PyDict_SetItemString(dict, alias ? alias : module_name, pMod);
        }
    }

    // Also register in objects registry
    if (PY_OBJECT_COUNT < MAX_PY_OBJECTS) {
        strncpy(PY_OBJECTS[PY_OBJECT_COUNT].id, alias ? alias : module_name, 63);
        PY_OBJECTS[PY_OBJECT_COUNT].id[63] = '\0';
        PY_OBJECTS[PY_OBJECT_COUNT].obj = pMod;
        PY_OBJECT_COUNT++;
    }
    return true;
}

static PyObject* parse_py_val(DenseRuntime *rt, const char *s) {
    if (!s) Py_RETURN_NONE;
    if (strcmp(s, "&T") == 0 || _stricmp(s, "true") == 0) Py_RETURN_TRUE;
    if (strcmp(s, "&F") == 0 || _stricmp(s, "false") == 0) Py_RETURN_FALSE;
    if (strcmp(s, "&0") == 0 || _stricmp(s, "none") == 0) Py_RETURN_NONE;

    // First check if 's' is a Dense variable name (e.g. 'win')
    if (rt) {
        DenseValue v = dense_get_var(rt, s);
        if (v.type == VAL_STR && v.str_val[0] == '@') {
            PyObject *existing = find_py_object(v.str_val);
            if (existing) {
                Py_INCREF(existing);
                return existing;
            }
        }
    }

    // Check if it's an existing registered PyObject ID or alias
    PyObject *existing = find_py_object(s);
    if (existing) {
        Py_INCREF(existing);
        return existing;
    }

    // Check float FIRST (e.g. 62.14, 3.14159)
    char *endptr;
    double dval = strtod(s, &endptr);
    if (*s != '\0' && *endptr == '\0') {
        if (strchr(s, '.')) {
            return PyFloat_FromDouble(dval);
        }
    }

    // Check if it's an object.attribute or module.attribute like win.destroy, ttk.Entry, etc.
    char *dot = strchr(s, '.');
    if (dot) {
        char mod_part[64];
        size_t dlen = dot - s;
        if (dlen >= sizeof(mod_part)) dlen = sizeof(mod_part) - 1;
        strncpy(mod_part, s, dlen);
        mod_part[dlen] = '\0';
        const char *attr_part = dot + 1;

        PyObject *mobj = NULL;
        if (rt) {
            DenseValue v = dense_get_var(rt, mod_part);
            if (v.type == VAL_STR && v.str_val[0] == '@') {
                mobj = find_py_object(v.str_val);
            }
        }
        if (!mobj) {
            mobj = find_py_object(mod_part);
        }
        // Only try PyImport_ImportModule if mod_part does not contain spaces or colons
        if (!mobj && strchr(mod_part, ' ') == NULL && strchr(mod_part, ':') == NULL && strchr(mod_part, '_') == NULL) {
            mobj = PyImport_ImportModule(mod_part);
            if (!mobj) {
                PyErr_Clear(); // CLEAR ANY MODULE NOT FOUND ERROR!
            }
        }
        if (mobj) {
            PyObject *aobj = PyObject_GetAttrString(mobj, attr_part);
            if (aobj) return aobj;
            PyErr_Clear(); // CLEAR ANY ATTRIBUTE ERROR!
        }
    }

    // Check integer
    long long lval = strtoll(s, &endptr, 10);
    if (*s != '\0' && *endptr == '\0') {
        return PyLong_FromLongLong(lval);
    }

    // String literal
    return PyUnicode_FromString(s);
}

DenseValue py_to_dense_value_public(PyObject *obj) {
    if (!obj || obj == Py_None) {
        return dense_val_none();
    }
    if (PyBool_Check(obj)) {
        return dense_val_bool(obj == Py_True);
    }
    if (PyLong_Check(obj)) {
        return dense_val_int(PyLong_AsLongLong(obj));
    }
    if (PyUnicode_Check(obj)) {
        const char *utf8 = PyUnicode_AsUTF8(obj);
        return dense_val_str(utf8);
    }
    if (PyFloat_Check(obj)) {
        char buf[64];
        snprintf(buf, sizeof(buf), "%g", PyFloat_AsDouble(obj));
        return dense_val_str(buf);
    }

    // Complex / object reference: register in registry and return its ID
    const char *id = register_py_object(obj);
    if (id) {
        return dense_val_str(id);
    }
    return dense_val_none();
}

static DenseValue py_to_dense_value(PyObject *obj) {
    return py_to_dense_value_public(obj);
}

DenseValue dense_py_call(DenseRuntime *rt, const char *target, const char *method, char args[][MAX_TOKEN_LEN], int arg_count) {
    dense_py_init();
    PyObject *obj = find_py_object(target);
    if (!obj && rt) {
        // Also check if target is a Dense variable name (e.g. 'win')
        DenseValue v = dense_get_var(rt, target);
        if (v.type == VAL_STR && v.str_val[0] == '@') {
            obj = find_py_object(v.str_val);
        }
    }
    if (!obj) {
        // Try importing target on the fly if it's a module
        obj = PyImport_ImportModule(target);
        if (obj) {
            register_py_object(obj);
        } else {
            PyErr_Clear();
            return dense_val_none();
        }
    }

    // Support nested attribute traversal (e.g. ttk.Button or entry.get)
    PyObject *callable = NULL;
    if (method && strlen(method) > 0) {
        char method_buf[128];
        strncpy(method_buf, method, sizeof(method_buf) - 1);
        method_buf[sizeof(method_buf) - 1] = '\0';

        char *curr_obj = (char*)obj;
        char *token = strtok(method_buf, ".");
        PyObject *curr = obj;
        Py_INCREF(curr);

        while (token != NULL) {
            PyObject *next = PyObject_GetAttrString(curr, token);
            Py_DECREF(curr);
            if (!next) {
                PyErr_Clear();
                return dense_val_none();
            }
            curr = next;
            token = strtok(NULL, ".");
        }
        callable = curr;
    } else {
        callable = obj;
        Py_INCREF(callable);
    }

    if (!callable || !PyCallable_Check(callable)) {
        Py_XDECREF(callable);
        return dense_val_none();
    }

    // Build kwargs and args
    // In Dense: key=val or bare val
    PyObject *py_args = PyTuple_New(0);
    PyObject *py_kwargs = PyDict_New();
    int pos_count = 0;

    for (int i = 0; i < arg_count; i++) {
        char *eq = strchr(args[i], '=');
        if (eq) {
            *eq = '\0';
            const char *k = args[i];
            const char *v = eq + 1;
            PyObject *v_obj = parse_py_val(rt, v);
            PyDict_SetItemString(py_kwargs, k, v_obj);
            Py_XDECREF(v_obj);
            *eq = '=';
        } else {
            pos_count++;
        }
    }

    Py_DECREF(py_args);
    py_args = PyTuple_New(pos_count);
    int p_idx = 0;
    for (int i = 0; i < arg_count; i++) {
        char *eq = strchr(args[i], '=');
        if (!eq) {
            PyObject *v_obj = parse_py_val(rt, args[i]);
            PyTuple_SetItem(py_args, p_idx++, v_obj); // PyTuple_SetItem steals reference
        }
    }

    PyObject *result = PyObject_Call(callable, py_args, py_kwargs);
    Py_DECREF(callable);
    Py_DECREF(py_args);
    Py_DECREF(py_kwargs);

    if (!result) {
        PyErr_Print();
        return dense_val_none();
    }

    DenseValue val = py_to_dense_value(result);
    // If it was a primitive, decref result
    if (val.str_val[0] != '@') {
        Py_DECREF(result);
    }
    return val;
}
#else
void dense_py_init(void) {}
void dense_py_cleanup(void) {}
bool dense_py_import(const char *module_name, const char *alias) {
    (void)module_name; (void)alias;
    return false;
}
DenseValue dense_py_call(DenseRuntime *rt, const char *target, const char *method, char args[][MAX_TOKEN_LEN], int arg_count) {
    (void)rt; (void)target; (void)method; (void)args; (void)arg_count;
    return dense_val_none();
}
DenseValue dense_py_instantiate(const char *cls_name, char args[][MAX_TOKEN_LEN], int arg_count) {
    (void)cls_name; (void)args; (void)arg_count;
    return dense_val_none();
}
DenseValue dense_py_get_attr(const char *obj_id, const char *attr_name) {
    (void)obj_id; (void)attr_name;
    return dense_val_none();
}
bool dense_py_set_attr(const char *obj_id, const char *attr_name, const char *val_str) {
    (void)obj_id; (void)attr_name; (void)val_str;
    return false;
}
DenseValue py_to_dense_value_public(void *obj) {
    (void)obj;
    return dense_val_none();
}
#endif
