#include <pebble.h>

Window *window;
TextLayer *text_layer;

GBitmap *future_bitmap, *past_bitmap;
BitmapLayer *future_layer, *past_layer;

char buffer[] = "00:00";

InverterLayer *inv_layer;

void window_load(Window *window)
{	
  text_layer = text_layer_create(GRect(0, 53, 144, 168));
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorBlack);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));
  layer_add_child(window_get_root_layer(window), (Layer*) text_layer);
  //I'm a retard
  //I always feel like Katharine is watching over me
  //Knowing that I'm terrible at this
  //Mocking me
	
  inv_layer = inverter_layer_create(GRect(0, 50, 144, 62));
  layer_add_child(window_get_root_layer(window), (Layer*) inv_layer);
	
  //Load bitmaps into GBitmap structures
  //The ID you chose when uploading is prefixed with 'RESOURCE_ID_'
  future_bitmap = gbitmap_create_with_resource(RESOURCE_ID_FUTURE);
  past_bitmap = gbitmap_create_with_resource(RESOURCE_ID_PAST);
 
  //Create BitmapLayers to show GBitmaps and add to Window
  //Sample images are 144 x 50 pixels
  future_layer = bitmap_layer_create(GRect(0, 0, 144, 50));
  bitmap_layer_set_bitmap(future_layer, future_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(future_layer));
 
  past_layer = bitmap_layer_create(GRect(0, 112, 144, 50));
  bitmap_layer_set_bitmap(past_layer, past_bitmap);
  layer_add_child(window_get_root_layer(window), bitmap_layer_get_layer(past_layer));
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
   //Format the buffer string using tick_time as the time source
   strftime(buffer, sizeof("00:00"), "%H:%M", tick_time);
 
   //Change the TextLayer text to show the new time!
   text_layer_set_text(text_layer, buffer);
}
    


void window_unload(Window *window)
{
  text_layer_destroy(text_layer);
  inverter_layer_destroy(inv_layer);
  //Destroy GBitmaps
  gbitmap_destroy(future_bitmap);
  gbitmap_destroy(past_bitmap);
 
  //Destroy BitmapLayers
  bitmap_layer_destroy(future_layer);
  bitmap_layer_destroy(past_layer);
}

void handle_init()
{
	//Initialize the window
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
    .load = window_load,
    .unload = window_unload,
  });
  window_stack_push(window, true);
	//Push the window to front and center, m8ey
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
	//subscribe to the time, updates on tick
}

void handle_deinit(void) {
	//This is for deinitializing the window
	  text_layer_destroy(text_layer);
	  window_destroy(window);
}

int main(void) {
	//This is our main function, which returns nothing (void)
	//It handles ticks, as well as initializing and deinitializing the window
	  handle_init();
	  app_event_loop();
	  handle_deinit();
}
