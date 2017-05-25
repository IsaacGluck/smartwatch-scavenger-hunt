/*****************************************************************/
/* This program          */
/*****************************************************************/
#include <pebble.h>
#include "key_assembly.h"


// Radio definitions
#define RADIO_BUTTON_WINDOW_NUM_ROWS     4
#define RADIO_BUTTON_WINDOW_CELL_HEIGHT  44
#define RADIO_BUTTON_WINDOW_RADIO_RADIUS 6

// Global Structs
typedef struct fieldagent_info {
	char *gameID;
	char* pebbleId;
	char* name;
	char* team;
	double latitude;
	double longitude;
	int num_claimed;
	int num_left;
	char* known_chars;
	char** hints_received;
} fieldagent_info_t;

// Globals
static Window *s_main_window;
static MenuLayer *choose_name_menulayer;
static int s_current_selection = 1; // for choosing the name, start at 1 for the
static fieldagent_info_t *FA_INFO;
static char teamName[7] = "views6";
static char isaac[6] = "Isaac";
static char morgan[7] = "Morgan";
static char laya[5] = "Laya";
static char kazuma[7] = "Kazuma";

static char *fa_claim = "opCode=FA_CLAIM|"
												"gameId=FEED|"
												"pebbleId=8080477D|"
												"team=aqua|"
												"player=Alice|"
												"latitude=43.706552|"
												"longitude=-72.287418|"
												"kragId=8080";


// static function defintions
static void init();
static void deinit();
static void main_window_load(Window *window); // choose name
static void main_window_unload(Window *window);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void update_time();

// AppMessage functions
static void inbox_received_callback(DictionaryIterator *iterator, void *context);
static void outbox_sent_callback(DictionaryIterator *iterator, void *context);
static void inbox_dropped_callback(AppMessageResult reason, void *context);
static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context);

// my message functions
static void request_pebbleId();
static void request_location();
static void send_message(char *message);

// name choose menu layer functions
uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context);
void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context);
int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context);
void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context);
void radio_button_window_push();


// init
static void init() {
    /* 1. Create main Window element. */
    s_main_window = window_create();

    /* 2. Set handlers to manage the elements inside the window. */
    window_set_window_handlers(s_main_window, (WindowHandlers) {
        .load = main_window_load,
        .unload = main_window_unload
    });

    /* 3. Register our tick_handler function with TickTimerService. */
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

    /* 4. Show the Window on the watch, with animated=true. */
    window_stack_push(s_main_window, true);

    /* 5. Choose name window is displayed from the start. */
    update_time();

    /* 6. Set the handlers for AppMessage inbox/outbox events. Set these    *
     *    handlers BEFORE calling open, otherwise you might miss a message. */
    app_message_register_inbox_received(inbox_received_callback);
    app_message_register_outbox_sent(outbox_sent_callback);
    app_message_register_inbox_dropped(inbox_dropped_callback);
    app_message_register_outbox_failed(outbox_failed_callback);

    /* 7. open the app message communication protocol. Request as much space *
     *    as possible, because our messages can be quite large at times.     */
    app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());

		FA_INFO = malloc(sizeof(fieldagent_info_t)); // must free this later
    FA_INFO->team = teamName;
}

// main
int main(void) {
  init();
  LOG("field_agent initialized!");
  app_event_loop();
  LOG("field_agent deinitialized!");
  deinit();
}

// deinit
static void deinit() {
    /* 1. Destroy the window. */
    window_destroy(s_main_window);

    /* 2. Unsubscribe from sensors. */
    tick_timer_service_unsubscribe();
}

// main_window_load
static void main_window_load(Window *window) {
    /* 1. Get information about the window. */
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    /* 2. Create the MenuLayer with the bounds from above. */
    choose_name_menulayer = menu_layer_create(bounds);

    /* 3. Set the callback for clicking the on the menu. */
    menu_layer_set_click_config_onto_window(choose_name_menulayer, window);
	  menu_layer_set_callbacks(choose_name_menulayer, NULL, (MenuLayerCallbacks) {
	      .get_num_rows = get_num_rows_callback,
	      .draw_row = draw_row_callback,
	      .get_cell_height = get_cell_height_callback,
	      .select_click = select_callback,
	  });

	  /* 4. Add MenuLayer as a child layer to the Window root layer. */
	  layer_add_child(window_layer, menu_layer_get_layer(choose_name_menulayer));
}

