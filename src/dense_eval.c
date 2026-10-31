#include "dense.h"
#include "dense_py.h"
#include "dense_de.h"
#include <ctype.h>
#include <sys/stat.h>

bool dense_file_exists(const char *path) {
    if (!path || path[0] == '\0') return false;
#ifdef _WIN32
    DWORD attr = GetFileAttributesA(path);
    return (attr != INVALID_FILE_ATTRIBUTES);
#else
    struct stat st;
    return (stat(path, &st) == 0);
#endif
}

void dense_sleep_ms(int ms) {
#ifdef _WIN32
    Sleep((DWORD)ms);
#else
    usleep((useconds_t)(ms * 1000));
#endif
}

static DenseValue dense_call_command(DenseRuntime *rt, const char *action_code, char args[][MAX_TOKEN_LEN], int arg_count);

// Value constructors & helpers
DenseValue dense_val_none(void) {
    DenseValue v;
    v.type = VAL_NONE;
    v.int_val = 0;
    v.str_val[0] = '\0';
    return v;
}

DenseValue dense_val_int(int64_t v) {
    DenseValue val;
    val.type = VAL_INT;
    val.int_val = v;
    snprintf(val.str_val, sizeof(val.str_val), "%lld", (long long)v);
    return val;
}

DenseValue dense_val_str(const char *s) {
    DenseValue val;
    val.type = VAL_STR;
    val.int_val = 0;
    if (s) {
        strncpy(val.str_val, s, sizeof(val.str_val) - 1);
        val.str_val[sizeof(val.str_val) - 1] = '\0';
    } else {
        val.str_val[0] = '\0';
    }
    return val;
}

DenseValue dense_val_bool(bool b) {
    DenseValue val;
    val.type = VAL_BOOL;
    val.int_val = b ? 1 : 0;
    strcpy(val.str_val, b ? "true" : "false");
    return val;
}

void dense_val_print(DenseValue v, bool newline) {
    switch (v.type) {
        case VAL_NONE:
            printf("None");
            break;
        case VAL_INT:
            printf("%lld", (long long)v.int_val);
            break;
        case VAL_STR:
            printf("%s", v.str_val);
            break;
        case VAL_BOOL:
            printf("%s", v.int_val ? "True" : "False");
            break;
    }
    if (newline) printf("\n");
}

bool dense_val_is_truthy(DenseValue v) {
    switch (v.type) {
        case VAL_NONE:
            return false;
        case VAL_BOOL:
        case VAL_INT:
            return v.int_val != 0;
        case VAL_STR:
            if (strcmp(v.str_val, "0") == 0 || _stricmp(v.str_val, "false") == 0 || _stricmp(v.str_val, "none") == 0) return false;
            return strlen(v.str_val) > 0;
    }
    return false;
}

bool dense_val_equals(DenseValue a, DenseValue b) {
    if (a.type == VAL_NONE && b.type == VAL_NONE) return true;
    if (a.type == VAL_NONE || b.type == VAL_NONE) return false;

    if (a.type == VAL_INT && b.type == VAL_INT) {
        return a.int_val == b.int_val;
    }

    // String / boolean / integer comparison
    return strcmp(a.str_val, b.str_val) == 0;
}

void dense_runtime_init(DenseRuntime *rt) {
    memset(rt, 0, sizeof(DenseRuntime));
    dense_symtable_init(&rt->symtable);
    dense_symtable_load_master(&rt->symtable);
    rt->var_count = 0;
    rt->debug_mode = false;
    rt->exit_requested = 0;
    rt->exit_code = 0;
}

void dense_set_var(DenseRuntime *rt, const char *name, DenseValue val) {
    for (int i = 0; i < rt->var_count; i++) {
        if (strcmp(rt->vars[i].name, name) == 0) {
            rt->vars[i].val = val;
            return;
        }
    }
    if (rt->var_count < MAX_VARS) {
        strncpy(rt->vars[rt->var_count].name, name, sizeof(rt->vars[rt->var_count].name) - 1);
        rt->vars[rt->var_count].name[sizeof(rt->vars[rt->var_count].name) - 1] = '\0';
        rt->vars[rt->var_count].val = val;
        rt->var_count++;
    }
}

DenseValue dense_get_var(DenseRuntime *rt, const char *name) {
    for (int i = 0; i < rt->var_count; i++) {
        if (strcmp(rt->vars[i].name, name) == 0) {
            return rt->vars[i].val;
        }
    }
    return dense_val_none();
}

