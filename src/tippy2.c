#include <pebble.h>

static Window *window;
static TextLayer *temp_layer;
static Layer *path_layer;
static Layer *results_path_layer;
static InverterLayer *inverter_layer;
static InverterLayer *results_inverter_layer;
static int layer_count_max = 4;
static TextLayer *layer_array[4];
static int layer_count_start = 0;
static Layer *path_layer_array[4];
static int dollars = 25;
static int cents = 0;
static int tip_percent = 15;
static int people = 1;
static char buffer[5];
static char dollar_str[25] = "Dollars:";
static char cents_str[25] = "Cents:";
static char tip_str[25] = "Tip percent:";
static char people_str[25] = "People:";
static double total_amt;
static double total_amt_without_tip;
static double tip_amt;
static int menu_item_width = 80;
static int menu_item_height = 25;
static TextLayer *results_layer;
static TextLayer *plus_layer;
static TextLayer *minus_layer;
static TextLayer *total_layer;
static TextLayer *total_label;
static TextLayer *tip_layer;
static TextLayer *tip_label;
static GBitmap *up_down_icon;
static char tip_buffer[5];
static BitmapLayer *image_layer;




// This is the layer update callback which is called on render updates
static void path_layer_update_callback(Layer *me, GContext *ctx) {
  (void)me;

     GRect bounds = layer_get_bounds(me);

    // Draw a black filled rectangle with sharp corners
    //gpath_draw_outline(ctx, bounds);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_rect(ctx, bounds);

    //graphics_context_set_fill_color(ctx, GColorWhite);
    //gpath_draw_filled(ctx, current_path);
  
}


static void update_total(){
  
   //char total_str[15];
  
  char decimal_buffer[5];
  total_amt = (float)dollars + (float)cents/100;
  total_amt_without_tip = total_amt;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "total amount double: %02u", (int)total_amt);
  // get the total with the tip
  total_amt = total_amt * (1 + (double)tip_percent/100);
  total_amt = total_amt / people;
  
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "total amount with tip: %02u", (int)total_amt);
  int whole_num = (int)total_amt / 1;
  snprintf(buffer, 5, "%d", (int)whole_num);
  strcat(buffer, ".");
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "buffer %s", buffer);
  // take float, multiply by 100 to shift the decimals over two places, and then do a mod 100 to get teh two decimals places
  int decimal_places = (int)(total_amt * 100) % 100;
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "decimal places %u", decimal_places);
  // append the decimals to the buffer
  snprintf(decimal_buffer, 5, "%d", (int)decimal_places);
  //char decimal_buffer[5];
  strcat(buffer, decimal_buffer);
  //APP_LOG(APP_LOG_LEVEL_DEBUG, "final buffer %s", buffer);
  text_layer_set_text(total_layer, buffer);
  // tip amount -- same procedure as above
  tip_amt = total_amt_without_tip * (double)tip_percent/100;
  tip_amt = tip_amt / people;
  int whole_num_tip = (int)tip_amt / 1;
  char tip_decimal_buffer[5];
  snprintf(tip_buffer, 5, "%d", (int)whole_num_tip);
  strcat(tip_buffer,".");
  int tip_decimal_places = (int)(tip_amt * 100) % 100;
  snprintf(tip_decimal_buffer, 5, "%d", (int)tip_decimal_places);
  strcat(tip_buffer, tip_decimal_buffer);
  text_layer_set_text(tip_layer, tip_buffer);


  layer_mark_dirty(text_layer_get_layer(tip_layer));
  layer_mark_dirty(text_layer_get_layer(total_layer));
  layer_mark_dirty(text_layer_get_layer(layer_array[0]));
  layer_mark_dirty(text_layer_get_layer(layer_array[1]));
  layer_mark_dirty(text_layer_get_layer(layer_array[2]));
  layer_mark_dirty(text_layer_get_layer(layer_array[3]));

}




static void results_path_layer_update_callback(Layer *me, GContext *ctx) {
 (void)me;

    GRect bounds = layer_get_bounds(me);

    // Draw a black filled rectangle with sharp corners
    //gpath_draw_outline(ctx, bounds);
    graphics_context_set_stroke_color(ctx, GColorBlack);
    graphics_draw_rect(ctx, bounds);
  }

