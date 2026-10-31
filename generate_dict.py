import random
import string
import itertools

# Set deterministic seed for static random Base64 IDs
random.seed(42891337)

# Specific mandatory seeds from the prompt:
SPEC_EXPLICIT_COMMANDS = {
    "K": "echo",
    "7": "del",
    "x": "killforce",
    "Q": "mkdir",
    "2": "rmdir",
    "m": "copy",
    "B": "move",
    "d": "dir",
    "t": "type",
    "s": "cls",
    "p": "pause",
    "X": "exit",
    "8": "sleep",
    "f": "find",
    "w": "whoami",
    "i": "input",
    "J": "read",
    "9": "write",
    "v": "range",
    "I": "len",
    "w4": "taskkill",
    "kQ": "isinstance",
    "9z": "enumerate",
    "b7": "zip",
    "X1": "sorted",
    "l1": "lambda",
}

SPEC_EXPLICIT_VOCAB = {
    "&T": "true",
    "&F": "false",
    "&0": "none",
    "&s": "self",
    "&c": "cls",
    "&h": "high",
    "&l": "low",
    "&n": "normal",
    "&3M": "int",
    "&9z": "str",
    "&k8": "ValueError",
    "&w1": "FileNotFoundError",
    "&g4": "high",
    "&377": "low",
}

# Base64 charset for zero-punctuation Dense: alphanumeric standard chars
# (A-Z, a-z, 0-9)
B64_CHARS = list(string.ascii_letters + string.digits) # 62 chars

# Create randomized permutations for length 1, 2, 3...
chars_1 = list(B64_CHARS)
random.shuffle(chars_1)

chars_2 = [''.join(p) for p in itertools.product(B64_CHARS, repeat=2)]
random.shuffle(chars_2)

chars_3 = [''.join(p) for p in itertools.product(B64_CHARS, repeat=3)]
random.shuffle(chars_3)

# Build command allocation queue:
used_cmd_codes = set(SPEC_EXPLICIT_COMMANDS.keys())
cmd_queue_1 = [c for c in chars_1 if c not in used_cmd_codes]
cmd_queue_2 = [c for c in chars_2 if c not in used_cmd_codes]
cmd_queue_3 = [c for c in chars_3 if c not in used_cmd_codes]

# Original list of standard actions / commands to map
commands_to_map = [
    # Core OS & File
    "append", "exists", "replace", "upper", "lower", "split", "join",
    "count", "head", "tail", "base64", "sha256", "md5", "zip_file", "unzip",
    "popen", "env_get", "env_set", "cwd", "cd", "time", "date", "rand",
    "math", "net_ping", "fetch", "glob", "lines", "os_name", "assert", "benchmark",
    # Additional Python/Standard builtins
    "filter", "map", "sum", "min", "max", "abs", "round", "floor", "ceil",
    "all", "any", "chr", "ord", "hex", "bin", "oct", "reverse", "format",
    "slice", "index", "find_in", "starts_with", "ends_with", "strip", "lstrip", "rstrip",
    "keys", "values", "items", "get", "pop", "push", "shift", "unshift",
    "read_lines", "write_lines", "stat", "chmod", "curl", "download", "upload",
    "sleep_ms", "uptime", "pid", "memory_usage", "cpu_count", "listdir",
    
    # === Wayland, Labwc, and Desktop Environment (DE) Building Blocks ===
    "wayland_init", "wayland_poll",
    "panel", "panel_create", "panel_widget", "panel_style", "panel_show",
    "dock", "dock_create", "dock_app", "dock_show",
    "wallpaper", "wallpaper_set",
    "notify", "notify_send", "osd", "osd_show",
    "toplevel", "toplevel_list", "toplevel_focus", "toplevel_close",
    "toplevel_maximize", "toplevel_minimize", "toplevel_fullscreen",
    "workspace", "workspace_switch", "workspace_list",
    "labwc", "labwc_reload", "labwc_command", "labwc_config",
    "session", "session_lock", "session_logout", "session_reboot", "session_poweroff", "session_suspend",
    "launcher", "app_list", "app_launch",
    "sysstat", "volume", "brightness", "keybind", "layer_shell",
    "de_build", "de_start", "de_export", "de_status",

    # === Advanced QML/Python/C Replacement Blocks & Controls ===
    "box", "box_create", "box_child",
    "widget_create", "widget_set", "widget_style",
    "window_create", "window_show", "window_close",
    "menu_create", "menu_item", "menu_show",
    "animation_create", "animation_start",
    "dbus_call", "dbus_get", "dbus_set",
    "audio_volume", "audio_mute", "audio_sink",
    "net_scan", "net_status", "net_connect",
    "battery_info", "power_governor",
    "bluetooth_scan", "bluetooth_connect", "bluetooth_status",
    "media_play", "media_pause", "media_next", "media_prev", "media_info",
    "layershell_surface", "layershell_anchor", "layershell_margin",
    "windowrule_add", "windowrule_clear",
    "input_pointer", "input_keyboard", "input_touchpad",
    "monitor_list", "monitor_config",
    "event_listen", "event_emit",
    "lockscreen_show", "controlcenter_show",

    # === Even More QML/Python/C Replacement Controls & Blocks ===
    "style_set", "style_get",
    "animation_add",
    "state_set",
    "repeater_create",
    "service_start", "service_stop", "service_restart", "service_status",
    "timer_start", "timer_stop",
    "bluetooth_pair", "bluetooth_unpair",
    "cpu_temp", "mem_used", "disk_free",
    "gesture_bind", "gesture_trigger",
    "compositor_set", "compositor_get",
    "layershell_add",

    # === Ubuntu Deployment & Desktop Environment Wiping Controls ===
    "wipe_desktop", "wipe_gnome", "desktop_wipe",
    "de_deploy", "session_register",
    "pkg_install", "pkg_purge", "pkg_autoremove",
    "dense_bar", "sys_reboot", "sys_poweroff"
]

