#define RGFW_DEBUG
#define GL_SILENCE_DEPRECATION
#define RGFW_OPENGL
#define RGFW_IMPLEMENTATION
#include "RGFW.h"

#include <stdio.h>

#ifdef RGFW_MACOS
#include <OpenGL/gl.h>
#else
#include <GL/gl.h>
#endif

#define MICROUI_API inline
#include "microui.h"
#include "microui.c"

#include "renderer.h"
#include "renderer.c"

static float bg[3] = { 90, 95, 100 };

static  char logbuf[64000];
static   int logbuf_updated = 0;
static mu_StringView logbuf_string = mu_stringView(logbuf, 0);

void write_log(const mu_StringView* string) {
  if (logbuf_string.length && logbuf_string.length < sizeof(logbuf)) {
	logbuf[logbuf_string.length] = '\n';
	logbuf_string.length += 1;
  }

  size_t length = string->length;
  if (logbuf_string.length + length > sizeof(logbuf)) {
     length = (logbuf_string.length - sizeof(logbuf));
  }

  if (length == 0) return;

  memcpy(&logbuf[logbuf_string.length], string->data, length);
  logbuf_string.length += length;
  logbuf_updated = 1;
}

static void log_window(mu_Context* ctx) {
	mu_StringView title = mu_stringView("Log Window", 10);
	if (mu_begin_window(ctx, &title, mu_rect(350, 40, 300, 200)) == 0) {
		return;
	}

	/* output text panel */
	int arr[] = { -1 };
	mu_layout_row(ctx, 1, arr, -25);

	mu_StringView panel_title = mu_stringView("Log Output", 10);
	mu_begin_panel(ctx, &panel_title);
	mu_Container *panel = mu_get_current_container(ctx);
	int arr1[] =  { -1 };
	mu_layout_row(ctx, 1, arr1, -1);

	mu_text(ctx, &logbuf_string);
	mu_end_panel(ctx);
	if (logbuf_updated) {
		panel->scroll.y = panel->content_size.y;
		logbuf_updated = 0;
	}

	/* input textbox + submit button */
	static char buf[128];
	static mu_String string_buf = mu_string(buf, 0, sizeof(buf));

	int submitted = 0;
	int arr2[] = { -70, -1 };
	mu_layout_row(ctx, 2, arr2, 0);

	mu_StringView button_name = mu_stringViewLiteral("Submit");

	if ((mu_textbox(ctx, &string_buf) & MU_RES_SUBMIT) || mu_button(ctx, &button_name)) {
		mu_set_focus(ctx, ctx->last_id);

		mu_StringView view = mu_stringViewFromString(string_buf);
		write_log(&view);
		string_buf.length = 0;
	}

	mu_end_window(ctx);
}



