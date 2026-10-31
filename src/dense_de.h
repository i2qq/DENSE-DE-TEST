#ifndef DENSE_DE_H
#define DENSE_DE_H

#include "dense.h"

#define MAX_PANEL_WIDGETS 16
#define MAX_DOCK_APPS 32
#define MAX_KEYBINDS 64
#define MAX_TOPLEVELS 32
#define MAX_PANELS 4
#define MAX_BOX_CHILDREN 16
#define MAX_BOXES 32
#define MAX_WIDGET_ELEMENTS 64
#define MAX_WINDOWS 16
#define MAX_MENU_ITEMS 16
#define MAX_WINDOW_RULES 32
#ifdef MAX_MONITORS
#undef MAX_MONITORS
#endif
#define MAX_MONITORS 8
#define MAX_ANIMATIONS 32
#define MAX_TIMERS 16
#define MAX_GESTURES 16
#define MAX_SERVICES 16
#define MAX_LAYERSHELL_SURFACES 8

typedef enum {
    DE_POS_TOP,
    DE_POS_BOTTOM,
    DE_POS_LEFT,
    DE_POS_RIGHT,
    DE_POS_CENTER
} DEPosition;

typedef enum {
    DE_LAYER_BACKGROUND,
    DE_LAYER_BOTTOM,
    DE_LAYER_TOP,
    DE_LAYER_OVERLAY
} DELayer;

typedef enum {
    DE_THEME_DARK,
    DE_THEME_LIGHT,
    DE_THEME_TRANS,
    DE_THEME_BLUR
} DETheme;

typedef enum {
    DE_WP_FILL,
    DE_WP_FIT,
    DE_WP_TILE,
    DE_WP_STRETCH,
    DE_WP_SPAN
} DEWallpaperMode;

typedef enum {
    DE_LAYOUT_ROW,
    DE_LAYOUT_COL,
    DE_LAYOUT_GRID,
    DE_LAYOUT_STACK
} DELayoutType;

typedef enum {
    DE_ELEM_LABEL,
    DE_ELEM_BUTTON,
    DE_ELEM_ENTRY,
    DE_ELEM_SLIDER,
    DE_ELEM_PROGRESS,
    DE_ELEM_ICON,
    DE_ELEM_TOGGLE,
    DE_ELEM_IMAGE,
    DE_ELEM_SEPARATOR
} DEElementType;

typedef struct {
    char id[32];
    DEElementType type;
    char text[128];
    char icon[64];
    int value; // for slider / progress / toggle (0/1)
    char on_click[128];
    char on_change[128];
    char bg_color[32];
    char fg_color[32];
    int border_radius;
} DEWidgetElement;

typedef struct {
    char id[32];
    DELayoutType type;
    int spacing;
    int padding;
    char children[MAX_BOX_CHILDREN][32];
    int child_count;
} DEBox;

typedef struct {
    char id[32];
    char title[128];
    int width;
    int height;
    DELayer layer;
    int anchor_mask; // 1=top, 2=bot, 4=left, 8=right
    char root_box_id[32];
    bool is_visible;
} DEWindow;

typedef struct {
    char id[32];
    char items[MAX_MENU_ITEMS][2][64]; // [0] = label, [1] = command
    int item_count;
} DEMenu;

typedef struct {
    char app_id[64];
    char title_pattern[64];
    int workspace;
    bool maximized;
    bool minimized;
    bool fullscreen;
    bool floating;
    char geometry[32];
} DEWindowRule;

typedef struct {
    char name[32];
    int width;
    int height;
    int refresh_rate;
    int scale;
    int pos_x;
    int pos_y;
} DEMonitor;

typedef struct {
    char tap_to_click[16];
    char natural_scroll[16];
    int accel_speed; // -100 to 100
    char kb_layout[32]; // us, de, etc.
    char kb_variant[32];
    int kb_repeat_rate;
} DEInputConfig;

typedef struct {
    char connected_ssid[64];
    char ip_address[48];
    bool is_connected;
    int signal_strength;
} DENetworkState;

typedef struct {
    int volume;
    bool is_muted;
    char default_sink[64];
} DEAudioState;

typedef struct {
    int percentage;
    bool is_charging;
    int time_to_empty;
    char governor[32]; // performance, powersave, balanced
} DEBatteryState;

typedef struct {
    char track_title[128];
    char artist[128];
    char playback_status[32]; // Playing, Paused, Stopped
} DEMediaState;

typedef struct {
    char id[32];
    DEPosition pos;
    DELayer layer;
    DETheme theme;
    int height;
    int width;
    bool exclusive_zone;
    char bg_color[32];
    char fg_color[32];
    int border_radius;
    char widgets[MAX_PANEL_WIDGETS][64];
    int widget_count;
    char output_monitor[64];
    bool is_active;
} DEPanel;

typedef struct {
    DEPosition pos;
    int icon_size;
    bool autohide;
    char pinned_apps[MAX_DOCK_APPS][64];
    int app_count;
    bool is_active;
} DEDock;

