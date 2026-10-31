#include "dense_de.h"
#include <ctype.h>
#include <sys/stat.h>

#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define mkdir_compat(dir) _mkdir(dir)
#else
#include <unistd.h>
#define mkdir_compat(dir) mkdir(dir, 0755)
#endif

static DESystemState G_DE_STATE;
static bool G_DE_INITIALIZED = false;

static char* trim_str(char *s) {
    if (!s) return NULL;
    while (isspace((unsigned char)*s)) s++;
    if (*s == '\0') return s;
    char *end = s + strlen(s) - 1;
    while (end > s && isspace((unsigned char)*end)) end--;
    end[1] = '\0';
    return s;
}

void dense_de_init(DenseRuntime *rt) {
    (void)rt;
    if (!G_DE_INITIALIZED) {
        memset(&G_DE_STATE, 0, sizeof(DESystemState));
        G_DE_STATE.labwc.workspaces = 4;
        G_DE_STATE.labwc.current_workspace = 1;
        G_DE_STATE.labwc.snap_to_edge = true;
        strncpy(G_DE_STATE.labwc.theme, "Openbox", sizeof(G_DE_STATE.labwc.theme) - 1);

        // Prepopulate default toplevel windows
        G_DE_STATE.toplevel_count = 3;
        strncpy(G_DE_STATE.toplevels[0].title, "Terminal - Dense Shell", 127);
        strncpy(G_DE_STATE.toplevels[0].app_id, "alacritty", 63);
        G_DE_STATE.toplevels[0].workspace = 1;
        G_DE_STATE.toplevels[0].state = 4; // active

        strncpy(G_DE_STATE.toplevels[1].title, "Web Browser - Wayland", 127);
        strncpy(G_DE_STATE.toplevels[1].app_id, "firefox", 63);
        G_DE_STATE.toplevels[1].workspace = 1;
        G_DE_STATE.toplevels[1].state = 0;

        strncpy(G_DE_STATE.toplevels[2].title, "File Manager", 127);
        strncpy(G_DE_STATE.toplevels[2].app_id, "thunar", 63);
        G_DE_STATE.toplevels[2].workspace = 2;
        G_DE_STATE.toplevels[2].state = 1; // minimized

        // Prepopulate System State (Audio, Network, Battery, Media, Input, Monitors)
        G_DE_STATE.audio.volume = 65;
        G_DE_STATE.audio.is_muted = false;
        strcpy(G_DE_STATE.audio.default_sink, "alsa_output.pci-0000_00_1f.3.analog-stereo");

        strcpy(G_DE_STATE.network.connected_ssid, "Victorious-WiFi-6E");
        strcpy(G_DE_STATE.network.ip_address, "192.168.1.100");
        G_DE_STATE.network.is_connected = true;
        G_DE_STATE.network.signal_strength = 92;

        G_DE_STATE.battery.percentage = 88;
        G_DE_STATE.battery.is_charging = true;
        G_DE_STATE.battery.time_to_empty = 240; // minutes
        strcpy(G_DE_STATE.battery.governor, "balanced");

        strcpy(G_DE_STATE.media.track_title, "Synthwave Dreams - 2026");
        strcpy(G_DE_STATE.media.artist, "Dense Collective");
        strcpy(G_DE_STATE.media.playback_status, "Playing");

        strcpy(G_DE_STATE.input.tap_to_click, "enabled");
        strcpy(G_DE_STATE.input.natural_scroll, "enabled");
        G_DE_STATE.input.accel_speed = 0;
        strcpy(G_DE_STATE.input.kb_layout, "us");
        G_DE_STATE.input.kb_repeat_rate = 25;

        G_DE_STATE.monitor_count = 2;
        strcpy(G_DE_STATE.monitors[0].name, "eDP-1");
        G_DE_STATE.monitors[0].width = 2560;
        G_DE_STATE.monitors[0].height = 1600;
        G_DE_STATE.monitors[0].refresh_rate = 120;
        G_DE_STATE.monitors[0].scale = 1;

        strcpy(G_DE_STATE.monitors[1].name, "HDMI-A-1");
        G_DE_STATE.monitors[1].width = 3840;
        G_DE_STATE.monitors[1].height = 2160;
        G_DE_STATE.monitors[1].refresh_rate = 144;
        G_DE_STATE.monitors[1].scale = 1;

        // Prepopulate Theme & Style (Catppuccin Mocha style default)
        strcpy(G_DE_STATE.style.bg_color, "#1e1e2e");
        strcpy(G_DE_STATE.style.fg_color, "#cdd6f4");
        strcpy(G_DE_STATE.style.accent_color, "#89b4fa");
        strcpy(G_DE_STATE.style.border_color, "#313244");
        G_DE_STATE.style.border_radius = 10;
        strcpy(G_DE_STATE.style.font_family, "JetBrains Mono");
        G_DE_STATE.style.font_size = 11;
        G_DE_STATE.style.drop_shadow = true;
        G_DE_STATE.style.background_blur = true;

        // Prepopulate System Services
        G_DE_STATE.service_count = 4;
        strcpy(G_DE_STATE.services[0].name, "pipewire");
        strcpy(G_DE_STATE.services[0].status, "running");
        G_DE_STATE.services[0].enabled = true;

        strcpy(G_DE_STATE.services[1].name, "wireplumber");
        strcpy(G_DE_STATE.services[1].status, "running");
        G_DE_STATE.services[1].enabled = true;

        strcpy(G_DE_STATE.services[2].name, "NetworkManager");
        strcpy(G_DE_STATE.services[2].status, "running");
        G_DE_STATE.services[2].enabled = true;

        strcpy(G_DE_STATE.services[3].name, "bluetooth");
        strcpy(G_DE_STATE.services[3].status, "running");
        G_DE_STATE.services[3].enabled = true;

        // Prepopulate Compositor Settings
        G_DE_STATE.compositor.corner_radius = 10;
        G_DE_STATE.compositor.drop_shadows = true;
        G_DE_STATE.compositor.animations = true;
        strcpy(G_DE_STATE.compositor.decorations, "server");
        G_DE_STATE.compositor.vsync = true;
        G_DE_STATE.compositor.tearing = false;

        // Prepopulate Gestures
        G_DE_STATE.gesture_count = 3;
        strcpy(G_DE_STATE.gestures[0].trigger, "swipe3_up");
        strcpy(G_DE_STATE.gestures[0].action, "workspace_switch|next");
        strcpy(G_DE_STATE.gestures[1].trigger, "swipe3_down");
        strcpy(G_DE_STATE.gestures[1].action, "workspace_switch|prev");
        strcpy(G_DE_STATE.gestures[2].trigger, "pinch_in");
        strcpy(G_DE_STATE.gestures[2].action, "overview_toggle");

        // Prepopulate Ubuntu Session & Deployment & Wiping configs
        strcpy(G_DE_STATE.session.name, "Dense Desktop");
        strcpy(G_DE_STATE.session.comment, "Dense-Orchestrated Modern Wayland Desktop Environment");
        strcpy(G_DE_STATE.session.compositor, "labwc");
        strcpy(G_DE_STATE.session.exec_path, "/usr/local/bin/dense-session");
        strcpy(G_DE_STATE.session.desktop_names, "Dense;labwc");

        G_DE_STATE.wipe.wipe_gnome = true;
        G_DE_STATE.wipe.wipe_kde = true;
        G_DE_STATE.wipe.wipe_xfce = true;
        G_DE_STATE.wipe.wipe_cinnamon = true;
        G_DE_STATE.wipe.wipe_mate = true;
        G_DE_STATE.wipe.wipe_lxde = true;
        G_DE_STATE.wipe.purge_configs = true;
        G_DE_STATE.wipe.clean_sessions = true;
        G_DE_STATE.wipe.dry_run = false;

        strcpy(G_DE_STATE.deploy.distro, "ubuntu");
        strcpy(G_DE_STATE.deploy.target_dir, "/etc/dense");
        strcpy(G_DE_STATE.deploy.session_file, "/usr/share/wayland-sessions/dense.desktop");
        strcpy(G_DE_STATE.deploy.launcher_file, "/usr/local/bin/dense-session");
        G_DE_STATE.deploy.autologin = true;
        G_DE_STATE.deploy.auto_wipe = true;

        G_DE_INITIALIZED = true;
    }
}

DESystemState* dense_de_get_state(void) {
    if (!G_DE_INITIALIZED) {
        dense_de_init(NULL);
    }
    return &G_DE_STATE;
}

// -------------------------------------------------------------
// Panel operations
// -------------------------------------------------------------
int dense_de_panel_create(DEPosition pos, int height, DETheme theme) {
    DESystemState *st = dense_de_get_state();
    if (st->panel_count >= MAX_PANELS) return -1;

    int idx = st->panel_count++;
    DEPanel *p = &st->panels[idx];
    memset(p, 0, sizeof(DEPanel));
    snprintf(p->id, sizeof(p->id), "@panel_%d", idx);
    p->pos = pos;
    p->height = (height > 0) ? height : 36;
    p->theme = theme;
    p->layer = DE_LAYER_TOP;
    p->exclusive_zone = true;
    p->is_active = true;

    if (theme == DE_THEME_DARK) {
        strcpy(p->bg_color, "#181825");
        strcpy(p->fg_color, "#cdd6f4");
    } else if (theme == DE_THEME_LIGHT) {
        strcpy(p->bg_color, "#eff1f5");
        strcpy(p->fg_color, "#4c4f69");
    } else {
        strcpy(p->bg_color, "rgba(24,24,37,0.85)");
        strcpy(p->fg_color, "#ffffff");
    }
    return idx;
}

bool dense_de_panel_add_widget(int panel_idx, const char *widget_name) {
    DESystemState *st = dense_de_get_state();
    if (panel_idx < 0 || panel_idx >= st->panel_count) return false;
    DEPanel *p = &st->panels[panel_idx];
    if (p->widget_count >= MAX_PANEL_WIDGETS) return false;

    strncpy(p->widgets[p->widget_count++], widget_name, 63);
    return true;
}

bool dense_de_panel_set_style(int panel_idx, const char *key, const char *val) {
    DESystemState *st = dense_de_get_state();
    if (panel_idx < 0 || panel_idx >= st->panel_count) return false;
    DEPanel *p = &st->panels[panel_idx];

    if (_stricmp(key, "height") == 0) p->height = atoi(val);
    else if (_stricmp(key, "width") == 0) p->width = atoi(val);
    else if (_stricmp(key, "bg") == 0 || _stricmp(key, "bg_color") == 0) strncpy(p->bg_color, val, 31);
    else if (_stricmp(key, "fg") == 0 || _stricmp(key, "fg_color") == 0) strncpy(p->fg_color, val, 31);
    else if (_stricmp(key, "radius") == 0) p->border_radius = atoi(val);
    else if (_stricmp(key, "exclusive") == 0) {
        p->exclusive_zone = (_stricmp(val, "true") == 0 || strcmp(val, "&T") == 0 || strcmp(val, "1") == 0);
    }
    return true;
}

bool dense_de_panel_show(int panel_idx) {
    DESystemState *st = dense_de_get_state();
    if (panel_idx < 0 || panel_idx >= st->panel_count) return false;
    st->panels[panel_idx].is_active = true;
    printf("[Dense DE] Panel %s activated (Pos:%d Height:%d Widgets:%d)\n",
           st->panels[panel_idx].id, st->panels[panel_idx].pos,
           st->panels[panel_idx].height, st->panels[panel_idx].widget_count);
    return true;
}

// -------------------------------------------------------------
// Dock operations
// -------------------------------------------------------------
bool dense_de_dock_create(DEPosition pos, int icon_size, bool autohide) {
    DESystemState *st = dense_de_get_state();
    st->dock.pos = pos;
    st->dock.icon_size = (icon_size > 0) ? icon_size : 48;
    st->dock.autohide = autohide;
    st->dock.app_count = 0;
    st->dock.is_active = true;
    return true;
}

bool dense_de_dock_add_app(const char *app_id) {
    DESystemState *st = dense_de_get_state();
    if (st->dock.app_count >= MAX_DOCK_APPS) return false;
    strncpy(st->dock.pinned_apps[st->dock.app_count++], app_id, 63);
    return true;
}

bool dense_de_dock_show(void) {
    DESystemState *st = dense_de_get_state();
    st->dock.is_active = true;
    printf("[Dense DE] Dock activated (Size:%d Autohide:%s Pinned:%d)\n",
           st->dock.icon_size, st->dock.autohide ? "true" : "false", st->dock.app_count);
    return true;
}

// -------------------------------------------------------------
// Wallpaper, Notifications, OSD
// -------------------------------------------------------------
bool dense_de_wallpaper_set(const char *path, DEWallpaperMode mode) {
    DESystemState *st = dense_de_get_state();
    strncpy(st->wallpaper.path, path, sizeof(st->wallpaper.path) - 1);
    st->wallpaper.mode = mode;
    st->wallpaper.is_set = true;
    printf("[Dense DE] Wallpaper configured -> %s (Mode:%d)\n", path, mode);
    return true;
}

bool dense_de_notify(const char *title, const char *body, int urgency) {
    DESystemState *st = dense_de_get_state();
    strncpy(st->last_notification.title, title, sizeof(st->last_notification.title) - 1);
    strncpy(st->last_notification.body, body, sizeof(st->last_notification.body) - 1);
    st->last_notification.urgency = urgency;
    st->last_notification.timeout_ms = 4000;
    printf("[Dense Notification] [%s] %s: %s\n",
           urgency == 2 ? "URGENT" : (urgency == 0 ? "LOW" : "NORMAL"),
           title, body);
    return true;
}

bool dense_de_osd_show(const char *type, int value) {
    DESystemState *st = dense_de_get_state();
    strncpy(st->osd.type, type, sizeof(st->osd.type) - 1);
    st->osd.value = (value < 0) ? 0 : (value > 100 ? 100 : value);
    st->osd.visible = true;

    if (_stricmp(type, "volume") == 0) st->audio.volume = st->osd.value;
    else if (_stricmp(type, "brightness") == 0) st->osd.value = value;

    printf("[Dense OSD] %s: %d%%\n", type, st->osd.value);
    return true;
}

