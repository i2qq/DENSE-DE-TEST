#include "dense.h"

static void print_banner(void) {
    printf("Dense Interpreter (dense.exe) v%s [token-quantized zero-whitespace]\n", DENSE_VERSION);
}

static void print_usage(const char *prog) {
    printf("Usage: %s [options] <script.dense>\n\n", prog);
    printf("Options:\n");
    printf("  -p, --proj <path>     Path to project manifest (default: project.denseproj)\n");
    printf("  -t, --table <path>    Path to lookup table (default: table.denselook)\n");
    printf("  -d, --debug           Enable debug output\n");
    printf("  -v, --version         Print version and exit\n");
    printf("  -h, --help            Print this help message\n");
}

int main(int argc, char *argv[]) {
    const char *proj_file = "project.denseproj";
    const char *table_file = "table.denselook";
    const char *source_file = NULL;
    bool debug = false;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--version") == 0) {
            print_banner();
            return 0;
        } else if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_banner();
            print_usage(argv[0]);
            return 0;
        } else if (strcmp(argv[i], "-d") == 0 || strcmp(argv[i], "--debug") == 0) {
            debug = true;
        } else if ((strcmp(argv[i], "-p") == 0 || strcmp(argv[i], "--proj") == 0) && i + 1 < argc) {
            proj_file = argv[++i];
        } else if ((strcmp(argv[i], "-t") == 0 || strcmp(argv[i], "--table") == 0) && i + 1 < argc) {
            table_file = argv[++i];
        } else if (argv[i][0] != '-') {
            source_file = argv[i];
        }
    }

    if (!source_file) {
        print_banner();
        print_usage(argv[0]);
        return 1;
    }

    DenseRuntime rt;
    dense_runtime_init(&rt);
    rt.debug_mode = debug;

    // 1. Mandatory File Check: table.denselook
    if (!dense_file_exists(table_file)) {
        fprintf(stderr, "Fatal Error: Mandatory lookup table '%s' not found on disk!\n", table_file);
        return 1;
    }

    if (!dense_load_denselook(&rt, table_file)) {
        return 1;
    }

    // 2. Mandatory File Check: project.denseproj
    if (!dense_file_exists(proj_file)) {
        fprintf(stderr, "Fatal Error: Mandatory project manifest '%s' not found on disk!\n", proj_file);
        return 1;
    }

    if (!dense_load_project(&rt, proj_file)) {
        return 1;
    }

    if (debug) {
        printf("[DEBUG] Mode: %s\n", rt.symtable.is_custom_mode ? "custom" : "normal");
        printf("[DEBUG] Registered Mappings: %d\n", rt.symtable.mapping_count);
        printf("[DEBUG] Registered Imports: %d\n", rt.project.import_count);
        for (int i = 0; i < rt.project.import_count; i++) {
            printf("[DEBUG]   Import %s -> %s\n", rt.project.imports[i].module, rt.project.imports[i].alias);
        }
    }

    // 3. Execute source file
    if (!dense_execute_file(&rt, source_file)) {
        return 1;
    }

    return rt.exit_code;
}
