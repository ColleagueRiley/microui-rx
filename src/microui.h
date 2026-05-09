/*
** Copyright (c) 2024 rxi
**				 2026 Riley Mabb (ColleagueRiley)
**
** This library is free software; you can redistribute it and/or modify it
** under the terms of the MIT license. See `microui.c` for details.
*/

#ifndef MICROUI_H
#define MICROUI_H

#define MU_VERSION "3.00-dev"

#ifndef MU_API
	#define MU_API
#endif

#include <stddef.h>

#ifdef MU_USE_INT /* optional for any system that might not have stdint.h */
	typedef unsigned char       mu_u8;
	typedef signed char         mu_i8;
	typedef unsigned short     mu_u16;
	typedef signed short 	   mu_i16;
	typedef unsigned long int  mu_u32;
	typedef signed long int    mu_i32;
	typedef unsigned long long mu_u64;
	typedef signed long long   mu_i64;
#else /* use stdint standard types instead of c "standard" types */
	#include <stdint.h>

	typedef uint8_t     mu_u8;
	typedef int8_t      mu_i8;
	typedef uint16_t   mu_u16;
	typedef int16_t    mu_i16;
	typedef uint32_t   mu_u32;
	typedef int32_t    mu_i32;
	typedef uint64_t   mu_u64;
	typedef int64_t    mu_i64;
#endif

#define MU_COMMANDLIST_SIZE     (256 * 1024)
#define MU_ROOTLIST_SIZE        32
#define MU_CONTAINERSTACK_SIZE  32
#define MU_CLIPSTACK_SIZE       32
#define MU_IDSTACK_SIZE         32
#define MU_LAYOUTSTACK_SIZE     16
#define MU_CONTAINERPOOL_SIZE   48
#define MU_TREENODEPOOL_SIZE    48
#define MU_MAX_WIDTHS           16
#define MU_REAL                 float
#define MU_REAL_FMT             "%.3g"
#define MU_SLIDER_FMT           "%.2f"
#define MU_MAX_FMT              127

#define mu_stack(T, n)          struct { mu_i32 idx; T items[n]; }
#define mu_min(a, b)            ((a) < (b) ? (a) : (b))
#define mu_max(a, b)            ((a) > (b) ? (a) : (b))
#define mu_clamp(x, a, b)       mu_min(b, mu_max(a, x))

enum {
  MU_CLIP_PART = 1,
  MU_CLIP_ALL
};

enum {
  MU_COMMAND_JUMP = 1,
  MU_COMMAND_CLIP,
  MU_COMMAND_RECT,
  MU_COMMAND_TEXT,
  MU_COMMAND_ICON,
  MU_COMMAND_MAX
};

enum {
  MU_COLOR_TEXT,
  MU_COLOR_BORDER,
  MU_COLOR_WINDOWBG,
  MU_COLOR_TITLEBG,
  MU_COLOR_TITLETEXT,
  MU_COLOR_PANELBG,
  MU_COLOR_BUTTON,
  MU_COLOR_BUTTONHOVER,
  MU_COLOR_BUTTONFOCUS,
  MU_COLOR_BASE,
  MU_COLOR_BASEHOVER,
  MU_COLOR_BASEFOCUS,
  MU_COLOR_SCROLLBASE,
  MU_COLOR_SCROLLTHUMB,
  MU_COLOR_MAX
};

enum {
  MU_ICON_CLOSE = 1,
  MU_ICON_CHECK,
  MU_ICON_COLLAPSED,
  MU_ICON_EXPANDED,
  MU_ICON_MAX
};

enum {
  MU_RES_ACTIVE       = (1 << 0),
  MU_RES_SUBMIT       = (1 << 1),
  MU_RES_CHANGE       = (1 << 2)
};

enum {
  MU_OPT_ALIGNCENTER  = (1 << 0),
  MU_OPT_ALIGNRIGHT   = (1 << 1),
  MU_OPT_NOINTERACT   = (1 << 2),
  MU_OPT_NOFRAME      = (1 << 3),
  MU_OPT_NORESIZE     = (1 << 4),
  MU_OPT_NOSCROLL     = (1 << 5),
  MU_OPT_NOCLOSE      = (1 << 6),
  MU_OPT_NOTITLE      = (1 << 7),
  MU_OPT_HOLDFOCUS    = (1 << 8),
  MU_OPT_AUTOSIZE     = (1 << 9),
  MU_OPT_POPUP        = (1 << 10),
  MU_OPT_CLOSED       = (1 << 11),
  MU_OPT_EXPANDED     = (1 << 12)
};