command_mappings = dict(SPEC_EXPLICIT_COMMANDS)
for target in commands_to_map:
    if target in command_mappings.values():
        continue
    # Exhaust 1-char first
    if cmd_queue_1:
        code = cmd_queue_1.pop(0)
    elif cmd_queue_2:
        code = cmd_queue_2.pop(0)
    else:
        code = cmd_queue_3.pop(0)
    used_cmd_codes.add(code)
    command_mappings[code] = target

# Now do the same for Vocabulary / Types / Constants (& prefix)
used_vocab_codes = set(k[1:] for k in SPEC_EXPLICIT_VOCAB.keys())
v_chars_1 = list(B64_CHARS)
random.shuffle(v_chars_1)
v_chars_2 = [''.join(p) for p in itertools.product(B64_CHARS, repeat=2)]
random.shuffle(v_chars_2)
v_chars_3 = [''.join(p) for p in itertools.product(B64_CHARS, repeat=3)]
random.shuffle(v_chars_3)

vocab_queue_1 = [c for c in v_chars_1 if c not in used_vocab_codes]
vocab_queue_2 = [c for c in v_chars_2 if c not in used_vocab_codes]
vocab_queue_3 = [c for c in v_chars_3 if c not in used_vocab_codes]

vocab_to_map = [
    # Types
    "bool", "float", "list", "dict", "set", "bytes", "tuple", "complex",
    # Status / States
    "ok", "err", "warn", "info", "debug", "unknown", "pass", "fail", "yes", "no", "all", "zero",
    # Built-in Exceptions
    "Exception", "TypeError", "IndexError", "KeyError", "AttributeError", "ZeroDivisionError",
    "IOError", "PermissionError", "TimeoutError", "RuntimeError", "NotImplementedError",
    # Constants / Streams
    "stdout", "stderr", "stdin", "devnull", "utf8", "ascii", "binary", "raw",
    # Common words
    "success", "failure", "pending", "active", "idle", "busy", "timeout", "abort",
    "start", "stop", "pause", "resume", "enable", "disable", "true_val", "false_val",

    # === Wayland, Labwc & Desktop Environment (DE) Vocabulary & States ===
    "top", "bot", "left", "right", "center",
    "toplayer", "botlayer", "overlay", "bglayer",
    "exclusive", "autohide",
    "dark", "light", "trans", "blur",
    "fill", "fit", "tile", "stretch", "span",
    "tasks", "pager", "tray", "clock", "quicksettings", "spacer",
    "minimized", "maximized", "fullscreen", "floating", "tiled",
    "urgent", "low_urg", "norm_urg",
    "lock", "logout", "reboot", "poweroff", "suspend",

    # === QML/Python/C Advanced Vocabulary ===
    "row", "col", "grid", "stack",
    "label", "button", "entry", "slider", "progress", "icon", "toggle", "image", "separator",
    "system_bus", "session_bus",
    "performance", "powersave", "balanced",
    "kb_none", "kb_exclusive", "kb_ondemand",
    "anchor_top", "anchor_bot", "anchor_left", "anchor_right", "anchor_all",
    "align_start", "align_center", "align_end", "align_stretch",
    "on_click", "on_change", "on_hover", "on_focus", "on_window_open", "on_window_close",
    "connected", "disconnected", "connecting",
    "muted", "unmuted",
    "playing", "paused", "stopped",

    # === Extended QML/Python/C Replacement Vocab & States ===
    "theme", "animation", "duration", "easing", "linear", "ease_in", "ease_out", "ease_in_out", "spring", "bounce",
    "hovered", "pressed", "focused", "disabled", "delegate",
    "daemon", "service", "timer", "interval", "repeat", "running", "dead",
    "celsius", "fahrenheit", "paired", "unpaired",
    "gesture", "swipe_up", "swipe_down", "swipe_left", "swipe_right", "pinch_in", "pinch_out",
    "corner_radius", "drop_shadow", "tearing", "vsync", "server_side", "client_side",

    # === Ubuntu Deployment & Wiping Vocabulary ===
    "ubuntu", "debian", "gnome", "gdm3", "purged", "installed",
    "active_session", "clean_sessions", "keep_essentials"
]