static void add_menu_item(int position, GRect bounds){
  
  Layer *window_layer = window_get_root_layer(window);
  temp_layer = text_layer_create(bounds);
  if (position == 0){
     snprintf(dollar_str, sizeof(dollar_str), "Dollars: %u", dollars);
     text_layer_set_text(temp_layer, dollar_str);
  }else if(position == 1){
     snprintf(cents_str, sizeof(cents_str), "Cents: %u", cents);
     text_layer_set_text(temp_layer, cents_str);
  }else if(position == 2){
      snprintf(tip_str, sizeof(tip_str), "Tip: %u ", tip_percent);
      text_layer_set_text(temp_layer, tip_str);
  }else{
      snprintf(people_str, sizeof(people_str), "People: %u", people);
      text_layer_set_text(temp_layer, people_str);
  }
  //text_layer_set_font(temp_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  
   text_layer_set_font(temp_layer, fonts_get_system_font(FONT_KEY_GOTHIC_18));
    

  
  //text_layer_set_text_alignment(temp_layer, GTextAlignmentCenter);
 
  
  path_layer = layer_create(bounds);
  if(position == layer_count_start){
     layer_set_update_proc(path_layer, path_layer_update_callback);
     //text_layer_set_text_color(temp_layer, GColorWhite);
  }
  layer_add_child(window_layer, text_layer_get_layer(temp_layer));
  layer_add_child(window_layer, path_layer);
  layer_array[position] = temp_layer;
  path_layer_array[position] = path_layer;
  // add this second so font color will appear above it
  
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  
  
  //text_layer_set_text(layer_array[layer_count_start], "Select");
  layer_destroy(path_layer_array[0]);
  layer_destroy(path_layer_array[1]);
  layer_destroy(path_layer_array[2]);
  layer_destroy(path_layer_array[3]);
  
  layer_count_start++;
  if (layer_count_start == layer_count_max ){
      layer_count_start = 0;
  }
  add_menu_item(0,GRect(10,0,menu_item_width,menu_item_height));
  add_menu_item(1,GRect(10,25,menu_item_width,menu_item_height));
  add_menu_item(2,GRect(10,50,menu_item_width,menu_item_height));
  add_menu_item(3,GRect(10,75,menu_item_width,menu_item_height));
  
  

}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(one_layer, "Up");
  if(layer_count_start == 0){
    dollars = dollars+4;
    //text_layer_destroy(layer_array[0]);
    add_menu_item(0,GRect(10,0,menu_item_width,menu_item_height));  
  }else if(layer_count_start == 1){
    cents = cents + 5;
    //text_layer_destroy(layer_array[1]);
    add_menu_item(1,GRect(10,25,menu_item_width,menu_item_height)); 
  }else if(layer_count_start == 2){
    tip_percent++;
    //text_layer_destroy(layer_array[2]);
    add_menu_item(2,GRect(10,50,menu_item_width,menu_item_height));
  }else{
    people++;
    //text_layer_destroy(layer_array[3]);
    add_menu_item(3,GRect(10,75,menu_item_width,menu_item_height));
  }
  update_total();
}
static void up_click_handler_repeating(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(one_layer, "Up");
  if(layer_count_start == 0){
    dollars = dollars + 5;
    //text_layer_destroy(layer_array[0]);
    add_menu_item(0,GRect(10,0,menu_item_width,menu_item_height));  
  }else if(layer_count_start == 1){
    cents = cents + 5;
    //text_layer_destroy(layer_array[1]);
    add_menu_item(1,GRect(10,25,menu_item_width,menu_item_height)); 
  }else if(layer_count_start == 2){
    tip_percent++;
    //text_layer_destroy(layer_array[2]);
    add_menu_item(2,GRect(10,50,menu_item_width,menu_item_height));
  }else{
    people++;
    //text_layer_destroy(layer_array[3]);
    add_menu_item(3,GRect(10,75,menu_item_width,menu_item_height));
  }
  update_total();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(one_layer, "Down");
    if(layer_count_start == 0){
          if (dollars > 0)
      dollars--;
      
      //text_layer_destroy(layer_array[0]);
      add_menu_item(0,GRect(10,0,menu_item_width,menu_item_height));
      
    }else if(layer_count_start == 1){
          if (cents > 0)
            cents--;
    //text_layer_destroy(layer_array[1]);
    add_menu_item(1,GRect(10,25,menu_item_width,menu_item_height));     
          

    }else if(layer_count_start == 2){
          if(tip_percent > 0)
            tip_percent--;
    //text_layer_destroy(layer_array[2]);
    add_menu_item(2,GRect(10,50,menu_item_width,menu_item_height));    
          

    }else{
       if (people > 0)
     people--;
     //text_layer_destroy(layer_array[3]);
     add_menu_item(3, GRect(10,75,menu_item_width,menu_item_height)); 
      
  }

  update_total();
}

static void down_click_handler_repeating(ClickRecognizerRef recognizer, void *context) {
  //text_layer_set_text(one_layer, "Down");
    if(layer_count_start == 0){
          if (dollars > 5)
      dollars = dollars - 5;
      
      //text_layer_destroy(layer_array[0]);
      add_menu_item(0,GRect(10,0,menu_item_width,menu_item_height));
      
    }else if(layer_count_start == 1){
          if (cents > 5)
            cents = cents - 5;
    //text_layer_destroy(layer_array[1]);
    add_menu_item(1,GRect(10,25,menu_item_width,menu_item_height));     
          

    }else if(layer_count_start == 2){
          if(tip_percent > 0)
            tip_percent--;
    //text_layer_destroy(layer_array[2]);
    add_menu_item(2,GRect(10,50,menu_item_width,menu_item_height));    
          

    }else{
       if (people > 0)
     people--;
     //text_layer_destroy(layer_array[3]);
     add_menu_item(3, GRect(10,75,menu_item_width,menu_item_height)); 
      
  }

  update_total();
}



