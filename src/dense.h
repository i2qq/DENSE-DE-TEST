#ifndef DENSE_H
#define DENSE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define dense_mkdir(dir) _mkdir(dir)
#ifndef strcasecmp
#define strcasecmp _stricmp
#endif
#else
#include <unistd.h>
#include <strings.h>
#include <sys/stat.h>
#include <sys/types.h>
#define dense_mkdir(dir) mkdir(dir, 0755)
#ifndef _stricmp
#define _stricmp strcasecmp
#endif
#define Sleep(ms) usleep((ms) * 1000)
#endif

bool dense_file_exists(const char *path);
void dense_sleep_ms(int ms);

#define DENSE_VERSION "0.2.0"
#define MAX_LINE_LEN 4096
#define MAX_TOKEN_LEN 512
#define MAX_VARS 512
#define MAX_TABLE_ENTRIES 1024
#define MAX_IMPORTS 128

typedef enum {
    VAL_NONE,
    VAL_INT,
    VAL_STR,
    VAL_BOOL
} DenseValueType;

typedef struct {
    DenseValueType type;
    int64_t int_val;
    char str_val[MAX_TOKEN_LEN];
} DenseValue;

typedef struct {
    char name[MAX_TOKEN_LEN];
    DenseValue val;
} DenseVar;

typedef struct {
    char code[64];      // Base64 code (e.g. "K", "7", "&T", "&h")
    char target[128];   // Target command / name (e.g. "echo", "del", "True", "high")
    bool is_vocab;      // True if starts with '&'
} DenseMapping;

typedef struct {
    char module[128];
    char alias[128];
} DenseImport;

typedef struct {
    bool is_custom_mode;
    DenseMapping mappings[MAX_TABLE_ENTRIES];
    int mapping_count;
} DenseSymbolTable;

typedef struct {
    char encoding[64];
    DenseImport imports[MAX_IMPORTS];
    int import_count;
} DenseProject;

typedef struct {
    DenseProject project;
    DenseSymbolTable symtable;
    DenseVar vars[MAX_VARS];
    int var_count;
    bool debug_mode;
    int exit_requested;
    int exit_code;
} DenseRuntime;

// Core functions
DenseValue dense_val_none(void);
DenseValue dense_val_int(int64_t v);
DenseValue dense_val_str(const char *s);
DenseValue dense_val_bool(bool b);
void dense_val_print(DenseValue v, bool newline);
bool dense_val_is_truthy(DenseValue v);
bool dense_val_equals(DenseValue a, DenseValue b);

// Environment
void dense_runtime_init(DenseRuntime *rt);
void dense_set_var(DenseRuntime *rt, const char *name, DenseValue val);
DenseValue dense_get_var(DenseRuntime *rt, const char *name);

// Symbol Table & Mappings
void dense_symtable_init(DenseSymbolTable *tbl);
void dense_symtable_load_master(DenseSymbolTable *tbl);
bool dense_symtable_add(DenseSymbolTable *tbl, const char *code, const char *target);
const char* dense_symtable_resolve(DenseSymbolTable *tbl, const char *code);
const char* dense_symtable_find_code(DenseSymbolTable *tbl, const char *target);

// Manifest and table loaders
bool dense_load_project(DenseRuntime *rt, const char *filepath);
bool dense_load_denselook(DenseRuntime *rt, const char *filepath);

// Evaluator
DenseValue dense_eval_expr(DenseRuntime *rt, const char *expr);
bool dense_execute_line(DenseRuntime *rt, const char *line);
bool dense_execute_file(DenseRuntime *rt, const char *filepath);

#endif // DENSE_H
