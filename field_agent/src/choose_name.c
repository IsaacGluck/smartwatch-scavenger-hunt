/*****************************************************************/
/* This program */
// radio button code drawn from https://github.com/pebble-examples/ui-patterns/blob/master/src/windows/radio_button_window.c
/*****************************************************************/
#include "choose_name.h"
#include "field_agent_data.h"
#include "main_menu.h"


static Window *s_main_window_choose_name;
static MenuLayer *choose_name_menulayer;

static char isaac[6] = "Isaac";
static char morgan[7] = "Morgan";
static char laya[5] = "Laya";
static char kazuma[7] = "Kazuma";
static int s_current_selection = 1; // for choosing the name, start at 1 from the menu




// Setup the radion menu layer to choose a name
//
static uint16_t get_num_rows_callback_choose_name(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return CHOOSE_NAME_WINDOW_NUM_ROWS + 2; // 2 extra for the join game and title
}

//
static void draw_row_callback_choose_name(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  if(cell_index->row == CHOOSE_NAME_WINDOW_NUM_ROWS + 1) {
    // This is the submit item
    menu_cell_basic_draw(ctx, cell_layer, "Join team views6 >", NULL, NULL);
  } else if (cell_index->row == 0) {
  	menu_cell_basic_draw(ctx, cell_layer, "Choose a name", NULL, NULL);
  } else {
    // This is a choice item
    static char s_buff[16];

    switch((int)cell_index->row) {
    	case 1 :
        snprintf(s_buff, sizeof(s_buff), "Isaac");
        break;
      case 2 :
      	snprintf(s_buff, sizeof(s_buff), "Morgan");
        break;
      case 3 :
      	snprintf(s_buff, sizeof(s_buff), "Kazuma");
        break;
      case 4 :
      	snprintf(s_buff, sizeof(s_buff), "Laya");
        break;
    }

    menu_cell_basic_draw(ctx, cell_layer, s_buff, NULL, NULL);

    GRect bounds = layer_get_bounds(cell_layer);
    GPoint p = GPoint(bounds.size.w - (3 * CHOOSE_NAME_WINDOW_RADIO_RADIUS), (bounds.size.h / 2));

    // Selected?
    if(menu_cell_layer_is_highlighted(cell_layer)) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    } else {
      graphics_context_set_fill_color(ctx, GColorBlack);
    }


    // Draw radio filled/empty
    graphics_draw_circle(ctx, p, CHOOSE_NAME_WINDOW_RADIO_RADIUS);
    if(cell_index->row == s_current_selection) {
      // This is the selection
      graphics_fill_circle(ctx, p, CHOOSE_NAME_WINDOW_RADIO_RADIUS - 3);
    }
  }
}

// 
static int16_t get_cell_height_callback_choose_name(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    44);
}

//
static void select_callback_choose_name(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if(cell_index->row == CHOOSE_NAME_WINDOW_NUM_ROWS + 1) {

    switch(s_current_selection) {
    	case 1 :
        APP_LOG(APP_LOG_LEVEL_INFO, "Chose Isaac");
        strcpy(FA_INFO->name, isaac);
        // FA_INFO->name = isaac;
        // chosen_name = isaac;
        break;
      case 2 :
      	APP_LOG(APP_LOG_LEVEL_INFO, "Chose Morgan");
      	strcpy(FA_INFO->name, morgan);
      	// FA_INFO->name = morgan;
        // chosen_name = morgan;
        break;
      case 3 :
      	APP_LOG(APP_LOG_LEVEL_INFO, "Chose Kazuma");
      	strcpy(FA_INFO->name, kazuma);
      	// FA_INFO->name = kazuma;
        // chosen_name = kazuma;
        break;
      case 4 :
      	APP_LOG(APP_LOG_LEVEL_INFO, "Chose Laya");
      	strcpy(FA_INFO->name, laya);
      	// FA_INFO->name = laya;
        // chosen_name = laya;
        break;
    }

    // They must choose a name
    if (s_current_selection != 0) {
	    window_stack_pop(true);
	    // print_FA();

	    // add the main screen
	    FA_INFO->game_started = true;
	    main_menu_window_push();
    }
  } else {
  	// change selection as long as its not the title
  	if (cell_index->row != 0) {
  		s_current_selection = cell_index->row;
  		menu_layer_reload_data(menu_layer);
  	}
  }
}



// main_window
static void window_load_choose_name(Window *window) {
    /* 1. Get information about the window. */
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    /* 2. Create the MenuLayer with the bounds from above. */
    choose_name_menulayer = menu_layer_create(bounds);

    /* 3. Set the callback for clicking the on the menu. */
    menu_layer_set_click_config_onto_window(choose_name_menulayer, window);
	  menu_layer_set_callbacks(choose_name_menulayer, NULL, (MenuLayerCallbacks) {
	      .get_num_rows = get_num_rows_callback_choose_name,
	      .draw_row = draw_row_callback_choose_name,
	      .get_cell_height = get_cell_height_callback_choose_name,
	      .select_click = select_callback_choose_name,
	  });

	  /* 4. Add MenuLayer as a child layer to the Window root layer. */
	  layer_add_child(window_layer, menu_layer_get_layer(choose_name_menulayer));
}

// window_unload_choose_name
static void window_unload_choose_name(Window *window) {
    /* 1. Destroy MenuLayer. */
    menu_layer_destroy(choose_name_menulayer);
	  window_destroy(window);
	  s_main_window_choose_name = NULL;
}



void choose_name_window_push() {
  if(!s_main_window_choose_name) {
    s_main_window_choose_name = window_create();
    window_set_window_handlers(s_main_window_choose_name, (WindowHandlers) {
        .load = window_load_choose_name,
        .unload = window_unload_choose_name,
    });
  }
  window_stack_push(s_main_window_choose_name, true);
}

void main_menu_reload_pass_up() {
	main_menu_reload();
}