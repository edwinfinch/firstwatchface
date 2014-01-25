/*
Clean w/ battery and bluetooth
Created by Edwin Finch
Open source
Feel free to modify, redistribute and do whatever you'd like with it :)

Trek font taken from PebbleTrek by Kyle Potts: https://github.com/kylepotts/pebbletrek

Note:
I was in the middle of working on adding the date when I added 12/24hr functionality
So please ignore/delete the lines that contain stuff about the date
*/

#include <pebble.h>
 
Window* window;
TextLayer *text_layer;
TextLayer *battery_text_layer;
							//TextLayer* date_text_layer;
static GFont trek20;

static GBitmap* bt_connected;
static BitmapLayer* bt_connected_layer;

char timeBuffer[] = "00:00";
char dateBuffer[] = "14 August";

void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  //Format the Time buffer string using tick_time as the time source
  strftime(timeBuffer, sizeof("00:00"), "%H:%M", tick_time);
  //Format dateBuffer using tick_time as source
 // strftime(dateBuffer, sizeof(dateBuffer), "%e %b", tick_time);
	
	if(clock_is_24h_style()){
      strftime(timeBuffer, sizeof(timeBuffer), "%H:%M", tick_time);
    }
    else{
      strftime(timeBuffer,sizeof(timeBuffer),"%I:%M",tick_time);
    }
	
  //Change the TextLayer(s) text to show the new time and date
  text_layer_set_text(text_layer, timeBuffer);
  							//text_layer_set_text(date_text_layer, dateBuffer);
}
 
void handle_bt(bool connected){
  if(connected == 1){
  bt_connected = gbitmap_create_with_resource(RESOURCE_ID_bt_connected);
  bitmap_layer_set_bitmap(bt_connected_layer,bt_connected);
  }

  else{
  bt_connected = gbitmap_create_with_resource(RESOURCE_ID_bt_disconnected);
  bitmap_layer_set_bitmap(bt_connected_layer,bt_connected);
  }

}

void handle_battery(BatteryChargeState charge_state) {
  static char battery_text[] = "100%";

  if (charge_state.is_charging) {
      snprintf(battery_text, sizeof(battery_text), "chg");
    } else {
        snprintf(battery_text, sizeof(battery_text), "%d%%", charge_state.charge_percent);
      }
  text_layer_set_text(battery_text_layer, battery_text);
}

void window_load(Window *window)
{
  Layer *window_layer = window_get_root_layer(window);
  trek20 = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_trekfont_20));
  //Time layer
  text_layer = text_layer_create(GRect(0, 53, 140, 168));
  text_layer_set_background_color(text_layer, GColorClear);
  text_layer_set_text_color(text_layer, GColorBlack);
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  text_layer_set_font(text_layer, fonts_get_system_font(FONT_KEY_BITHAM_42_MEDIUM_NUMBERS));    
  layer_add_child(window_get_root_layer(window), (Layer*) text_layer);
	
  //Date layer (work in progress)
  /*
  date_text_layer = text_layer_create(GRect(120,0,40,40));
  text_layer_set_font(date_text_layer,trek20);
  text_layer_set_background_color(date_text_layer, GColorBlack);
  text_layer_set_text_color(date_text_layer, GColorWhite);
  layer_add_child(window_layer, text_layer_get_layer(date_text_layer));
  */
	
  //Battery text layer
  battery_text_layer = text_layer_create(GRect(0,145,40,40));
  text_layer_set_font(battery_text_layer,trek20);
  text_layer_set_background_color(battery_text_layer, GColorClear);
  text_layer_set_text_color(battery_text_layer, GColorBlack);
  layer_add_child(window_layer, text_layer_get_layer(battery_text_layer));
    
  //Bluetooth layer
  bt_connected = gbitmap_create_with_resource(RESOURCE_ID_bt_disconnected);
  bt_connected_layer = bitmap_layer_create(GRect(115,135,40,40));
  bitmap_layer_set_background_color(bt_connected_layer,GColorClear);
  layer_add_child(window_layer,bitmap_layer_get_layer(bt_connected_layer));
	
  //Get a time structure so that it doesn't start blank
  struct tm *t;
  time_t temp;        
  temp = time(NULL);        
  t = localtime(&temp);        
        
  //Manually call the tick handler when the window is loading
  tick_handler(t, MINUTE_UNIT);

  //Battery update handler
  BatteryChargeState btchg = battery_state_service_peek();
  handle_battery(btchg);
	
  //Bluetooth update handler
  bool connected = bluetooth_connection_service_peek();
  handle_bt(connected);

}
 
void window_unload(Window *window)
{
  //Destroy elements
  text_layer_destroy(text_layer);
  								//text_layer_destroy(date_text_layer);
  text_layer_destroy(battery_text_layer);
  fonts_unload_custom_font(trek20);
  bitmap_layer_destroy(bt_connected_layer);
  gbitmap_destroy(bt_connected);
}
 
void init()
{
 //Initialize the app elements
  window = window_create();
  window_set_window_handlers(window, (WindowHandlers) {
  .load = window_load,
  .unload = window_unload,
  });
        
  tick_timer_service_subscribe(MINUTE_UNIT, (TickHandler) tick_handler);
  battery_state_service_subscribe(&handle_battery);
  window_stack_push(window, true);
  bluetooth_connection_service_subscribe(&handle_bt);
}
 
void deinit()
{
  //De-initialize elements, saves memory
  tick_timer_service_unsubscribe();
  battery_state_service_unsubscribe();
  bluetooth_connection_service_unsubscribe();
  gbitmap_destroy(bt_connected);
  bitmap_layer_destroy(bt_connected_layer);
  window_destroy(window);                                               
}
 
int main(void)
{
  init();
  app_event_loop();
  deinit();
}