// -------------------------------------------------------------
// Window Management / Toplevels
// -------------------------------------------------------------
int dense_de_toplevel_list(char out_buf[][128], int max_out) {
    DESystemState *st = dense_de_get_state();
    int count = (st->toplevel_count < max_out) ? st->toplevel_count : max_out;
    for (int i = 0; i < count; i++) {
        snprintf(out_buf[i], 128, "%s|%s|ws:%d|state:%d",
                 st->toplevels[i].app_id, st->toplevels[i].title,
                 st->toplevels[i].workspace, st->toplevels[i].state);
    }
    return count;
}

bool dense_de_toplevel_focus(const char *target) {
    DESystemState *st = dense_de_get_state();
    for (int i = 0; i < st->toplevel_count; i++) {
        if (_stricmp(st->toplevels[i].app_id, target) == 0 || strstr(st->toplevels[i].title, target) != NULL) {
            for (int j = 0; j < st->toplevel_count; j++) {
                if (st->toplevels[j].state == 4) st->toplevels[j].state = 0;
            }
            st->toplevels[i].state = 4;
            printf("[Dense WM] Focused window: %s\n", st->toplevels[i].title);
            return true;
        }
    }
    return false;
}

bool dense_de_toplevel_close(const char *target) {
    DESystemState *st = dense_de_get_state();
    for (int i = 0; i < st->toplevel_count; i++) {
        if ((_stricmp(target, "active") == 0 && st->toplevels[i].state == 4) ||
            _stricmp(st->toplevels[i].app_id, target) == 0) {
            printf("[Dense WM] Closed window: %s\n", st->toplevels[i].title);
            for (int k = i; k < st->toplevel_count - 1; k++) {
                st->toplevels[k] = st->toplevels[k + 1];
            }
            st->toplevel_count--;
            return true;
        }
    }
    return false;
}

bool dense_de_toplevel_maximize(const char *target) {
    DESystemState *st = dense_de_get_state();
    for (int i = 0; i < st->toplevel_count; i++) {
        if (_stricmp(target, "active") == 0 || _stricmp(st->toplevels[i].app_id, target) == 0) {
            st->toplevels[i].state = (st->toplevels[i].state == 2) ? 0 : 2;
            printf("[Dense WM] Toggled maximize: %s -> state %d\n", st->toplevels[i].title, st->toplevels[i].state);
            return true;
        }
    }
    return false;
}

bool dense_de_toplevel_minimize(const char *target) {
    DESystemState *st = dense_de_get_state();
    for (int i = 0; i < st->toplevel_count; i++) {
        if (_stricmp(target, "active") == 0 || _stricmp(st->toplevels[i].app_id, target) == 0) {
            st->toplevels[i].state = 1;
            printf("[Dense WM] Minimized: %s\n", st->toplevels[i].title);
            return true;
        }
    }
    return false;
}

bool dense_de_toplevel_fullscreen(const char *target) {
    DESystemState *st = dense_de_get_state();
    for (int i = 0; i < st->toplevel_count; i++) {
        if (_stricmp(target, "active") == 0 || _stricmp(st->toplevels[i].app_id, target) == 0) {
            st->toplevels[i].state = (st->toplevels[i].state == 3) ? 0 : 3;
            printf("[Dense WM] Toggled fullscreen: %s -> state %d\n", st->toplevels[i].title, st->toplevels[i].state);
            return true;
        }
    }
    return false;
}

bool dense_de_workspace_switch(int ws_idx) {
    DESystemState *st = dense_de_get_state();
    if (ws_idx >= 1 && ws_idx <= st->labwc.workspaces) {
        st->labwc.current_workspace = ws_idx;
        printf("[Dense Workspace] Switched to workspace %d\n", ws_idx);
        return true;
    }
    return false;
}

int dense_de_workspace_get(void) {
    return dense_de_get_state()->labwc.current_workspace;
}

bool dense_de_labwc_reload(void) {
    printf("[Dense Labwc] Compositor reload signaled (reconfigure)\n");
    return true;
}

bool dense_de_labwc_command(const char *cmd) {
    printf("[Dense Labwc IPC] Executed command: %s\n", cmd);
    return true;
}

bool dense_de_labwc_add_keybind(const char *keychord, const char *action) {
    DESystemState *st = dense_de_get_state();
    if (st->labwc.keybind_count >= MAX_KEYBINDS) return false;
    strncpy(st->labwc.keybinds[st->labwc.keybind_count].key, keychord, 63);
    strncpy(st->labwc.keybinds[st->labwc.keybind_count].action, action, 127);
    st->labwc.keybind_count++;
    return true;
}

bool dense_de_labwc_add_rule(const char *app_id, const char *props) {
    DESystemState *st = dense_de_get_state();
    if (st->labwc.rule_count >= MAX_WINDOW_RULES) return false;
    DEWindowRule *r = &st->labwc.rules[st->labwc.rule_count++];
    strncpy(r->app_id, app_id, 63);
    if (strstr(props, "floating")) r->floating = true;
    if (strstr(props, "maximized")) r->maximized = true;
    printf("[Dense WindowRule] Registered rule for '%s' -> %s\n", app_id, props);
    return true;
}

// -------------------------------------------------------------
// QML-Replacement UI Building Blocks (Boxes, Widgets, Windows, Menus)
// -------------------------------------------------------------
int dense_de_box_create(DELayoutType type, int spacing, int padding) {
    DESystemState *st = dense_de_get_state();
    if (st->box_count >= MAX_BOXES) return -1;
    int idx = st->box_count++;
    DEBox *b = &st->boxes[idx];
    memset(b, 0, sizeof(DEBox));
    snprintf(b->id, sizeof(b->id), "@box_%d", idx);
    b->type = type;
    b->spacing = spacing;
    b->padding = padding;
    printf("[Dense UI] Created Box %s (Type:%d Spacing:%d Padding:%d)\n", b->id, type, spacing, padding);
    return idx;
}

bool dense_de_box_add_child(int box_idx, const char *child_id) {
    DESystemState *st = dense_de_get_state();
    if (box_idx < 0 || box_idx >= st->box_count) return false;
    DEBox *b = &st->boxes[box_idx];
    if (b->child_count >= MAX_BOX_CHILDREN) return false;
    strncpy(b->children[b->child_count++], child_id, 31);
    printf("[Dense UI] Box %s added child: %s\n", b->id, child_id);
    return true;
}

int dense_de_widget_create(DEElementType type, const char *text, const char *icon) {
    DESystemState *st = dense_de_get_state();
    if (st->element_count >= MAX_WIDGET_ELEMENTS) return -1;
    int idx = st->element_count++;
    DEWidgetElement *e = &st->elements[idx];
    memset(e, 0, sizeof(DEWidgetElement));
    snprintf(e->id, sizeof(e->id), "@elem_%d", idx);
    e->type = type;
    if (text) strncpy(e->text, text, 127);
    if (icon) strncpy(e->icon, icon, 63);
    strcpy(e->bg_color, "transparent");
    strcpy(e->fg_color, "#ffffff");
    printf("[Dense UI] Created Widget %s (Type:%d Text:'%s')\n", e->id, type, e->text);
    return idx;
}

bool dense_de_widget_set(int widget_idx, const char *key, const char *val) {
    DESystemState *st = dense_de_get_state();
    if (widget_idx < 0 || widget_idx >= st->element_count) return false;
    DEWidgetElement *e = &st->elements[widget_idx];

    if (_stricmp(key, "text") == 0) strncpy(e->text, val, 127);
    else if (_stricmp(key, "icon") == 0) strncpy(e->icon, val, 63);
    else if (_stricmp(key, "value") == 0) e->value = atoi(val);
    else if (_stricmp(key, "on_click") == 0) strncpy(e->on_click, val, 127);
    else if (_stricmp(key, "on_change") == 0) strncpy(e->on_change, val, 127);
    else if (_stricmp(key, "bg") == 0) strncpy(e->bg_color, val, 31);
    else if (_stricmp(key, "fg") == 0) strncpy(e->fg_color, val, 31);
    else if (_stricmp(key, "radius") == 0) e->border_radius = atoi(val);
    return true;
}

int dense_de_window_create(const char *title, int width, int height, DELayer layer) {
    DESystemState *st = dense_de_get_state();
    if (st->window_count >= MAX_WINDOWS) return -1;
    int idx = st->window_count++;
    DEWindow *w = &st->windows[idx];
    memset(w, 0, sizeof(DEWindow));
    snprintf(w->id, sizeof(w->id), "@win_%d", idx);
    strncpy(w->title, title, 127);
    w->width = width;
    w->height = height;
    w->layer = layer;
    w->is_visible = true;
    printf("[Dense UI] Created Surface Window %s ('%s' %dx%d Layer:%d)\n", w->id, title, width, height, layer);
    return idx;
}

bool dense_de_window_show(int win_idx) {
    DESystemState *st = dense_de_get_state();
    if (win_idx < 0 || win_idx >= st->window_count) return false;
    st->windows[win_idx].is_visible = true;
    printf("[Dense UI] Surface Window %s presented on Wayland display\n", st->windows[win_idx].id);
    return true;
}

int dense_de_menu_create(const char *menu_id) {
    DESystemState *st = dense_de_get_state();
    if (st->menu_count >= 8) return -1;
    int idx = st->menu_count++;
    DEMenu *m = &st->menus[idx];
    memset(m, 0, sizeof(DEMenu));
    snprintf(m->id, sizeof(m->id), "%s", menu_id ? menu_id : "@menu_0");
    printf("[Dense UI] Created Menu %s\n", m->id);
    return idx;
}

bool dense_de_menu_add_item(int menu_idx, const char *label, const char *cmd) {
    DESystemState *st = dense_de_get_state();
    if (menu_idx < 0 || menu_idx >= st->menu_count) return false;
    DEMenu *m = &st->menus[menu_idx];
    if (m->item_count >= MAX_MENU_ITEMS) return false;
    strncpy(m->items[m->item_count][0], label, 63);
    strncpy(m->items[m->item_count][1], cmd, 63);
    m->item_count++;
    printf("[Dense UI] Menu %s added item: %s -> %s\n", m->id, label, cmd);
    return true;
}

// -------------------------------------------------------------
// Python-Replacement System Services & D-Bus
// -------------------------------------------------------------
DenseValue dense_de_dbus_call(const char *bus, const char *dest, const char *path, const char *iface, const char *method, char args[][MAX_TOKEN_LEN], int arg_count) {
    (void)bus; (void)path; (void)iface; (void)args; (void)arg_count;
    printf("[Dense D-Bus] Called %s %s.%s()\n", dest, iface ? iface : "", method);
    return dense_val_str("dbus_ok");
}

DenseValue dense_de_dbus_get(const char *bus, const char *dest, const char *path, const char *iface, const char *prop) {
    (void)bus; (void)dest; (void)path; (void)iface;
    printf("[Dense D-Bus] Query property: %s\n", prop);
    return dense_val_str("property_value");
}

bool dense_de_audio_set_volume(int vol) {
    DESystemState *st = dense_de_get_state();
    st->audio.volume = (vol < 0) ? 0 : (vol > 100 ? 100 : vol);
    st->audio.is_muted = false;
    printf("[Dense Audio / PipeWire] Volume set to: %d%%\n", st->audio.volume);
    return true;
}

bool dense_de_audio_set_mute(bool mute) {
    DESystemState *st = dense_de_get_state();
    st->audio.is_muted = mute;
    printf("[Dense Audio / PipeWire] Mute: %s\n", mute ? "true" : "false");
    return true;
}

bool dense_de_net_connect(const char *ssid) {
    DESystemState *st = dense_de_get_state();
    strncpy(st->network.connected_ssid, ssid, 63);
    st->network.is_connected = true;
    printf("[Dense NetworkManager] Connected to SSID: %s\n", ssid);
    return true;
}

bool dense_de_media_command(const char *action) {
    DESystemState *st = dense_de_get_state();
    if (_stricmp(action, "play") == 0) strcpy(st->media.playback_status, "Playing");
    else if (_stricmp(action, "pause") == 0) strcpy(st->media.playback_status, "Paused");
    else if (_stricmp(action, "next") == 0) printf("[Dense MPRIS2] Track Next\n");
    else if (_stricmp(action, "prev") == 0) printf("[Dense MPRIS2] Track Prev\n");
    printf("[Dense MPRIS2] Media command '%s' executed\n", action);
    return true;
}

// -------------------------------------------------------------
// Session controls & Sysstat
// -------------------------------------------------------------
bool dense_de_session_lock(void) {
    DESystemState *st = dense_de_get_state();
    st->session_locked = true;
    printf("[Dense Session] Screen locked (ext-session-lock-v1 active)\n");
    return true;
}

bool dense_de_session_logout(void) {
    printf("[Dense Session] Logging out session\n");
    return true;
}

bool dense_de_session_reboot(void) {
    printf("[Dense Session] System reboot signaled\n");
    return true;
}

bool dense_de_session_poweroff(void) {
    printf("[Dense Session] System poweroff signaled\n");
    return true;
}

int dense_de_sysstat_get(const char *metric) {
    DESystemState *st = dense_de_get_state();
    if (_stricmp(metric, "cpu") == 0) return 18;
    if (_stricmp(metric, "ram") == 0) return 42;
    if (_stricmp(metric, "battery") == 0) return st->battery.percentage;
    if (_stricmp(metric, "volume") == 0) return st->audio.volume;
    if (_stricmp(metric, "wifi") == 0) return st->network.signal_strength;
    return 0;
}

// -------------------------------------------------------------
// Even More QML/Python/C Replacement API Implementations
// -------------------------------------------------------------
bool dense_de_style_set(const char *key, const char *val) {
    DESystemState *st = dense_de_get_state();
    if (!key || !val) return false;
    if (_stricmp(key, "bg") == 0 || _stricmp(key, "bg_color") == 0) strncpy(st->style.bg_color, val, 31);
    else if (_stricmp(key, "fg") == 0 || _stricmp(key, "fg_color") == 0) strncpy(st->style.fg_color, val, 31);
    else if (_stricmp(key, "accent") == 0 || _stricmp(key, "accent_color") == 0) strncpy(st->style.accent_color, val, 31);
    else if (_stricmp(key, "border") == 0 || _stricmp(key, "border_color") == 0) strncpy(st->style.border_color, val, 31);
    else if (_stricmp(key, "radius") == 0 || _stricmp(key, "border_radius") == 0) st->style.border_radius = atoi(val);
    else if (_stricmp(key, "font") == 0 || _stricmp(key, "font_family") == 0) strncpy(st->style.font_family, val, 63);
    else if (_stricmp(key, "size") == 0 || _stricmp(key, "font_size") == 0) st->style.font_size = atoi(val);
    else if (_stricmp(key, "shadow") == 0 || _stricmp(key, "drop_shadow") == 0) st->style.drop_shadow = (_stricmp(val, "true") == 0 || _stricmp(val, "&T") == 0 || _stricmp(val, "1") == 0);
    else if (_stricmp(key, "blur") == 0) st->style.background_blur = (_stricmp(val, "true") == 0 || _stricmp(val, "&T") == 0 || _stricmp(val, "1") == 0);
    else return false;
    printf("[Dense Style] Configured %s = %s\n", key, val);
    return true;
}

