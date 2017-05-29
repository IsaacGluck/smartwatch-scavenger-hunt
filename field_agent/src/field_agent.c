/*****************************************************************/
/* This program          */
/*****************************************************************/
#include <pebble.h>
#include "key_assembly.h"
#include "field_agent_data.h"
#include "location.h"
#include "choose_name.h"


// Globals
static time_t start;
static char *fa_claim = "opCode=FA_CLAIM|"
												"gameId=FEED|"
												"pebbleId=8080477D|"
												"team=aqua|"
												"player=Alice|"
												"latitude=43.706552|"
												"longitude=-72.287418|"
												"kragId=8080";


// static function defintions
// base functions
static void init();
static void deinit();
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
  /* 1. Setup the info struct with the FA data */
  create_info();

  /* 2. Register our tick_handler function with TickTimerService. */
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  /* 3. Show the Window on the watch, with animated=true. */
  choose_name_window_push();

  /* 4. Update time information. */
  time(&start); // set the start time
  update_time();

  /* 5. Set the handlers for AppMessage inbox/outbox events. Set these    *
   *    handlers BEFORE calling open, otherwise you might miss a message. */
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);

  /* 6. open the app message communication protocol. Request as much space *
   *    as possible, because our messages can be quite large at times.     */
  app_message_open(app_message_inbox_size_maximum(), app_message_outbox_size_maximum());
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
    // window_destroy(s_main_window);

    /* 2. Unsubscribe from sensors. */
    tick_timer_service_unsubscribe();

    /* 3. Free memory. */
    delete_info();

}

// tick_handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  // APP_LOG(APP_LOG_LEVEL_INFO, "Tick.");
  static int seconds = 5;
  static int reqOption = 0;

  time_t current;
  int elapsed = 0;

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
        // send_message(fa_claim);
        time(&current);
        elapsed = ((current - start) / 60);
        FA_INFO->time_passed = elapsed;
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

        if (FA_INFO != NULL) { // struct keeping track of the game has been setup
        	// parse the location string
	        location_t *location_parsed = parse_location(location);
          APP_LOG(APP_LOG_LEVEL_INFO, "Parsed AppKeyLocation: %s|%s\n", location_parsed->latitude, location_parsed->longitude);
	        
	        if (location_parsed != NULL) {
	        	// add the location to the FA_INFO
		        FA_INFO->latitude = location_parsed->latitude;
		        FA_INFO->longitude = location_parsed->longitude;
            APP_LOG(APP_LOG_LEVEL_INFO, "Added to FA_INFO: %s|%s\n", FA_INFO->latitude, FA_INFO->longitude);
		        free(location_parsed);
	        }

	        //TEST
        	print_FA();
        }
    }

    /* 4. Check to see if a PebbleId message received. */
    Tuple *id_tuple = dict_find(iterator, AppKeyPebbleId);
    if(id_tuple) {
        // Log the value sent as part of the received message.
        char *pebbleId = id_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyPebbleId: %s\n", pebbleId);
        FA_INFO->pebbleId = pebbleId;
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