static bool is_integer(const char *s) {
    if (!s || *s == '\0') return false;
    if (*s == '-' || *s == '+') s++;
    if (*s == '\0') return false;
    while (*s) {
        if (!isdigit((unsigned char)*s)) return false;
        s++;
    }
    return true;
}

// Split string by pipe '|' into parts
// Note: respects zero-whitespace and zero-punctuation specification
static int split_pipes(const char *input, char parts[][MAX_TOKEN_LEN], int max_parts) {
    int count = 0;
    const char *curr = input;
    const char *next = NULL;

    while (curr && *curr && count < max_parts) {
        next = strchr(curr, '|');
        if (next) {
            size_t len = next - curr;
            if (len >= MAX_TOKEN_LEN) len = MAX_TOKEN_LEN - 1;
            strncpy(parts[count], curr, len);
            parts[count][len] = '\0';
            count++;
            curr = next + 1;
        } else {
            size_t len = strlen(curr);
            if (len >= MAX_TOKEN_LEN) len = MAX_TOKEN_LEN - 1;
            strncpy(parts[count], curr, len);
            parts[count][len] = '\0';
            count++;
            break;
        }
    }
    return count;
}

// Evaluate atomic token (number, &code vocab, variable, or raw literal)
static DenseValue dense_eval_atom(DenseRuntime *rt, const char *token) {
    if (!token || strlen(token) == 0) return dense_val_none();

    // Check if integer literal
    if (is_integer(token)) {
        return dense_val_int(_atoi64(token));
    }

    // Check if vocabulary / type / constant starting with '&'
    if (token[0] == '&') {
        const char *resolved = dense_symtable_resolve(&rt->symtable, token);
        if (resolved) {
            if (_stricmp(resolved, "true") == 0) return dense_val_bool(true);
            if (_stricmp(resolved, "false") == 0) return dense_val_bool(false);
            if (_stricmp(resolved, "none") == 0 || _stricmp(resolved, "null") == 0) return dense_val_none();
            return dense_val_str(resolved);
        }
        // If not in table, return token as raw literal or lookup standard
        if (strcmp(token, "&T") == 0) return dense_val_bool(true);
        if (strcmp(token, "&F") == 0) return dense_val_bool(false);
        if (strcmp(token, "&0") == 0) return dense_val_none();
        return dense_val_str(token);
    }

    // Check if variable exists
    for (int i = 0; i < rt->var_count; i++) {
        if (strcmp(rt->vars[i].name, token) == 0) {
            return rt->vars[i].val;
        }
    }

    // Check if it's an alias from project imports
    for (int i = 0; i < rt->project.import_count; i++) {
        if (strcmp(rt->project.imports[i].alias, token) == 0) {
            return dense_val_str(rt->project.imports[i].module);
        }
    }

    // Check if it's a 0-argument command action code (e.g. ti, V7, Xs, w, etc.) or action name (e.g. net_status, battery_info, media_info)
    const char *resolved_cmd = dense_symtable_resolve(&rt->symtable, token);
    const char *found_code = dense_symtable_find_code(&rt->symtable, token);
    if ((resolved_cmd != NULL || found_code != NULL) && token[0] != '&') {
        char dummy_args[1][MAX_TOKEN_LEN];
        DenseValue cmd_ret = dense_call_command(rt, token, dummy_args, 0);
        if (cmd_ret.type != VAL_NONE) {
            return cmd_ret;
        }
    }

    // Return as raw string literal (e.g. paths, bare words)
    return dense_val_str(token);
}