// main_window_unload
static void main_window_unload(Window *window) {
    /* 1. Destroy MenuLayer. */
    menu_layer_destroy(choose_name_menulayer);
	  window_destroy(window);
	  s_main_window = NULL;
}

// tick_handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    // APP_LOG(APP_LOG_LEVEL_INFO, "Tick.");
    static int seconds = 5;
    static int reqOption = 0;

    /* 1. Only send a request/message every 5 seconds. */
    if(seconds == 0) {
        switch(reqOption) {
            case 0 :
                request_pebbleId();
                reqOption++;
                break;
            case 1 :
                request_location();
                reqOption++;
                break;
            case 2 :
                send_message(fa_claim);
                reqOption = 0;
                break;
            default:
                reqOption = 0;
                break;
        }
        seconds = 5;
    } else {
        seconds--;
    }

    /* 2. Update time on watchface. */
    // update_time();
}

// update_time
static void update_time() {
    /* 1. Get a tm structure. */
    // time_t temp = time(NULL);
    // struct tm *tick_time = localtime(&temp);

    // /* 2. Write the current hours and minutes into a buffer. */
    // static char s_buffer[8];
    // strftime(s_buffer, sizeof(s_buffer),
    // 	clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

    // /* 3. Display this time on the TextLayer. */
    // text_layer_set_text(main_layer, s_buffer);

}

// inbox_received_callback
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    /* Possible message types defined in key_assembly.h
    enum {
      AppKeyJSReady = 0,      // The JS environment on the companion smartphone is up and running!
      AppKeySendMsg,          // Send a message over Bluetooth to the companion smartphone and then on to the Game Server
      AppKeyRecvMsg,          // A message from the Game Server is available (arrived over Bluetooth)
      AppKeyLocation,         // Request your GPS location from the companion smartphone
      AppKeyPebbleId,         // Request your unique pebble ID from the companion smartphone
      AppKeySendError         // Error: companion app can't connect to the Proxy (and ultimately the Game Server).
    };
    */

    /* 1. Check to see if JS environment ready message received. */
    Tuple *ready_tuple = dict_find(iterator, AppKeyJSReady);
    if(ready_tuple) {
        // Log the value sent as part of the received message.
        char *ready_str = ready_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyJSReady: %s\n", ready_str);
    }

    /* 2. Check to see if an opcode message was received received. */
    Tuple *msg_tuple = dict_find(iterator, AppKeyRecvMsg);
    if(msg_tuple) {
        // Log the value sent as part of the received message.
        char *msg = msg_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyrecvMsg: %s\n", msg);
    }

    /* 3. Check to see if a location message received. */
    Tuple *loc_tuple = dict_find(iterator, AppKeyLocation);
    if(loc_tuple) {
        // Log the value sent as part of the received message.
        char *location = loc_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyLocation: %s\n", location);
    }

    /* 4. Check to see if a PebbleId message received. */
    Tuple *id_tuple = dict_find(iterator, AppKeyPebbleId);
    if(id_tuple) {
        // Log the value sent as part of the received message.
        char *pebbleId = id_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyPebbleId: %s\n", pebbleId);
    }

    /* 5. Check to see if an error message was received. */
    Tuple *error_tuple = dict_find(iterator, AppKeyLocation);
    if(error_tuple) {
        // Log the value sent as part of the received message.
        char *error = error_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeySendError: %s\n", error);
    }
}

// outbox_sent_callback
static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message successfully sent.\n");
}

// inbox_dropped_callback
static void inbox_dropped_callback(AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message dropped from inbox.\n");
}

// outbox_failed_callback
static void outbox_failed_callback(DictionaryIterator *iter, AppMessageResult reason, void *context) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Message failed to send.\n");
}

