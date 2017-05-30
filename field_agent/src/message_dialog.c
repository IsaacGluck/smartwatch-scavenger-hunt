#include "message_dialog.h"

static Window *s_window;
static TextLayer *s_text_layer;

static char* message = NULL;

static void window_load(Window *window)
{

  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  const GEdgeInsets text_insets = {.top = (bounds.size.h / 2) - 24};
  s_text_layer = text_layer_create(grect_inset(bounds, text_insets));
  text_layer_set_text(s_text_layer, message);
  text_layer_set_text_color(s_text_layer, GColorWhite);
  text_layer_set_background_color(s_text_layer, GColorClear);
  text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
  text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
}


static void window_unload(Window *window)
{
  text_layer_destroy(s_text_layer);
  window_destroy(s_window);
  s_window = NULL;

  message = NULL;
}


void dialog_message_window_push(char* input_message)
{
  if(!s_window) {
    message = input_message;

    s_window = window_create();
    window_set_background_color(s_window, PBL_IF_COLOR_ELSE(GColorBlueMoon, GColorBlack));
    window_set_window_handlers(s_window, (WindowHandlers) {
      .load = window_load,
      .unload = window_unload
    });
  }

  window_stack_push(s_window, true);
}