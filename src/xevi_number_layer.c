#include <pebble.h>
#include "xevi_number_layer.h"

#define STROKE_WIDTH (5)

#define GEN          (2)
#define GEN_NEW      (0)
#define GEN_OLD      (1)

#if PBL_COLOR
 #define COLOR_GEN_NEW GColorRed
 #define COLOR_GEN_OLD GColorDarkGray
#else
 #define COLOR_GEN_NEW GColorWhite
 #define COLOR_GEN_OLD GColorBlack
#endif
    
typedef struct XeviNumberLayer {
    Layer *layer;
    uint8_t number[GEN];
    GColor color[GEN];
} XeviNumberLayer;

static void s_draw_number(GContext *ctx, GRect bounds, uint8_t number) {
    // points
    GPoint tl = GPoint(0, 0);
    GPoint tr = GPoint(bounds.size.w - 1, 0);
    GPoint bl = GPoint(0, bounds.size.h - 1);
    GPoint br = GPoint(bounds.size.w - 1, bounds.size.h - 1);

    // draw
    if (number <= XEVI_NUMBER_MAX) {
        // 0
        graphics_draw_line(ctx, tr, bl);
        // 1
        if ((number & 0x1) == 0x1) {
            graphics_draw_line(ctx, bl, br);
        }
        // 2
        if ((number & 0x2) == 0x2) {
            graphics_draw_line(ctx, br, tr);
        }
        // 4
        if ((number & 0x4) == 0x4) {
            graphics_draw_line(ctx, tr, tl);
        }
        // 8
        if ((number & 0x8) == 0x8) {
            graphics_draw_line(ctx, tl, bl);
        }
    } else {
        graphics_draw_line(ctx, tr, bl);
        graphics_draw_line(ctx, bl, br);
        graphics_draw_line(ctx, br, tr);
        graphics_draw_line(ctx, tr, tl);
        graphics_draw_line(ctx, tl, bl);
        graphics_draw_line(ctx, tl, br);
    }
}

static void s_layer_update_proc(struct Layer *layer, GContext *ctx) {
    XeviNumberLayer *xevi_layer = (XeviNumberLayer*)layer_get_data(layer);
    GRect bounds = layer_get_bounds(layer);

#if PBL_PLATFORM_BASALT
    graphics_context_set_antialiased(ctx, true);
    graphics_context_set_stroke_width(ctx, STROKE_WIDTH);
#endif
    // draw GEN_OLD
    if (xevi_layer->number[GEN_NEW] != xevi_layer->number[GEN_OLD]) {
        graphics_context_set_stroke_color(ctx, xevi_layer->color[GEN_OLD]);
        s_draw_number(ctx, bounds, xevi_layer->number[GEN_OLD]);
    }

    // draw GEN_NEW
    graphics_context_set_stroke_color(ctx, xevi_layer->color[GEN_NEW]);
    s_draw_number(ctx, bounds, xevi_layer->number[GEN_NEW]);
}

XeviNumberLayer *xevi_number_layer_create(GRect frame) {
    XeviNumberLayer *xevi_layer = NULL;

    Layer *layer = layer_create_with_data(frame, sizeof(XeviNumberLayer));
    if (layer != NULL) {
        // layer
        layer_set_update_proc(layer, s_layer_update_proc);

        // set member
        xevi_layer = (XeviNumberLayer*)layer_get_data(layer);
        xevi_layer->layer = layer;
        xevi_layer->number[GEN_NEW] = 0;
        xevi_layer->number[GEN_OLD] = 0;
        xevi_layer->color[GEN_NEW] = COLOR_GEN_NEW;
        xevi_layer->color[GEN_OLD] = COLOR_GEN_OLD;
    }
    return xevi_layer;
}

void xevi_number_layer_destroy(XeviNumberLayer *xevi_layer) {
    layer_destroy(xevi_number_layer_get_layer(xevi_layer));
}

Layer *xevi_number_layer_get_layer(XeviNumberLayer *xevi_layer) {
    return xevi_layer->layer;
}

uint8_t xevi_number_layer_get_number(XeviNumberLayer *xevi_layer, uint8_t number) {
    return xevi_layer->number[GEN_NEW];
}

void xevi_number_layer_set_number(XeviNumberLayer *xevi_layer, uint8_t number) {
    if (number <= XEVI_NUMBER_MAX) {
        xevi_layer->number[GEN_OLD] = xevi_layer->number[GEN_NEW];
        xevi_layer->number[GEN_NEW] = number;
        layer_mark_dirty(xevi_number_layer_get_layer(xevi_layer));
    }
}