typedef struct {
    char path[256];
    DEWallpaperMode mode;
    char color[32];
    bool is_set;
} DEWallpaper;

typedef struct {
    char title[128];
    char body[256];
    char app_name[64];
    char icon[64];
    int timeout_ms;
    int urgency;
} DENotification;

typedef struct {
    char type[32];
    int value;
    bool visible;
} DEOSD;

typedef struct {
    char title[128];
    char app_id[64];
    int workspace;
    int state;
} DEToplevel;

typedef struct {
    char key[64];
    char action[128];
} DEKeybind;

typedef struct {
    char theme[64];
    int workspaces;
    int current_workspace;
    bool snap_to_edge;
    DEKeybind keybinds[MAX_KEYBINDS];
    int keybind_count;
    DEWindowRule rules[MAX_WINDOW_RULES];
    int rule_count;
    char autostart_cmds[16][128];
    int autostart_count;
} DELabwcConfig;

typedef struct {
    char bg_color[32];
    char fg_color[32];
    char accent_color[32];
    char border_color[32];
    int border_radius;
    char font_family[64];
    int font_size;
    bool drop_shadow;
    bool background_blur;
} DEThemeStyle;

typedef struct {
    char target[64];
    char property[32];
    int duration_ms;
    char easing[32]; // linear, ease_in, ease_out, ease_in_out, spring
    int from_val;
    int to_val;
} DEAnimation;

typedef struct {
    char target[64];
    char state_name[32];
    char bg_color[32];
    char fg_color[32];
    int scale_pct;
    int opacity_pct;
} DEWidgetState;

typedef struct {
    int id;
    int interval_ms;
    bool repeat;
    char action[128];
    bool active;
} DETimer;

typedef struct {
    char trigger[32]; // swipe3_up, swipe3_down, pinch_in, pinch_out
    char action[128];
} DEGesture;

typedef struct {
    char name[64];
    char status[32]; // running, stopped, failed
    bool enabled;
} DEService;

typedef struct {
    char namespace_name[64];
    DELayer layer;
    int anchor_mask; // 1=top, 2=bot, 4=left, 8=right
    int margin_top;
    int margin_bottom;
    int margin_left;
    int margin_right;
    int exclusive_zone;
    char keyboard_mode[32]; // none, exclusive, on_demand
} DELayerShellSurface;

typedef struct {
    int corner_radius;
    bool drop_shadows;
    bool animations;
    char decorations[32]; // server, client, none
    bool tearing;
    bool vsync;
} DECompositorSettings;

typedef struct {
    bool wipe_gnome;
    bool wipe_kde;
    bool wipe_xfce;
    bool wipe_cinnamon;
    bool wipe_mate;
    bool wipe_lxde;
    bool purge_configs;
    bool clean_sessions;
    bool dry_run;
    char keep_packages[16][64];
    int keep_count;
} DEWipeConfig;

typedef struct {
    char name[64];
    char comment[128];
    char compositor[32]; // labwc
    char exec_path[128]; // /usr/local/bin/dense-session
    char desktop_names[64]; // Dense;labwc
} DESessionConfig;

typedef struct {
    char distro[32]; // ubuntu
    char target_dir[256]; // /etc/dense or dist_ubuntu_de
    char session_file[256]; // /usr/share/wayland-sessions/dense.desktop
    char launcher_file[256]; // /usr/local/bin/dense-session
    bool autologin;
    char user[64];
    bool auto_wipe;
} DEDeployConfig;

typedef struct {
    DEPanel panels[MAX_PANELS];
    int panel_count;
    DEDock dock;
    DEWallpaper wallpaper;
    DENotification last_notification;
    DEOSD osd;
    DEToplevel toplevels[MAX_TOPLEVELS];
    int toplevel_count;
    DELabwcConfig labwc;
    bool session_locked;

    // Advanced QML/Python/C Replacement State
    DEBox boxes[MAX_BOXES];
    int box_count;
    DEWidgetElement elements[MAX_WIDGET_ELEMENTS];
    int element_count;
    DEWindow windows[MAX_WINDOWS];
    int window_count;
    DEMenu menus[8];
    int menu_count;
    DEMonitor monitors[MAX_MONITORS];
    int monitor_count;
    DEInputConfig input;
    DENetworkState network;
    DEAudioState audio;
    DEBatteryState battery;
    DEMediaState media;

    // Even More QML/Python/C Replacement Controls
    DEThemeStyle style;
    DEAnimation animations[MAX_ANIMATIONS];
    int animation_count;
    DEWidgetState states[32];
    int state_count;
    DETimer timers[MAX_TIMERS];
    int timer_count;
    DEGesture gestures[MAX_GESTURES];
    int gesture_count;
    DEService services[MAX_SERVICES];
    int service_count;
    DELayerShellSurface layersurfaces[MAX_LAYERSHELL_SURFACES];
    int layersurface_count;
    DECompositorSettings compositor;

    // Ubuntu Integration, Session Registration & Desktop Wiping
    DEWipeConfig wipe;
    DESessionConfig session;
    DEDeployConfig deploy;
} DESystemState;

