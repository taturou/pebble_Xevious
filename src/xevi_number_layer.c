#include <pebble.h>
#include "xevi_number_layer.h"
#include "animation.h"
    
#define STROKE_WIDTH (4)

#define MAX_LINE     (5)
#define MAX_POINT    (MAX_LINE * 2)    // MAX_LINE * 2points (from, to)

#define GEN          (2)
#define GEN_NEW      (0)
#define GEN_OLD      (1)

#if PBL_COLOR
 #define DEFAULT_COLOR GColorRed
#else
 #define DEFAULT_COLOR GColorWhite
#endif

#define ISBIT(num, mask)    ((num & mask) == mask)
#define gpoint_equal(p0,p1) ((p0.x == p1.x) && (p0.y == p1.y))

typedef struct XeviNumberLayer {
    Layer *layer;
    uint8_t number[GEN];
    GColor color;
    GPoint point[MAX_POINT];
    GPoint anime_from[MAX_POINT];
    GPoint anime_to[MAX_POINT];
    uint32_t line_num;
} XeviNumberLayer;

static void s_layer_update_proc(struct Layer *layer, GContext *ctx) {
    XeviNumberLayer *xevi_layer = (XeviNumberLayer*)layer_get_data(layer);

#if PBL_PLATFORM_BASALT
    graphics_context_set_antialiased(ctx, true);
    graphics_context_set_stroke_width(ctx, STROKE_WIDTH);
#endif
    graphics_context_set_stroke_color(ctx, xevi_layer->color);

    const GPoint *point = xevi_layer->point;
    for (uint32_t i = 0; i < xevi_layer->line_num; i += 2) {
        if (! gpoint_equal(point[i], point[i+1])) {
            graphics_draw_line(ctx, point[i], point[i+1]);
        }
    }
}

static void s_number_to_line(XeviNumberLayer *xevi_layer) {
    GRect bounds = layer_get_bounds(xevi_number_layer_get_layer(xevi_layer));
    const uint8_t *number = xevi_layer->number;
    GPoint *anime_from = xevi_layer->anime_from;
    GPoint *anime_to = xevi_layer->anime_to;
    uint32_t line_num = 0;

    // points
    GPoint tl = GPoint(0, 0);
    GPoint tr = GPoint(bounds.size.w - 1, 0);
    GPoint bl = GPoint(0, bounds.size.h - 1);
    GPoint br = GPoint(bounds.size.w - 1, bounds.size.h - 1);

    struct {
        uint8_t mask;
        GPoint p0;
        GPoint p1;
    } p[MAX_LINE - 1] = {
        {0x1, bl, br}, // 1
        {0x2, br, tr}, // 2
        {0x4, tr, tl}, // 4
        {0x8, tl, bl}, // 8
    };
    
    // 0 (not animation)
    {
        GPoint p0 = tr;
        GPoint p1 = bl;
        // p0
        anime_from[line_num] = p0;
        anime_to[line_num] = p0;
        line_num++;
        // p1
        anime_from[line_num] = p1;
        anime_to[line_num] = p1;
        line_num++;
    }

    // 1 - 8
    for (int i = 0; i < (MAX_LINE - 1); i++) {
        if (ISBIT(number[GEN_OLD], p[i].mask) && ISBIT(number[GEN_NEW], p[i].mask)) {
            // p0
            anime_from[line_num] = p[i].p0;
            anime_to[line_num] = p[i].p0;
            line_num++;
            // p1
            anime_from[line_num] = p[i].p1;
            anime_to[line_num] = p[i].p1;
            line_num++;
        } else if (ISBIT(number[GEN_OLD], p[i].mask) && !ISBIT(number[GEN_NEW], p[i].mask)) {
            // p0
            anime_from[line_num] = p[i].p0;
            anime_to[line_num] = p[i].p1;
            line_num++;
            // p1
            anime_from[line_num] = p[i].p1;
            anime_to[line_num] = p[i].p1;
            line_num++;
        } else if (!ISBIT(number[GEN_OLD], p[i].mask) && ISBIT(number[GEN_NEW], p[i].mask)) {
            // p0
            anime_from[line_num] = p[i].p0;
            anime_to[line_num] = p[i].p0;
            line_num++;
            // p1
            anime_from[line_num] = p[i].p0;
            anime_to[line_num] = p[i].p1;
            line_num++;
        } else { /* if (!ISBIT(number[GEN_OLD], p[i].mask) && !ISBIT(number[GEN_NEW], p[i].mask)) */
            /* do nothing */
        }
    }
    xevi_layer->line_num = line_num;
}

static void s_anime_line_callback(const GPoint *now_array, uint32_t array_len, void *context) {
    XeviNumberLayer *xevi_layer = (XeviNumberLayer*)context;

    memcpy(xevi_layer->point, now_array, sizeof(GPoint) * array_len);
    layer_mark_dirty(xevi_number_layer_get_layer(xevi_layer));
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
        xevi_layer->number[GEN_NEW] = XEVI_NUMBER_INVALID;
        xevi_layer->number[GEN_OLD] = XEVI_NUMBER_INVALID;
        xevi_layer->color = DEFAULT_COLOR;
        xevi_number_layer_set_number(xevi_layer, 0);
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
        if (xevi_layer->number[GEN_NEW] != number) {
            xevi_layer->number[GEN_OLD] = xevi_layer->number[GEN_NEW];
            xevi_layer->number[GEN_NEW] = number;
            
            // create points
            s_number_to_line(xevi_layer);
            
            // create animation
            PointAnimation *point_anime = point_animation_create(xevi_layer->anime_from, xevi_layer->anime_to, xevi_layer->line_num);
            point_animation_set_callback(point_anime, s_anime_line_callback, (void*)xevi_layer);
            Animation *anime = point_animation_get_animation(point_anime);
            animation_set_duration(anime, 1000);
            animation_set_curve(anime, AnimationCurveEaseIn);

            
            // run animation
            animation_schedule(anime);
        }
    }
}