DenseValue dense_de_style_get(const char *key) {
    DESystemState *st = dense_de_get_state();
    if (!key) return dense_val_none();
    if (_stricmp(key, "bg") == 0) return dense_val_str(st->style.bg_color);
    if (_stricmp(key, "fg") == 0) return dense_val_str(st->style.fg_color);
    if (_stricmp(key, "accent") == 0) return dense_val_str(st->style.accent_color);
    if (_stricmp(key, "border") == 0) return dense_val_str(st->style.border_color);
    if (_stricmp(key, "radius") == 0) return dense_val_int(st->style.border_radius);
    if (_stricmp(key, "font") == 0) return dense_val_str(st->style.font_family);
    return dense_val_none();
}

bool dense_de_animation_add(const char *target, const char *prop, int duration_ms, const char *easing, int from_v, int to_v) {
    DESystemState *st = dense_de_get_state();
    if (st->animation_count >= MAX_ANIMATIONS) return false;
    DEAnimation *anim = &st->animations[st->animation_count++];
    strncpy(anim->target, target ? target : "item", 63);
    strncpy(anim->property, prop ? prop : "opacity", 31);
    anim->duration_ms = duration_ms > 0 ? duration_ms : 200;
    strncpy(anim->easing, easing ? easing : "ease_in_out", 31);
    anim->from_val = from_v;
    anim->to_val = to_v;
    printf("[Dense Animation] Added transition on %s.%s (%dms %s from %d to %d)\n",
           anim->target, anim->property, anim->duration_ms, anim->easing, anim->from_val, anim->to_val);
    return true;
}

bool dense_de_state_set(const char *target, const char *state_name, const char *prop, const char *val) {
    DESystemState *st = dense_de_get_state();
    if (st->state_count >= 32) return false;
    DEWidgetState *s = NULL;
    for (int i = 0; i < st->state_count; i++) {
        if (_stricmp(st->states[i].target, target) == 0 && _stricmp(st->states[i].state_name, state_name) == 0) {
            s = &st->states[i];
            break;
        }
    }
    if (!s) {
        s = &st->states[st->state_count++];
        strncpy(s->target, target, 63);
        strncpy(s->state_name, state_name, 31);
        s->scale_pct = 100;
        s->opacity_pct = 100;
    }
    if (_stricmp(prop, "bg") == 0) strncpy(s->bg_color, val, 31);
    else if (_stricmp(prop, "fg") == 0) strncpy(s->fg_color, val, 31);
    else if (_stricmp(prop, "scale") == 0) s->scale_pct = atoi(val);
    else if (_stricmp(prop, "opacity") == 0) s->opacity_pct = atoi(val);
    printf("[Dense State] Configured %s:%s -> %s=%s\n", target, state_name, prop, val);
    return true;
}

int dense_de_timer_add(int interval_ms, bool repeat, const char *action) {
    DESystemState *st = dense_de_get_state();
    if (st->timer_count >= MAX_TIMERS) return -1;
    int id = st->timer_count;
    DETimer *t = &st->timers[st->timer_count++];
    t->id = id;
    t->interval_ms = interval_ms;
    t->repeat = repeat;
    strncpy(t->action, action ? action : "tick", 127);
    t->active = true;
    printf("[Dense Timer %d] Started timer (Interval:%dms Repeat:%s Action:%s)\n",
           id, interval_ms, repeat ? "true" : "false", t->action);
    return id;
}

bool dense_de_timer_stop(int timer_id) {
    DESystemState *st = dense_de_get_state();
    if (timer_id >= 0 && timer_id < st->timer_count) {
        st->timers[timer_id].active = false;
        printf("[Dense Timer %d] Stopped timer\n", timer_id);
        return true;
    }
    return false;
}

bool dense_de_service_control(const char *name, const char *action) {
    DESystemState *st = dense_de_get_state();
    if (!name || !action) return false;
    for (int i = 0; i < st->service_count; i++) {
        if (_stricmp(st->services[i].name, name) == 0) {
            if (_stricmp(action, "start") == 0 || _stricmp(action, "restart") == 0) {
                strcpy(st->services[i].status, "running");
                st->services[i].enabled = true;
            } else if (_stricmp(action, "stop") == 0) {
                strcpy(st->services[i].status, "stopped");
            }
            printf("[Dense Service] %s -> %s (Status: %s)\n", name, action, st->services[i].status);
            return true;
        }
    }
    if (st->service_count < MAX_SERVICES) {
        int idx = st->service_count++;
        strncpy(st->services[idx].name, name, 63);
        strcpy(st->services[idx].status, (_stricmp(action, "stop") == 0) ? "stopped" : "running");
        st->services[idx].enabled = true;
        printf("[Dense Service] Registered %s (Status: %s)\n", name, st->services[idx].status);
        return true;
    }
    return false;
}

const char* dense_de_service_status(const char *name) {
    DESystemState *st = dense_de_get_state();
    if (!name) return "unknown";
    for (int i = 0; i < st->service_count; i++) {
        if (_stricmp(st->services[i].name, name) == 0) {
            return st->services[i].status;
        }
    }
    return "inactive";
}

bool dense_de_gesture_add(const char *trigger, const char *action) {
    DESystemState *st = dense_de_get_state();
    if (!trigger || !action || st->gesture_count >= MAX_GESTURES) return false;
    for (int i = 0; i < st->gesture_count; i++) {
        if (_stricmp(st->gestures[i].trigger, trigger) == 0) {
            strncpy(st->gestures[i].action, action, 127);
            return true;
        }
    }
    int idx = st->gesture_count++;
    strncpy(st->gestures[idx].trigger, trigger, 31);
    strncpy(st->gestures[idx].action, action, 127);
    printf("[Dense Gesture] Bound %s -> %s\n", trigger, action);
    return true;
}

bool dense_de_gesture_trigger(const char *trigger) {
    DESystemState *st = dense_de_get_state();
    if (!trigger) return false;
    for (int i = 0; i < st->gesture_count; i++) {
        if (_stricmp(st->gestures[i].trigger, trigger) == 0) {
            printf("[Dense Gesture] Triggered gesture: %s => Executing: %s\n", trigger, st->gestures[i].action);
            return true;
        }
    }
    printf("[Dense Gesture] Unhandled gesture: %s\n", trigger);
    return false;
}

bool dense_de_layershell_surface_add(const char *ns, DELayer layer, int anchor_mask, int exclusive_zone) {
    DESystemState *st = dense_de_get_state();
    if (st->layersurface_count >= MAX_LAYERSHELL_SURFACES) return false;
    DELayerShellSurface *s = &st->layersurfaces[st->layersurface_count++];
    strncpy(s->namespace_name, ns ? ns : "dense_surface", 63);
    s->layer = layer;
    s->anchor_mask = anchor_mask;
    s->exclusive_zone = exclusive_zone;
    strcpy(s->keyboard_mode, "none");
    printf("[Dense LayerShell] Registered surface '%s' (Layer:%d Anchor:0x%x ExclusiveZone:%d)\n",
           s->namespace_name, s->layer, s->anchor_mask, s->exclusive_zone);
    return true;
}

bool dense_de_compositor_set(const char *key, const char *val) {
    DESystemState *st = dense_de_get_state();
    if (!key || !val) return false;
    if (_stricmp(key, "corner_radius") == 0 || _stricmp(key, "radius") == 0) st->compositor.corner_radius = atoi(val);
    else if (_stricmp(key, "drop_shadows") == 0 || _stricmp(key, "shadows") == 0) st->compositor.drop_shadows = (_stricmp(val, "true") == 0 || _stricmp(val, "&T") == 0 || _stricmp(val, "1") == 0);
    else if (_stricmp(key, "animations") == 0) st->compositor.animations = (_stricmp(val, "true") == 0 || _stricmp(val, "&T") == 0 || _stricmp(val, "1") == 0);
    else if (_stricmp(key, "decorations") == 0) strncpy(st->compositor.decorations, val, 31);
    else if (_stricmp(key, "vsync") == 0) st->compositor.vsync = (_stricmp(val, "true") == 0 || _stricmp(val, "&T") == 0 || _stricmp(val, "1") == 0);
    else if (_stricmp(key, "tearing") == 0) st->compositor.tearing = (_stricmp(val, "true") == 0 || _stricmp(val, "&T") == 0 || _stricmp(val, "1") == 0);
    else return false;
    printf("[Dense Compositor] Configured %s = %s\n", key, val);
    return true;
}

DenseValue dense_de_compositor_get(const char *key) {
    DESystemState *st = dense_de_get_state();
    if (!key) return dense_val_none();
    if (_stricmp(key, "corner_radius") == 0) return dense_val_int(st->compositor.corner_radius);
    if (_stricmp(key, "drop_shadows") == 0) return dense_val_bool(st->compositor.drop_shadows);
    if (_stricmp(key, "animations") == 0) return dense_val_bool(st->compositor.animations);
    if (_stricmp(key, "decorations") == 0) return dense_val_str(st->compositor.decorations);
    if (_stricmp(key, "vsync") == 0) return dense_val_bool(st->compositor.vsync);
    return dense_val_none();
}

