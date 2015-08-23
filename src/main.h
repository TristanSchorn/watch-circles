#pragma once

#include "pebble.h"
  
  
static const GPathInfo MINUTE_HAND_POINTS = {
  4,
  (GPoint []) {
    { -2, -7 },
    { 2, -7 },
    { 2, -70 },
    { -2, -70 }
  }
};

static const GPathInfo HOUR_HAND_POINTS = {
  8, (GPoint []){
    {-20, -10},
    {0, 0},
    {20, -10},
    {40, -30},
    {20, -50},
    {0, -60},
    {-20, -50},
    {-40, -30}
    
  }
};
