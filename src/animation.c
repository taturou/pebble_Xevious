#include <pebble.h>
#include "animation.h"

typedef struct AnimeLineContext {
    Animation *anime;
    AnimationImplementation impl;
    const GPoint *from_array;
    const GPoint *to_array;
    uint32_t array_len;
    AnimeLineCallback callback;
    void *context;
} AnimeLineContext;

#define ABS(n)    (((n) < 0) ? ((n) * -1) : (n))

static void s_animation_started_handler(Animation *anime, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "started");
}

static void s_animation_stopped_handler(Animation *anime, bool finished, void *context) {
    APP_LOG(APP_LOG_LEVEL_INFO, "stopped");
}

static void s_animation_setup_implementation(Animation *anime) {
    APP_LOG(APP_LOG_LEVEL_INFO, "setup");
}

static void s_animation_teardown_implementation(Animation *anime) {
    APP_LOG(APP_LOG_LEVEL_INFO, "teardown");
    AnimeLineContext *alctx = (AnimeLineContext*)animation_get_context(anime);
    free(alctx);
    animation_destroy(anime);
}

static void s_animation_update_implementation(Animation *anime, const AnimationProgress progress) {
    AnimeLineContext *alctx = (AnimeLineContext*)animation_get_context(anime);
    GPoint now_array[alctx->array_len];
    
    for (uint32_t i = 0; i < alctx->array_len; i++) {
        const GPoint *from = &alctx->from_array[i];
        const GPoint *to = &alctx->to_array[i];
        GPoint *now = &now_array[i];
        
        now->x = from->x + (((long)progress * (to->x - from->x)) / ANIMATION_NORMALIZED_MAX) + 0.5;
        now->y = from->y + (((long)progress * (to->y - from->y)) / ANIMATION_NORMALIZED_MAX) + 0.5;
    }
    (alctx->callback)(now_array, alctx->array_len, alctx->context);
}

void anime_line(const GPoint *from_array, const GPoint *to_array, uint32_t array_len, AnimeLineCallback callback, void *context) {
    AnimeLineContext *alctx = calloc(sizeof(AnimeLineContext), 1);
    if (alctx != NULL) {
        Animation *anime = animation_create();

        // set member
        alctx->anime = anime;
        alctx->impl = (AnimationImplementation) {
            .setup = s_animation_setup_implementation,
            .teardown = s_animation_teardown_implementation,
            .update = s_animation_update_implementation,
        };
        alctx->from_array = from_array;
        alctx->to_array = to_array;
        alctx->array_len = array_len;
        alctx->callback = callback;
        alctx->context = context;
        
        // animation
        animation_set_duration(anime, 150);
        animation_set_delay(anime, 0);
        animation_set_curve(anime, AnimationCurveLinear);
        animation_set_handlers(anime,
                               (AnimationHandlers) {
                                   .started = s_animation_started_handler,
                                   .stopped = s_animation_stopped_handler,
                               },
                               (void*)alctx);
        animation_set_implementation(anime, &alctx->impl);
        
        // run
        animation_schedule(anime);
    }
}