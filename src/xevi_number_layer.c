#include <pebble.h>
#include "xevi_number_layer.h"

typedef struct XeviNumberLayer {
    Layer *layer;
    uint8_t number;
    GColor color;
} XeviNumberLayer;

static void s_layer_update_proc(struct Layer *layer, GContext *ctx) {
    XeviNumberLayer *xevi_layer = (XeviNumberLayer*)layer_get_data(layer);
    GRect bounds = layer_get_bounds(layer);
    
    graphics_context_set_stroke_color(ctx, xevi_layer->color);

    // points
    GPoint tl = GPoint(0, 0);
    GPoint tr = GPoint(bounds.size.w - 1, 0);
    GPoint bl = GPoint(0, bounds.size.h - 1);
    GPoint br = GPoint(bounds.size.w - 1, bounds.size.h - 1);

    // draw
    if (xevi_layer->number <= XEVI_NUMBER_MAX) {
        // 0
        graphics_draw_line(ctx, tr, bl);
        // 1
        if ((xevi_layer->number & 0x1) == 0x1) {
            graphics_draw_line(ctx, bl, br);
        }
        // 2
        if ((xevi_layer->number & 0x2) == 0x2) {
            graphics_draw_line(ctx, br, tr);
        }
        // 4
        if ((xevi_layer->number & 0x4) == 0x4) {
            graphics_draw_line(ctx, tr, tl);
        }
        // 8
        if ((xevi_layer->number & 0x8) == 0x8) {
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

XeviNumberLayer *xevi_number_layer_create(GRect frame) {
    XeviNumberLayer *xevi_layer = NULL;
    
    Layer *layer = layer_create_with_data(frame, sizeof(XeviNumberLayer));
    if (layer != NULL) {
        // layer
        layer_set_update_proc(layer, s_layer_update_proc);
        
        // set member
        xevi_layer = (XeviNumberLayer*)layer_get_data(layer);
        xevi_layer->layer = layer;
        xevi_layer->number = 0;
        xevi_layer->color = GColorBlack;
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
    return xevi_layer->number;
}

void xevi_number_layer_set_number(XeviNumberLayer *xevi_layer, uint8_t number) {
    if (number <= XEVI_NUMBER_MAX) {
        xevi_layer->number = number;
    } else {
        xevi_layer->number = XEVI_NUMBER_INVALID;
    }
    layer_mark_dirty(xevi_number_layer_get_layer(xevi_layer));
}