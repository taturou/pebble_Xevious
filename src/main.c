#include <pebble.h>
#include "xevi_number_layer.h"

static Window *s_window;
static TextLayer *s_text_layer;
static XeviNumberLayer *s_xevi_layer;
static uint8_t s_number = 0;
static char s_str[32];

static void s_draw_number(void *context) {
    snprintf(s_str, 32, "%d", s_number);
    text_layer_set_text(s_text_layer, s_str);

    xevi_number_layer_set_number(s_xevi_layer, s_number);
}

static void s_select_click_handler(ClickRecognizerRef recognizer, void *context) {
    s_number = 0;
    s_draw_number(context);
}

static void s_up_click_handler(ClickRecognizerRef recognizer, void *context) {
    s_number++;
    s_draw_number(context);
}

static void s_down_click_handler(ClickRecognizerRef recognizer, void *context) {
    s_number--;
    s_draw_number(context);
}

static void s_click_config_provider(void *context) {
#if 0
    window_single_click_subscribe(BUTTON_ID_SELECT, s_select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, s_up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, s_down_click_handler);
#else
    window_single_repeating_click_subscribe(BUTTON_ID_SELECT, 500, s_select_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_UP, 500, s_up_click_handler);
    window_single_repeating_click_subscribe(BUTTON_ID_DOWN, 500, s_down_click_handler);
#endif
}

static void s_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    s_text_layer = text_layer_create(GRect(0, 0, bounds.size.w, 20));
    text_layer_set_text(s_text_layer, "Press a button");
    text_layer_set_text_alignment(s_text_layer, GTextAlignmentCenter);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer));
    
    s_xevi_layer = xevi_number_layer_create(GRect(50, 50, 51, 51));
    layer_add_child(window_layer, xevi_number_layer_get_layer(s_xevi_layer));
}

static void s_window_unload(Window *window) {
    text_layer_destroy(s_text_layer);
}

static void s_init(void) {
    s_window = window_create();
    window_set_click_config_provider(s_window, s_click_config_provider);
    window_set_window_handlers(s_window, (WindowHandlers) {
        .load = s_window_load,
        .unload = s_window_unload,
    });
    const bool animated = true;
    window_stack_push(s_window, animated);
}

static void s_deinit(void) {
    window_destroy(s_window);
}

int main(void) {
    s_init();
    app_event_loop();
    s_deinit();
}