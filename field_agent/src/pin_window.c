#include <pebble.h>
#include "pin_window.h"
#include "selection_layer.h"

static char* selection_handle_get_text(int index, void *context) {
  PinWindow *pin_window = (PinWindow*)context;
  snprintf(
    pin_window->field_buffs[index], 
    sizeof(pin_window->field_buffs[0]), "%c",
    pin_window->pin.digits[index]
  );
  return pin_window->field_buffs[index];
}

static void selection_handle_complete(void *context) {
  PinWindow *pin_window = (PinWindow*)context;
  pin_window->callbacks.pin_complete(pin_window->pin, pin_window);
}

static void selection_handle_inc(int index, uint8_t clicks, void *context) {
  PinWindow *pin_window = (PinWindow*)context;

  char cur = pin_window->pin.digits[index];
  switch(cur) {
    case '0' :
      pin_window->pin.digits[index] = '1';
      break;
    case '1' :
      pin_window->pin.digits[index] = '2';
      break;
    case '2' :
      pin_window->pin.digits[index] = '3';
      break;
    case '3' :
      pin_window->pin.digits[index] = '4';
      break;
    case '4' :
      pin_window->pin.digits[index] = '5';
      break;
    case '5' :
      pin_window->pin.digits[index] = '6';
      break;
    case '6' :
      pin_window->pin.digits[index] = '7';
      break;
    case '7' :
      pin_window->pin.digits[index] = '8';
      break;
    case '8' :
      pin_window->pin.digits[index] = '9';
      break;
    case '9' :
      pin_window->pin.digits[index] = 'a';
      break;
    case 'a' :
      pin_window->pin.digits[index] = 'b';
      break;
    case 'b' :
      pin_window->pin.digits[index] = 'c';
      break;
    case 'c' :
      pin_window->pin.digits[index] = 'd';
      break;
    case 'd' :
      pin_window->pin.digits[index] = 'e';
      break;
    case 'e' :
      pin_window->pin.digits[index] = 'f';
      break;
    case 'f' :
      pin_window->pin.digits[index] = '0';
      break;

    default:
      pin_window->pin.digits[index] = '0';
      break;
  }
}

static void selection_handle_dec(int index, uint8_t clicks, void *context) {
  PinWindow *pin_window = (PinWindow*)context;
  
  char cur = pin_window->pin.digits[index];
  switch(cur) {
    case '0' :
      pin_window->pin.digits[index] = 'f';
      break;
    case '1' :
      pin_window->pin.digits[index] = '0';
      break;
    case '2' :
      pin_window->pin.digits[index] = '1';
      break;
    case '3' :
      pin_window->pin.digits[index] = '2';
      break;
    case '4' :
      pin_window->pin.digits[index] = '3';
      break;
    case '5' :
      pin_window->pin.digits[index] = '4';
      break;
    case '6' :
      pin_window->pin.digits[index] = '5';
      break;
    case '7' :
      pin_window->pin.digits[index] = '6';
      break;
    case '8' :
      pin_window->pin.digits[index] = '7';
      break;
    case '9' :
      pin_window->pin.digits[index] = '8';
      break;
    case 'a' :
      pin_window->pin.digits[index] = '9';
      break;
    case 'b' :
      pin_window->pin.digits[index] = 'a';
      break;
    case 'c' :
      pin_window->pin.digits[index] = 'b';
      break;
    case 'd' :
      pin_window->pin.digits[index] = 'c';
      break;
    case 'e' :
      pin_window->pin.digits[index] = 'd';
      break;
    case 'f' :
      pin_window->pin.digits[index] = 'e';
      break;

    default:
      pin_window->pin.digits[index] = '0';
      break;
  }
}

