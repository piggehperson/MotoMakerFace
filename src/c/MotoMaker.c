#include <pebble.h>

static void init() {

}

static void deinit() {

}

int main(void) {
  init();
  app_event_loop();
  deinit();
}