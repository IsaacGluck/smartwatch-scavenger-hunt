/*****************************************************************/
/* This program */
/*****************************************************************/
#include "main_menu.h"
#include "field_agent_data.h"

static Window *s_main_window_main_menu;
static MenuLayer *s_menu_layer_main_menu;

static int s_current_selection_main_menu = 0;

static uint16_t get_num_rows_callback_main_menu(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return MAIN_MENU_WINDOW_NUM_ROWS;
}

static void draw_row_callback_main_menu(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  // This is a choice item
  static char s_buff[16];
  // snprintf(s_buff, sizeof(s_buff), "Choice %d", (int)cell_index->row);

  switch((int)cell_index->row) {
  	case 0 :
  		snprintf(s_buff, sizeof(s_buff), "Name: %s", FA_INFO->name);
      break;
  	case 1 :
      snprintf(s_buff, sizeof(s_buff), "Team: %s", FA_INFO->team);
      break;
    case 2 :
    	snprintf(s_buff, sizeof(s_buff), "Time Passed: %d mins", FA_INFO->time_passed);
      break;
    case 3 :
    	snprintf(s_buff, sizeof(s_buff), "Latitude: %d", (int)FA_INFO->latitude);
      break;
    case 4 :
    	snprintf(s_buff, sizeof(s_buff), "Longitude: %d", (int)FA_INFO->longitude);
      break;
    case 5 :
    	snprintf(s_buff, sizeof(s_buff), "KRAGS Left: %d", FA_INFO->num_left);
      break;
    case 6 :
    	snprintf(s_buff, sizeof(s_buff), "Decoded: %s", FA_INFO->known_chars);
      break;
    case 7 :
    	snprintf(s_buff, sizeof(s_buff), "Hints received >");
      break;
    case 8 :
    	snprintf(s_buff, sizeof(s_buff), "Input KRAG >");
      break;
    default:
    	break;
  }
  menu_cell_basic_draw(ctx, cell_layer, s_buff, NULL, NULL);


  // Selected?
  if(menu_cell_layer_is_highlighted(cell_layer)) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);
  } else {
    graphics_context_set_fill_color(ctx, GColorBlack);
  }
  
}

static int16_t get_cell_height_callback_main_menu(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    44);
}

static void select_callback_main_menu(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if(cell_index->row == MAIN_MENU_WINDOW_NUM_ROWS) {
    // Do something with user choice
    APP_LOG(APP_LOG_LEVEL_INFO, "Submitted choice %d", s_current_selection_main_menu);
    window_stack_pop(true);
  } else {
    // Change selection
    s_current_selection_main_menu = cell_index->row;
    menu_layer_reload_data(menu_layer);
  }
}

static void window_load_main_menu(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer_main_menu = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer_main_menu, window);
  menu_layer_set_callbacks(s_menu_layer_main_menu, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback_main_menu,
      .draw_row = draw_row_callback_main_menu,
      .get_cell_height = get_cell_height_callback_main_menu,
      .select_click = select_callback_main_menu,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer_main_menu));
}

static void window_unload_main_menu(Window *window) {
  menu_layer_destroy(s_menu_layer_main_menu);

  window_destroy(window);
  s_main_window_main_menu = NULL;
}

void main_menu_window_push() {
  if(!s_main_window_main_menu) {
    s_main_window_main_menu = window_create();
    window_set_window_handlers(s_main_window_main_menu, (WindowHandlers) {
        .load = window_load_main_menu,
        .unload = window_unload_main_menu,
    });
  }
  window_stack_push(s_main_window_main_menu, true);
}
