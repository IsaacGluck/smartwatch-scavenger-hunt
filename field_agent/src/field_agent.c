/*****************************************************************/
/* This program          */
/*****************************************************************/
#include <pebble.h>
#include "key_assembly.h"
#include "field_agent_data.h"
#include "location.h"
#include "choose_name.h"
#include "message_dialog.h"
#include "message_handler.h"


// Globals
static time_t start;
static char* error_message;
static int malloc_size = 200;
char pebbleId_init_string[5] = "init";


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
  error_message = malloc(malloc_size);

  /* 2. Register our tick_handler function with TickTimerService. */
  tick_timer_service_subscribe(SECOND_UNIT, tick_handler);

  /* 3. Show the Window on the watch, with animated=true. */
  choose_name_window_push();

  /* 4. Update time information. */
  time(&start); // set the start time

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
    /* 1. Remove all windows from the stack */
    // Window* window = window_stack_pop(false);
    // while (window != NULL) {
    //   window_destroy(window);
    //   window = window_stack_pop(false);
    // }
    window_stack_pop_all(false); 

    /* 2. Unsubscribe from sensors. */
    tick_timer_service_unsubscribe();

    /* 3. Free memory. */
    delete_info();
    free(error_message);

}

// tick_handler
static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
  static int seconds = 5;
  static int reqOption = 0;

  if (FA_INFO->submit_krag) {
    char* FA_CLAIM = create_fa_claim(FA_INFO->krag_to_submit);
    send_message(FA_CLAIM);
    free(FA_CLAIM);
    FA_INFO->submit_krag = false; 
  }

  if (FA_INFO->game_over_received) {
    window_stack_pop_all(false); // pop all windows
    dialog_message_window_push(FA_INFO->end_message);
    return; // do no more
  }

  if (FA_INFO->wrong_name) { // SH_ERROR_INVALId_PLAYERNAME or SH_ERROR_DUPLICATE_PLAYERNAME
    char buff[51] = "";
    snprintf(buff, sizeof(buff), "Name '%s' could not be used. Please choose again.", FA_INFO->name);
    strcpy(error_message, buff);
    window_stack_pop(false); // pop all windows
    choose_name_window_push();
    dialog_message_window_push(error_message);
    FA_INFO->wrong_name = false;
  }

  if (FA_INFO->krag_claimed_already) {
    char buff[51] = "Sorry, the krag you submitted was already claimed.";
    strcpy(error_message,buff);
    dialog_message_window_push(error_message);
    FA_INFO->krag_claimed_already = false;
  }

  if (FA_INFO->krag_claimed) {
    char buff[51] = "The KRAG you submitted was claimed!";
    strcpy(error_message,buff);
    dialog_message_window_push(error_message);
    if (FA_INFO->num_left > 0) {
      FA_INFO->num_left = FA_INFO->num_left - 1;
    }
    FA_INFO->num_claimed = FA_INFO->num_claimed + 1;
    FA_INFO->krag_claimed = false;
  }

  /* 1. Only send a request/message every 5 seconds. */
  if(seconds == 0) {
    switch(reqOption) {
      case 0 :
        if (strcmp(FA_INFO->pebbleId, pebbleId_init_string) == 0) {
          request_pebbleId();
        }
        print_FA();
        request_location();
        reqOption++;
        break;
      case 1 :
        // request_location();
        if(FA_INFO->game_started && strcmp(FA_INFO->pebbleId, pebbleId_init_string) != 0){
          char* FA_LOCATION = create_fa_location("1");
          send_message(FA_LOCATION);
          free(FA_LOCATION);
        }
        reqOption++;
        break;
      case 2 :
        request_location();
        main_menu_reload_pass_up();
        reqOption = 0;

        // Test Game Over
        // char secret_buff[200];
        // snprintf(secret_buff, sizeof(secret_buff), "Game over!\n The secret was: qwertyuiop");

        // strcpy(FA_INFO->known_chars, secret_buff);
        // FA_INFO->game_over_received = true;

        // Test wrong name
        // FA_INFO->wrong_name = true;

        // Test claimed already
        // FA_INFO->krag_claimed_already = true;

        // Test claimed
        // FA_INFO->krag_claimed = true;

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
 
}

// update_time
static void update_time() {
  time_t current; // current second
  int elapsed = 0; // initialize elapsed time

  time(&current);
  elapsed = ((current - start) / 60);
  FA_INFO->time_passed = elapsed;

}

// inbox_received_callback
static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
    /* Possible message types defined in key_assembly.h
    enum {
      AppKeyJSReady = 0,      // The JS environment on the companion smartphone is up and running!
      AppKeySendMsg,          // Send a message over Bluetooth to the companion smartphone and then on to the Game Server
      AppKeyRecvMsg,          // A message from the Game Server is available (arrived over Bluetooth)
      AppKeyLocation,         // Request your GPS location from the companion smartphone
      AppKeyPebbleId,         // Request your unique pebble Id from the companion smartphone
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
        incoming_message(msg);
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
	        
	        if (location_parsed != NULL) {
	        	// add the location to the FA_INFO
            strcpy(FA_INFO->latitude, location_parsed->latitude);
            strcpy(FA_INFO->longitude, location_parsed->longitude);
		        free(location_parsed);
	        }
        }
    }

    /* 4. Check to see if a PebbleId message received. */
    Tuple *id_tuple = dict_find(iterator, AppKeyPebbleId);
    if(id_tuple) {
        // Log the value sent as part of the received message.
        char *pebbleId = id_tuple->value->cstring;
        APP_LOG(APP_LOG_LEVEL_INFO, "Got AppKeyPebbleId: %s\n", pebbleId);
        strcpy(FA_INFO->pebbleId, pebbleId);
    }

    /* 5. Check to see if an error message was received. */
    Tuple *error_tuple = dict_find(iterator, AppKeySendError);
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
  APP_LOG(APP_LOG_LEVEL_INFO, "\n\n SENDING: %s \n\n", message);

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

