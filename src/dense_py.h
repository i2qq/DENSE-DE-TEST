#ifndef DENSE_PY_H
#define DENSE_PY_H

#include "dense.h"

// Initialize Python embedded runtime
void dense_py_init(void);
void dense_py_cleanup(void);

// Import a module and register alias
bool dense_py_import(const char *module_name, const char *alias);

// Call a python function or method: alias.method(args...) or module.func(args...)
// In Dense: OBJ|METHOD|arg1|arg2...
DenseValue dense_py_call(DenseRuntime *rt, const char *target, const char *method, char args[][MAX_TOKEN_LEN], int arg_count);

// Create object / call constructor
DenseValue dense_py_instantiate(const char *cls_name, char args[][MAX_TOKEN_LEN], int arg_count);

// Attribute access / property get/set
DenseValue dense_py_get_attr(const char *obj_id, const char *attr_name);
bool dense_py_set_attr(const char *obj_id, const char *attr_name, const char *val_str);

#endif // DENSE_PY_H