static void test_window(mu_Context *ctx) {
  /* do window */
  if (mu_begin_window(ctx, &mu_stringViewLiteral("Demo Window"), mu_rect(40, 40, 300, 450))) {
    mu_Container *win = mu_get_current_container(ctx);
    win->rect.w = mu_max(win->rect.w, 240);
    win->rect.h = mu_max(win->rect.h, 300);

    /* window info */
    if (mu_header(ctx, &mu_stringViewLiteral("Window Info"))) {
      mu_Container *win = mu_get_current_container(ctx);
      char buf[64];
      mu_layout_row(ctx, 2, (int[]) { 54, -1 }, 0);
      mu_label(ctx, &mu_stringViewLiteral("Position:"));
      int len = snprintf(buf, sizeof(buf), "%d, %d", win->rect.x, win->rect.y);
      if (len > 0 && len < sizeof(buf)) mu_label(ctx, &mu_stringView(buf, len));

      mu_label(ctx, &mu_stringViewLiteral("Size:"));
      len = snprintf(buf, sizeof(buf), "%d, %d", win->rect.w, win->rect.h);
      if (len > 0 && len < sizeof(buf)) mu_label(ctx, &mu_stringView(buf, len));
    }

    /* labels + buttons */
    if (mu_header_ex(ctx, &mu_stringViewLiteral("Test Buttons"), MU_OPT_EXPANDED)) {
      mu_layout_row(ctx, 3, (int[]) { 86, -110, -1 }, 0);
      mu_label(ctx, &mu_stringViewLiteral("Test buttons 1:"));
      if (mu_button(ctx, &mu_stringViewLiteral("Button 1"))) { write_log(&mu_stringViewLiteral("Pressed button 1")); }
      if (mu_button(ctx, &mu_stringViewLiteral("Button 2"))) { write_log(&mu_stringViewLiteral("Pressed button 2")); }
      mu_label(ctx, &mu_stringViewLiteral("Test buttons 2:"));
      if (mu_button(ctx, &mu_stringViewLiteral("Button 3"))) { write_log(&mu_stringViewLiteral("Pressed button 3")); }
      if (mu_button(ctx, &mu_stringViewLiteral("Popup"))) { mu_open_popup(ctx, &mu_stringViewLiteral("Test Popup")); }
      if (mu_begin_popup(ctx, &mu_stringViewLiteral("Test Popup"))) {
        mu_button(ctx, &mu_stringViewLiteral("Hello"));
        mu_button(ctx, &mu_stringViewLiteral("World"));
        mu_end_popup(ctx);
      }
    }

    /* tree */
    if (mu_header_ex(ctx, &mu_stringViewLiteral("Tree and Text"), MU_OPT_EXPANDED)) {
      mu_layout_row(ctx, 2, (int[]) { 140, -1 }, 0);
      mu_layout_begin_column(ctx);
      if (mu_begin_treenode(ctx, &mu_stringViewLiteral("Test 1"))) {
        if (mu_begin_treenode(ctx, &mu_stringViewLiteral("Test 1a"))) {
          mu_label(ctx, &mu_stringViewLiteral("Hello"));
          mu_label(ctx, &mu_stringViewLiteral("world"));
          mu_end_treenode(ctx);
        }
        if (mu_begin_treenode(ctx, &mu_stringViewLiteral("Test 1b"))) {
          if (mu_button(ctx, &mu_stringViewLiteral("Button 1"))) { write_log(&mu_stringViewLiteral("Pressed button 1")); }
          if (mu_button(ctx, &mu_stringViewLiteral("Button 2"))) { write_log(&mu_stringViewLiteral("Pressed button 2")); }
          mu_end_treenode(ctx);
        }
        mu_end_treenode(ctx);
      }
      if (mu_begin_treenode(ctx, &mu_stringViewLiteral("Test 2"))) {
        mu_layout_row(ctx, 2, (int[]) { 54, 54 }, 0);
        if (mu_button(ctx, &mu_stringViewLiteral("Button 3"))) { write_log(&mu_stringViewLiteral("Pressed button 3")); }
        if (mu_button(ctx, &mu_stringViewLiteral("Button 4"))) { write_log(&mu_stringViewLiteral("Pressed button 4")); }
        if (mu_button(ctx, &mu_stringViewLiteral("Button 5"))) { write_log(&mu_stringViewLiteral("Pressed button 5")); }
        if (mu_button(ctx, &mu_stringViewLiteral("Button 6"))) { write_log(&mu_stringViewLiteral("Pressed button 6")); }
        mu_end_treenode(ctx);
      }
      if (mu_begin_treenode(ctx, &mu_stringViewLiteral("Test 3"))) {
        static int checks[3] = { 1, 0, 1 };
        mu_checkbox(ctx, &mu_stringViewLiteral("Checkbox 1"), &checks[0]);
        mu_checkbox(ctx, &mu_stringViewLiteral("Checkbox 2"), &checks[1]);
        mu_checkbox(ctx, &mu_stringViewLiteral("Checkbox 3"), &checks[2]);
        mu_end_treenode(ctx);
      }
      mu_layout_end_column(ctx);

      mu_layout_begin_column(ctx);
      mu_layout_row(ctx, 1, (int[]) { -1 }, 0);
      mu_text(ctx, &mu_stringViewLiteral("Lorem ipsum dolor sit amet, consectetur adipiscing "
        "elit. Maecenas lacinia, sem eu lacinia molestie, mi risus faucibus "
        "ipsum, eu varius magna felis a nulla."));
      mu_layout_end_column(ctx);
    }

    /* background color sliders */
    if (mu_header_ex(ctx, &mu_stringViewLiteral("Background Color"), MU_OPT_EXPANDED)) {
      mu_layout_row(ctx, 2, (int[]) { -78, -1 }, 74);
      /* sliders */
      mu_layout_begin_column(ctx);
      mu_layout_row(ctx, 2, (int[]) { 46, -1 }, 0);
      mu_label(ctx, &mu_stringViewLiteral("Red:"));   mu_slider(ctx, &bg[0], 0, 255);
      mu_label(ctx, &mu_stringViewLiteral("Green:")); mu_slider(ctx, &bg[1], 0, 255);
      mu_label(ctx, &mu_stringViewLiteral("Blue:"));  mu_slider(ctx, &bg[2], 0, 255);
      mu_layout_end_column(ctx);
      /* color preview */
      mu_Rect r;
      mu_layout_next(ctx, &r);
      mu_draw_rect(ctx, r, mu_color(bg[0], bg[1], bg[2], 255));
      char buf[32];
      int len = snprintf(buf, sizeof(buf), "#%02X%02X%02X", (int) bg[0], (int) bg[1], (int) bg[2]);
      if (len > 0 && len < sizeof(buf)) {
         mu_draw_control_text(ctx, &mu_stringView(buf, len), r, MU_COLOR_TEXT, MU_OPT_ALIGNCENTER);
	  }
    }

    mu_end_window(ctx);
  }
}

