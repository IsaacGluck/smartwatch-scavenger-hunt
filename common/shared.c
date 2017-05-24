// Conditional inclusion for platform specific builds
#ifdef NOPEBBLE // we are *not* building for pebble
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#else // we are building for pebble
#include <pebble.h>
#endif

#include "shared.h"

const char *version_str = "1.0";

void print_shared() {
  printf("*** shared function! (v%s) ***", version_str);
}