PinWindow* pin_window_create(PinWindowCallbacks callbacks) {
  PinWindow *pin_window = (PinWindow*)malloc(sizeof(PinWindow));
  if (pin_window) {
    pin_window->window = window_create();
    pin_window->callbacks = callbacks;
    if (pin_window->window) {
      pin_window->field_selection = 0;
      for(int i = 0; i < PIN_WINDOW_NUM_CELLS; i++) {
        pin_window->pin.digits[i] = '0';
      }
      // pin_window->pin.digits[PIN_WINDOW_NUM_CELLS] = '\0';
      
      // Get window parameters
      Layer *window_layer = window_get_root_layer(pin_window->window);
      GRect bounds = layer_get_bounds(window_layer);
      
      // Main TextLayer
      const GEdgeInsets main_text_insets = {.top = 30};
      pin_window->main_text = text_layer_create(grect_inset(bounds, main_text_insets));
      text_layer_set_text(pin_window->main_text, "PIN Required");
      text_layer_set_font(pin_window->main_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
      text_layer_set_text_alignment(pin_window->main_text, GTextAlignmentCenter);
      layer_add_child(window_layer, text_layer_get_layer(pin_window->main_text));
      
      // Sub TextLayer
      const GEdgeInsets sub_text_insets = {.top = 115, .right = 5, .bottom = 10, .left = 5};
      pin_window->sub_text = text_layer_create(grect_inset(bounds, sub_text_insets));
      text_layer_set_text(pin_window->sub_text, "Enter your PIN to continue");
      text_layer_set_text_alignment(pin_window->sub_text, GTextAlignmentCenter);
      text_layer_set_font(pin_window->sub_text, fonts_get_system_font(FONT_KEY_GOTHIC_18_BOLD));
      layer_add_child(window_layer, text_layer_get_layer(pin_window->sub_text));
      
      // Create selection layer
      const GEdgeInsets selection_insets = GEdgeInsets(
        (bounds.size.h - PIN_WINDOW_SIZE.h) / 2, 
        (bounds.size.w - PIN_WINDOW_SIZE.w) / 2);
      pin_window->selection = selection_layer_create(grect_inset(bounds, selection_insets), PIN_WINDOW_NUM_CELLS);
      for (int i = 0; i < PIN_WINDOW_NUM_CELLS; i++) {
        selection_layer_set_cell_width(pin_window->selection, i, 30); // changed from 40
      }
      selection_layer_set_cell_padding(pin_window->selection, 3); // changed from 4
      selection_layer_set_active_bg_color(pin_window->selection, GColorRed);
      selection_layer_set_inactive_bg_color(pin_window->selection, GColorDarkGray);
      selection_layer_set_click_config_onto_window(pin_window->selection, pin_window->window);
      selection_layer_set_callbacks(pin_window->selection, pin_window, (SelectionLayerCallbacks) {
        .get_cell_text = selection_handle_get_text,
        .complete = selection_handle_complete,
        .increment = selection_handle_inc,
        .decrement = selection_handle_dec,
      });
      layer_add_child(window_get_root_layer(pin_window->window), pin_window->selection);

      // Create status bar
      pin_window->status = status_bar_layer_create();
      status_bar_layer_set_colors(pin_window->status, GColorClear, GColorBlack);
      layer_add_child(window_layer, status_bar_layer_get_layer(pin_window->status));
      return pin_window;
    }
  }

  APP_LOG(APP_LOG_LEVEL_ERROR, "Failed to create PinWindow");
  return NULL;
}

void pin_window_destroy(PinWindow *pin_window) {
  if (pin_window) {
    status_bar_layer_destroy(pin_window->status);
    selection_layer_destroy(pin_window->selection);
    text_layer_destroy(pin_window->sub_text);
    text_layer_destroy(pin_window->main_text);
    free(pin_window);
    pin_window = NULL;
    return;
  }
}

void pin_window_push(PinWindow *pin_window, bool animated) {
  window_stack_push(pin_window->window, animated);
}

void pin_window_pop(PinWindow *pin_window, bool animated) {
  window_stack_remove(pin_window->window, animated);
}

bool pin_window_get_topmost_window(PinWindow *pin_window) {
  return window_stack_get_top_window() == pin_window->window;
}

void pin_window_set_highlight_color(PinWindow *pin_window, GColor color) {
  pin_window->highlight_color = color;
  selection_layer_set_active_bg_color(pin_window->selection, color);
}