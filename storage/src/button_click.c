#include <pebble.h>

/* A lot of the time you need to write data to the watch so it is saved the next time the app launches. 
To do this, you must take advantage of the Pebbles storage API (application programming interface) to 
take care of saving data. You can save integers, booleans, strings, and raw data.  */

// In this example, we will be making a small app to keep track of how many glasses of water you have drank today

//initialize a window and a text layer
static Window *window;
static TextLayer *text_layer;

//create a second text layer and a new font for that text layer
static GFont s_res_gothic_18;
static TextLayer *s_textlayer_1;

//set up our counter
int glassesOfWater = 0;
bool hasBeenLaunchedBefore;

static void up_click_handler(ClickRecognizerRef recognizer, void *context) { // if the up button is pressed
  
  //increment our counter for the number of glasses of water
  glassesOfWater = glassesOfWater + 1;
  
  // turn our integer of the number of glasses of water into a string which we can display on the screen
  static char buf[] = "00000000000";    /* <-- implicit NUL-terminator at the end here */
  snprintf(buf, sizeof(buf), "%d", glassesOfWater);

  //finally display the number of glasses of water
  text_layer_set_text(text_layer, buf);
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  if(glassesOfWater > 0) { // we dont want negative glasses!
    //increment our counter for the number of glasses of water
  glassesOfWater = glassesOfWater - 1;
  
  // turn our integer of the number of glasses of water into a string which we can display on the screen
  static char buf[] = "00000000000";    /* <-- implicit NUL-terminator at the end here */
  snprintf(buf, sizeof(buf), "%d", glassesOfWater);

  //finally display the number of glasses of water
  text_layer_set_text(text_layer, buf);
    } 
}

static void click_config_provider(void *context) {
  //setup our configuration for the buttons
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  //create our main window and give it a size
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  //next we create a text layer with a location and size
  s_textlayer_1 = text_layer_create(GRect(12, 90, 113, 49));
  // then we assign it a default text value
  text_layer_set_text(s_textlayer_1, "Glasses Drank Today");
  //center it on the screen
  text_layer_set_text_alignment(s_textlayer_1, GTextAlignmentCenter);
  //create a custom font
  s_res_gothic_18 = fonts_get_system_font(FONT_KEY_GOTHIC_18);
  //set the font of the text layer to this custom font
  text_layer_set_font(s_textlayer_1, s_res_gothic_18);
  //and finally add the text layer to the main window
  layer_add_child(window_layer, text_layer_get_layer(s_textlayer_1));
  
  //next we create another text layer with a location and size to display our glasses drank count
  text_layer = text_layer_create(GRect(0, 40, bounds.size.w, 75));
  // then we assign it a default text value
  text_layer_set_text(text_layer, "0");
  //set its background color to clear so it doesnt interfere with any other text
  text_layer_set_background_color(text_layer, GColorClear);
  //set its font to a system font that is big and bold
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_LECO_42_NUMBERS));
  //center it on the screen
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  // and finally add it to the main window
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
  
  
  //then we check if this is the first time the app has been launched
  hasBeenLaunchedBefore = persist_read_bool(0);
  
  //if it hasnt been launched before, we have to do a little setup
  if (!hasBeenLaunchedBefore) {
    //first we write create a space for our number of glasses to be stored
    StatusCode intStatus = persist_write_int(1, glassesOfWater);
    //then we tell the watch that we have launched for the first time
    StatusCode boolStatus = persist_write_bool(0, true);
    //next we output some info about whether or not the info was saved, this can be ignored
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Writing... int code %d" , intStatus);
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Writing... bool code %d" , boolStatus);
  }
  
  //then we read our number of glasses
  glassesOfWater = persist_read_int(1);
  
  //and then turn it into a string to be displayed to the user
  static char buf[] = "00000000000";    /* <-- implicit NUL-terminator at the end here */
  snprintf(buf, sizeof(buf), "%d", glassesOfWater);

  // then we finally display the # of glasses to the user
  text_layer_set_text(text_layer, buf);

}

static void window_unload(Window *window) {
  //when the window is unloaded we must destroy our text layers to clean up
  text_layer_destroy(text_layer);
  text_layer_destroy(s_textlayer_1);
}

static void init(void) {
 //when we first initialize we need to do some setup, first create a window
  window = window_create();
  //then we set its click providers to be the ones we set up on line 39 - 45
  window_set_click_config_provider(window, click_config_provider);
  //then set up the window handlers for loading and unloading
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  //then we push the window onto the screen and tell it to animate on
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
   // when our app stops we have to destroy all our windows
  window_destroy(window);
}

int main(void) {
 // this is our main loop, we want to first initialize everything
  init();

  //then print to the console so we can see the app has started
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  //then our main loop runs which handles all of our uer interface, vibrations, and button presses
  app_event_loop();
  
  //finally when the app is done, destroy everything! Muhaha
  deinit();
}