enum {
  MU_MOUSE_LEFT       = (1 << 0),
  MU_MOUSE_RIGHT      = (1 << 1),
  MU_MOUSE_MIDDLE     = (1 << 2)
};

enum {
  MU_KEY_SHIFT        = (1 << 0),
  MU_KEY_CTRL         = (1 << 1),
  MU_KEY_ALT          = (1 << 2),
  MU_KEY_BACKSPACE    = (1 << 3),
  MU_KEY_RETURN       = (1 << 4)
};


typedef struct mu_Context mu_Context;
typedef mu_u32 mu_Id;
typedef MU_REAL mu_Real;
typedef void* mu_Font;

typedef struct { mu_i32 x, y; } mu_Vec2;
typedef struct { mu_i32 x, y, w, h; } mu_Rect;
typedef struct { mu_u8 r, g, b, a; } mu_Color;
typedef struct { mu_Id id; mu_i32 last_update; } mu_PoolItem;
typedef struct { char* data; size_t length; } mu_String;

#define mu_vec2(x, y) (mu_Vec2){(x), (y)}
#define mu_rect(x, y, w, h) (mu_Rect){(x), (y), (w), (h)}
#define mu_color(r, g, b, a) (mu_Color){(r), (g), (b), (a)}
#define mu_String(str, length) (mu_String){ str, length }

typedef struct { mu_i32 type, size; } mu_BaseCommand;
typedef struct { mu_BaseCommand base; void *dst; } mu_JumpCommand;
typedef struct { mu_BaseCommand base; mu_Rect rect; } mu_ClipCommand;
typedef struct { mu_BaseCommand base; mu_Rect rect; mu_Color color; } mu_RectCommand;
typedef struct { mu_BaseCommand base; mu_Font font; mu_Vec2 pos; mu_Color color; char str[1]; } mu_TextCommand;
typedef struct { mu_BaseCommand base; mu_Rect rect; mu_i32 id; mu_Color color; } mu_IconCommand;

typedef union {
  mu_i32 type;
  mu_BaseCommand base;
  mu_JumpCommand jump;
  mu_ClipCommand clip;
  mu_RectCommand rect;
  mu_TextCommand text;
  mu_IconCommand icon;
} mu_Command;

typedef struct {
  mu_Rect body;
  mu_Rect next;
  mu_Vec2 position;
  mu_Vec2 size;
  mu_Vec2 max;
  mu_i32 widths[MU_MAX_WIDTHS];
  mu_i32 items;
  mu_i32 item_index;
  mu_i32 next_row;
  mu_i32 next_type;
  mu_i32 indent;
} mu_Layout;

typedef struct {
  mu_Command *head, *tail;
  mu_Rect rect;
  mu_Rect body;
  mu_Vec2 content_size;
  mu_Vec2 scroll;
  mu_i32 zindex;
  mu_i32 open;
} mu_Container;

typedef struct {
  mu_Font font;
  mu_Vec2 size;
  mu_i32 padding;
  mu_i32 spacing;
  mu_i32 indent;
  mu_i32 title_height;
  mu_i32 scrollbar_size;
  mu_i32 thumb_size;
  mu_Color colors[MU_COLOR_MAX];
} mu_Style;