// Evaluate condition expressions (e.g., cfg==&0, x>10, a!=b, x<=5)
static bool dense_eval_condition(DenseRuntime *rt, const char *cond_str, bool *is_cond) {
    const char *ops[] = {"==", "!=", ">=", "<=", ">", "<", NULL};
    for (int i = 0; ops[i] != NULL; i++) {
        char *op_pos = strstr(cond_str, ops[i]);
        if (op_pos) {
            *is_cond = true;
            char lhs_str[MAX_TOKEN_LEN];
            char rhs_str[MAX_TOKEN_LEN];

            size_t lhs_len = op_pos - cond_str;
            if (lhs_len >= MAX_TOKEN_LEN) lhs_len = MAX_TOKEN_LEN - 1;
            strncpy(lhs_str, cond_str, lhs_len);
            lhs_str[lhs_len] = '\0';

            const char *rhs_start = op_pos + strlen(ops[i]);
            strncpy(rhs_str, rhs_start, sizeof(rhs_str) - 1);
            rhs_str[sizeof(rhs_str) - 1] = '\0';

            DenseValue lhs = dense_eval_atom(rt, lhs_str);
            DenseValue rhs = dense_eval_atom(rt, rhs_str);

            if (strcmp(ops[i], "==") == 0) {
                return dense_val_equals(lhs, rhs);
            } else if (strcmp(ops[i], "!=") == 0) {
                return !dense_val_equals(lhs, rhs);
            } else if (strcmp(ops[i], ">") == 0) {
                if (lhs.type == VAL_INT && rhs.type == VAL_INT) return lhs.int_val > rhs.int_val;
                return strcmp(lhs.str_val, rhs.str_val) > 0;
            } else if (strcmp(ops[i], "<") == 0) {
                if (lhs.type == VAL_INT && rhs.type == VAL_INT) return lhs.int_val < rhs.int_val;
                return strcmp(lhs.str_val, rhs.str_val) < 0;
            } else if (strcmp(ops[i], ">=") == 0) {
                if (lhs.type == VAL_INT && rhs.type == VAL_INT) return lhs.int_val >= rhs.int_val;
                return strcmp(lhs.str_val, rhs.str_val) >= 0;
            } else if (strcmp(ops[i], "<=") == 0) {
                if (lhs.type == VAL_INT && rhs.type == VAL_INT) return lhs.int_val <= rhs.int_val;
                return strcmp(lhs.str_val, rhs.str_val) <= 0;
            }
        }
    }
    *is_cond = false;
    return false;
}

