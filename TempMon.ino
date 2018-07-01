#include <OneWire.h>
#include <DS18B20.h>
#include <SH1106_I2C_Adaptor.h>
#include <glcd_fonts.h>
#include "WeekStat.h"

SH1106_I2C_Adaptor g_display;

OneWire g_w(10);
DS18B20 g_t(g_w);
uint8_t g_rom[8];

WeekStat g_stat;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  g_display.init();
  g_t.probe(g_rom);
  g_t.convert();
}

#define T_HI 28       // high temperature limit
#define T_LO 3        // low temperature limit
#define PERIOD 30     // measuring period in seconds
#define DISPLAY_W 128 // display width in pixels
#define TEMP_W 80     // temperature field width in pixels

void loop()
{
  int16_t t;

  delay(PERIOD * 1000);

  g_t.read(t);
  g_t.convert();
  t /= 16;

  int active = (t <= T_LO || T_HI <= t);
  g_stat.update(t, active);

  String t_str(t);
  glcd_print_str_w(&g_display, 0, 0, TEMP_W, t_str.c_str(), &g_font_Tahoma44x47D, 1);

  int active_h = g_stat.active_count() / (3600 / PERIOD);
  String t_active_str = String(active_h) + String(" h");
  String t_min_max_str = String(g_stat.minimum()) + String(" .. ") + String(g_stat.mean()) + String(" .. ") + String(g_stat.maximum());
  glcd_print_str_r(&g_display, TEMP_W, 0, DISPLAY_W-TEMP_W, t_active_str.c_str(),  &g_font_Tahoma15x16, 1);
  glcd_print_str_r(&g_display, 0, 6, DISPLAY_W, t_min_max_str.c_str(), &g_font_Tahoma15x16, 1);

  if (active) {
    digitalWrite(LED_BUILTIN, 1);
  } else {
    digitalWrite(LED_BUILTIN, 0);
  }
}