struct mu_Context {
  /* callbacks */
  mu_i32 (*text_width)(mu_Font font, const char *str, mu_i32 len);
  mu_i32 (*text_height)(mu_Font font);
  void (*draw_frame)(mu_Context *ctx, mu_Rect rect, mu_i32 colorid);
  /* core state */
  mu_Style _style;
  mu_Style *style;
  mu_Id hover;
  mu_Id focus;
  mu_Id last_id;
  mu_Rect last_rect;
  mu_i32 last_zindex;
  mu_i32 updated_focus;
  mu_i32 frame;
  mu_Container *hover_root;
  mu_Container *next_hover_root;
  mu_Container *scroll_target;
  char number_edit_buf[MU_MAX_FMT];
  mu_Id number_edit;
  /* stacks */
  mu_stack(char, MU_COMMANDLIST_SIZE) command_list;
  mu_stack(mu_Container*, MU_ROOTLIST_SIZE) root_list;
  mu_stack(mu_Container*, MU_CONTAINERSTACK_SIZE) container_stack;
  mu_stack(mu_Rect, MU_CLIPSTACK_SIZE) clip_stack;
  mu_stack(mu_Id, MU_IDSTACK_SIZE) id_stack;
  mu_stack(mu_Layout, MU_LAYOUTSTACK_SIZE) layout_stack;
  /* retained state pools */
  mu_PoolItem container_pool[MU_CONTAINERPOOL_SIZE];
  mu_Container containers[MU_CONTAINERPOOL_SIZE];
  mu_PoolItem treenode_pool[MU_TREENODEPOOL_SIZE];
  /* input state */
  mu_Vec2 mouse_pos;
  mu_Vec2 last_mouse_pos;
  mu_Vec2 mouse_delta;
  mu_Vec2 scroll_delta;
  mu_i32 mouse_down;
  mu_i32 mouse_pressed;
  mu_i32 key_down;
  mu_i32 key_pressed;
  char input_text[32];
};

MU_API void mu_init(mu_Context *ctx);
MU_API void mu_begin(mu_Context *ctx);
MU_API void mu_end(mu_Context *ctx);
MU_API void mu_set_focus(mu_Context *ctx, mu_Id id);
MU_API mu_Id mu_get_id(mu_Context *ctx, const void *data, mu_i32 size);
MU_API void mu_push_id(mu_Context *ctx, const void *data, mu_i32 size);
MU_API void mu_pop_id(mu_Context *ctx);
MU_API void mu_push_clip_rect(mu_Context *ctx, mu_Rect rect);
MU_API void mu_pop_clip_rect(mu_Context *ctx);
MU_API void mu_get_clip_rect(mu_Context *ctx, mu_Rect* rect_output);
MU_API mu_i32 mu_check_clip(mu_Context *ctx, mu_Rect r);
MU_API mu_Container* mu_get_current_container(mu_Context *ctx);
MU_API mu_Container* mu_get_container(mu_Context *ctx, const char *name);
MU_API void mu_bring_to_front(mu_Context *ctx, mu_Container *cnt);

MU_API mu_i32 mu_pool_init(mu_Context *ctx, mu_PoolItem *items, mu_i32 len, mu_Id id);
MU_API mu_i32 mu_pool_get(mu_Context *ctx, mu_PoolItem *items, mu_i32 len, mu_Id id);
MU_API void mu_pool_update(mu_Context *ctx, mu_PoolItem *items, mu_i32 idx);

MU_API void mu_input_mousemove(mu_Context *ctx, mu_i32 x, mu_i32 y);
MU_API void mu_input_mousedown(mu_Context *ctx, mu_i32 btn);
MU_API void mu_input_mouseup(mu_Context *ctx, mu_i32 btn);
MU_API void mu_input_scroll(mu_Context *ctx, mu_i32 x, mu_i32 y);
MU_API void mu_input_keydown(mu_Context *ctx, mu_i32 key);
MU_API void mu_input_keyup(mu_Context *ctx, mu_i32 key);
MU_API void mu_input_text(mu_Context *ctx, const char *text);

MU_API mu_Command* mu_push_command(mu_Context *ctx, mu_i32 type, mu_i32 size);
MU_API mu_i32 mu_next_command(mu_Context *ctx, mu_Command **cmd);
MU_API void mu_set_clip(mu_Context *ctx, mu_Rect rect);
MU_API void mu_draw_rect(mu_Context *ctx, mu_Rect rect, mu_Color color);
MU_API void mu_draw_box(mu_Context *ctx, mu_Rect rect, mu_Color color);
MU_API void mu_draw_text(mu_Context *ctx, mu_Font font, const char *str, mu_i32 len, mu_Vec2 pos, mu_Color color);
MU_API void mu_draw_icon(mu_Context *ctx, mu_i32 id, mu_Rect rect, mu_Color color);

