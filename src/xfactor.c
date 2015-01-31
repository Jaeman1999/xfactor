#include <pebble.h>
#include <pebble_fonts.h>

Window *my_window;
TextLayer *hour_place, *minute_place, *day_full_place, *date_place;
GFont *hour_font, *minute_font, *small_font, *smaller_font;
GBitmap *x;
BitmapLayer *x_layer;
Layer *hour_layer, *minute_layer, *day_full_layer, *date_layer;
const char *hour, *minute, *day_full0, *date;

void get_time() {
	//Get the time
	time_t temp = time(NULL);
	struct tm *times_for_days = localtime(&temp);
	
	//Create the hour and minute arrays
	static char placemat[] = "00";
	static char placemat2[] = "00";
	static char day_full[] = "000000000";
	static char date[] = "000. 00";
	
	//Place the hour/minutes/date into the arrays
	strftime(placemat, sizeof("00"), "%I", times_for_days);
	strftime(placemat2, sizeof("00"), "%M", times_for_days);
	strftime(day_full, sizeof("000000000"), "%A", times_for_days);
	strftime(date, sizeof("000. 00"), "%b. %d", times_for_days);
	
	//Erase the 0 in the hour if earlier than 10
	if (placemat[0]=='0') {
		memmove(placemat, &placemat[1], sizeof(placemat) - 1);
	};
	
	//Create and set text layer with hour and minutes
	hour_place = text_layer_create(GRect(0, 0, 0, 0));
	minute_place = text_layer_create(GRect(0, 0, 0, 0));
	day_full_place = text_layer_create(GRect(0, 0, 0, 0));
	date_place = text_layer_create(GRect(0, 0, 0, 0));
	text_layer_set_text(hour_place, placemat);
	text_layer_set_text(minute_place, placemat2);
	text_layer_set_text(day_full_place, day_full);
	text_layer_set_text(date_place, date);
}

void hour_update(Layer *this_layer, GContext *con) {
	//Take the hour from the hour text layer
	hour = text_layer_get_text(hour_place);
	
	//Draws the hour onto the screen.
	graphics_draw_text(con, hour, hour_font, GRect(0, 0, 100, 85), GTextOverflowModeTrailingEllipsis, GTextAlignmentLeft, NULL);
};

void minute_update(Layer *this_layer, GContext *con) {
	//Take the minute from the minute text layer
	minute = text_layer_get_text(minute_place);
	
	//Draws the minute onto the screen.
	graphics_draw_text(con, minute, minute_font, GRect(0, 0, 48, 48), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
};

void date_update(Layer *this_layer, GContext *con) {
	//Take the day from the day text layer
	date = text_layer_get_text(date_place);
	
	//Draws the date onto the screen
	graphics_draw_text(con, date, smaller_font, GRect(0, 0, 100, 30), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

void day_full_update(Layer *this_layer, GContext *con) {
	//Take the day from the day text layer
	day_full0 = text_layer_get_text(day_full_place);
	
	//Draws the day onto the screen
	graphics_draw_text(con, day_full0, small_font, GRect(0, 0, 84, 30), GTextOverflowModeTrailingEllipsis, GTextAlignmentCenter, NULL);
}

void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
	get_time();
	layer_mark_dirty(minute_layer);
	layer_mark_dirty(hour_layer);
	layer_mark_dirty(day_full_layer);
	layer_mark_dirty(date_layer);
};

void window_load(Window *my_window) {
	window_set_background_color(my_window, GColorBlack);
	
	//Sets up the background image
	x_layer = bitmap_layer_create(GRect(0, 0, 144, 168));
	x = gbitmap_create_with_resource(RESOURCE_ID_x);
	bitmap_layer_set_bitmap(x_layer, x);
	bitmap_layer_set_alignment(x_layer, GAlignBottom);
	
	//Sets the fonts to their variables
	hour_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_APACHE_BOLD_82));
	minute_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_APACHE_BOLD_48));
	small_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_APACHE_BOLD_20));
	smaller_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_APACHE_BOLD_14));
	
	//Creates the layers for the time/date
	hour_layer = layer_create(GRect(16, -10, 144, 168));
	minute_layer = layer_create(GRect(90, 75, 48, 48));
	date_layer = layer_create(GRect(72, 0, 84, 30));
	day_full_layer = layer_create(GRect(11, 145, 84, 30));

	//Pastes the background to the screen
	layer_add_child(window_get_root_layer(my_window), bitmap_layer_get_layer(x_layer));
	
	//Pastes the time onto the screen
	layer_add_child(window_get_root_layer(my_window), hour_layer);
	layer_add_child(window_get_root_layer(my_window), minute_layer);
	layer_add_child(window_get_root_layer(my_window), date_layer);
	layer_add_child(window_get_root_layer(my_window), day_full_layer);
	
	get_time();
	
	//Draws the time/date
	layer_set_update_proc(hour_layer, hour_update);
	layer_set_update_proc(minute_layer, minute_update);
	layer_set_update_proc(date_layer, date_update);
	layer_set_update_proc(day_full_layer, day_full_update);
	
	//Updates time every minute
	get_time();
	tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);
}

void window_unload(Window *my_window) {
	text_layer_destroy(hour_place);
	text_layer_destroy(minute_place);
	text_layer_destroy(date_place);
	text_layer_destroy(day_full_place);
	layer_remove_from_parent(hour_layer);
	layer_remove_from_parent(minute_layer);
	layer_remove_from_parent(date_layer);
	layer_remove_from_parent(day_full_layer);
	layer_destroy(hour_layer);
	layer_destroy(minute_layer);
	layer_destroy(date_layer);
	layer_destroy(day_full_layer);
	
	fonts_unload_custom_font(hour_font);
	fonts_unload_custom_font(minute_font);
	fonts_unload_custom_font(small_font);
	fonts_unload_custom_font(smaller_font);
	tick_timer_service_unsubscribe();
	
	hour = NULL;
	minute = NULL;
	day_full0 = NULL;
	date = NULL;
	
	gbitmap_destroy(x);
	bitmap_layer_destroy(x_layer);
}

void handle_init(void) {
	my_window = window_create();
	window_set_window_handlers(my_window, (WindowHandlers) {
		.load = window_load,
		.unload = window_unload,
	});
	window_stack_push(my_window, true);
};

void handle_deinit(void) {
	window_destroy(my_window);
};

int main(void) {
	handle_init();
	app_event_loop();
	handle_deinit();
};