// Builtin command executor
// Returns result DenseValue (for assignments) or sets exit code/status
static DenseValue dense_call_command(DenseRuntime *rt, const char *action_code, char args[][MAX_TOKEN_LEN], int arg_count) {
    const char *action = dense_symtable_resolve(&rt->symtable, action_code);
    if (!action) action = action_code;

    // 1. K -> print / echo
    if (strcmp(action_code, "K") == 0 || _stricmp(action, "echo") == 0 || _stricmp(action, "print") == 0) {
        for (int i = 0; i < arg_count; i++) {
            DenseValue v = dense_eval_atom(rt, args[i]);
            dense_val_print(v, (i == arg_count - 1));
            if (i < arg_count - 1) printf(" ");
        }
        if (arg_count == 0) printf("\n");
        return dense_val_int(0);
    }

    // 2. J -> read_file
    if (strcmp(action_code, "J") == 0 || _stricmp(action, "read") == 0 || _stricmp(action, "read_file") == 0) {
        if (arg_count < 1) return dense_val_none();
        DenseValue path_val = dense_eval_atom(rt, args[0]);
        const char *filepath = path_val.str_val;

        FILE *fp = fopen(filepath, "rb");
        if (!fp) {
            // Check fallback if provided: e.g. J|config.json|&0
            if (arg_count >= 2) {
                return dense_eval_atom(rt, args[1]);
            }
            return dense_val_none();
        }
        fseek(fp, 0, SEEK_END);
        long sz = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char *buf = (char *)malloc(sz + 1);
        if (!buf) {
            fclose(fp);
            return dense_val_none();
        }
        size_t read_bytes = fread(buf, 1, sz, fp);
        buf[read_bytes] = '\0';
        fclose(fp);

        DenseValue res = dense_val_str(buf);
        free(buf);
        return res;
    }

    // 3. 9 -> write_file: 9|filepath|data
    if (strcmp(action_code, "9") == 0 || _stricmp(action, "write") == 0 || _stricmp(action, "write_file") == 0) {
        if (arg_count < 2) return dense_val_int(-1);
        DenseValue path_val = dense_eval_atom(rt, args[0]);
        DenseValue data_val = dense_eval_atom(rt, args[1]);

        FILE *fp = fopen(path_val.str_val, "w");
        if (!fp) return dense_val_int(-1);
        fputs(data_val.str_val, fp);
        fputc('\n', fp);
        fclose(fp);
        return dense_val_int(0);
    }

    // 4. a -> append: a|filepath|data
    if (strcmp(action_code, "a") == 0 || _stricmp(action, "append") == 0) {
        if (arg_count < 2) return dense_val_int(-1);
        DenseValue path_val = dense_eval_atom(rt, args[0]);
        DenseValue data_val = dense_eval_atom(rt, args[1]);

        FILE *fp = fopen(path_val.str_val, "a");
        if (!fp) return dense_val_int(-1);
        fputs(data_val.str_val, fp);
        fputc('\n', fp);
        fclose(fp);
        return dense_val_int(0);
    }

    // 5. Q -> mkdir (if not exist)
    if (strcmp(action_code, "Q") == 0 || _stricmp(action, "mkdir") == 0) {
        if (arg_count < 1) return dense_val_int(-1);
        DenseValue path_val = dense_eval_atom(rt, args[0]);
        dense_mkdir(path_val.str_val);
        return dense_val_int(0);
    }

    // 6. 7 -> del / rm: 7|filepath
    if (strcmp(action_code, "7") == 0 || _stricmp(action, "del") == 0 || _stricmp(action, "rm") == 0) {
        if (arg_count < 1) return dense_val_int(-1);
        DenseValue path_val = dense_eval_atom(rt, args[0]);
        remove(path_val.str_val);
        return dense_val_int(0);
    }

    // 7. 2 -> rmdir
    if (strcmp(action_code, "2") == 0 || _stricmp(action, "rmdir") == 0) {
        if (arg_count < 1) return dense_val_int(-1);
        DenseValue path_val = dense_eval_atom(rt, args[0]);
#ifdef _WIN32
        RemoveDirectoryA(path_val.str_val);
#else
        rmdir(path_val.str_val);
#endif
        return dense_val_int(0);
    }

    // 8. m -> copy: m|src|dst
    if (strcmp(action_code, "m") == 0 || _stricmp(action, "copy") == 0) {
        if (arg_count < 2) return dense_val_int(-1);
        DenseValue src = dense_eval_atom(rt, args[0]);
        DenseValue dst = dense_eval_atom(rt, args[1]);
#ifdef _WIN32
        CopyFileA(src.str_val, dst.str_val, FALSE);
#else
        FILE *in = fopen(src.str_val, "rb");
        if (in) {
            FILE *out = fopen(dst.str_val, "wb");
            if (out) {
                char buf[4096];
                size_t n;
                while ((n = fread(buf, 1, sizeof(buf), in)) > 0) fwrite(buf, 1, n, out);
                fclose(out);
            }
            fclose(in);
        }
#endif
        return dense_val_int(0);
    }

    // 9. B -> move / rename: B|src|dst
    if (strcmp(action_code, "B") == 0 || _stricmp(action, "move") == 0 || _stricmp(action, "ren") == 0) {
        if (arg_count < 2) return dense_val_int(-1);
        DenseValue src = dense_eval_atom(rt, args[0]);
        DenseValue dst = dense_eval_atom(rt, args[1]);
        rename(src.str_val, dst.str_val);
        return dense_val_int(0);
    }

    // 10. e -> exists: e|path
    if (strcmp(action_code, "e") == 0 || _stricmp(action, "exists") == 0) {
        if (arg_count < 1) return dense_val_bool(false);
        DenseValue p = dense_eval_atom(rt, args[0]);
        return dense_val_bool(dense_file_exists(p.str_val));
    }

    // 11. s -> cls / clear
    if (strcmp(action_code, "s") == 0 || _stricmp(action, "cls") == 0 || _stricmp(action, "clear") == 0) {
#ifdef _WIN32
        system("cls");
#else
        system("clear");
#endif
        return dense_val_int(0);
    }

    // 12. 8 -> sleep: 8|ms_or_s
    if (strcmp(action_code, "8") == 0 || _stricmp(action, "sleep") == 0) {
        int64_t dur = 1000;
        if (arg_count >= 1) {
            DenseValue v = dense_eval_atom(rt, args[0]);
            dur = v.int_val;
        }
        dense_sleep_ms((int)dur);
        return dense_val_int(0);
    }

    // 13. X -> exit: X|code
    if (strcmp(action_code, "X") == 0 || _stricmp(action, "exit") == 0) {
        int code = 0;
        if (arg_count >= 1) {
            DenseValue v = dense_eval_atom(rt, args[0]);
            code = (int)v.int_val;
        }
        rt->exit_requested = 1;
        rt->exit_code = code;
        return dense_val_int(code);
    }

    // 14. I -> len: I|str_or_val
    if (strcmp(action_code, "I") == 0 || _stricmp(action, "len") == 0) {
        if (arg_count < 1) return dense_val_int(0);
        DenseValue v = dense_eval_atom(rt, args[0]);
        return dense_val_int(strlen(v.str_val));
    }

    // 15. w -> whoami
    if (strcmp(action_code, "w") == 0 || _stricmp(action, "whoami") == 0) {
#ifdef _WIN32
        char username[256];
        DWORD size = sizeof(username);
        if (GetUserNameA(username, &size)) {
            return dense_val_str(username);
        }
        return dense_val_str("unknown");
#else
        const char *user = getenv("USER");
        if (!user) user = getlogin();
        return dense_val_str(user ? user : "unknown");
#endif
    }

    // 16. i -> input: i|prompt
    if (strcmp(action_code, "i") == 0 || _stricmp(action, "input") == 0) {
        if (arg_count >= 1) {
            DenseValue prompt = dense_eval_atom(rt, args[0]);
            printf("%s", prompt.str_val);
            fflush(stdout);
        }
        char input_buf[MAX_TOKEN_LEN];
        if (fgets(input_buf, sizeof(input_buf), stdin)) {
            char *nl = strchr(input_buf, '\n');
            if (nl) *nl = '\0';
            nl = strchr(input_buf, '\r');
            if (nl) *nl = '\0';
            return dense_val_str(input_buf);
        }
        return dense_val_none();
    }

    // 17. x -> killforce (safe unlinker)
    if (strcmp(action_code, "x") == 0 || _stricmp(action, "killforce") == 0) {
        if (arg_count >= 1) {
            DenseValue path_val = dense_eval_atom(rt, args[0]);
#ifdef _WIN32
            SetFileAttributesA(path_val.str_val, FILE_ATTRIBUTE_NORMAL);
#endif
            remove(path_val.str_val);
        }
        return dense_val_int(0);
    }

    // 18. v -> loop / range: v|var|count|action_code|arg1...
    if (strcmp(action_code, "v") == 0 || _stricmp(action, "range") == 0) {
        if (arg_count < 3) return dense_val_int(0);
        const char *loop_var = args[0];
        DenseValue count_val = dense_eval_atom(rt, args[1]);
        int64_t count = count_val.int_val;

        const char *sub_action = args[2];
        char sub_args[16][MAX_TOKEN_LEN];
        int sub_argc = 0;
        for (int i = 3; i < arg_count && sub_argc < 16; i++) {
            strncpy(sub_args[sub_argc++], args[i], MAX_TOKEN_LEN - 1);
        }

        for (int64_t i = 0; i < count && !rt->exit_requested; i++) {
            dense_set_var(rt, loop_var, dense_val_int(i));
            dense_call_command(rt, sub_action, sub_args, sub_argc);
        }
        return dense_val_int(count);
    }

    // 19. N -> popen / shell execution: N|command_string
    if (strcmp(action_code, "N") == 0 || _stricmp(action, "popen") == 0) {
        if (arg_count < 1) return dense_val_int(-1);
        DenseValue cmd_val = dense_eval_atom(rt, args[0]);
        int ret = system(cmd_val.str_val);
        return dense_val_int(ret);
    }

    // 20. l1 -> lambda / callback: l1|ACTION_EXPR
    if (strcmp(action_code, "l1") == 0 || _stricmp(action, "lambda") == 0) {
        if (arg_count < 1) return dense_val_none();
        // Reconstruct action line if separated by pipes
        char action_buf[MAX_LINE_LEN] = {0};
        for (int i = 0; i < arg_count; i++) {
            if (i > 0) strcat(action_buf, "|");
            strcat(action_buf, args[i]);
        }
        extern DenseValue dense_py_create_callback(const char *action_line);
        extern void dense_set_active_runtime(DenseRuntime *rt);
        dense_set_active_runtime(rt);
        return dense_py_create_callback(action_buf);
    }

    // 21. Desktop Environment / Wayland / Labwc Commands
    DenseValue de_val = dense_de_dispatch_command(rt, action_code, args, arg_count);
    if (de_val.type != VAL_NONE) return de_val;
    de_val = dense_de_dispatch_command(rt, action, args, arg_count);
    if (de_val.type != VAL_NONE) return de_val;

    // Fallback: Check if target is a custom command or unknown
    return dense_val_none();
}

