#include <pebble.h>
#include "xevi_number_layer.h"

#define IS_WATCHAPP (0)

#define COLOR_BG GColorBlack

#define NUM_XEVI_LAYER (6)
#define TM_HOUR_0      (0)
#define TM_HOUR_1      (1)
#define TM_MIN_0       (2)
#define TM_MIN_1       (3)
#define TM_SEC_0       (4)
#define TM_SEC_1       (5)

#if PBL_PLATFORM_APLITE
    #define TIME_FONT      FONT_KEY_BITHAM_42_LIGHT
    #define TIME_COLOR_FG  GColorWhite
    #define TIME_COLOR_BG  GColorClear
    #define TIME_ORIGIN_Y  56
    #define TIME_SIZE_H    42
#else
    #define TIME_FONT      FONT_KEY_LECO_38_BOLD_NUMBERS
    #define TIME_COLOR_FG  GColorWhite
    #define TIME_COLOR_BG  GColorClear
    #define TIME_ORIGIN_Y  56
    #define TIME_SIZE_H    42
#endif

static Window *s_window;
static TextLayer *s_time_layer;
static XeviNumberLayer *s_xevi_layer[NUM_XEVI_LAYER];
static char s_str[32];

static void s_time_layer_set_hidden(void) {
    bool hidden = layer_get_hidden(text_layer_get_layer(s_time_layer));
    hidden = hidden == true ? false : true;
    layer_set_hidden(text_layer_get_layer(s_time_layer), hidden);
}

#if IS_WATCHAPP
static void s_select_click_handler(ClickRecognizerRef recognizer, void *context) {
    s_time_layer_set_hidden();
}

static void s_up_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void s_down_click_handler(ClickRecognizerRef recognizer, void *context) {
}

static void s_click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, s_select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, s_up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, s_down_click_handler);
}
#endif /* IS_WATCHAPP */

static void s_accel_tap_handler(AccelAxisType axis, int32_t direction) {
    s_time_layer_set_hidden();
}

static void s_tick_handler(struct tm *tick_time, TimeUnits units_changed) {
//    snprintf(s_str, 32, "%d:%02d:%02d", tick_time->tm_hour, tick_time->tm_min, tick_time->tm_sec);
    snprintf(s_str, 32, "%2d:%02d", tick_time->tm_hour, tick_time->tm_min);
    text_layer_set_text(s_time_layer, s_str);

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
        GRect(20,      7,           47, 47),
        GRect(20+47+6, 7,           47, 47),
        GRect(20,      7+47+6,      47, 47),
        GRect(20+47+6, 7+47+6,      47, 47),
        GRect(20,      7+47+6+47+6, 47, 47),
        GRect(20+47+6, 7+47+6+47+6, 47, 47),
    };
    for (int i = 0; i < NUM_XEVI_LAYER; i++) {
        s_xevi_layer[i] = xevi_number_layer_create(frames[i]);
        layer_add_child(window_layer, xevi_number_layer_get_layer(s_xevi_layer[i]));
    }

    // text-layer
    s_time_layer = text_layer_create(GRect(0, TIME_ORIGIN_Y, bounds.size.w, TIME_SIZE_H));
    text_layer_set_font(s_time_layer, fonts_get_system_font(TIME_FONT));
    text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);
    text_layer_set_background_color(s_time_layer, TIME_COLOR_BG);
    text_layer_set_text_color(s_time_layer, TIME_COLOR_FG);
    layer_set_hidden(text_layer_get_layer(s_time_layer), true);
    layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

    // tick-timer-service
    tick_timer_service_subscribe(SECOND_UNIT, s_tick_handler);

    // tap-service
    accel_service_set_sampling_rate(ACCEL_SAMPLING_50HZ);
    accel_tap_service_subscribe(s_accel_tap_handler);
}

static void s_window_unload(Window *window) {
    // tap-service
    accel_tap_service_unsubscribe();

    // tick-timer-service
    tick_timer_service_unsubscribe();

    // xevious-number-layer
    for (int i = 0; i < NUM_XEVI_LAYER; i++) {
        xevi_number_layer_destroy(s_xevi_layer[i]);
    }

    // text-layer
    text_layer_destroy(s_time_layer);
}

static void s_init(void) {
    s_window = window_create();
#if IS_WATCHAPP
    window_set_click_config_provider(s_window, s_click_config_provider);
#endif /* IS_WATCHAPP */
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