// -------------------------------------------------------------
// Full Config Exporter (Labwc, Menus, Panels, Autostart)
// -------------------------------------------------------------
bool dense_de_export_config(const char *target_dir) {
    DESystemState *st = dense_de_get_state();
    mkdir_compat(target_dir);

    char path_buf[512];

    // 1. Export labwc rc.xml
    snprintf(path_buf, sizeof(path_buf), "%s/rc.xml", target_dir);
    FILE *fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "<?xml version=\"1.0\"?>\n");
        fprintf(fp, "<labwc_config>\n");
        fprintf(fp, "  <theme>\n");
        fprintf(fp, "    <name>%s</name>\n", st->labwc.theme);
        fprintf(fp, "    <cornerRadius>%d</cornerRadius>\n", st->compositor.corner_radius);
        fprintf(fp, "    <dropShadows>%s</dropShadows>\n", st->compositor.drop_shadows ? "yes" : "no");
        fprintf(fp, "  </theme>\n");
        fprintf(fp, "  <desktops>\n");
        fprintf(fp, "    <number>%d</number>\n", st->labwc.workspaces);
        fprintf(fp, "  </desktops>\n");
        fprintf(fp, "  <snapping>\n");
        fprintf(fp, "    <range>10</range>\n");
        fprintf(fp, "  </snapping>\n");
        fprintf(fp, "  <keyboard>\n");
        fprintf(fp, "    <default/>\n");
        for (int i = 0; i < st->labwc.keybind_count; i++) {
            fprintf(fp, "    <keybind key=\"%s\">\n", st->labwc.keybinds[i].key);
            fprintf(fp, "      <action name=\"Execute\" command=\"%s\"/>\n", st->labwc.keybinds[i].action);
            fprintf(fp, "    </keybind>\n");
        }
        fprintf(fp, "  </keyboard>\n");
        if (st->gesture_count > 0) {
            fprintf(fp, "  <mouse>\n");
            for (int g = 0; g < st->gesture_count; g++) {
                fprintf(fp, "    <gesture type=\"%s\"><action name=\"Execute\" command=\"%s\"/></gesture>\n",
                        st->gestures[g].trigger, st->gestures[g].action);
            }
            fprintf(fp, "  </mouse>\n");
        }
        if (st->labwc.rule_count > 0) {
            fprintf(fp, "  <windowRules>\n");
            for (int r = 0; r < st->labwc.rule_count; r++) {
                fprintf(fp, "    <windowRule identifier=\"%s\" serverDecoration=\"yes\"/>\n", st->labwc.rules[r].app_id);
            }
            fprintf(fp, "  </windowRules>\n");
        }
        fprintf(fp, "</labwc_config>\n");
        fclose(fp);
    }

    // 2. Export labwc menu.xml
    snprintf(path_buf, sizeof(path_buf), "%s/menu.xml", target_dir);
    fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "<?xml version=\"1.0\"?>\n");
        fprintf(fp, "<openbox_menu>\n");
        fprintf(fp, "  <menu id=\"root-menu\" label=\"Dense DE\">\n");
        for (int m = 0; m < st->menu_count; m++) {
            for (int i = 0; i < st->menus[m].item_count; i++) {
                fprintf(fp, "    <item label=\"%s\"><action name=\"Execute\" command=\"%s\"/></item>\n",
                        st->menus[m].items[i][0], st->menus[m].items[i][1]);
            }
        }
        fprintf(fp, "    <separator/>\n");
        fprintf(fp, "    <item label=\"Lock Screen\"><action name=\"Execute\" command=\"dense lock.dense\"/></item>\n");
        fprintf(fp, "    <item label=\"Exit Labwc\"><action name=\"Exit\"/></item>\n");
        fprintf(fp, "  </menu>\n");
        fprintf(fp, "</openbox_menu>\n");
        fclose(fp);
    }

    // 3. Export labwc autostart
    snprintf(path_buf, sizeof(path_buf), "%s/autostart", target_dir);
    fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "#!/bin/sh\n");
        fprintf(fp, "# Generated by Dense Language Desktop Environment Engine\n");
        for (int s = 0; s < st->service_count; s++) {
            if (st->services[s].enabled) {
                fprintf(fp, "%s &\n", st->services[s].name);
            }
        }
        if (st->wallpaper.is_set) {
            fprintf(fp, "wbg \"%s\" &\n", st->wallpaper.path);
        }
        for (int p = 0; p < st->panel_count; p++) {
            fprintf(fp, "dense panel_%d.dense &\n", p);
        }
        fclose(fp);
    }

    // 4. Export labwc environment
    snprintf(path_buf, sizeof(path_buf), "%s/environment", target_dir);
    fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "XDG_CURRENT_DESKTOP=labwc:dense\n");
        fprintf(fp, "MOZ_ENABLE_WAYLAND=1\n");
        fprintf(fp, "GDK_BACKEND=wayland\n");
        fprintf(fp, "QT_QPA_PLATFORM=wayland\n");
        fclose(fp);
    }

    // 5. Export panel definitions
    for (int p = 0; p < st->panel_count; p++) {
        snprintf(path_buf, sizeof(path_buf), "%s/panel_%d.dense", target_dir, p);
        fp = fopen(path_buf, "w");
        if (fp) {
            fprintf(fp, "// Dense Panel Definition\n");
            fprintf(fp, "panel/\n");
            fprintf(fp, "pos=%d\n", st->panels[p].pos);
            fprintf(fp, "height=%d\n", st->panels[p].height);
            fprintf(fp, "theme=%d\n", st->panels[p].theme);
            fprintf(fp, "exclusive=%s\n", st->panels[p].exclusive_zone ? "&T" : "&F");
            fprintf(fp, "widgets=");
            for (int w = 0; w < st->panels[p].widget_count; w++) {
                if (w > 0) fprintf(fp, "|");
                fprintf(fp, "%s", st->panels[p].widgets[w]);
            }
            fprintf(fp, "\n\\\n");
            fclose(fp);
        }
    }

    // 6. Export Desktop Environment Styling (style.css)
    snprintf(path_buf, sizeof(path_buf), "%s/style.css", target_dir);
    fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "/* Generated by Dense Language QML/CSS Replacement Engine */\n");
        fprintf(fp, ":root {\n");
        fprintf(fp, "  --de-bg: %s;\n", st->style.bg_color);
        fprintf(fp, "  --de-fg: %s;\n", st->style.fg_color);
        fprintf(fp, "  --de-accent: %s;\n", st->style.accent_color);
        fprintf(fp, "  --de-border: %s;\n", st->style.border_color);
        fprintf(fp, "  --de-radius: %dpx;\n", st->style.border_radius);
        fprintf(fp, "  --de-font-family: '%s';\n", st->style.font_family);
        fprintf(fp, "  --de-font-size: %dpt;\n", st->style.font_size);
        fprintf(fp, "  --de-shadow: %s;\n", st->style.drop_shadow ? "0 4px 12px rgba(0,0,0,0.35)" : "none");
        fprintf(fp, "  --de-blur: %s;\n", st->style.background_blur ? "blur(12px)" : "none");
        fprintf(fp, "}\n");
        fclose(fp);
    }

    // 7. Export Wayland Layer Shell definitions (layershell.dense)
    snprintf(path_buf, sizeof(path_buf), "%s/layershell.dense", target_dir);
    fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "// Dense Wayland Layer Shell Declarations\n");
        for (int ls = 0; ls < st->layersurface_count; ls++) {
            fprintf(fp, "layershell/\n");
            fprintf(fp, "namespace=%s\n", st->layersurfaces[ls].namespace_name);
            fprintf(fp, "layer=%d\n", st->layersurfaces[ls].layer);
            fprintf(fp, "anchor=0x%x\n", st->layersurfaces[ls].anchor_mask);
            fprintf(fp, "exclusive_zone=%d\n", st->layersurfaces[ls].exclusive_zone);
            fprintf(fp, "\\\n");
        }
        fclose(fp);
    }

    // 8. Export Ubuntu Wayland Session Entry (dense.desktop)
    snprintf(path_buf, sizeof(path_buf), "%s/dense.desktop", target_dir);
    fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "[Desktop Entry]\n");
        fprintf(fp, "Name=%s\n", st->session.name[0] ? st->session.name : "Dense Desktop");
        fprintf(fp, "Comment=%s\n", st->session.comment[0] ? st->session.comment : "Ultra-fast Dense Wayland Desktop");
        fprintf(fp, "Exec=%s\n", st->session.exec_path[0] ? st->session.exec_path : "/usr/local/bin/dense-session");
        fprintf(fp, "TryExec=%s\n", st->session.exec_path[0] ? st->session.exec_path : "/usr/local/bin/dense-session");
        fprintf(fp, "Type=Application\n");
        fprintf(fp, "DesktopNames=%s\n", st->session.desktop_names[0] ? st->session.desktop_names : "Dense;labwc");
        fclose(fp);
    }

    // 9. Export Wayland Session Runner (dense-session)
    snprintf(path_buf, sizeof(path_buf), "%s/dense-session", target_dir);
    fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "#!/bin/bash\n");
        fprintf(fp, "# =========================================================================\n");
        fprintf(fp, "# Dense Wayland Session Launcher\n");
        fprintf(fp, "# Orchestrated and configured in Dense Language\n");
        fprintf(fp, "# =========================================================================\n\n");
        fprintf(fp, "export XDG_CURRENT_DESKTOP=\"Dense:labwc\"\n");
        fprintf(fp, "export XDG_SESSION_DESKTOP=\"Dense\"\n");
        fprintf(fp, "export XDG_SESSION_TYPE=\"wayland\"\n");
        fprintf(fp, "export MOZ_ENABLE_WAYLAND=1\n");
        fprintf(fp, "export GDK_BACKEND=\"wayland,x11\"\n");
        fprintf(fp, "export QT_QPA_PLATFORM=\"wayland;xcb\"\n");
        fprintf(fp, "export SDL_VIDEODRIVER=\"wayland\"\n");
        fprintf(fp, "export CLUTTER_BACKEND=\"wayland\"\n\n");
        fprintf(fp, "if [ -z \"$XDG_RUNTIME_DIR\" ]; then\n");
        fprintf(fp, "    export XDG_RUNTIME_DIR=\"/run/user/$(id -u)\"\n");
        fprintf(fp, "fi\n\n");
        fprintf(fp, "DENSE_CONF_DIR=\"${XDG_CONFIG_HOME:-$HOME/.config}/dense\"\n");
        fprintf(fp, "mkdir -p \"$DENSE_CONF_DIR\"\n\n");
        fprintf(fp, "# If user config is empty, seed from /etc/dense\n");
        fprintf(fp, "if [ ! -f \"$DENSE_CONF_DIR/rc.xml\" ] && [ -d \"/etc/dense\" ]; then\n");
        fprintf(fp, "    cp -r /etc/dense/* \"$DENSE_CONF_DIR/\" 2>/dev/null || true\n");
        fprintf(fp, "fi\n\n");
        fprintf(fp, "exec labwc -C \"$DENSE_CONF_DIR\"\n");
        fclose(fp);
    }

    // 10. Export Standalone GNOME & Desktop Environment Purge Script (wipe_ubuntu_desktops.sh)
    snprintf(path_buf, sizeof(path_buf), "%s/wipe_ubuntu_desktops.sh", target_dir);
    fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "#!/bin/bash\n");
        fprintf(fp, "set -e\n\n");
        fprintf(fp, "echo \"==========================================================\"\n");
        fprintf(fp, "echo \" Dense OS: Complete Ubuntu GNOME & Legacy DE Purge Tool   \"\n");
        fprintf(fp, "echo \"==========================================================\"\n\n");
        fprintf(fp, "if [ \"$EUID\" -ne 0 ]; then\n");
        fprintf(fp, "  echo \"Error: Please run as root (sudo ./wipe_ubuntu_desktops.sh)\"\n");
        fprintf(fp, "  exit 1\n");
        fprintf(fp, "fi\n\n");
        fprintf(fp, "echo \"[1/6] Stopping and disabling active display managers...\"\n");
        fprintf(fp, "systemctl stop gdm3 2>/dev/null || true\n");
        fprintf(fp, "systemctl stop gdm 2>/dev/null || true\n");
        fprintf(fp, "systemctl stop lightdm 2>/dev/null || true\n");
        fprintf(fp, "systemctl stop sddm 2>/dev/null || true\n");
        fprintf(fp, "systemctl disable gdm3 2>/dev/null || true\n");
        fprintf(fp, "systemctl disable gdm 2>/dev/null || true\n");
        fprintf(fp, "systemctl disable lightdm 2>/dev/null || true\n");
        fprintf(fp, "systemctl disable sddm 2>/dev/null || true\n\n");

        fprintf(fp, "echo \"[2/6] Purging GNOME desktop environment and Ubuntu shell...\"\n");
        fprintf(fp, "apt-get purge -y \\\n");
        fprintf(fp, "  ubuntu-desktop \\\n");
        fprintf(fp, "  ubuntu-desktop-minimal \\\n");
        fprintf(fp, "  gnome-shell \\\n");
        fprintf(fp, "  gnome-session \\\n");
        fprintf(fp, "  gnome-session-bin \\\n");
        fprintf(fp, "  gnome-session-common \\\n");
        fprintf(fp, "  gdm3 \\\n");
        fprintf(fp, "  gnome-control-center \\\n");
        fprintf(fp, "  gnome-terminal \\\n");
        fprintf(fp, "  mutter \\\n");
        fprintf(fp, "  yaru-theme-* \\\n");
        fprintf(fp, "  gnome-shell-extension-* 2>/dev/null || true\n\n");

        fprintf(fp, "echo \"[3/6] Purging KDE, XFCE, Cinnamon, MATE, LXQt/LXDE environments...\"\n");
        fprintf(fp, "apt-get purge -y \\\n");
        fprintf(fp, "  kubuntu-desktop kde-plasma-desktop plasma-desktop sddm \\\n");
        fprintf(fp, "  xubuntu-desktop xfce4 xfce4-session lightdm \\\n");
        fprintf(fp, "  cinnamon cinnamon-desktop-environment \\\n");
        fprintf(fp, "  ubuntu-mate-desktop mate-desktop-environment \\\n");
        fprintf(fp, "  lubuntu-desktop lxde lxqt 2>/dev/null || true\n\n");

        fprintf(fp, "echo \"[4/6] Wiping legacy desktop sessions from X11 and Wayland session trees...\"\n");
        fprintf(fp, "mkdir -p /usr/share/wayland-sessions /usr/share/xsessions\n");
        fprintf(fp, "find /usr/share/wayland-sessions/ -type f ! -name \"dense.desktop\" -delete 2>/dev/null || true\n");
        fprintf(fp, "rm -rf /usr/share/xsessions/* 2>/dev/null || true\n\n");

        fprintf(fp, "echo \"[5/6] Autoremoving orphaned dependencies and cleaning package cache...\"\n");
        fprintf(fp, "apt-get autoremove --purge -y\n");
        fprintf(fp, "apt-get clean\n\n");

        fprintf(fp, "echo \"[6/6] Ensuring core Wayland & system daemons remain functional...\"\n");
        fprintf(fp, "apt-get install -y --no-install-recommends \\\n");
        fprintf(fp, "  labwc \\\n");
        fprintf(fp, "  wbg \\\n");
        fprintf(fp, "  alacritty \\\n");
        fprintf(fp, "  fuzzel \\\n");
        fprintf(fp, "  pipewire \\\n");
        fprintf(fp, "  wireplumber \\\n");
        fprintf(fp, "  pipewire-pulse \\\n");
        fprintf(fp, "  pamixer \\\n");
        fprintf(fp, "  brightnessctl \\\n");
        fprintf(fp, "  network-manager \\\n");
        fprintf(fp, "  dbus-user-session \\\n");
        fprintf(fp, "  seatd\n\n");

        fprintf(fp, "echo \"==========================================================\"\n");
        fprintf(fp, "echo \" Success: GNOME and all other desktop environments wiped! \"\n");
        fprintf(fp, "echo \" System is clean and ready for Dense Desktop Environment! \"\n");
        fprintf(fp, "echo \"==========================================================\"\n");
        fclose(fp);
    }

    // 11. Export One-Click Ubuntu Plug & Installer (setup_ubuntu.sh)
    snprintf(path_buf, sizeof(path_buf), "%s/setup_ubuntu.sh", target_dir);
    fp = fopen(path_buf, "w");
    if (fp) {
        fprintf(fp, "#!/bin/bash\n");
        fprintf(fp, "set -e\n\n");
        fprintf(fp, "echo \"=================================================================\"\n");
        fprintf(fp, "echo \" Dense Desktop Environment: Ubuntu Plug & Installation Suite    \"\n");
        fprintf(fp, "echo \"=================================================================\"\n\n");
        fprintf(fp, "if [ \"$EUID\" -ne 0 ]; then\n");
        fprintf(fp, "  echo \"Error: Please run as root (sudo ./setup_ubuntu.sh)\"\n");
        fprintf(fp, "  exit 1\n");
        fprintf(fp, "fi\n\n");
        fprintf(fp, "SCRIPT_DIR=\"$(cd \"$(dirname \"${BASH_SOURCE[0]}\")\" && pwd)\"\n\n");

        fprintf(fp, "echo \"[1/6] Installing Wayland stack (labwc, wbg, alacritty, fuzzel, audio)...\"\n");
        fprintf(fp, "apt-get update\n");
        fprintf(fp, "apt-get install -y --no-install-recommends \\\n");
        fprintf(fp, "  gcc make build-essential labwc wbg alacritty fuzzel \\\n");
        fprintf(fp, "  pipewire wireplumber pipewire-pulse pamixer brightnessctl \\\n");
        fprintf(fp, "  network-manager seatd mako-notifier thunar\n\n");

        fprintf(fp, "echo \"[2/6] Executing wipe of GNOME and all other desktop environments...\"\n");
        fprintf(fp, "chmod +x \"$SCRIPT_DIR/wipe_ubuntu_desktops.sh\"\n");
        fprintf(fp, "\"$SCRIPT_DIR/wipe_ubuntu_desktops.sh\"\n\n");

        fprintf(fp, "echo \"[3/6] Installing Dense language compiler/runtime to /usr/local/bin/dense...\"\n");
        fprintf(fp, "if [ -f \"$SCRIPT_DIR/dense\" ]; then\n");
        fprintf(fp, "    cp \"$SCRIPT_DIR/dense\" /usr/local/bin/dense\n");
        fprintf(fp, "elif [ -d \"$SCRIPT_DIR/src\" ]; then\n");
        fprintf(fp, "    gcc -std=c11 -Wall -O2 \"$SCRIPT_DIR/src/\"*.c -o /usr/local/bin/dense\n");
        fprintf(fp, "fi\n");
        fprintf(fp, "chmod 755 /usr/local/bin/dense 2>/dev/null || true\n\n");

        fprintf(fp, "echo \"[4/6] Registering Dense Wayland session entry...\"\n");
        fprintf(fp, "mkdir -p /usr/share/wayland-sessions\n");
        fprintf(fp, "cp \"$SCRIPT_DIR/dense.desktop\" /usr/share/wayland-sessions/dense.desktop\n");
        fprintf(fp, "chmod 644 /usr/share/wayland-sessions/dense.desktop\n\n");
        fprintf(fp, "cp \"$SCRIPT_DIR/dense-session\" /usr/local/bin/dense-session\n");
        fprintf(fp, "chmod 755 /usr/local/bin/dense-session\n\n");

        fprintf(fp, "echo \"[5/6] Deploying Dense desktop environment configurations...\"\n");
        fprintf(fp, "mkdir -p /etc/dense\n");
        fprintf(fp, "cp \"$SCRIPT_DIR/rc.xml\" /etc/dense/\n");
        fprintf(fp, "cp \"$SCRIPT_DIR/menu.xml\" /etc/dense/\n");
        fprintf(fp, "cp \"$SCRIPT_DIR/autostart\" /etc/dense/\n");
        fprintf(fp, "chmod 755 /etc/dense/autostart\n");
        fprintf(fp, "cp \"$SCRIPT_DIR/environment\" /etc/dense/\n");
        fprintf(fp, "cp \"$SCRIPT_DIR/style.css\" /etc/dense/\n");
        fprintf(fp, "cp \"$SCRIPT_DIR/\"*.dense /etc/dense/ 2>/dev/null || true\n\n");

        fprintf(fp, "ACTUAL_USER=\"${SUDO_USER:-$USER}\"\n");
        fprintf(fp, "if [ -n \"$ACTUAL_USER\" ] && [ \"$ACTUAL_USER\" != \"root\" ]; then\n");
        fprintf(fp, "    USER_HOME=$(getent passwd \"$ACTUAL_USER\" | cut -d: -f6)\n");
        fprintf(fp, "    USER_CONF=\"$USER_HOME/.config/dense\"\n");
        fprintf(fp, "    mkdir -p \"$USER_CONF\"\n");
        fprintf(fp, "    cp -r /etc/dense/* \"$USER_CONF/\"\n");
        fprintf(fp, "    chown -R \"$ACTUAL_USER:$ACTUAL_USER\" \"$USER_CONF\"\n");
        fprintf(fp, "fi\n\n");

        fprintf(fp, "echo \"[6/6] Activating system services (seatd, audio, networks)...\"\n");
        fprintf(fp, "systemctl enable seatd 2>/dev/null || true\n");
        fprintf(fp, "systemctl start seatd 2>/dev/null || true\n\n");

        fprintf(fp, "echo \"=================================================================\"\n");
        fprintf(fp, "echo \" Dense Desktop Environment has been successfully installed!     \"\n");
        fprintf(fp, "echo \" GNOME and all other desktop environments have been wiped.       \"\n");
        fprintf(fp, "echo \" Log in or start Wayland: select 'Dense Desktop' session!       \"\n");
        fprintf(fp, "echo \"=================================================================\"\n");
        fclose(fp);
    }

    printf("[Dense DE] Successfully exported full Labwc Desktop Environment configuration & Ubuntu deployment suite to: %s\n", target_dir);
    return true;
}