static int uint8_slider(mu_Context *ctx, unsigned char *value, int low, int high) {
  static float tmp;
  mu_StringView hash = mu_stringView((char*)&value, sizeof(value));
  mu_push_id(ctx, &hash);
  tmp = *value;
  int res = mu_slider_ex(ctx, &tmp, low, high, 0, "%.0f", MU_OPT_ALIGNCENTER);
  *value = tmp;
  mu_pop_id(ctx);
  return res;
}


static void style_window(mu_Context *ctx) {
  static struct { const mu_StringView label; int idx; } colors[] = {
    { mu_stringViewLiteral("text:"),         MU_COLOR_TEXT        },
    { mu_stringViewLiteral("border:"),       MU_COLOR_BORDER      },
    { mu_stringViewLiteral("windowbg:"),     MU_COLOR_WINDOWBG    },
    { mu_stringViewLiteral("titlebg:"),      MU_COLOR_TITLEBG     },
    { mu_stringViewLiteral("titletext:"),    MU_COLOR_TITLETEXT   },
    { mu_stringViewLiteral("panelbg:"),      MU_COLOR_PANELBG     },
    { mu_stringViewLiteral("button:"),       MU_COLOR_BUTTON      },
    { mu_stringViewLiteral("buttonhover:"),  MU_COLOR_BUTTONHOVER },
    { mu_stringViewLiteral("buttonfocus:"),  MU_COLOR_BUTTONFOCUS },
    { mu_stringViewLiteral("base:"),         MU_COLOR_BASE        },
    { mu_stringViewLiteral("basehover:"),    MU_COLOR_BASEHOVER   },
    { mu_stringViewLiteral("basefocus:"),    MU_COLOR_BASEFOCUS   },
    { mu_stringViewLiteral("scrollbase:"),   MU_COLOR_SCROLLBASE  },
    { mu_stringViewLiteral("scrollthumb:"),  MU_COLOR_SCROLLTHUMB },
    { mu_stringViewLiteral(NULL) }
  };

  if (mu_begin_window(ctx, &mu_stringViewLiteral("Style Editor"), mu_rect(350, 250, 300, 240))) {
    int sw = mu_get_current_container(ctx)->body.w * 0.14;
    mu_layout_row(ctx, 6, (int[]) { 80, sw, sw, sw, sw, -1 }, 0);
    for (int i = 0; colors[i].label.data; i++) {
      mu_label(ctx, &colors[i].label);
      uint8_slider(ctx, &ctx->style->colors[i].r, 0, 255);
      uint8_slider(ctx, &ctx->style->colors[i].g, 0, 255);
      uint8_slider(ctx, &ctx->style->colors[i].b, 0, 255);
      uint8_slider(ctx, &ctx->style->colors[i].a, 0, 255);

      mu_Rect next;
	  mu_layout_next(ctx, &next);
      mu_draw_rect(ctx, next, ctx->style->colors[i]);
    }
    mu_end_window(ctx);
  }
}


static void process_frame(mu_Context *ctx) {
  mu_begin(ctx);
  style_window(ctx);
  log_window(ctx);
  test_window(ctx);
  mu_end(ctx);
}



static const char button_map[256] = {
  [ RGFW_mouseLeft   & 0xff ] =  MU_MOUSE_LEFT,
  [ RGFW_mouseRight  & 0xff ] =  MU_MOUSE_RIGHT,
  [ RGFW_mouseMiddle & 0xff ] =  MU_MOUSE_MIDDLE,
};