// Evaluate expression containing pipe multiplexer or atomic value
DenseValue dense_eval_expr(DenseRuntime *rt, const char *expr) {
    if (!expr || strlen(expr) == 0) return dense_val_none();

    char parts[16][MAX_TOKEN_LEN];
    int count = split_pipes(expr, parts, 16);

    if (count == 0) return dense_val_none();
    if (count == 1) {
        return dense_eval_atom(rt, parts[0]);
    }

    if (rt->debug_mode) {
        printf("[DEBUG] dense_eval_expr count=%d parts[0]='%s' parts[1]='%s'\n", count, parts[0], parts[1]);
    }

    // Check if parts[0] is an imported module alias or PyObject first
    // Module aliases take precedence over short command codes!
    for (int i = 0; i < rt->project.import_count; i++) {
        if (strcmp(rt->project.imports[i].alias, parts[0]) == 0) {
            char args[15][MAX_TOKEN_LEN];
            int arg_count = (count >= 2) ? count - 2 : 0;
            const char *method = (count >= 2) ? parts[1] : "";
            for (int a = 0; a < arg_count; a++) {
                strncpy(args[a], parts[a + 2], MAX_TOKEN_LEN - 1);
                args[a][MAX_TOKEN_LEN - 1] = '\0';
            }
            DenseValue r = dense_py_call(rt, parts[0], method, args, arg_count);
            if (rt->debug_mode) {
                printf("[DEBUG] dense_py_call target:'%s' method:'%s' returned type:%d str:'%s'\n", parts[0], method, r.type, r.str_val);
            }
            return r;
        }
    }

    if (parts[0][0] == '@') {
        char args[15][MAX_TOKEN_LEN];
        int arg_count = (count >= 2) ? count - 2 : 0;
        const char *method = (count >= 2) ? parts[1] : "";
        for (int a = 0; a < arg_count; a++) {
            strncpy(args[a], parts[a + 2], MAX_TOKEN_LEN - 1);
            args[a][MAX_TOKEN_LEN - 1] = '\0';
        }
        return dense_py_call(rt, parts[0], method, args, arg_count);
    }

    // Check if parts[0] is a variable holding a @py object
    DenseValue var_val = dense_get_var(rt, parts[0]);
    if (var_val.type == VAL_STR && var_val.str_val[0] == '@' && strncmp(var_val.str_val, "@py_", 5) == 0) {
        char args[15][MAX_TOKEN_LEN];
        int arg_count = (count >= 2) ? count - 2 : 0;
        const char *method = (count >= 2) ? parts[1] : "";
        for (int a = 0; a < arg_count; a++) {
            strncpy(args[a], parts[a + 2], MAX_TOKEN_LEN - 1);
            args[a][MAX_TOKEN_LEN - 1] = '\0';
        }
        return dense_py_call(rt, var_val.str_val, method, args, arg_count);
    }

    // Check if parts[0] is a variable holding a DE @panel_ object or starts with @panel_
    const char *target_panel = NULL;
    if (strncmp(parts[0], "@panel_", 7) == 0) {
        target_panel = parts[0];
    } else if (var_val.type == VAL_STR && strncmp(var_val.str_val, "@panel_", 7) == 0) {
        target_panel = var_val.str_val;
    }
    if (target_panel != NULL && count >= 2) {
        int p_idx = atoi(target_panel + 7);
        const char *m_res = dense_symtable_resolve(&rt->symtable, parts[1]);
        if (!m_res) m_res = parts[1];
        if (_stricmp(m_res, "panel_widget") == 0 || strcmp(parts[1], "4b") == 0 || _stricmp(m_res, "widget") == 0) {
            for (int i = 2; i < count; i++) {
                DenseValue w = dense_eval_expr(rt, parts[i]);
                dense_de_panel_add_widget(p_idx, w.str_val);
            }
            return dense_val_int(dense_de_get_state()->panels[p_idx].widget_count);
        }
        if (_stricmp(m_res, "panel_show") == 0 || strcmp(parts[1], "IM") == 0 || _stricmp(m_res, "show") == 0) {
            dense_de_panel_show(p_idx);
            return dense_val_bool(true);
        }
        if (_stricmp(m_res, "panel_style") == 0 || strcmp(parts[1], "oo") == 0 || _stricmp(m_res, "style") == 0) {
            for (int i = 2; i < count; i++) {
                char *eq_s = strchr(parts[i], '=');
                if (eq_s) {
                    *eq_s = '\0';
                    dense_de_panel_set_style(p_idx, parts[i], eq_s + 1);
                    *eq_s = '=';
                }
            }
            return dense_val_bool(true);
        }
    }

    // Check if parts[0] is a variable holding a DE @dock object or starts with @dock
    const char *target_dock = NULL;
    if (strcmp(parts[0], "@dock") == 0) {
        target_dock = parts[0];
    } else if (var_val.type == VAL_STR && strcmp(var_val.str_val, "@dock") == 0) {
        target_dock = var_val.str_val;
    }
    if (target_dock != NULL && count >= 2) {
        const char *m_res = dense_symtable_resolve(&rt->symtable, parts[1]);
        if (!m_res) m_res = parts[1];
        if (_stricmp(m_res, "dock_app") == 0 || strcmp(parts[1], "0c") == 0 || _stricmp(m_res, "app") == 0) {
            for (int i = 2; i < count; i++) {
                DenseValue a = dense_eval_expr(rt, parts[i]);
                dense_de_dock_add_app(a.str_val);
            }
            return dense_val_int(dense_de_get_state()->dock.app_count);
        }
        if (_stricmp(m_res, "dock_show") == 0 || strcmp(parts[1], "7Y") == 0 || _stricmp(m_res, "show") == 0) {
            dense_de_dock_show();
            return dense_val_bool(true);
        }
    }

    // Check if parts[0] is a command code or action name (e.g. J|config.json, Q|dir, 9|file|data, volume|75)
    const char *resolved_cmd = dense_symtable_resolve(&rt->symtable, parts[0]);
    const char *found_code = dense_symtable_find_code(&rt->symtable, parts[0]);
    bool is_command = ((resolved_cmd != NULL || found_code != NULL) && parts[0][0] != '&');

    // Also check if parts[0] contains a condition (e.g. x>10|&h|&l or cfg==&0|K|Using fallback|K|Config loaded)
    bool is_cond = false;
    dense_eval_condition(rt, parts[0], &is_cond);

    if (!is_command && is_cond) {
        // Branching / Ternary / Multiplexer: cond|true_val|false_val
        bool cond_met = dense_eval_condition(rt, parts[0], &is_cond);
        if (count == 3) {
            return cond_met ? dense_eval_atom(rt, parts[1]) : dense_eval_atom(rt, parts[2]);
        }
        // Cascading switch: c1|v1|c2|v2...|fallback
        if (count > 3 && count % 2 == 1) {
            for (int i = 0; i < count - 1; i += 2) {
                bool c_sub = false;
                bool pass = dense_eval_condition(rt, parts[i], &c_sub);
                if (pass) {
                    return dense_eval_atom(rt, parts[i + 1]);
                }
            }
            return dense_eval_atom(rt, parts[count - 1]); // fallback
        }
    }

    if (is_command) {
        char args[15][MAX_TOKEN_LEN];
        int arg_count = count - 1;
        for (int i = 0; i < arg_count; i++) {
            strncpy(args[i], parts[i + 1], MAX_TOKEN_LEN - 1);
            args[i][MAX_TOKEN_LEN - 1] = '\0';
        }
        return dense_call_command(rt, parts[0], args, arg_count);
    }

    // Safe Fallback Execution: action|fallback
    // Try to eval parts[0], if none, return parts[1]
    DenseValue val0 = dense_eval_atom(rt, parts[0]);
    if (val0.type != VAL_NONE) {
        return val0;
    }
    return dense_eval_atom(rt, parts[1]);
}