bool dense_de_wipe_desktops(const char *targets, bool purge_configs, bool dry_run) {
    DESystemState *st = dense_de_get_state();
    printf("\n============================================================\n");
    printf("[Dense DE Wipe] Wiping Ubuntu Desktop Environments & GNOME...\n");
    printf("============================================================\n");

    if (targets && strlen(targets) > 0) {
        if (strstr(targets, "gnome") || strstr(targets, "all")) st->wipe.wipe_gnome = true;
        if (strstr(targets, "kde") || strstr(targets, "plasma") || strstr(targets, "all")) st->wipe.wipe_kde = true;
        if (strstr(targets, "xfce") || strstr(targets, "all")) st->wipe.wipe_xfce = true;
        if (strstr(targets, "cinnamon") || strstr(targets, "all")) st->wipe.wipe_cinnamon = true;
        if (strstr(targets, "mate") || strstr(targets, "all")) st->wipe.wipe_mate = true;
        if (strstr(targets, "lxde") || strstr(targets, "lxqt") || strstr(targets, "all")) st->wipe.wipe_lxde = true;
    }
    st->wipe.purge_configs = purge_configs;
    st->wipe.dry_run = dry_run;

    printf("[Dense Wipe] Targets: %s | Purge Configs: %s | Dry Run: %s\n",
           (targets && strlen(targets) > 0) ? targets : "all",
           purge_configs ? "true" : "false", dry_run ? "true" : "false");

    if (st->wipe.wipe_gnome) {
        printf("[Dense Wipe] -> Purging GNOME & Display Manager (ubuntu-desktop, gnome-shell, gdm3, gnome-session)...\n");
    }
    if (st->wipe.wipe_kde) {
        printf("[Dense Wipe] -> Purging KDE Plasma & SDDM (kubuntu-desktop, plasma-desktop, sddm)...\n");
    }
    if (st->wipe.wipe_xfce) {
        printf("[Dense Wipe] -> Purging XFCE & LightDM (xubuntu-desktop, xfce4, lightdm)...\n");
    }
    if (st->wipe.wipe_cinnamon) {
        printf("[Dense Wipe] -> Purging Cinnamon Desktop Environment...\n");
    }
    if (st->wipe.wipe_mate) {
        printf("[Dense Wipe] -> Purging MATE Desktop Environment...\n");
    }
    if (st->wipe.wipe_lxde) {
        printf("[Dense Wipe] -> Purging LXDE / LXQt Desktop Environments...\n");
    }
    printf("[Dense Wipe] -> Purging legacy session files in /usr/share/xsessions/ and /usr/share/wayland-sessions/...\n");
    printf("[Dense Wipe] -> Running autoremove & cache cleaning...\n");

#ifndef _WIN32
    if (!dry_run) {
        system("systemctl stop gdm3 2>/dev/null || systemctl stop gdm 2>/dev/null || true");
        system("systemctl disable gdm3 2>/dev/null || systemctl disable gdm 2>/dev/null || true");
        system("apt-get purge -y ubuntu-desktop ubuntu-desktop-minimal gnome-shell gnome-session gdm3 2>/dev/null || true");
        system("apt-get autoremove --purge -y 2>/dev/null || true");
    }
#endif

    printf("[Dense Wipe] Wipe operation completed successfully!\n");
    return true;
}

bool dense_de_session_register(const char *name, const char *exec_path) {
    DESystemState *st = dense_de_get_state();
    if (name && strlen(name) > 0) strncpy(st->session.name, name, sizeof(st->session.name) - 1);
    if (exec_path && strlen(exec_path) > 0) strncpy(st->session.exec_path, exec_path, sizeof(st->session.exec_path) - 1);

    printf("[Dense Session] Registered Wayland Session Entry: %s (Exec: %s)\n",
           st->session.name, st->session.exec_path);
    return true;
}

bool dense_de_deploy_ubuntu(const char *target_dir) {
    const char *dir = (target_dir && strlen(target_dir) > 0) ? target_dir : "dist_ubuntu_de";
    printf("\n============================================================\n");
    printf("[Dense Deploy] Deploying Desktop Environment for Ubuntu Linux -> %s\n", dir);
    printf("============================================================\n");
    return dense_de_export_config(dir);
}

bool dense_de_pkg_op(const char *op, char pkgs[][MAX_TOKEN_LEN], int pkg_count) {
    if (!op) return false;
    printf("[Dense Package %s] Packages:", op);
    for (int i = 0; i < pkg_count; i++) {
        printf(" %s", pkgs[i]);
    }
    printf("\n");
    return true;
}

bool dense_de_start_session(const char *config_dir) {
    dense_de_export_config(config_dir);
    printf("[Dense DE] Starting Labwc desktop session with config from: %s\n", config_dir);
    return true;
}

