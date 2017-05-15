#include <pebble.h>

// Local includes
#include "key_assembly.h"

static void init() {
  // TODO: initializations...
}

static void deinit() {
  // TODO: deinitializations...
}

/************************************* main ***********************************/

int main(void) {
  init();
  LOG("field_agent initialized!");
  app_event_loop();
  LOG("field_agent deinitialized!");
  deinit();
}
