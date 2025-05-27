#pragma once

#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/core/helpers.h"

#define NUM_LEDS 256

namespace esphome {
  namespace zmatrix {

    class ZMatrix : public Component {
    public:
      void setup() override;
      void dump_config() override;

      void set_width(const int width) { this->width = width; this->num_leds = this->width * this->width; };
      void set_height(const int height) { this->height = height; this->num_leds = this->width * this->width; };
      void set_segment_size(const int segmentSize) { this->segmentSize = segmentSize; };

      void showTime(Color color, const int hour, const int minute, const int pos);
      void randColor(uint8_t hue, uint8_t var, int time);
      void matrix();
      void nationalDay();
      void fire();

      float get_setup_priority() const override { return esphome::setup_priority::PROCESSOR; }

      void set_update_interval(uint32_t val) { /* ignore */
      }

      void select_index(size_t index);
      Color get_finalled(uint8_t x, uint8_t y, float brightness);

      uint16_t XY( uint8_t x, uint8_t y) {
        if ( (y & 0x01)) {
          return ((this->height - 1 - y) * this->width) + x;
        } else {
          return ((this->height - 1 - y) * this->width) + this->width - 1 - x;
        }
      }
      float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
      }

      float RGB_to_H(float r, float g, float b);

      void copy_leds();

      int gifanim = 0;
      int gifanimstart = 0;
      int gifanimlastframe = 0;
      size_t gifanimPrevMode = 0;

    protected:
      void loop() {/* ignore */}

    private:
      int width = 16;
      int height = 16;
      int num_leds = 256;
      int segmentSize = 1;

      Color finalleds[NUM_LEDS];
      Color leds[NUM_LEDS];
      Color start[NUM_LEDS];
      Color end[NUM_LEDS];
      int skiped = 1;
      int previousTime = 0;
      int animBrightnessValue = 250;
      int animSkipValue = 0;

      int prevmin = -1;
      int cx = 0;
      int cy = 0;

      void displayDigit(Color color, const int digit, const int x, const int y);

      int mapint(int x, int in_min, int in_max, int out_min, int out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
      }
    };


  }  // namespace empty_component
}  // namespace esphome
