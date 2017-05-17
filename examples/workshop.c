/*****************************************************************/
/* This program demonstrates how to configure AppMessage handler */
/* functions and use them to send and receive messages.          */
/*****************************************************************/
#include <pebble.h>
#include "key_assembly.h"


// Globals
static TextLayer *main_layer;
static Window *s_main_window;
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
static void main_window_load(Window *window);
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

    /* 5. Make sure the time is displayed from the start. */
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
}

// main
int main(void) {
    init();
    app_event_loop();
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

    /* 2. Create the TextLayer with the bounds from above. */
    main_layer = text_layer_create(GRect(0, PBL_IF_ROUND_ELSE(58,52),
                                           bounds.size.w, 50));

    /* 3. Modify the layout to look more like a watchface. */
    text_layer_set_background_color(main_layer, GColorClear);
    text_layer_set_text_color(main_layer, GColorBlack);
    text_layer_set_font(main_layer,
                        fonts_get_system_font(FONT_KEY_BITHAM_42_BOLD));
    text_layer_set_text_alignment(main_layer, GTextAlignmentCenter);

    /* 4. Add TextLayer as a child layer to the Window root layer. */
    layer_add_child(window_layer, text_layer_get_layer(main_layer));
}

// main_window_unload
static void main_window_unload(Window *window) {
    /* 1. Destroy TextLayer. */
    text_layer_destroy(main_layer);
}

// tick_handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    APP_LOG(APP_LOG_LEVEL_INFO, "Tick.");
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
    update_time();
}

// update_time
static void update_time() {
    /* 1. Get a tm structure. */
    time_t temp = time(NULL);
    struct tm *tick_time = localtime(&temp);

    /* 2. Write the current hours and minutes into a buffer. */
    static char s_buffer[8];
    strftime(s_buffer, sizeof(s_buffer),
             clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);

    /* 3. Display this time on the TextLayer. */
    text_layer_set_text(main_layer, s_buffer);

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