// request_pebbleId
static void request_pebbleId() {
    /* 1. Declare dictionary iterator */
    DictionaryIterator *out_iter;

    /* 2. Prepare the outbox */
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    /* 3. If outbox was prepared, send request. Otherwise, log error. */
    if (result == APP_MSG_OK) {
        int value = 1;

        /* construct and send message, outgoing value ignored */
        dict_write_int(out_iter, AppKeyPebbleId, &value, sizeof(value), true);

        result = app_message_outbox_send();

        if (result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending pebbleId request from outbox.\n");
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing outbox for pebbleId request.\n");
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Requested pebbleId.\n");
}

// request_location
static void request_location() {
    /* 1. Declare dictionary iterator */
    DictionaryIterator *out_iter;

    /* 2. Prepare the outbox */
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    /* 3. If outbox was prepared, send request. Otherwise, log error. */
    if (result == APP_MSG_OK) {
        int value = 1;

        /* construct and send message, outgoing value ignored */
        dict_write_int(out_iter, AppKeyLocation, &value, sizeof(value), true);

        result = app_message_outbox_send();

        if (result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending locationrequest from outbox.\n");
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing outbox for location request.\n");
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Requested location.\n");
}

// send_message
static void send_message(char *message) {
    /* 1. Declare dictionary iterator */
    DictionaryIterator *out_iter;

    /* 2. Prepare the outbox */
    AppMessageResult result = app_message_outbox_begin(&out_iter);

    /* 3. If outbox was prepared, send message. Otherwise, log error. */
    if (result == APP_MSG_OK) {
      
        /* Construct and send the message */
        dict_write_cstring(out_iter, AppKeySendMsg, message);
      
        result = app_message_outbox_send();
      
        if(result != APP_MSG_OK) {
            APP_LOG(APP_LOG_LEVEL_ERROR, "Error sending message from outbox.\n");
        }
    } else {
        APP_LOG(APP_LOG_LEVEL_ERROR, "Error preparing outbox for send_message.\n");
    }
    APP_LOG(APP_LOG_LEVEL_INFO, "Sent message.\n");
}



// Setup the radion menu layer to choose a name
//
uint16_t get_num_rows_callback(MenuLayer *menu_layer, uint16_t section_index, void *context) {
  return RADIO_BUTTON_WINDOW_NUM_ROWS + 2; // 2 extra for the join game and title
}

//
void draw_row_callback(GContext *ctx, const Layer *cell_layer, MenuIndex *cell_index, void *context) {
  if(cell_index->row == RADIO_BUTTON_WINDOW_NUM_ROWS + 1) {
    // This is the submit item
    menu_cell_basic_draw(ctx, cell_layer, "Join team views6", NULL, NULL);
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
    GPoint p = GPoint(bounds.size.w - (3 * RADIO_BUTTON_WINDOW_RADIO_RADIUS), (bounds.size.h / 2));

    // Selected?
    if(menu_cell_layer_is_highlighted(cell_layer)) {
      graphics_context_set_stroke_color(ctx, GColorWhite);
      graphics_context_set_fill_color(ctx, GColorWhite);
    } else {
      graphics_context_set_fill_color(ctx, GColorBlack);
    }

    // Draw radio filled/empty
    graphics_draw_circle(ctx, p, RADIO_BUTTON_WINDOW_RADIO_RADIUS);
    if(cell_index->row == s_current_selection) {
      // This is the selection
      graphics_fill_circle(ctx, p, RADIO_BUTTON_WINDOW_RADIO_RADIUS - 3);
    }
  }
}

// 
int16_t get_cell_height_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *context) {
  return PBL_IF_ROUND_ELSE(
    menu_layer_is_index_selected(menu_layer, cell_index) ? 
      MENU_CELL_ROUND_FOCUSED_SHORT_CELL_HEIGHT : MENU_CELL_ROUND_UNFOCUSED_TALL_CELL_HEIGHT,
    44);
}

//
void select_callback(struct MenuLayer *menu_layer, MenuIndex *cell_index, void *callback_context) {
  if(cell_index->row == RADIO_BUTTON_WINDOW_NUM_ROWS + 1) {
    // Do something with user choice
    if (s_current_selection != 0) {
    	APP_LOG(APP_LOG_LEVEL_INFO, "Submitted choice %d", s_current_selection);
    }

    switch(s_current_selection) {
    	case 1 :
        APP_LOG(APP_LOG_LEVEL_INFO, "Chose Isaac");
        FA_INFO->name = isaac;
        break;
      case 2 :
      	APP_LOG(APP_LOG_LEVEL_INFO, "Chose Morgan");
      	FA_INFO->name = morgan;
        break;
      case 3 :
      	APP_LOG(APP_LOG_LEVEL_INFO, "Chose Kazuma");
      	FA_INFO->name = kazuma;
        break;
      case 4 :
      	APP_LOG(APP_LOG_LEVEL_INFO, "Chose Laya");
      	FA_INFO->name = laya;
        break;
    }

    // They must choose a name
    if (s_current_selection != 0) {
	    window_stack_pop(true);

	    // add the main screen 
    }
  } else {
  	// change selection as long as its not the title
  	if (cell_index->row != 0) {
  		s_current_selection = cell_index->row;
  		menu_layer_reload_data(menu_layer);
  	}
  }
}
