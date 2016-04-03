#include <pebble.h>


#define KEY_USERNAME 0
#define KEY_STEPS 1

// persistent storage keys
#define STORAGE_VERSION_CODE_KEY 1
#define STORAGE_NAME_KEY 2


static Window *window;
static TextLayer *text_layer;

static char s_buffer[64];

// persistent storage version (i.e. app version at last write)
static int s_storage_version_code = 0;
static char s_username[64];


static void inbox_received_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Message received!");

  // TODO
  /*
  // Get the first pair
  Tuple *data = dict_find(iterator, KEY_DIR);
  if (data) {
    snprintf(s_buffer, sizeof(s_buffer), "Received '%d'", data->value->uint8);
    text_layer_set_text(text_layer, s_buffer);
  }  
  */
}

static void inbox_dropped_callback(AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context) {
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context) {
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}



static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Select");


  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  int step_count = 0;
  if(HealthServiceAccessibilityMaskAvailable & health_service_metric_accessible(HealthMetricStepCount, 
    time_start_of_today(), time(NULL))) {
    step_count = health_service_sum_today(HealthMetricStepCount);
  } 

  // Add a key-value pair
  dict_write_cstring(iter, KEY_USERNAME, s_username);
  dict_write_int32(iter, KEY_STEPS, step_count); // STEPS?!?!

  // Send the message!
  app_message_outbox_send();
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Up");


  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_cstring(iter, KEY_USERNAME, s_username);
  dict_write_uint8(iter, KEY_STEPS, 1); // UP

  // Send the message!
  app_message_outbox_send();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(text_layer, "Down");


  // Begin dictionary
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);

  // Add a key-value pair
  dict_write_cstring(iter, KEY_USERNAME, s_username);
  dict_write_uint8(iter, KEY_STEPS, 0); // DOWN

  // Send the message!
  app_message_outbox_send();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create(GRect(0, 72, bounds.size.w, 20));
  text_layer_set_text(text_layer, "Press a button");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(text_layer);
}

static void init(void) {
  int username_read_result = 0;

  // get initial storage version, or set to 0 if none
  s_storage_version_code = persist_exists(STORAGE_VERSION_CODE_KEY) ? persist_read_int(STORAGE_VERSION_CODE_KEY) : 0;
  s_storage_version_code = 1;
  APP_LOG(APP_LOG_LEVEL_DEBUG, "s_storage_version_code: %d ", s_storage_version_code);

  // get username if set
  if (persist_exists(STORAGE_NAME_KEY)) {
    // TODO: handle return value etc. ?
    username_read_result = persist_read_string(STORAGE_NAME_KEY, s_username, sizeof(s_username));

    APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_exists; persist_read_string() result %d ", username_read_result);

  } else {
    snprintf(s_username, sizeof(s_username), "unknown");

    APP_LOG(APP_LOG_LEVEL_DEBUG, "!persist_exists");
  }

  snprintf(s_username, sizeof(s_username), "ishotjr");
  APP_LOG(APP_LOG_LEVEL_DEBUG, s_username);

  
  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);
  
  // Open AppMessage with sensible buffer sizes
  app_message_open(64, 64);
  // TODO: ^^^ unsure why small (accurate?!) values fail after several messages?


  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  int username_write_result = 0;

  // persist storage version and username between launches
  persist_write_int(STORAGE_VERSION_CODE_KEY, s_storage_version_code);

  username_write_result = persist_write_string(STORAGE_NAME_KEY, s_username);

  APP_LOG(APP_LOG_LEVEL_DEBUG, "persist_write_string() result %d ", username_write_result);
  APP_LOG(APP_LOG_LEVEL_DEBUG, s_username);

  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}