static void click_config_provider(void *context) {
   const uint16_t repeat_interval_ms = 35;
  window_single_repeating_click_subscribe(BUTTON_ID_UP, repeat_interval_ms, (ClickHandler) up_click_handler);
  window_single_repeating_click_subscribe(BUTTON_ID_DOWN, repeat_interval_ms, (ClickHandler) down_click_handler);
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
 
}




static void window_load(Window *window) {
   
   Layer *window_layer = window_get_root_layer(window);
   add_menu_item(0,GRect(10,0,menu_item_width,menu_item_height));
   add_menu_item(1,GRect(10,25,menu_item_width,menu_item_height));
   add_menu_item(2,GRect(10,50,menu_item_width,menu_item_height));
   add_menu_item(3,GRect(10,75,menu_item_width,menu_item_height));
   
   inverter_layer = inverter_layer_create(GRect(100,0,52,168));
   layer_add_child(window_layer, inverter_layer_get_layer(inverter_layer));
    //right side buttons
  
   plus_layer =  text_layer_create(GRect(120,0,32,32));
   text_layer_set_font(plus_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
   text_layer_set_text(plus_layer, "+");
   
   minus_layer =  text_layer_create(GRect(120,115,32,32));
   text_layer_set_font(minus_layer, fonts_get_system_font(FONT_KEY_BITHAM_30_BLACK));
   text_layer_set_text(minus_layer, "-");

      // icon create
    // The bitmap layer holds the image for display
   up_down_icon = gbitmap_create_with_resource(RESOURCE_ID_UP_DOWN);
   image_layer = bitmap_layer_create(GRect(110,55,32,32));
   bitmap_layer_set_bitmap(image_layer, up_down_icon);
   bitmap_layer_set_alignment(image_layer, GAlignCenter);
   layer_add_child(window_layer, bitmap_layer_get_layer(image_layer));

   layer_add_child(window_layer, text_layer_get_layer(plus_layer));
   layer_add_child(window_layer, text_layer_get_layer(minus_layer));

   results_layer = text_layer_create(GRect(0,105, 100, 55));
   results_path_layer = layer_create(GRect(0,105, 100, 55));
   results_inverter_layer = inverter_layer_create(GRect(0,100, 100, 60));
   layer_add_child(window_layer, inverter_layer_get_layer(results_inverter_layer));
   layer_add_child(window_layer, text_layer_get_layer(results_layer));
  
   layer_set_update_proc(results_path_layer, results_path_layer_update_callback);
   layer_add_child(window_layer, results_path_layer);

   // total layer 
   total_label = text_layer_create(GRect(0,0,40,25));
   //FONT_KEY_DROID_SERIF_28_BOLD
   //FONT_KEY_BITHAM_30_BLACK
   layer_add_child(text_layer_get_layer(results_layer), text_layer_get_layer(total_label));
   text_layer_set_font(total_label, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
   text_layer_set_text(total_label, "Total:");

      // total layer 
   total_layer = text_layer_create(GRect(40,0,60,25));
   layer_add_child(text_layer_get_layer(results_layer), text_layer_get_layer(total_layer));
   text_layer_set_font(total_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));

   // tip  label 
   tip_label = text_layer_create(GRect(0,25,40,25));
   layer_add_child(text_layer_get_layer(results_layer), text_layer_get_layer(tip_label));
   text_layer_set_text(tip_label, "Tip:");
   text_layer_set_font(tip_label, fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));

      // total layer 
   tip_layer = text_layer_create(GRect(40,25,60,25));
   text_layer_set_font(tip_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
   layer_add_child(text_layer_get_layer(results_layer), text_layer_get_layer(tip_layer));





}

static void window_unload(Window *window) {
  text_layer_destroy(temp_layer);
  text_layer_destroy(plus_layer);
  text_layer_destroy(minus_layer);
  text_layer_destroy(total_layer);
  text_layer_destroy(total_label);
  text_layer_destroy(tip_layer);
  text_layer_destroy(tip_label);
  inverter_layer_destroy(inverter_layer);
  inverter_layer_destroy(results_inverter_layer);
  text_layer_destroy(results_layer);
  layer_destroy(path_layer);
  layer_destroy(results_path_layer);
  gbitmap_destroy(up_down_icon);
  bitmap_layer_destroy(image_layer);
}

static void init(void) {
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
  window_destroy(window);
}

int main(void) {
  init();

  APP_LOG(APP_LOG_LEVEL_DEBUG, "Done initializing, pushed window: %p", window);

  app_event_loop();
  deinit();
}