bool dense_execute_line(DenseRuntime *rt, const char *line) {
    if (!line) return true;
    while (isspace((unsigned char)*line)) line++;
    if (*line == '\0' || (line[0] == '/' && line[1] == '/')) {
        return true; // Skip comment or empty line
    }

    // Check if it starts with an action command (e.g. 9|... or a|...)
    // If it does, do NOT treat inner '=' as a variable assignment!
    char first_token[MAX_TOKEN_LEN] = {0};
    const char *first_pipe = strchr(line, '|');
    if (first_pipe) {
        size_t tlen = first_pipe - line;
        if (tlen >= MAX_TOKEN_LEN) tlen = MAX_TOKEN_LEN - 1;
        strncpy(first_token, line, tlen);
        first_token[tlen] = '\0';
    }

    bool is_command_start = false;
    if (first_token[0] != '\0' && first_token[0] != '&') {
        const char *resolved = dense_symtable_resolve(&rt->symtable, first_token);
        if (resolved != NULL) {
            is_command_start = true;
        }
    }

    // Check assignment: VAR=EXPR (only if line does not begin with a command action)
    char *eq = strchr(line, '=');
    if (!is_command_start && eq && *(eq + 1) != '=' && (eq == line || (*(eq - 1) != '=' && *(eq - 1) != '!' && *(eq - 1) != '<' && *(eq - 1) != '>'))) {
        char varname[MAX_TOKEN_LEN];
        size_t len = eq - line;
        if (len >= MAX_TOKEN_LEN) len = MAX_TOKEN_LEN - 1;
        strncpy(varname, line, len);
        varname[len] = '\0';

        const char *expr = eq + 1;
        DenseValue val = dense_eval_expr(rt, expr);
        if (rt->debug_mode) {
            printf("[DEBUG] Assign var '%s' = type:%d str:'%s' int:%lld\n", varname, val.type, val.str_val, (long long)val.int_val);
        }
        dense_set_var(rt, varname, val);
        return true;
    }

    // Otherwise, it's a statement with pipe multiplexer or command call
    // Check if line is a conditional branch with command statements:
    // e.g.: cfg==&0|K|Using fallback|K|Config loaded
    char parts[16][MAX_TOKEN_LEN];
    int count = split_pipes(line, parts, 16);

    bool is_cond = false;
    bool cond_met = dense_eval_condition(rt, parts[0], &is_cond);
    if (is_cond && count >= 3) {
        // Format: cond | CMD1 | ARG... | CMD2 | ARG...
        // If count == 5: cond | CMD_TRUE | ARG_TRUE | CMD_FALSE | ARG_FALSE
        if (count == 5) {
            if (cond_met) {
                char args[1][MAX_TOKEN_LEN];
                strncpy(args[0], parts[2], MAX_TOKEN_LEN - 1);
                dense_call_command(rt, parts[1], args, 1);
            } else {
                char args[1][MAX_TOKEN_LEN];
                strncpy(args[0], parts[4], MAX_TOKEN_LEN - 1);
                dense_call_command(rt, parts[3], args, 1);
            }
            return true;
        } else if (count == 3) {
            // cond | ACTION_TRUE | ACTION_FALSE
            if (cond_met) {
                dense_eval_expr(rt, parts[1]);
            } else {
                dense_eval_expr(rt, parts[2]);
            }
            return true;
        }
    }

    // Direct command execution: CMD|ARG1|ARG2...
    dense_eval_expr(rt, line);
    return true;
}

