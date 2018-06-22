#include <OneWire.h>
#include <DS18B20.h>
#include "SH1106_I2C_Adaptor.h"
#include "glcd_fonts.h"

SH1106_I2C_Adaptor g_display;

OneWire g_w(10);
DS18B20 g_t(g_w);
uint8_t g_rom[8];

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  g_display.init();
  g_t.probe(g_rom);
  g_t.convert();
}

// Keep track the minimum / maximum temperature for the last 7 days
int g_t_min[7];
int g_t_max[7];
int g_t_day = -1;

// Update min / max values
void t_update(int t)
{
  int d = millis() / (24UL*3600UL*1000UL);
  d %= 7; // day 0..6
  if (g_t_day < 0) {
    // First time initialization
    for (int i = 0; i < 7; ++i) {
      g_t_min[i] = t;
      g_t_max[i] = t;
    }
  } else if (g_t_day != d) {
    // Day changed
    g_t_min[d] = t;
    g_t_max[d] = t;    
  } else {
    // Update current day
    if (g_t_min[d] > t) g_t_min[d] = t;
    if (g_t_max[d] < t) g_t_max[d] = t;
  }
  g_t_day = d;
}

// Get minimum temperature value
int t_min()
{
  int t = g_t_min[0];
  for (int i = 1; i < 7; ++i)
    if (g_t_min[i] < t) t = g_t_min[i];
  return t;
}

// Get maximum temperature value
int t_max()
{
  int t = g_t_max[0];
  for (int i = 1; i < 7; ++i)
    if (g_t_max[i] > t) t = g_t_max[i];
  return t;
}

#define T_HI 28
#define T_LO 3
#define PERIOD 10000

void loop()
{
  int16_t t;

  delay(PERIOD);

  g_t.read(t);
  g_t.convert();
  t /= 16;
  t_update(t);

  String t_str(t);
  glcd_print_str_r(&g_display, 0, 0, g_display.width(), t_str.c_str(), &g_font_Tahoma44x47D, 1);

  String t_min_max_str = String(t_min()) + String(" .. ") + String((t_max()));
  glcd_print_str_w(&g_display, 0, 6, g_display.width(), t_min_max_str.c_str(), &g_font_Tahoma15x16, 1);

  if (t >= T_HI || t <= T_LO) {
    digitalWrite(LED_BUILTIN, 1);
  } else {
    digitalWrite(LED_BUILTIN, 0);
  }
}