// -------------------------------------------------------------
// Block syntax handler (QML/Python/C replacement blocks)
// -------------------------------------------------------------
bool dense_de_handle_block(DenseRuntime *rt, const char *block_name, char lines[][MAX_LINE_LEN], int line_count) {
    dense_de_init(rt);
    DESystemState *st = dense_de_get_state();

    // 1. Panel Block
    if (_stricmp(block_name, "panel") == 0 || _stricmp(block_name, "taskbar") == 0) {
        DEPosition pos = DE_POS_BOTTOM;
        int height = 36;
        DETheme theme = DE_THEME_DARK;
        bool exclusive = true;
        char widgets_raw[MAX_LINE_LEN] = {0};

        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "pos") == 0) {
                    if (strstr(v, "top") || strstr(v, "&TN")) pos = DE_POS_TOP;
                    else if (strstr(v, "left") || strstr(v, "&xR")) pos = DE_POS_LEFT;
                    else if (strstr(v, "right") || strstr(v, "&xS")) pos = DE_POS_RIGHT;
                    else pos = DE_POS_BOTTOM;
                } else if (_stricmp(k, "height") == 0 || _stricmp(k, "size") == 0) {
                    height = atoi(v);
                } else if (_stricmp(k, "theme") == 0) {
                    if (strstr(v, "light") || strstr(v, "&Rx")) theme = DE_THEME_LIGHT;
                    else if (strstr(v, "trans") || strstr(v, "&eJ")) theme = DE_THEME_TRANS;
                    else if (strstr(v, "blur") || strstr(v, "&AD")) theme = DE_THEME_BLUR;
                    else theme = DE_THEME_DARK;
                } else if (_stricmp(k, "exclusive") == 0) {
                    exclusive = (_stricmp(v, "&T") == 0 || _stricmp(v, "true") == 0 || _stricmp(v, "1") == 0);
                } else if (_stricmp(k, "widgets") == 0) {
                    strncpy(widgets_raw, v, sizeof(widgets_raw) - 1);
                }
            }
        }
        int p_idx = dense_de_panel_create(pos, height, theme);
        st->panels[p_idx].exclusive_zone = exclusive;
        if (widgets_raw[0] != '\0') {
            char *tok = strtok(widgets_raw, "|");
            while (tok) {
                dense_de_panel_add_widget(p_idx, tok);
                tok = strtok(NULL, "|");
            }
        }
        dense_de_panel_show(p_idx);
        return true;
    }

    // 2. Dock Block
    if (_stricmp(block_name, "dock") == 0) {
        DEPosition pos = DE_POS_BOTTOM;
        int size = 48;
        bool autohide = false;
        char pinned_raw[MAX_LINE_LEN] = {0};

        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "size") == 0) size = atoi(v);
                else if (_stricmp(k, "autohide") == 0) autohide = (_stricmp(v, "&T") == 0 || _stricmp(v, "true") == 0);
                else if (_stricmp(k, "pinned") == 0 || _stricmp(k, "apps") == 0) {
                    strncpy(pinned_raw, v, sizeof(pinned_raw) - 1);
                }
            }
        }
        dense_de_dock_create(pos, size, autohide);
        if (pinned_raw[0] != '\0') {
            char *tok = strtok(pinned_raw, "|");
            while (tok) {
                dense_de_dock_add_app(tok);
                tok = strtok(NULL, "|");
            }
        }
        dense_de_dock_show();
        return true;
    }

    // 3. Wallpaper Block
    if (_stricmp(block_name, "wallpaper") == 0) {
        char path[256] = {0};
        DEWallpaperMode mode = DE_WP_FILL;
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "path") == 0) strncpy(path, v, sizeof(path) - 1);
                else if (_stricmp(k, "mode") == 0) {
                    if (strstr(v, "fit") || strstr(v, "&ca")) mode = DE_WP_FIT;
                    else if (strstr(v, "tile") || strstr(v, "&Qf")) mode = DE_WP_TILE;
                    else mode = DE_WP_FILL;
                }
            }
        }
        if (path[0] != '\0') dense_de_wallpaper_set(path, mode);
        return true;
    }

    // 4. Labwc Compositor Block
    if (_stricmp(block_name, "labwc") == 0) {
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "theme") == 0) strncpy(st->labwc.theme, v, sizeof(st->labwc.theme) - 1);
                else if (_stricmp(k, "workspaces") == 0) st->labwc.workspaces = atoi(v);
                else if (_stricmp(k, "snap") == 0) st->labwc.snap_to_edge = (_stricmp(v, "&T") == 0 || _stricmp(v, "true") == 0);
                else if (_stricmp(k, "keybind") == 0) {
                    char *pipe = strchr(v, '|');
                    if (pipe) {
                        *pipe = '\0';
                        dense_de_labwc_add_keybind(trim_str(v), trim_str(pipe + 1));
                    }
                }
            }
        }
        return true;
    }

    // 5. QML-Replacement Box Layout Block (box/)
    if (_stricmp(block_name, "box") == 0 || _stricmp(block_name, "layout") == 0) {
        DELayoutType type = DE_LAYOUT_COL;
        int spacing = 8;
        int padding = 8;
        char children_raw[MAX_LINE_LEN] = {0};

        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "type") == 0 || _stricmp(k, "layout") == 0) {
                    if (strstr(v, "row")) type = DE_LAYOUT_ROW;
                    else if (strstr(v, "grid")) type = DE_LAYOUT_GRID;
                    else if (strstr(v, "stack")) type = DE_LAYOUT_STACK;
                    else type = DE_LAYOUT_COL;
                } else if (_stricmp(k, "spacing") == 0) spacing = atoi(v);
                else if (_stricmp(k, "padding") == 0) padding = atoi(v);
                else if (_stricmp(k, "children") == 0) strncpy(children_raw, v, sizeof(children_raw) - 1);
            }
        }
        int b_idx = dense_de_box_create(type, spacing, padding);
        if (children_raw[0] != '\0') {
            char *tok = strtok(children_raw, "|");
            while (tok) {
                dense_de_box_add_child(b_idx, tok);
                tok = strtok(NULL, "|");
            }
        }
        return true;
    }

    // 6. QML-Replacement Widget / Element Block (widget/)
    if (_stricmp(block_name, "widget") == 0) {
        DEElementType type = DE_ELEM_LABEL;
        char text[128] = {0};
        char icon[64] = {0};
        int val = 0;

        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "type") == 0) {
                    if (strstr(v, "button")) type = DE_ELEM_BUTTON;
                    else if (strstr(v, "slider")) type = DE_ELEM_SLIDER;
                    else if (strstr(v, "progress")) type = DE_ELEM_PROGRESS;
                    else if (strstr(v, "toggle")) type = DE_ELEM_TOGGLE;
                    else if (strstr(v, "entry")) type = DE_ELEM_ENTRY;
                    else if (strstr(v, "icon")) type = DE_ELEM_ICON;
                    else type = DE_ELEM_LABEL;
                } else if (_stricmp(k, "text") == 0) strncpy(text, v, 127);
                else if (_stricmp(k, "icon") == 0) strncpy(icon, v, 63);
                else if (_stricmp(k, "value") == 0) val = atoi(v);
            }
        }
        int w_idx = dense_de_widget_create(type, text, icon);
        st->elements[w_idx].value = val;
        return true;
    }

    // 7. QML-Replacement Surface Window Block (window/)
    if (_stricmp(block_name, "window") == 0 || _stricmp(block_name, "surface") == 0) {
        char title[128] = "Dense Window";
        int width = 400;
        int height = 300;
        DELayer layer = DE_LAYER_TOP;

        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "title") == 0) strncpy(title, v, 127);
                else if (_stricmp(k, "width") == 0) width = atoi(v);
                else if (_stricmp(k, "height") == 0) height = atoi(v);
                else if (_stricmp(k, "layer") == 0) {
                    if (strstr(v, "overlay")) layer = DE_LAYER_OVERLAY;
                    else if (strstr(v, "bottom")) layer = DE_LAYER_BOTTOM;
                }
            }
        }
        int win_idx = dense_de_window_create(title, width, height, layer);
        dense_de_window_show(win_idx);
        return true;
    }

    // 8. Desktop Context Menu Block (menu/)
    if (_stricmp(block_name, "menu") == 0) {
        int m_idx = dense_de_menu_create("root_menu");
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *pipe = strchr(line, '|');
            if (pipe) {
                *pipe = '\0';
                dense_de_menu_add_item(m_idx, trim_str(line), trim_str(pipe + 1));
            }
        }
        return true;
    }

    // 9. C/Labwc Window Rules Block (windowrule/)
    if (_stricmp(block_name, "windowrule") == 0 || _stricmp(block_name, "rules") == 0) {
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *pipe = strchr(line, '|');
            if (pipe) {
                *pipe = '\0';
                dense_de_labwc_add_rule(trim_str(line), trim_str(pipe + 1));
            }
        }
        return true;
    }

    // 10. Hardware Input Devices Block (input/)
    if (_stricmp(block_name, "input") == 0) {
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "tap_to_click") == 0) strncpy(st->input.tap_to_click, v, 15);
                else if (_stricmp(k, "natural_scroll") == 0) strncpy(st->input.natural_scroll, v, 15);
                else if (_stricmp(k, "accel_speed") == 0) st->input.accel_speed = atoi(v);
                else if (_stricmp(k, "layout") == 0) strncpy(st->input.kb_layout, v, 31);
            }
        }
        printf("[Dense Input] Configured pointer/keyboard: Layout=%s TapToClick=%s NaturalScroll=%s\n",
               st->input.kb_layout, st->input.tap_to_click, st->input.natural_scroll);
        return true;
    }

    // 11. Multi-monitor Output Configuration Block (monitor/)
    if (_stricmp(block_name, "monitor") == 0) {
        if (st->monitor_count < MAX_MONITORS) {
            DEMonitor *m = &st->monitors[st->monitor_count++];
            memset(m, 0, sizeof(DEMonitor));
            strcpy(m->name, "DP-1");
            m->width = 1920; m->height = 1080; m->refresh_rate = 60; m->scale = 1;
            for (int i = 0; i < line_count; i++) {
                char *line = trim_str(lines[i]);
                char *eq = strchr(line, '=');
                if (eq) {
                    *eq = '\0';
                    char *k = trim_str(line);
                    char *v = trim_str(eq + 1);
                    if (_stricmp(k, "name") == 0) strncpy(m->name, v, 31);
                    else if (_stricmp(k, "width") == 0) m->width = atoi(v);
                    else if (_stricmp(k, "height") == 0) m->height = atoi(v);
                    else if (_stricmp(k, "rate") == 0) m->refresh_rate = atoi(v);
                    else if (_stricmp(k, "scale") == 0) m->scale = atoi(v);
                }
            }
            printf("[Dense Monitor] Configured output %s (%dx%d@%dHz Scale:%d)\n", m->name, m->width, m->height, m->refresh_rate, m->scale);
        }
        return true;
    }

    // 12. Control Center Block (controlcenter/)
    if (_stricmp(block_name, "controlcenter") == 0) {
        printf("[Dense ControlCenter] Activated QuickSettings Surface (Audio:%d%% WiFi:%s Battery:%d%%)\n",
               st->audio.volume, st->network.connected_ssid, st->battery.percentage);
        return true;
    }

    // 13. Screen Locker Block (lockscreen/)
    if (_stricmp(block_name, "lockscreen") == 0) {
        dense_de_session_lock();
        return true;
    }

    // 14. Theme & Styling Block (theme/ or style/) - QML/CSS Replacement
    if (_stricmp(block_name, "style") == 0 || _stricmp(block_name, "theme") == 0) {
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                dense_de_style_set(trim_str(line), trim_str(eq + 1));
            }
        }
        return true;
    }

    // 15. Animation Block (animation/) - QML Replacement
    if (_stricmp(block_name, "animation") == 0) {
        char target[64] = "item";
        char prop[32] = "opacity";
        int duration = 200;
        char easing[32] = "ease_in_out";
        int from_v = 0;
        int to_v = 100;
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "target") == 0) strncpy(target, v, 63);
                else if (_stricmp(k, "prop") == 0 || _stricmp(k, "property") == 0) strncpy(prop, v, 31);
                else if (_stricmp(k, "duration") == 0) duration = atoi(v);
                else if (_stricmp(k, "easing") == 0) strncpy(easing, v, 31);
                else if (_stricmp(k, "from") == 0) from_v = atoi(v);
                else if (_stricmp(k, "to") == 0) to_v = atoi(v);
            }
        }
        dense_de_animation_add(target, prop, duration, easing, from_v, to_v);
        return true;
    }

    // 16. State Transitions Block (state/) - QML Replacement
    if (_stricmp(block_name, "state") == 0) {
        char target[64] = "item";
        char state_name[32] = "hovered";
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "target") == 0) strncpy(target, v, 63);
                else if (_stricmp(k, "name") == 0) strncpy(state_name, v, 31);
                else dense_de_state_set(target, state_name, k, v);
            }
        }
        return true;
    }

    // 17. System Timer Block (timer/) - Python Replacement
    if (_stricmp(block_name, "timer") == 0) {
        int interval = 1000;
        bool repeat = true;
        char action[128] = "tick";
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "interval") == 0) interval = atoi(v);
                else if (_stricmp(k, "repeat") == 0) repeat = (_stricmp(v, "true") == 0 || _stricmp(v, "&T") == 0 || _stricmp(v, "1") == 0);
                else if (_stricmp(k, "action") == 0) strncpy(action, v, 127);
            }
        }
        dense_de_timer_add(interval, repeat, action);
        return true;
    }

    // 18. System Services Block (service/ or daemon/) - Python Replacement
    if (_stricmp(block_name, "service") == 0 || _stricmp(block_name, "daemon") == 0) {
        char name[64] = "";
        char action[32] = "start";
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "name") == 0) strncpy(name, v, 63);
                else if (_stricmp(k, "action") == 0) strncpy(action, v, 31);
            }
        }
        if (name[0] != '\0') dense_de_service_control(name, action);
        return true;
    }

    // 19. Compositor Hardware Settings Block (compositor/) - C/Wayland Replacement
    if (_stricmp(block_name, "compositor") == 0) {
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                dense_de_compositor_set(trim_str(line), trim_str(eq + 1));
            }
        }
        return true;
    }

    // 20. Wayland Gestures Block (gestures/) - C/Wayland Replacement
    if (_stricmp(block_name, "gestures") == 0) {
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                dense_de_gesture_add(trim_str(line), trim_str(eq + 1));
            } else {
                char *pipe = strchr(line, '|');
                if (pipe) {
                    *pipe = '\0';
                    dense_de_gesture_add(trim_str(line), trim_str(pipe + 1));
                }
            }
        }
        return true;
    }

    // 21. Wayland Layer Shell Surface Block (layershell/ or layer_surface/) - C/Wayland Replacement
    if (_stricmp(block_name, "layershell") == 0 || _stricmp(block_name, "layer_surface") == 0) {
        char ns[64] = "dense_bar";
        DELayer layer = DE_LAYER_TOP;
        int anchor = 7; // top|left|right
        int exclusive = 36;
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "namespace") == 0) strncpy(ns, v, 63);
                else if (_stricmp(k, "layer") == 0) {
                    if (strstr(v, "overlay")) layer = DE_LAYER_OVERLAY;
                    else if (strstr(v, "bottom")) layer = DE_LAYER_BOTTOM;
                    else if (strstr(v, "bg")) layer = DE_LAYER_BACKGROUND;
                    else layer = DE_LAYER_TOP;
                }
                else if (_stricmp(k, "anchor") == 0) {
                    anchor = 0;
                    if (strstr(v, "top")) anchor |= 1;
                    if (strstr(v, "bottom") || strstr(v, "bot")) anchor |= 2;
                    if (strstr(v, "left")) anchor |= 4;
                    if (strstr(v, "right")) anchor |= 8;
                }
                else if (_stricmp(k, "exclusive_zone") == 0 || _stricmp(k, "zone") == 0) exclusive = atoi(v);
            }
        }
        dense_de_layershell_surface_add(ns, layer, anchor, exclusive);
        return true;
    }

    // 22. Keyboard Shortcuts Block (keybinds/ or shortcuts/) - C/Wayland Replacement
    if (_stricmp(block_name, "keybinds") == 0 || _stricmp(block_name, "shortcuts") == 0) {
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                dense_de_labwc_add_keybind(trim_str(line), trim_str(eq + 1));
            } else {
                char *pipe = strchr(line, '|');
                if (pipe) {
                    *pipe = '\0';
                    dense_de_labwc_add_keybind(trim_str(line), trim_str(pipe + 1));
                }
            }
        }
        return true;
    }

    // 23. Autostart Daemons Block (autostart/) - C/Labwc Replacement
    if (_stricmp(block_name, "autostart") == 0) {
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            char *cmd = eq ? trim_str(eq + 1) : line;
            if (st->labwc.autostart_count < 16 && strlen(cmd) > 0) {
                strncpy(st->labwc.autostart_cmds[st->labwc.autostart_count++], cmd, 127);
                printf("[Dense Autostart] Registered startup command: %s\n", cmd);
            }
        }
        return true;
    }

    // 24. Repeater / Dynamic List Block (repeater/ or listview/) - QML Replacement
    if (_stricmp(block_name, "repeater") == 0 || _stricmp(block_name, "listview") == 0) {
        char model[64] = "toplevels";
        char delegate[64] = "task_item";
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "model") == 0) strncpy(model, v, 63);
                else if (_stricmp(k, "delegate") == 0) strncpy(delegate, v, 63);
            }
        }
        printf("[Dense Repeater] Created Dynamic ListView (Model:%s Delegate:%s Elements:%d)\n",
               model, delegate, st->toplevel_count);
        return true;
    }

    // 25. Ubuntu Wayland Session Block (session/)
    if (_stricmp(block_name, "session") == 0) {
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "name") == 0) strncpy(st->session.name, v, sizeof(st->session.name) - 1);
                else if (_stricmp(k, "comment") == 0) strncpy(st->session.comment, v, sizeof(st->session.comment) - 1);
                else if (_stricmp(k, "compositor") == 0) strncpy(st->session.compositor, v, sizeof(st->session.compositor) - 1);
                else if (_stricmp(k, "exec") == 0) strncpy(st->session.exec_path, v, sizeof(st->session.exec_path) - 1);
                else if (_stricmp(k, "desktop_names") == 0) strncpy(st->session.desktop_names, v, sizeof(st->session.desktop_names) - 1);
            }
        }
        printf("[Dense Session Block] Configured Wayland session: %s (Exec: %s)\n",
               st->session.name, st->session.exec_path);
        return true;
    }

    // 26. Desktop Environment Wiping Block (wipe/)
    if (_stricmp(block_name, "wipe") == 0 || _stricmp(block_name, "purge") == 0) {
        char targets[256] = "all";
        bool purge_configs = true;
        bool dry_run = false;
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "target") == 0 || _stricmp(k, "targets") == 0) strncpy(targets, v, sizeof(targets) - 1);
                else if (_stricmp(k, "purge_configs") == 0) purge_configs = (_stricmp(v, "true") == 0 || _stricmp(v, "&T") == 0 || _stricmp(v, "1") == 0);
                else if (_stricmp(k, "dry_run") == 0) dry_run = (_stricmp(v, "true") == 0 || _stricmp(v, "&T") == 0 || _stricmp(v, "1") == 0);
            }
        }
        dense_de_wipe_desktops(targets, purge_configs, dry_run);
        return true;
    }

    // 27. Desktop Environment Deployment Block (deploy/)
    if (_stricmp(block_name, "deploy") == 0 || _stricmp(block_name, "installer") == 0) {
        char target_dir[256] = "dist_ubuntu_de";
        for (int i = 0; i < line_count; i++) {
            char *line = trim_str(lines[i]);
            char *eq = strchr(line, '=');
            if (eq) {
                *eq = '\0';
                char *k = trim_str(line);
                char *v = trim_str(eq + 1);
                if (_stricmp(k, "target") == 0 || _stricmp(k, "dir") == 0 || _stricmp(k, "target_dir") == 0) {
                    strncpy(target_dir, v, sizeof(target_dir) - 1);
                } else if (_stricmp(k, "distro") == 0) {
                    strncpy(st->deploy.distro, v, sizeof(st->deploy.distro) - 1);
                } else if (_stricmp(k, "autologin") == 0) {
                    st->deploy.autologin = (_stricmp(v, "true") == 0 || _stricmp(v, "&T") == 0 || _stricmp(v, "1") == 0);
                }
            }
        }
        dense_de_deploy_ubuntu(target_dir);
        return true;
    }

    return false;
}

