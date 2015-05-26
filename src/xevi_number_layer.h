#pragma once
#include <pebble.h>

#define XEVI_NUMBER_INVALID (0xFF)
#define XEVI_NUMBER_MAX     (0x0F)

typedef struct XeviNumberLayer XeviNumberLayer;

XeviNumberLayer *xevi_number_layer_create(GRect frame);
void xevi_number_layer_destroy(XeviNumberLayer *xevi_layer);
Layer *xevi_number_layer_get_layer(XeviNumberLayer *xevi_layer);
uint8_t xevi_number_layer_get_number(XeviNumberLayer *xevi_layer, uint8_t number);
void xevi_number_layer_set_number(XeviNumberLayer *xevi_layer, uint8_t number);