static const char key_map[256] = {
  [ RGFW_keyShiftL       & 0xff ] = MU_KEY_SHIFT,
  [ RGFW_keyShiftR       & 0xff ] = MU_KEY_SHIFT,
  [ RGFW_keyControlL        & 0xff ] = MU_KEY_CTRL,
  [ RGFW_keyControlR        & 0xff ] = MU_KEY_CTRL,
  [ RGFW_keyAltL         & 0xff ] = MU_KEY_ALT,
  [ RGFW_keyAltR         & 0xff ] = MU_KEY_ALT,
  [ RGFW_keyReturn       & 0xff ] = MU_KEY_RETURN,
  [ RGFW_keyBackSpace    & 0xff ] = MU_KEY_BACKSPACE,
};


static int text_width(mu_Font font, const mu_StringView* view) {
  RGFW_UNUSED(font);
  return r_get_text_width(view->data, view->length);
}

static int text_height(mu_Font font) {
	RGFW_UNUSED(font);
	return r_get_text_height();
}

static void update_viewport(int width, int height) {
  GLsizei vw = (GLsizei)((float)width * pixelRatio);
  GLsizei vh = (GLsizei)((float)height * pixelRatio);
  glViewport(0, 0, vw, vh);
  glScissor(0, 0, vw, vh);
}

int main(int argc, char **argv) {
  RGFW_UNUSED(argc); RGFW_UNUSED(argv);
  /* init RGFW window */
  RGFW_window* window = RGFW_createWindow("", 0, 0, width, height, RGFW_windowCenter |  RGFW_windowOpenGL);

  RGFW_monitor* mon = RGFW_window_getMonitor(window);
  width = window->w;
  height = window->h;
  pixelRatio = mon->pixelRatio;

  r_init();
  RGFW_window_setExitKey(window, RGFW_keyEscape);

  /* init microui */
  mu_Context *ctx = (mu_Context*)malloc(sizeof(mu_Context));
  mu_init(ctx);
  ctx->text_width = text_width;
  ctx->text_height = text_height;

  update_viewport(width, height);
  /* main loop */
  while (RGFW_window_shouldClose(window) == RGFW_FALSE) {
    /* handle RGFW events */
    RGFW_event event;
    while (RGFW_window_checkEvent(window, &event)) {
      if (event.type == RGFW_windowClose) break;

      switch (event.type) {
        case RGFW_windowClose: break;
        case RGFW_mouseButtonPressed: {
          int b = button_map[event.button.value & 0xff];
		  if (b) mu_input_mousedown(ctx, b);
		  break;
	   }
	   case RGFW_mouseButtonReleased: {
          int b = button_map[event.button.value & 0xff];
          if (b) { mu_input_mouseup(ctx, b);   }
          break;
        }

        case RGFW_keyChar: {
		  u32 str[2] = {event.keyChar.value, 0};
		  mu_StringView string = mu_stringView((const char*)str, strlen((char*)str));
		  mu_input_text(ctx, &string);
		  break;
	    }
		case RGFW_keyPressed: {
			int c = key_map[event.key.value & 0xff];
			if (c) mu_input_keydown(ctx, c);
			break;
		}
		case RGFW_keyReleased: {
			int c = key_map[event.key.value & 0xff];
			if (c) mu_input_keyup(ctx, c);
          break;
        }

		case RGFW_windowResized:
		  width = window->w;
		  height = window->h;
      update_viewport(width, height);
		  break;
	  }
    }

	i32 mouseX, mouseY;
	RGFW_window_getMouse(window, &mouseX, &mouseY);
	mu_input_mousemove(ctx, mouseX, mouseY);

	float deltaX, deltaY;
	RGFW_getMouseScroll(&deltaX, &deltaY);
	mu_input_scroll(ctx, deltaX * -5.0f, deltaY * -5.0f);

    /* process frame */
    process_frame(ctx);

    /* render */
    r_clear(mu_color(bg[0], bg[1], bg[2], 255));
    mu_Command *cmd = NULL;
    while (mu_next_command(ctx, &cmd)) {
      switch (cmd->type) {
        case MU_COMMAND_TEXT: r_draw_text(cmd->text.str, cmd->text.pos, cmd->text.color); break;
        case MU_COMMAND_RECT: r_draw_rect(cmd->rect.rect, cmd->rect.color); break;
        case MU_COMMAND_ICON: r_draw_icon(cmd->icon.id, cmd->icon.rect, cmd->icon.color); break;
        case MU_COMMAND_CLIP: r_set_clip_rect(cmd->clip.rect); break;
      }
    }

    r_present();
    RGFW_window_swapBuffers_OpenGL(window);
  }

  return 0;
}


