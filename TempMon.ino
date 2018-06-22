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
int g_t_active[7];
int g_t_day = -1;

// Update min / max values
void t_update(int t, int active)
{
  int d = millis() / (24UL*3600UL*1000UL);
  d %= 7; // day 0..6
  if (g_t_day < 0) {
    // First time initialization
    for (int i = 0; i < 7; ++i) {
      g_t_min[i] = t;
      g_t_max[i] = t;
      g_t_active[i] = 0;
    }
  } else if (g_t_day != d) {
    // Day changed
    g_t_min[d] = t;
    g_t_max[d] = t;    
    g_t_active[d] = active;
  } else {
    // Update current day
    if (g_t_min[d] > t) g_t_min[d] = t;
    if (g_t_max[d] < t) g_t_max[d] = t;
    g_t_active[d] += active;
  }
  g_t_day = d;
}

// Get minimum temperature value for the last 7 days
int t_min()
{
  int t = g_t_min[0];
  for (int i = 1; i < 7; ++i)
    if (g_t_min[i] < t) t = g_t_min[i];
  return t;
}

// Get maximum temperature value for the last 7 days
int t_max()
{
  int t = g_t_max[0];
  for (int i = 1; i < 7; ++i)
    if (g_t_max[i] > t) t = g_t_max[i];
  return t;
}

// Returns the total activity count during the last 7 days
int t_active()
{
  int active = 0;
  for (int i = 0; i < 7; ++i)
    active += g_t_active[i];
  return active;
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
  t_update(t, active);

  String t_str(t);
  glcd_print_str_w(&g_display, 0, 0, TEMP_W, t_str.c_str(), &g_font_Tahoma44x47D, 1);

  int active_h = t_active() / (3600 / PERIOD);
  String t_active_str = String(active_h) + String(" h");
  String t_min_max_str = String(t_min()) + String(" .. ") + String(t_max());
  glcd_print_str_r(&g_display, TEMP_W, 0, DISPLAY_W-TEMP_W, t_active_str.c_str(),  &g_font_Tahoma15x16, 1);
  glcd_print_str_r(&g_display, 0, 6, DISPLAY_W, t_min_max_str.c_str(), &g_font_Tahoma15x16, 1);

  if (active) {
    digitalWrite(LED_BUILTIN, 1);
  } else {
    digitalWrite(LED_BUILTIN, 0);
  }
}


