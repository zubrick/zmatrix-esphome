#pragma once

#include "esphome/core/component.h"
#include "esphome/core/color.h"
#include "esphome/core/helpers.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/display/display.h"
#include "Digit.h"

namespace esphome {
  namespace zclock {

    class ZClock : public Component {
    public:
      void setup() override;
      void dump_config() override;

      void display();

      void set_update_interval(uint32_t val) { /* ignore */
      }
      void set_update_interval_ms(uint8_t update_interval_ms) {this->update_interval_ms_ = update_interval_ms;}

      void set_reftime(esphome::time::RealTimeClock *rtc) { this->_rtc = rtc; }
      void set_display(esphome::display::Display *display) { this->_display = display; }

      void setTimeColor(esphome::Color color);
      void setDateColor(esphome::Color color);

      float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
      }

      String get_date() {return this->_date_string;}

      uint16_t getxo() { return xo; }
      uint16_t getyo() { return yo; }

    protected:
      void loop();

    private:
      esphome::time::RealTimeClock *_rtc = NULL;
      esphome::display::Display *_display = NULL;

      esphome::Color time_color = esphome::Color(0, 255, 255);
      esphome::Color date_color = esphome::Color(0, 255, 0);

      uint8_t update_interval_ms_ = 10;

      Digit * digit0;
      Digit * digit1;
      Digit * digit2;
      Digit * digit3;
      Digit * digit4;
      Digit * digit5;


      Digit * date0;
      Digit * date1;
      Digit * date2;
      Digit * date3;
      Digit * date4;
      Digit * date5;
      Digit * date6;
      Digit * date7;


      String _date_string = "88.88.8888";
      int _prevDD = 88;
      uint16_t xo = 0;
      uint16_t yo = 0;
    };
  }
}
