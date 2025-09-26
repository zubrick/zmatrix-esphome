#ifndef DIGIT_H
#define DIGIT_H

#include <Arduino.h>

#include "esphome/components/display/display.h"
#include "esphome/core/color.h"


namespace esphome {
  namespace zclock {
    class Digit {

    public:
      Digit(esphome::display::Display* d, uint8_t value, uint16_t xo, uint16_t yo, esphome::Color color);
      Digit(esphome::display::Display* d, uint8_t value, uint16_t xo, uint16_t yo, esphome::Color color, int segHeight, int segWidth);
      void Draw(uint8_t value);
      void Morph(uint8_t newValue);
      uint8_t Value();
      bool Morphing();
      void DrawColon();
      void DrawDot();
      void setColor(esphome::Color color);
      void setOffset(uint16_t  xo, uint16_t yo);

    private:
      esphome::display::Display* _display;
      uint8_t _value;
      uint8_t _prev_value;
      esphome::Color _color;
      uint16_t _xOffset;
      uint16_t _yOffset;
      int animSpeed = 25;
      uint16_t sleep = 1;
      uint16_t display_draw_time = 1000;
      int16_t morph_step = -1;
      int16_t nb_step = 20;
      int segHeight = 7;
      int segWidth = 7;

      void drawPixel(uint16_t x, uint16_t y, esphome::Color c);
      void drawFillRect(int16_t x, int16_t y, uint16_t w, uint16_t h, esphome::Color c);
      void drawLine(int16_t x, int16_t y, int16_t x2, int16_t y2, esphome::Color c);
      void drawSeg(uint8_t seg);
      void clearSeg(uint8_t seg);
      void Morph2();
      void Morph3();
      void Morph4();
      void Morph5();
      void Morph6();
      void Morph7();
      void Morph8();
      void Morph9();
      void Morph0();
      void Morph1();
    };
  }
}

#endif
