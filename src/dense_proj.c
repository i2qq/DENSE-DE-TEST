#include "dense.h"
#include "dense_py.h"
#include <ctype.h>

static char* trim_whitespace(char *str) {
    if (!str) return NULL;
    while (isspace((unsigned char)*str)) str++;
    if (*str == 0) return str;
    char *end = str + strlen(str) - 1;
    while (end > str && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return str;
}

bool dense_load_denselook(DenseRuntime *rt, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "Fatal Error: table.denselook is strictly required but not found at: %s\n", filepath);
        return false;
    }

    char line[MAX_LINE_LEN];
    bool in_table_block = false;
    bool is_custom = false;

    // Default to normal mode (master dict loaded)
    dense_symtable_init(&rt->symtable);
    dense_symtable_load_master(&rt->symtable);

    while (fgets(line, sizeof(line), fp)) {
        char *p = trim_whitespace(line);
        if (strlen(p) == 0 || (p[0] == '/' && p[1] == '/')) {
            continue; // Skip comments and empty lines
        }

        if (strstr(p, "table /") != NULL) {
            in_table_block = true;
            continue;
        }

        if (in_table_block) {
            if (strcmp(p, "\\") == 0) {
                in_table_block = false;
                continue;
            }

            if (strstr(p, "style | custom") != NULL) {
                is_custom = true;
                rt->symtable.is_custom_mode = true;
                // Wipe default dictionary for custom mode
                rt->symtable.mapping_count = 0;
                continue;
            } else if (strstr(p, "style | normal") != NULL) {
                is_custom = false;
                rt->symtable.is_custom_mode = false;
                continue;
            }

            // Parse mapping line: CODE | TARGET
            char *pipe = strchr(p, '|');
            if (pipe) {
                *pipe = '\0';
                char *code = trim_whitespace(p);
                char *target = trim_whitespace(pipe + 1);
                if (strlen(code) > 0 && strlen(target) > 0) {
                    dense_symtable_add(&rt->symtable, code, target);
                }
            }
        }
    }

    fclose(fp);
    return true;
}

bool dense_load_project(DenseRuntime *rt, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        // Warning or error depending on mode; per spec project.denseproj is mandatory
        fprintf(stderr, "Fatal Error: project.denseproj is strictly required but not found at: %s\n", filepath);
        return false;
    }

    char line[MAX_LINE_LEN];
    int line_num = 0;
    bool in_scratchpad = false;
    bool in_imports = false;
    bool header_verified = false;

    rt->project.import_count = 0;
    strcpy(rt->project.encoding, "base8");

    while (fgets(line, sizeof(line), fp)) {
        line_num++;
        char *p = trim_whitespace(line);

        // Line 1: Header verification
        if (line_num == 1) {
            if (strncmp(p, "//denseproj", 11) == 0) {
                header_verified = true;
                continue;
            } else {
                fprintf(stderr, "Fatal Error: Invalid project header in %s. Expected '//denseproj'.\n", filepath);
                fclose(fp);
                return false;
            }
        }

        // Line 2: Encoding (if present)
        if (line_num == 2 && strncmp(p, "encoding=", 9) == 0) {
            strncpy(rt->project.encoding, p + 9, sizeof(rt->project.encoding) - 1);
            rt->project.encoding[sizeof(rt->project.encoding) - 1] = '\0';
            continue;
        }

        if (strstr(p, "scratchpad /") != NULL) {
            in_scratchpad = true;
            continue;
        }

        if (in_scratchpad) {
            if (strcmp(p, "\\") == 0) {
                in_scratchpad = false;
            }
            // Strictly ignore everything in scratchpad
            continue;
        }

        if (strstr(p, "global | imports /") != NULL) {
            in_imports = true;
            continue;
        }

        if (in_imports) {
            if (strcmp(p, "\\") == 0) {
                in_imports = false;
                continue;
            }
            if (strlen(p) == 0 || (p[0] == '/' && p[1] == '/')) {
                continue;
            }

            // Parse: module - ALIAS
            char *dash = strchr(p, '-');
            if (dash && rt->project.import_count < MAX_IMPORTS) {
                *dash = '\0';
                char *mod = trim_whitespace(p);
                char *alias = trim_whitespace(dash + 1);
                if (strlen(mod) > 0 && strlen(alias) > 0) {
                    DenseImport *imp = &rt->project.imports[rt->project.import_count++];
                    strncpy(imp->module, mod, sizeof(imp->module) - 1);
                    imp->module[sizeof(imp->module) - 1] = '\0';
                    strncpy(imp->alias, alias, sizeof(imp->alias) - 1);
                    imp->alias[sizeof(imp->alias) - 1] = '\0';
                    // Automatically bind into embedded Python runtime
                    dense_py_import(mod, alias);
                }
            }
            continue;
        }
    }

    fclose(fp);
    return header_verified;
}
