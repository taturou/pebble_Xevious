#pragma once
#include <pebble.h>
    
typedef void (*AnimeLineCallback)(const GPoint *now_array, uint32_t array_len, void *context);

void anime_line(const GPoint *from_array, const GPoint *to_array, uint32_t array_len, AnimeLineCallback callback, void *context);