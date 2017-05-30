/*****************************************************************/
/* This program */
/*****************************************************************/
#include "hints_window.h"
#include "field_agent_data.h"
#include "message_dialog.h"

static Window *s_main_window_hints_window = NULL;
static MenuLayer *s_menu_layer_hints_window = NULL;

static int s_current_selection_main_menu = 0;

static uint16_t get_num_rows_callback_hints_window(MenuLayer *menu_layer, uint16_t section_index, void *context)
{
  return HINTS_WINDOW_WINDOW_NUM_ROWS;
}

static void draw_row_callback_hints_window(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context)
{
  // This is a choice item
  static char s_buff[141];
  static char title[16];

  snprintf(title, sizeof(title), "Hint %d", (int)cell_index->row + 1);
  snprintf(s_buff, sizeof(s_buff), "%s", FA_INFO->hints_received[(int)cell_index->row]);
  menu_cell_basic_draw(ctx, cell_layer, title, s_buff, NULL);


  // Selected?
  if(menu_cell_layer_is_highlighted(cell_layer)) {
    graphics_context_set_stroke_color(ctx, GColorWhite);
    graphics_context_set_fill_color(ctx, GColorWhite);
  } else {
    graphics_context_set_fill_color(ctx, GColorBlack);
  }
  
}

static int16_t get_cell_height_callback_hints_window(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context)
{
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    44);
}

static void select_callback_hints_window(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Selected hint %d\n", (int)cell_index->row);

  dialog_message_window_push(FA_INFO->hints_received[(int)cell_index->row]);
}

static void window_load_hints_window(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  s_menu_layer_hints_window = menu_layer_create(bounds);
  menu_layer_set_click_config_onto_window(s_menu_layer_hints_window, window);
  menu_layer_set_callbacks(s_menu_layer_hints_window, NULL, (MenuLayerCallbacks) {
      .get_num_rows = get_num_rows_callback_hints_window,
      .draw_row = draw_row_callback_hints_window,
      .get_cell_height = get_cell_height_callback_hints_window,
      .select_click = select_callback_hints_window,
  });
  layer_add_child(window_layer, menu_layer_get_layer(s_menu_layer_hints_window));
}

static void window_unload_hints_window(Window *window)
{
  menu_layer_destroy(s_menu_layer_hints_window);

  window_destroy(window);
  s_main_window_hints_window = NULL;
}

void hints_window_window_push()
{
  if(!s_main_window_hints_window) {
    s_main_window_hints_window = window_create();
    window_set_window_handlers(s_main_window_hints_window, (WindowHandlers) {
        .load = window_load_hints_window,
        .unload = window_unload_hints_window,
    });
  }
  window_stack_push(s_main_window_hints_window, true);
}