// Core DE Engine API
void dense_de_init(DenseRuntime *rt);
DESystemState* dense_de_get_state(void);

// Theme Styling & Animation (QML Replacement)
bool dense_de_style_set(const char *key, const char *val);
DenseValue dense_de_style_get(const char *key);
bool dense_de_animation_add(const char *target, const char *prop, int duration_ms, const char *easing, int from_v, int to_v);
bool dense_de_state_set(const char *target, const char *state_name, const char *prop, const char *val);

// Timers & Services (Python Replacement)
int dense_de_timer_add(int interval_ms, bool repeat, const char *action);
bool dense_de_timer_stop(int timer_id);
bool dense_de_service_control(const char *name, const char *action);
const char* dense_de_service_status(const char *name);

// Wayland Gestures & LayerShell & Compositor (C Replacement)
bool dense_de_gesture_add(const char *trigger, const char *action);
bool dense_de_gesture_trigger(const char *trigger);
bool dense_de_layershell_surface_add(const char *ns, DELayer layer, int anchor_mask, int exclusive_zone);
bool dense_de_compositor_set(const char *key, const char *val);
DenseValue dense_de_compositor_get(const char *key);

// Panel & Dock
int dense_de_panel_create(DEPosition pos, int height, DETheme theme);
bool dense_de_panel_add_widget(int panel_idx, const char *widget_name);
bool dense_de_panel_set_style(int panel_idx, const char *key, const char *val);
bool dense_de_panel_show(int panel_idx);
bool dense_de_dock_create(DEPosition pos, int icon_size, bool autohide);
bool dense_de_dock_add_app(const char *app_id);
bool dense_de_dock_show(void);

// Wallpaper & Notifications & OSD
bool dense_de_wallpaper_set(const char *path, DEWallpaperMode mode);
bool dense_de_notify(const char *title, const char *body, int urgency);
bool dense_de_osd_show(const char *type, int value);

// Window Management / Toplevels
int dense_de_toplevel_list(char out_buf[][128], int max_out);
bool dense_de_toplevel_focus(const char *target);
bool dense_de_toplevel_close(const char *target);
bool dense_de_toplevel_maximize(const char *target);
bool dense_de_toplevel_minimize(const char *target);
bool dense_de_toplevel_fullscreen(const char *target);

// Workspace
bool dense_de_workspace_switch(int ws_idx);
int dense_de_workspace_get(void);

// Labwc Compositor integration
bool dense_de_labwc_reload(void);
bool dense_de_labwc_command(const char *cmd);
bool dense_de_labwc_add_keybind(const char *keychord, const char *action);
bool dense_de_labwc_add_rule(const char *app_id, const char *props);
bool dense_de_export_config(const char *target_dir);
bool dense_de_start_session(const char *config_dir);

// QML-Replacement UI API
int dense_de_box_create(DELayoutType type, int spacing, int padding);
bool dense_de_box_add_child(int box_idx, const char *child_id);
int dense_de_widget_create(DEElementType type, const char *text, const char *icon);
bool dense_de_widget_set(int widget_idx, const char *key, const char *val);
int dense_de_window_create(const char *title, int width, int height, DELayer layer);
bool dense_de_window_show(int win_idx);
int dense_de_menu_create(const char *menu_id);
bool dense_de_menu_add_item(int menu_idx, const char *label, const char *cmd);

// Python-Replacement System Services & D-Bus API
DenseValue dense_de_dbus_call(const char *bus, const char *dest, const char *path, const char *iface, const char *method, char args[][MAX_TOKEN_LEN], int arg_count);
DenseValue dense_de_dbus_get(const char *bus, const char *dest, const char *path, const char *iface, const char *prop);
bool dense_de_audio_set_volume(int vol);
bool dense_de_audio_set_mute(bool mute);
bool dense_de_net_connect(const char *ssid);
bool dense_de_media_command(const char *action);

// Session controls
bool dense_de_session_lock(void);
bool dense_de_session_logout(void);
bool dense_de_session_reboot(void);
bool dense_de_session_poweroff(void);
int dense_de_sysstat_get(const char *metric);

// Ubuntu Integration, Session Registration & Desktop Wiping API
bool dense_de_wipe_desktops(const char *targets, bool purge_configs, bool dry_run);
bool dense_de_session_register(const char *name, const char *exec_path);
bool dense_de_deploy_ubuntu(const char *target_dir);
bool dense_de_pkg_op(const char *op, char pkgs[][MAX_TOKEN_LEN], int pkg_count);

// Block syntax handler
bool dense_de_handle_block(DenseRuntime *rt, const char *block_name, char lines[][MAX_LINE_LEN], int line_count);

// Dispatcher for DE built-in commands
DenseValue dense_de_dispatch_command(DenseRuntime *rt, const char *action, char args[][MAX_TOKEN_LEN], int arg_count);

#endif // DENSE_DE_H
