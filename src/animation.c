#include <pebble.h>
#include "animation.h"

typedef struct PointAnimation {
    Animation *anime;
    AnimationImplementation impl;
    const GPoint *from_array;
    const GPoint *to_array;
    uint32_t array_len;
    PointAnimationUpdateCallback update_callback;
    void *context;
} PointAnimation;

#define ABS(n)    (((n) < 0) ? ((n) * -1) : (n))

static void s_animation_teardown_implementation(Animation *anime) {
    PointAnimation *point_anime = (PointAnimation*)animation_get_context(anime);
    free(point_anime);
    animation_destroy(anime);
}

static void s_animation_update_implementation(Animation *anime, const AnimationProgress progress) {
    PointAnimation *point_anime = (PointAnimation*)animation_get_context(anime);
    GPoint now_array[point_anime->array_len];
    
    for (uint32_t i = 0; i < point_anime->array_len; i++) {
        const GPoint *from = &point_anime->from_array[i];
        const GPoint *to = &point_anime->to_array[i];
        GPoint *now = &now_array[i];
        
        now->x = from->x + (((long)progress * (to->x - from->x)) / ANIMATION_NORMALIZED_MAX) + 0.5;
        now->y = from->y + (((long)progress * (to->y - from->y)) / ANIMATION_NORMALIZED_MAX) + 0.5;
    }
    (point_anime->update_callback)(now_array, point_anime->array_len, point_anime->context);
}

PointAnimation *point_animation_create(const GPoint *from_array, const GPoint *to_array, uint32_t array_len) {
    PointAnimation *point_anime = calloc(sizeof(PointAnimation), 1);
    if (point_anime != NULL) {
        Animation *anime = animation_create();

        // set member
        point_anime->anime = anime;
        point_anime->impl = (AnimationImplementation) {
            .setup = NULL,
            .teardown = s_animation_teardown_implementation,
            .update = s_animation_update_implementation,
        };
        point_anime->from_array = from_array;
        point_anime->to_array = to_array;
        point_anime->array_len = array_len;
        point_anime->update_callback = NULL;
        point_anime->context = NULL;
        
        // animation
        animation_set_handlers(anime,
                               (AnimationHandlers) {
                                   .started = NULL,
                                   .stopped = NULL,
                               },
                               (void*)point_anime);
        animation_set_implementation(anime, &point_anime->impl);
    }
    return point_anime;
}

void point_animation_destroy(PointAnimation *point_anime) {
    animation_destroy(point_animation_get_animation(point_anime));
    free(point_anime);
}

Animation *point_animation_get_animation(PointAnimation *point_anime) {
    return point_anime->anime;
}

void point_animation_set_callback(PointAnimation *point_anime, PointAnimationUpdateCallback callback, void *context) {
    point_anime->update_callback = callback;
    point_anime->context = context;
}