MU_API void mu_layout_row(mu_Context *ctx, mu_i32 items, const mu_i32 *widths, mu_i32 height);
MU_API void mu_layout_width(mu_Context *ctx, mu_i32 width);
MU_API void mu_layout_height(mu_Context *ctx, mu_i32 height);
MU_API void mu_layout_begin_column(mu_Context *ctx);
MU_API void mu_layout_end_column(mu_Context *ctx);
MU_API void mu_layout_set_next(mu_Context *ctx, mu_Rect r, mu_i32 relative);
MU_API void mu_layout_get_position(mu_Context* ctx, mu_Vec2* vec);
MU_API void mu_layout_next(mu_Context *ctx, mu_Rect* rect_output);

MU_API void mu_draw_control_frame(mu_Context *ctx, mu_Id id, mu_Rect rect, mu_i32 colorid, mu_i32 opt);
MU_API void mu_draw_control_text(mu_Context *ctx, const char *str, mu_Rect rect, mu_i32 colorid, mu_i32 opt);
MU_API mu_i32 mu_mouse_over(mu_Context *ctx, mu_Rect rect);
MU_API void mu_update_control(mu_Context *ctx, mu_Id id, mu_Rect rect, mu_i32 opt);

#define mu_button(ctx, label)             mu_button_ex(ctx, label, 0, MU_OPT_ALIGNCENTER)
#define mu_textbox(ctx, buf, bufsz)       mu_textbox_ex(ctx, buf, bufsz, 0)
#define mu_slider(ctx, value, lo, hi)     mu_slider_ex(ctx, value, lo, hi, 0, MU_SLIDER_FMT, MU_OPT_ALIGNCENTER)
#define mu_number(ctx, value, step)       mu_number_ex(ctx, value, step, MU_SLIDER_FMT, MU_OPT_ALIGNCENTER)
#define mu_header(ctx, label)             mu_header_ex(ctx, label, 0)
#define mu_begin_treenode(ctx, label)     mu_begin_treenode_ex(ctx, label, 0)
#define mu_begin_window(ctx, title, rect) mu_begin_window_ex(ctx, title, rect, 0)
#define mu_begin_panel(ctx, name)         mu_begin_panel_ex(ctx, name, 0)

MU_API void mu_text(mu_Context *ctx, const char *text);
MU_API void mu_label(mu_Context *ctx, const char *text);
MU_API mu_i32 mu_button_ex(mu_Context *ctx, const char *label, mu_i32 icon, mu_i32 opt);
MU_API mu_i32 mu_checkbox(mu_Context *ctx, const char *label, mu_i32 *state);
MU_API mu_i32 mu_textbox_raw(mu_Context *ctx, char *buf, mu_i32 bufsz, mu_Id id, mu_Rect r, mu_i32 opt);
MU_API mu_i32 mu_textbox_ex(mu_Context *ctx, char *buf, mu_i32 bufsz, mu_i32 opt);
MU_API mu_i32 mu_slider_ex(mu_Context *ctx, mu_Real *value, mu_Real low, mu_Real high, mu_Real step, const char *fmt, mu_i32 opt);
MU_API mu_i32 mu_number_ex(mu_Context *ctx, mu_Real *value, mu_Real step, const char *fmt, mu_i32 opt);
MU_API mu_i32 mu_header_ex(mu_Context *ctx, const char *label, mu_i32 opt);
MU_API mu_i32 mu_begin_treenode_ex(mu_Context *ctx, const char *label, mu_i32 opt);
MU_API void mu_end_treenode(mu_Context *ctx);
MU_API mu_i32 mu_begin_window_ex(mu_Context *ctx, const char *title, mu_Rect rect, mu_i32 opt);
MU_API void mu_end_window(mu_Context *ctx);
MU_API void mu_open_popup(mu_Context *ctx, const char *name);
MU_API mu_i32 mu_begin_popup(mu_Context *ctx, const char *name);
MU_API void mu_end_popup(mu_Context *ctx);
MU_API void mu_begin_panel_ex(mu_Context *ctx, const char *name, mu_i32 opt);
MU_API void mu_end_panel(mu_Context *ctx);

#endif
