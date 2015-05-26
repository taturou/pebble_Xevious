#pragma once
#include <pebble.h>

typedef struct PointAnimation PointAnimation;

typedef void (*PointAnimationUpdateCallback)(const GPoint *now_array, uint32_t array_len, void *context);

PointAnimation *point_animation_create(const GPoint *from_array, const GPoint *to_array, uint32_t array_len);
void point_animation_destroy(PointAnimation *point_anime);
Animation *point_animation_get_animation(PointAnimation *point_anime);
void point_animation_set_callback(PointAnimation *point_anime, PointAnimationUpdateCallback callback, void *context);