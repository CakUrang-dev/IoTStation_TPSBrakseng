#include "Display.h"

void Display::begin() {
  _oled.begin();
  _oled.clearBuffer();
}

void Display::printLine(const char* msg) {
  _oled.clearBuffer();
  _oled.setFont(u8g2_font_6x13_tf);
  _oled.drawStr(0, 20, msg);
  _oled.sendBuffer();
}

void Display::render(const SensorData& d) {
  char buf[32];

  _oled.clearBuffer();

  _oled.setFont(u8g2_font_ncenB08_tr);
  _oled.drawStr(10, 15, "TPA MONITOR");

  _oled.setFont(u8g2_font_6x13_tf);

  sprintf(buf, "CO2 : %d ppm", d.co2);
  _oled.drawStr(0, 35, buf);

  sprintf(buf, "PM2.5 : %d ug/m3", d.pm25);
  _oled.drawStr(0, 50, buf);

  sprintf(buf, "Temp : %.1f C", d.temperature);
  _oled.drawStr(0, 65, buf);

  sprintf(buf, "Hum : %.1f %%", d.humidity);
  _oled.drawStr(0, 80, buf);

  sprintf(buf, "Wind : %.1f m/s", d.wind_speed);
  _oled.drawStr(0, 95, buf);

  sprintf(buf, "Dir : %.0f deg", d.wind_dir);
  _oled.drawStr(0, 110, buf);

  _oled.sendBuffer();
}
