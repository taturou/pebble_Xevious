#include <pebble.h>
#include "xevi_number_layer.h"

#define COLOR_BG GColorBlack

#define NUM_XEVI_LAYER (6)
#define TM_HOUR_0      (0)
#define TM_HOUR_1      (1)
#define TM_MIN_0       (2)
#define TM_MIN_1       (3)
#define TM_SEC_0       (4)
#define TM_SEC_1       (5)

static Window *s_window;
static TextLayer *s_text_layer;
static XeviNumberLayer *s_xevi_layer[NUM_XEVI_LAYER];
static char s_str[32];

static void s_select_click_handler(ClickRecognizerRef recognizer, void *context) {
    bool hidden = layer_get_hidden(text_layer_get_layer(s_text_layer));
    hidden = hidden == true ? false : true;
    layer_set_hidden(text_layer_get_layer(s_text_layer), hidden);
}

static void s_up_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void s_down_click_handler(ClickRecognizerRef recognizer, void *context) {
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

static void s_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    snprintf(s_str, 32, "%d:%02d:%02d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_sec);
    text_layer_set_text(s_text_layer, s_str);

    xevi_number_layer_set_number(s_xevi_layer[TM_HOUR_0], tick_time->tm_hour / 10);
    xevi_number_layer_set_number(s_xevi_layer[TM_HOUR_1], tick_time->tm_hour % 10);
    xevi_number_layer_set_number(s_xevi_layer[TM_MIN_0],  tick_time->tm_min / 10);
    xevi_number_layer_set_number(s_xevi_layer[TM_MIN_1],  tick_time->tm_min % 10);
    xevi_number_layer_set_number(s_xevi_layer[TM_SEC_0],  tick_time->tm_sec / 10);
    xevi_number_layer_set_number(s_xevi_layer[TM_SEC_1],  tick_time->tm_sec % 10);
}

static void s_window_load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    // xevious-number-layer
    GRect frames[NUM_XEVI_LAYER] = {
        GRect(22,      7,           47, 47),
        GRect(22+47+6, 7,           47, 47),
        GRect(22,      7+47+6,      47, 47),
        GRect(22+47+6, 7+47+6,      47, 47),
        GRect(22,      7+47+6+47+6, 47, 47),
        GRect(22+47+6, 7+47+6+47+6, 47, 47),
    };
    for (int i = 0; i < NUM_XEVI_LAYER; i++) {
        s_xevi_layer[i] = xevi_number_layer_create(frames[i]);
        layer_add_child(window_layer, xevi_number_layer_get_layer(s_xevi_layer[i]));
    }

    // text-layer
    s_text_layer = text_layer_create(GRect(0, 0, bounds.size.w, 20));
#if PBL_PLATFORM_BASALT
    text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
#else
    text_layer_set_font(s_text_layer, fonts_get_system_font(FONT_KEY_GOTHIC_14));
#endif
    text_layer_set_text_alignment(s_text_layer, GTextAlignmentLeft);
    text_layer_set_background_color(s_text_layer, GColorClear);
    text_layer_set_text_color(s_text_layer, GColorWhite);
    layer_set_hidden(text_layer_get_layer(s_text_layer), true);
    layer_add_child(window_layer, text_layer_get_layer(s_text_layer));

    // tick-timer-service
    tick_timer_service_subscribe(SECOND_UNIT, s_tick_handler);
}

static void s_window_unload(Window *window) {
    // tick-timer-service
    tick_timer_service_unsubscribe();

    // xevious-number-layer
    for (int i = 0; i < NUM_XEVI_LAYER; i++) {
        xevi_number_layer_destroy(s_xevi_layer[i]);
    }

    // text-layer
    text_layer_destroy(s_text_layer);
}

static void s_init(void) {
    s_window = window_create();
    window_set_click_config_provider(s_window, s_click_config_provider);
    window_set_background_color(s_window, COLOR_BG);
#if PBL_PLATFORM_APLITE
    window_set_fullscreen(s_window, true);
#endif
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