vocab_mappings = dict(SPEC_EXPLICIT_VOCAB)
for target in vocab_to_map:
    if target in vocab_mappings.values():
        continue
    if vocab_queue_1:
        code = vocab_queue_1.pop(0)
    elif vocab_queue_2:
        code = vocab_queue_2.pop(0)
    else:
        code = vocab_queue_3.pop(0)
    used_vocab_codes.add(code)
    vocab_mappings["&" + code] = target

# Generate C file
c_lines = []
c_lines.append('#include "dense.h"')
c_lines.append('')
c_lines.append('// Hardcoded Master Dictionary of Static Random Base64 IDs')
c_lines.append('// Order of exhaustion: 1-char (64-entry pool), rolling to 2-char (4096-entry pool), then 3-char.')
c_lines.append('// All IDs are static random Base64 fixed in the binary.')
c_lines.append('')
c_lines.append('typedef struct {')
c_lines.append('    const char *code;')
c_lines.append('    const char *target;')
c_lines.append('} MasterEntry;')
c_lines.append('')
c_lines.append('static const MasterEntry MASTER_DICTIONARY[] = {')
c_lines.append('    // === Unprefixed Commands & Actions (Static Random Base64) ===')
for code, target in command_mappings.items():
    c_lines.append(f'    {{"{code}", "{target}"}},')

c_lines.append('')
c_lines.append('    // === Prefixed Vocabulary, States, Constants & Types (& Prefix) ===')
for code, target in vocab_mappings.items():
    c_lines.append(f'    {{"{code}", "{target}"}},')

c_lines.append('    {NULL, NULL}')
c_lines.append('};')
c_lines.append('')
c_lines.append('''void dense_symtable_init(DenseSymbolTable *tbl) {
    tbl->is_custom_mode = false;
    tbl->mapping_count = 0;
}

void dense_symtable_load_master(DenseSymbolTable *tbl) {
    tbl->mapping_count = 0;
    for (int i = 0; MASTER_DICTIONARY[i].code != NULL; i++) {
        dense_symtable_add(tbl, MASTER_DICTIONARY[i].code, MASTER_DICTIONARY[i].target);
    }
}

bool dense_symtable_add(DenseSymbolTable *tbl, const char *code, const char *target) {
    if (!code || !target || tbl->mapping_count >= MAX_TABLE_ENTRIES) return false;
    
    // If code already exists, overwrite
    for (int i = 0; i < tbl->mapping_count; i++) {
        if (strcmp(tbl->mappings[i].code, code) == 0) {
            strncpy(tbl->mappings[i].target, target, sizeof(tbl->mappings[i].target) - 1);
            tbl->mappings[i].target[sizeof(tbl->mappings[i].target) - 1] = '\\0';
            return true;
        }
    }

    DenseMapping *m = &tbl->mappings[tbl->mapping_count++];
    strncpy(m->code, code, sizeof(m->code) - 1);
    m->code[sizeof(m->code) - 1] = '\\0';
    strncpy(m->target, target, sizeof(m->target) - 1);
    m->target[sizeof(m->target) - 1] = '\\0';
    m->is_vocab = (code[0] == '&');
    return true;
}

const char* dense_symtable_resolve(DenseSymbolTable *tbl, const char *code) {
    if (!code) return NULL;
    for (int i = 0; tbl && i < tbl->mapping_count; i++) {
        if (strcmp(tbl->mappings[i].code, code) == 0) {
            return tbl->mappings[i].target;
        }
    }
    return NULL;
}

const char* dense_symtable_find_code(DenseSymbolTable *tbl, const char *target) {
    if (!target) return NULL;
    for (int i = 0; tbl && i < tbl->mapping_count; i++) {
        if (_stricmp(tbl->mappings[i].target, target) == 0) {
            return tbl->mappings[i].code;
        }
    }
    return NULL;
}
''')

with open("src/dense_dict.c", "w", encoding="utf-8") as f:
    f.write('\n'.join(c_lines))

print(f"Generated src/dense_dict.c successfully with {len(command_mappings)} commands and {len(vocab_mappings)} vocab entries!")