// -------------------------------------------------------------
// Dispatcher for DE built-in commands
// -------------------------------------------------------------
DenseValue dense_de_dispatch_command(DenseRuntime *rt, const char *action, char args[][MAX_TOKEN_LEN], int arg_count) {
    dense_de_init(rt);
    DESystemState *st = dense_de_get_state();

    // 1. Panel operations (panel, panel_create, CX, Pp)
    if (_stricmp(action, "panel") == 0 || _stricmp(action, "panel_create") == 0 ||
        strcmp(action, "CX") == 0 || strcmp(action, "Pp") == 0) {
        DEPosition pos = DE_POS_BOTTOM;
        int height = 36;
        DETheme theme = DE_THEME_DARK;
        if (arg_count >= 1) {
            DenseValue v0 = dense_eval_expr(rt, args[0]);
            if (strstr(v0.str_val, "top") || strstr(v0.str_val, "&TN")) pos = DE_POS_TOP;
            else if (strstr(v0.str_val, "left") || strstr(v0.str_val, "&xR")) pos = DE_POS_LEFT;
            else if (strstr(v0.str_val, "right") || strstr(v0.str_val, "&xS")) pos = DE_POS_RIGHT;
        }
        if (arg_count >= 2) {
            DenseValue v1 = dense_eval_expr(rt, args[1]);
            height = (int)v1.int_val;
        }
        if (arg_count >= 3) {
            DenseValue v2 = dense_eval_expr(rt, args[2]);
            if (strstr(v2.str_val, "light") || strstr(v2.str_val, "&Rx")) theme = DE_THEME_LIGHT;
            else if (strstr(v2.str_val, "trans") || strstr(v2.str_val, "&eJ")) theme = DE_THEME_TRANS;
        }
        int p_idx = dense_de_panel_create(pos, height, theme);
        return dense_val_str(st->panels[p_idx].id);
    }

    // 2. Panel widgets (panel_widget, 4b)
    if (_stricmp(action, "panel_widget") == 0 || strcmp(action, "4b") == 0) {
        if (st->panel_count == 0) dense_de_panel_create(DE_POS_BOTTOM, 36, DE_THEME_DARK);
        int p_idx = st->panel_count - 1;
        for (int i = 0; i < arg_count; i++) {
            DenseValue w = dense_eval_expr(rt, args[i]);
            dense_de_panel_add_widget(p_idx, w.str_val);
        }
        return dense_val_int(st->panels[p_idx].widget_count);
    }

    // 3. Panel show (panel_show, IM)
    if (_stricmp(action, "panel_show") == 0 || strcmp(action, "IM") == 0) {
        if (st->panel_count > 0) dense_de_panel_show(st->panel_count - 1);
        return dense_val_bool(true);
    }

    // 4. Dock operations (dock, dock_create, 9W, kT)
    if (_stricmp(action, "dock") == 0 || _stricmp(action, "dock_create") == 0 ||
        strcmp(action, "9W") == 0 || strcmp(action, "kT") == 0) {
        DEPosition pos = DE_POS_BOTTOM;
        int size = 48;
        bool autohide = false;
        if (arg_count >= 1) {
            DenseValue v0 = dense_eval_expr(rt, args[0]);
            if (strstr(v0.str_val, "top") || strstr(v0.str_val, "&TN")) pos = DE_POS_TOP;
        }
        if (arg_count >= 2) {
            DenseValue v1 = dense_eval_expr(rt, args[1]);
            size = (int)v1.int_val;
        }
        if (arg_count >= 3) {
            DenseValue v2 = dense_eval_expr(rt, args[2]);
            autohide = dense_val_is_truthy(v2);
        }
        dense_de_dock_create(pos, size, autohide);
        return dense_val_str("@dock");
    }

    // 5. Dock app (dock_app, 0c)
    if (_stricmp(action, "dock_app") == 0 || strcmp(action, "0c") == 0) {
        for (int i = 0; i < arg_count; i++) {
            DenseValue a = dense_eval_expr(rt, args[i]);
            dense_de_dock_add_app(a.str_val);
        }
        return dense_val_int(st->dock.app_count);
    }

    // 6. Dock show (dock_show, 7Y)
    if (_stricmp(action, "dock_show") == 0 || strcmp(action, "7Y") == 0) {
        dense_de_dock_show();
        return dense_val_bool(true);
    }

    // 7. Wallpaper set (wallpaper, wallpaper_set, Am, S7)
    if (_stricmp(action, "wallpaper") == 0 || _stricmp(action, "wallpaper_set") == 0 ||
        strcmp(action, "Am") == 0 || strcmp(action, "S7") == 0) {
        if (arg_count >= 1) {
            DenseValue p = dense_eval_expr(rt, args[0]);
            DEWallpaperMode mode = DE_WP_FILL;
            if (arg_count >= 2) {
                DenseValue m = dense_eval_expr(rt, args[1]);
                if (strstr(m.str_val, "fit") || strstr(m.str_val, "&ca")) mode = DE_WP_FIT;
                else if (strstr(m.str_val, "tile") || strstr(m.str_val, "&Qf")) mode = DE_WP_TILE;
            }
            dense_de_wallpaper_set(p.str_val, mode);
            return dense_val_bool(true);
        }
        return dense_val_bool(false);
    }

    // 8. Notifications (notify, notify_send, fc, If)
    if (_stricmp(action, "notify") == 0 || _stricmp(action, "notify_send") == 0 ||
        strcmp(action, "fc") == 0 || strcmp(action, "If") == 0) {
        if (arg_count >= 2) {
            DenseValue title = dense_eval_expr(rt, args[0]);
            DenseValue body = dense_eval_expr(rt, args[1]);
            int urg = 1;
            if (arg_count >= 3) {
                DenseValue u = dense_eval_expr(rt, args[2]);
                if (strstr(u.str_val, "urgent") || strstr(u.str_val, "&9g")) urg = 2;
                else if (strstr(u.str_val, "low") || strstr(u.str_val, "&jg")) urg = 0;
            }
            dense_de_notify(title.str_val, body.str_val, urg);
            return dense_val_bool(true);
        }
        return dense_val_bool(false);
    }

    // 9. OSD show (osd, osd_show, zN, d9)
    if (_stricmp(action, "osd") == 0 || _stricmp(action, "osd_show") == 0 ||
        strcmp(action, "zN") == 0 || strcmp(action, "d9") == 0) {
        if (arg_count >= 2) {
            DenseValue t = dense_eval_expr(rt, args[0]);
            DenseValue v = dense_eval_expr(rt, args[1]);
            dense_de_osd_show(t.str_val, (int)v.int_val);
            return dense_val_bool(true);
        }
        return dense_val_bool(false);
    }

    // 10. Toplevel / window controls
    if (_stricmp(action, "toplevel_list") == 0 || strcmp(action, "ti") == 0) {
        char buf[MAX_TOPLEVELS][128];
        int n = dense_de_toplevel_list(buf, MAX_TOPLEVELS);
        return dense_val_int(n);
    }
    if (_stricmp(action, "toplevel_focus") == 0 || strcmp(action, "Va") == 0) {
        const char *t = (arg_count >= 1) ? args[0] : "active";
        return dense_val_bool(dense_de_toplevel_focus(t));
    }
    if (_stricmp(action, "toplevel_close") == 0 || strcmp(action, "gf") == 0) {
        const char *t = (arg_count >= 1) ? args[0] : "active";
        return dense_val_bool(dense_de_toplevel_close(t));
    }
    if (_stricmp(action, "toplevel_maximize") == 0 || strcmp(action, "hg") == 0) {
        const char *t = (arg_count >= 1) ? args[0] : "active";
        return dense_val_bool(dense_de_toplevel_maximize(t));
    }
    if (_stricmp(action, "toplevel_minimize") == 0 || strcmp(action, "cC") == 0) {
        const char *t = (arg_count >= 1) ? args[0] : "active";
        return dense_val_bool(dense_de_toplevel_minimize(t));
    }
    if (_stricmp(action, "toplevel_fullscreen") == 0 || strcmp(action, "i7") == 0) {
        const char *t = (arg_count >= 1) ? args[0] : "active";
        return dense_val_bool(dense_de_toplevel_fullscreen(t));
    }

    // 11. Workspace controls
    if (_stricmp(action, "workspace_switch") == 0 || strcmp(action, "JF") == 0) {
        int ws = 1;
        if (arg_count >= 1) {
            DenseValue v = dense_eval_expr(rt, args[0]);
            ws = (int)v.int_val;
        }
        return dense_val_bool(dense_de_workspace_switch(ws));
    }
    if (_stricmp(action, "workspace_list") == 0 || strcmp(action, "V7") == 0) {
        return dense_val_int(st->labwc.workspaces);
    }

    // 12. Labwc controls
    if (_stricmp(action, "labwc_reload") == 0 || strcmp(action, "PQ") == 0) {
        return dense_val_bool(dense_de_labwc_reload());
    }
    if (_stricmp(action, "labwc_command") == 0 || strcmp(action, "4A") == 0) {
        if (arg_count >= 1) {
            DenseValue cmd = dense_eval_expr(rt, args[0]);
            return dense_val_bool(dense_de_labwc_command(cmd.str_val));
        }
        return dense_val_bool(false);
    }
    if (_stricmp(action, "windowrule_add") == 0) {
        if (arg_count >= 2) {
            return dense_val_bool(dense_de_labwc_add_rule(args[0], args[1]));
        }
        return dense_val_bool(false);
    }

    // 13. DE Export / Build / Start / Status
    if (_stricmp(action, "de_export") == 0 || _stricmp(action, "de_build") == 0 ||
        strcmp(action, "0j") == 0 || strcmp(action, "eS") == 0) {
        const char *dir = (arg_count >= 1) ? args[0] : "output_de";
        return dense_val_bool(dense_de_export_config(dir));
    }
    if (_stricmp(action, "de_start") == 0 || strcmp(action, "PK") == 0) {
        const char *dir = (arg_count >= 1) ? args[0] : "output_de";
        return dense_val_bool(dense_de_start_session(dir));
    }
    if (_stricmp(action, "de_status") == 0 || strcmp(action, "Xs") == 0) {
        printf("[Dense DE Status] Panels:%d Dock:%s Wallpaper:%s Toplevels:%d Workspaces:%d Windows:%d Boxes:%d Lock:%s\n",
               st->panel_count, st->dock.is_active ? "active" : "inactive",
               st->wallpaper.is_set ? "set" : "none", st->toplevel_count,
               st->labwc.workspaces, st->window_count, st->box_count,
               st->session_locked ? "locked" : "unlocked");
        return dense_val_int(st->panel_count);
    }

    // 14. Session controls
    if (_stricmp(action, "session_lock") == 0 || strcmp(action, "PL") == 0) {
        return dense_val_bool(dense_de_session_lock());
    }
    if (_stricmp(action, "session_logout") == 0 || strcmp(action, "3r") == 0) {
        return dense_val_bool(dense_de_session_logout());
    }
    if (_stricmp(action, "session_reboot") == 0 || strcmp(action, "7D") == 0) {
        return dense_val_bool(dense_de_session_reboot());
    }
    if (_stricmp(action, "session_poweroff") == 0 || strcmp(action, "sk") == 0) {
        return dense_val_bool(dense_de_session_poweroff());
    }

    // 15. Sysstat
    if (_stricmp(action, "sysstat") == 0 || strcmp(action, "Ly") == 0) {
        const char *m = (arg_count >= 1) ? args[0] : "cpu";
        return dense_val_int(dense_de_sysstat_get(m));
    }

    // 16. QML Replacement: Box Layouts (box, box_create, box_child)
    if (_stricmp(action, "box") == 0 || _stricmp(action, "box_create") == 0) {
        DELayoutType t = DE_LAYOUT_COL;
        int spacing = 8; int padding = 8;
        if (arg_count >= 1) {
            DenseValue v0 = dense_eval_expr(rt, args[0]);
            if (strstr(v0.str_val, "row")) t = DE_LAYOUT_ROW;
            else if (strstr(v0.str_val, "grid")) t = DE_LAYOUT_GRID;
            else if (strstr(v0.str_val, "stack")) t = DE_LAYOUT_STACK;
        }
        if (arg_count >= 2) spacing = (int)dense_eval_expr(rt, args[1]).int_val;
        if (arg_count >= 3) padding = (int)dense_eval_expr(rt, args[2]).int_val;
        int b_idx = dense_de_box_create(t, spacing, padding);
        return dense_val_str(st->boxes[b_idx].id);
    }
    if (_stricmp(action, "box_child") == 0) {
        if (st->box_count > 0 && arg_count >= 1) {
            for (int i = 0; i < arg_count; i++) {
                DenseValue c = dense_eval_expr(rt, args[i]);
                dense_de_box_add_child(st->box_count - 1, c.str_val);
            }
            return dense_val_int(st->boxes[st->box_count - 1].child_count);
        }
        return dense_val_int(0);
    }

    // 17. QML Replacement: Widgets (widget_create, widget_set)
    if (_stricmp(action, "widget_create") == 0) {
        DEElementType et = DE_ELEM_LABEL;
        const char *txt = (arg_count >= 2) ? args[1] : "";
        const char *ico = (arg_count >= 3) ? args[2] : "";
        if (arg_count >= 1) {
            if (strstr(args[0], "button")) et = DE_ELEM_BUTTON;
            else if (strstr(args[0], "slider")) et = DE_ELEM_SLIDER;
            else if (strstr(args[0], "progress")) et = DE_ELEM_PROGRESS;
            else if (strstr(args[0], "toggle")) et = DE_ELEM_TOGGLE;
            else if (strstr(args[0], "entry")) et = DE_ELEM_ENTRY;
            else if (strstr(args[0], "icon")) et = DE_ELEM_ICON;
        }
        int w_idx = dense_de_widget_create(et, txt, ico);
        return dense_val_str(st->elements[w_idx].id);
    }

    // 18. QML Replacement: Windows / Surfaces (window_create, window_show)
    if (_stricmp(action, "window_create") == 0) {
        const char *t = (arg_count >= 1) ? args[0] : "Window";
        int w = (arg_count >= 2) ? (int)dense_eval_expr(rt, args[1]).int_val : 400;
        int h = (arg_count >= 3) ? (int)dense_eval_expr(rt, args[2]).int_val : 300;
        int win_idx = dense_de_window_create(t, w, h, DE_LAYER_TOP);
        return dense_val_str(st->windows[win_idx].id);
    }
    if (_stricmp(action, "window_show") == 0) {
        if (st->window_count > 0) dense_de_window_show(st->window_count - 1);
        return dense_val_bool(true);
    }

    // 19. Python Replacement: Audio (audio_volume, audio_mute, volume)
    if (_stricmp(action, "audio_volume") == 0 || _stricmp(action, "volume") == 0 || strcmp(action, "fK") == 0) {
        if (arg_count >= 1) {
            DenseValue v = dense_eval_expr(rt, args[0]);
            dense_de_audio_set_volume((int)v.int_val);
            return v;
        }
        return dense_val_int(st->audio.volume);
    }
    if (_stricmp(action, "audio_mute") == 0) {
        bool m = (arg_count >= 1) ? dense_val_is_truthy(dense_eval_expr(rt, args[0])) : !st->audio.is_muted;
        dense_de_audio_set_mute(m);
        return dense_val_bool(m);
    }

    // 20. Python Replacement: Network (net_scan, net_status, net_connect)
    if (_stricmp(action, "net_status") == 0) {
        printf("[Dense Network] SSID:%s IP:%s Connected:%s Signal:%d%%\n",
               st->network.connected_ssid, st->network.ip_address,
               st->network.is_connected ? "true" : "false", st->network.signal_strength);
        return dense_val_str(st->network.connected_ssid);
    }
    if (_stricmp(action, "net_connect") == 0) {
        if (arg_count >= 1) {
            dense_de_net_connect(args[0]);
            return dense_val_bool(true);
        }
        return dense_val_bool(false);
    }

    // 21. Python Replacement: Battery & Power (battery_info, power_governor)
    if (_stricmp(action, "battery_info") == 0) {
        printf("[Dense Battery] %d%% (Charging:%s Governor:%s)\n",
               st->battery.percentage, st->battery.is_charging ? "yes" : "no", st->battery.governor);
        return dense_val_int(st->battery.percentage);
    }
    if (_stricmp(action, "power_governor") == 0) {
        if (arg_count >= 1) {
            strncpy(st->battery.governor, args[0], 31);
            printf("[Dense Power] Set CPU governor to: %s\n", args[0]);
            return dense_val_str(st->battery.governor);
        }
        return dense_val_str(st->battery.governor);
    }

    // 22. Python Replacement: Media player (media_play, media_pause, media_next, media_prev, media_info)
    if (_stricmp(action, "media_play") == 0) {
        dense_de_media_command("play");
        return dense_val_bool(true);
    }
    if (_stricmp(action, "media_pause") == 0) {
        dense_de_media_command("pause");
        return dense_val_bool(true);
    }
    if (_stricmp(action, "media_next") == 0) {
        dense_de_media_command("next");
        return dense_val_bool(true);
    }
    if (_stricmp(action, "media_prev") == 0) {
        dense_de_media_command("prev");
        return dense_val_bool(true);
    }
    if (_stricmp(action, "media_info") == 0) {
        printf("[Dense Media MPRIS2] %s - %s [%s]\n", st->media.artist, st->media.track_title, st->media.playback_status);
        return dense_val_str(st->media.track_title);
    }

    // 23. Python Replacement: D-Bus (dbus_call, dbus_get, dbus_set)
    if (_stricmp(action, "dbus_call") == 0) {
        const char *dest = (arg_count >= 1) ? args[0] : "";
        const char *method = (arg_count >= 2) ? args[1] : "";
        return dense_de_dbus_call("session", dest, "", "", method, args + 2, arg_count - 2);
    }
    if (_stricmp(action, "dbus_get") == 0) {
        const char *dest = (arg_count >= 1) ? args[0] : "";
        const char *prop = (arg_count >= 2) ? args[1] : "";
        return dense_de_dbus_get("session", dest, "", "", prop);
    }

    // 24. C / Wayland Replacement: Multi-monitors (monitor_list, monitor_config)
    if (_stricmp(action, "monitor_list") == 0) {
        for (int m = 0; m < st->monitor_count; m++) {
            printf("[Dense Monitor %d] %s: %dx%d@%dHz (Scale:%d)\n",
                   m, st->monitors[m].name, st->monitors[m].width, st->monitors[m].height,
                   st->monitors[m].refresh_rate, st->monitors[m].scale);
        }
        return dense_val_int(st->monitor_count);
    }

    // 25. QML Replacement: Theme & Style (style_set, style_get, 1V, HT)
    if (_stricmp(action, "style_set") == 0 || strcmp(action, "1V") == 0) {
        if (arg_count >= 2) {
            DenseValue k = dense_eval_expr(rt, args[0]);
            DenseValue v = dense_eval_expr(rt, args[1]);
            return dense_val_bool(dense_de_style_set(k.str_val, v.str_val));
        }
        return dense_val_bool(false);
    }
    if (_stricmp(action, "style_get") == 0 || strcmp(action, "HT") == 0) {
        if (arg_count >= 1) {
            DenseValue k = dense_eval_expr(rt, args[0]);
            return dense_de_style_get(k.str_val);
        }
        return dense_val_none();
    }

    // 26. QML Replacement: Animation & Transitions (animation_add, Ha)
    if (_stricmp(action, "animation_add") == 0 || strcmp(action, "Ha") == 0) {
        const char *target = (arg_count >= 1) ? args[0] : "item";
        const char *prop = (arg_count >= 2) ? args[1] : "opacity";
        int duration = (arg_count >= 3) ? (int)dense_eval_expr(rt, args[2]).int_val : 200;
        const char *easing = (arg_count >= 4) ? args[3] : "ease_in_out";
        int from_v = (arg_count >= 5) ? (int)dense_eval_expr(rt, args[4]).int_val : 0;
        int to_v = (arg_count >= 6) ? (int)dense_eval_expr(rt, args[5]).int_val : 100;
        return dense_val_bool(dense_de_animation_add(target, prop, duration, easing, from_v, to_v));
    }

    // 27. QML Replacement: State Transitions (state_set, JL)
    if (_stricmp(action, "state_set") == 0 || strcmp(action, "JL") == 0) {
        if (arg_count >= 4) {
            return dense_val_bool(dense_de_state_set(args[0], args[1], args[2], args[3]));
        }
        return dense_val_bool(false);
    }

    // 28. QML Replacement: Dynamic Repeater / ListView (repeater_create, Bw)
    if (_stricmp(action, "repeater_create") == 0 || strcmp(action, "Bw") == 0) {
        const char *model = (arg_count >= 1) ? args[0] : "toplevels";
        const char *delegate = (arg_count >= 2) ? args[1] : "task_item";
        printf("[Dense Repeater] Dynamic items created: Model=%s Delegate=%s Count=%d\n",
               model, delegate, st->toplevel_count);
        return dense_val_int(st->toplevel_count);
    }

    // 29. Python Replacement: Timers (timer_start, timer_stop, pj, el)
    if (_stricmp(action, "timer_start") == 0 || strcmp(action, "pj") == 0) {
        int interval = (arg_count >= 1) ? (int)dense_eval_expr(rt, args[0]).int_val : 1000;
        bool repeat = (arg_count >= 2) ? dense_val_is_truthy(dense_eval_expr(rt, args[1])) : true;
        const char *act = (arg_count >= 3) ? args[2] : "tick";
        int tid = dense_de_timer_add(interval, repeat, act);
        return dense_val_int(tid);
    }
    if (_stricmp(action, "timer_stop") == 0 || strcmp(action, "el") == 0) {
        int tid = (arg_count >= 1) ? (int)dense_eval_expr(rt, args[0]).int_val : 0;
        return dense_val_bool(dense_de_timer_stop(tid));
    }

    // 30. Python Replacement: Services (service_start, service_stop, service_restart, service_status, EV, ri, bN, Vr)
    if (_stricmp(action, "service_start") == 0 || strcmp(action, "EV") == 0) {
        const char *name = (arg_count >= 1) ? args[0] : "pipewire";
        return dense_val_bool(dense_de_service_control(name, "start"));
    }
    if (_stricmp(action, "service_stop") == 0 || strcmp(action, "ri") == 0) {
        const char *name = (arg_count >= 1) ? args[0] : "pipewire";
        return dense_val_bool(dense_de_service_control(name, "stop"));
    }
    if (_stricmp(action, "service_restart") == 0 || strcmp(action, "bN") == 0) {
        const char *name = (arg_count >= 1) ? args[0] : "pipewire";
        return dense_val_bool(dense_de_service_control(name, "restart"));
    }
    if (_stricmp(action, "service_status") == 0 || strcmp(action, "Vr") == 0) {
        const char *name = (arg_count >= 1) ? args[0] : "pipewire";
        return dense_val_str(dense_de_service_status(name));
    }

    // 31. Python Replacement: Hardware sensors (cpu_temp, mem_used, disk_free, iX, 4Q, eK)
    if (_stricmp(action, "cpu_temp") == 0 || strcmp(action, "iX") == 0) {
        return dense_val_int(48); // 48 deg C
    }
    if (_stricmp(action, "mem_used") == 0 || strcmp(action, "4Q") == 0) {
        return dense_val_int(3420); // 3420 MB
    }
    if (_stricmp(action, "disk_free") == 0 || strcmp(action, "eK") == 0) {
        return dense_val_int(248); // 248 GB
    }

    // 32. Python Replacement: Bluetooth (bluetooth_pair, bluetooth_unpair, bluetooth_status, Bc, 6g)
    if (_stricmp(action, "bluetooth_pair") == 0 || strcmp(action, "Bc") == 0) {
        const char *dev = (arg_count >= 1) ? args[0] : "Headphones";
        printf("[Dense Bluetooth] Paired and connected to: %s\n", dev);
        return dense_val_bool(true);
    }
    if (_stricmp(action, "bluetooth_unpair") == 0 || strcmp(action, "6g") == 0) {
        const char *dev = (arg_count >= 1) ? args[0] : "Headphones";
        printf("[Dense Bluetooth] Unpaired device: %s\n", dev);
        return dense_val_bool(true);
    }
    if (_stricmp(action, "bluetooth_status") == 0) {
        return dense_val_str("connected");
    }

    // 33. C / Wayland Replacement: Gestures (gesture_bind, gesture_trigger, Zs, K6)
    if (_stricmp(action, "gesture_bind") == 0 || strcmp(action, "Zs") == 0) {
        if (arg_count >= 2) {
            return dense_val_bool(dense_de_gesture_add(args[0], args[1]));
        }
        return dense_val_bool(false);
    }
    if (_stricmp(action, "gesture_trigger") == 0 || strcmp(action, "K6") == 0) {
        const char *trig = (arg_count >= 1) ? args[0] : "swipe3_up";
        return dense_val_bool(dense_de_gesture_trigger(trig));
    }

    // 34. C / Wayland Replacement: Compositor Hardware Settings (compositor_set, compositor_get, mE, vF)
    if (_stricmp(action, "compositor_set") == 0 || strcmp(action, "mE") == 0) {
        if (arg_count >= 2) {
            return dense_val_bool(dense_de_compositor_set(args[0], args[1]));
        }
        return dense_val_bool(false);
    }
    if (_stricmp(action, "compositor_get") == 0 || strcmp(action, "vF") == 0) {
        if (arg_count >= 1) {
            return dense_de_compositor_get(args[0]);
        }
        return dense_val_none();
    }

    // 35. C / Wayland Replacement: LayerShell Surfaces (layershell_add, uA)
    if (_stricmp(action, "layershell_add") == 0 || strcmp(action, "uA") == 0) {
        const char *ns = (arg_count >= 1) ? args[0] : "dense_surface";
        DELayer lay = DE_LAYER_TOP;
        int anchor = 7;
        int excl = 36;
        if (arg_count >= 2) {
            if (strstr(args[1], "overlay")) lay = DE_LAYER_OVERLAY;
            else if (strstr(args[1], "bottom")) lay = DE_LAYER_BOTTOM;
        }
        if (arg_count >= 3) anchor = (int)dense_eval_expr(rt, args[2]).int_val;
        if (arg_count >= 4) excl = (int)dense_eval_expr(rt, args[3]).int_val;
        return dense_val_bool(dense_de_layershell_surface_add(ns, lay, anchor, excl));
    }

    // 36. Ubuntu DE Wiping: wipe_desktop, wipe_gnome, desktop_wipe (23, Te, Py)
    if (_stricmp(action, "wipe_desktop") == 0 || strcmp(action, "23") == 0 ||
        _stricmp(action, "wipe_gnome") == 0 || strcmp(action, "Te") == 0 ||
        _stricmp(action, "desktop_wipe") == 0 || strcmp(action, "Py") == 0) {
        const char *targets = (arg_count >= 1) ? args[0] : "gnome";
        bool purge_configs = (arg_count >= 2) ? (_stricmp(args[1], "true") == 0 || _stricmp(args[1], "&T") == 0) : true;
        bool dry_run = (arg_count >= 3) ? (_stricmp(args[2], "true") == 0 || _stricmp(args[2], "&T") == 0) : false;
        return dense_val_bool(dense_de_wipe_desktops(targets, purge_configs, dry_run));
    }

    // 37. Ubuntu DE Deployment: de_deploy, session_register (QI, Xy)
    if (_stricmp(action, "de_deploy") == 0 || strcmp(action, "QI") == 0) {
        const char *target_dir = (arg_count >= 1) ? args[0] : "dist_ubuntu_de";
        return dense_val_bool(dense_de_deploy_ubuntu(target_dir));
    }
    if (_stricmp(action, "session_register") == 0 || strcmp(action, "Xy") == 0) {
        const char *name = (arg_count >= 1) ? args[0] : "Dense Desktop";
        const char *exec_path = (arg_count >= 2) ? args[1] : "/usr/local/bin/dense-session";
        return dense_val_bool(dense_de_session_register(name, exec_path));
    }

    // 38. Package operations: pkg_install, pkg_purge, pkg_autoremove (tm, kE, Sm)
    if (_stricmp(action, "pkg_install") == 0 || strcmp(action, "tm") == 0) {
        return dense_val_bool(dense_de_pkg_op("install", args, arg_count));
    }
    if (_stricmp(action, "pkg_purge") == 0 || strcmp(action, "kE") == 0) {
        return dense_val_bool(dense_de_pkg_op("purge", args, arg_count));
    }
    if (_stricmp(action, "pkg_autoremove") == 0 || strcmp(action, "Sm") == 0) {
        return dense_val_bool(dense_de_pkg_op("autoremove", args, arg_count));
    }

    // 39. Session Power Control: sys_reboot, sys_poweroff
    if (_stricmp(action, "sys_reboot") == 0) {
        return dense_val_bool(dense_de_session_reboot());
    }
    if (_stricmp(action, "sys_poweroff") == 0) {
        return dense_val_bool(dense_de_session_poweroff());
    }

    return dense_val_none();
}