bool dense_execute_file(DenseRuntime *rt, const char *filepath) {
    FILE *fp = fopen(filepath, "r");
    if (!fp) {
        fprintf(stderr, "Error: Unable to open source file: %s\n", filepath);
        return false;
    }

    char line[MAX_LINE_LEN];
    bool in_block = false;
    char block_name[64] = {0};
    char block_lines[64][MAX_LINE_LEN];
    int block_line_count = 0;

    while (fgets(line, sizeof(line), fp)) {
        // Strip trailing newline / carriage return
        char *nl = strchr(line, '\r');
        if (nl) *nl = '\0';
        nl = strchr(line, '\n');
        if (nl) *nl = '\0';

        char *trimmed = line;
        while (isspace((unsigned char)*trimmed)) trimmed++;
        if (*trimmed == '\0' || (trimmed[0] == '/' && trimmed[1] == '/')) {
            continue;
        }

        if (!in_block) {
            // Check if line declares a DE block header (e.g. panel/, dock/, wallpaper/, labwc/)
            size_t tlen = strlen(trimmed);
            if (tlen > 1 && trimmed[tlen - 1] == '/') {
                size_t bname_len = tlen - 1;
                if (bname_len >= sizeof(block_name)) bname_len = sizeof(block_name) - 1;
                strncpy(block_name, trimmed, bname_len);
                block_name[bname_len] = '\0';
                char *bend = block_name + strlen(block_name) - 1;
                while (bend > block_name && isspace((unsigned char)*bend)) { *bend = '\0'; bend--; }
                in_block = true;
                block_line_count = 0;
                continue;
            }

            if (!dense_execute_line(rt, trimmed)) {
                fclose(fp);
                return false;
            }
        } else {
            // Inside DE block: check for terminating '\'
            if (strcmp(trimmed, "\\") == 0) {
                in_block = false;
                dense_de_handle_block(rt, block_name, block_lines, block_line_count);
                continue;
            }
            if (block_line_count < 64) {
                strncpy(block_lines[block_line_count++], trimmed, MAX_LINE_LEN - 1);
            }
        }

        if (rt->exit_requested) {
            break;
        }
    }

    fclose(fp);
    return true;
}
