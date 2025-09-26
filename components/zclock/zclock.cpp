#include "esphome/core/log.h"
#include "zclock.h"

namespace esphome {
  namespace zclock {
    static const char *TAG = "zclock.component";

    void ZClock::setup() {
      digit0 = new Digit(_display, 8, 63 + 1 - 10*1, 15, this->time_color);
      digit1 = new Digit(_display, 8, 63 + 1 - 10*2, 15, this->time_color);
      digit2 = new Digit(_display, 8, 63 - 1 - 10*3, 15, this->time_color);
      digit3 = new Digit(_display, 8, 63 - 1 - 10*4, 15, this->time_color);
      digit4 = new Digit(_display, 8, 63 - 3 - 10*5, 15, this->time_color);
      digit5 = new Digit(_display, 8, 63 - 3 - 10*6, 15, this->time_color);

      date0 = new Digit(_display, 8, 0 + 7*0, 4, this->date_color, 3, 4);
      date1 = new Digit(_display, 8, 0 + 7*1, 4, this->date_color, 3, 4);
      date2 = new Digit(_display, 8, 4 + 7*2, 4, this->date_color, 3, 4);
      date3 = new Digit(_display, 8, 4 + 7*3, 4, this->date_color, 3, 4);
      date4 = new Digit(_display, 8, 8 + 7*4, 4, this->date_color, 3, 4);
      date5 = new Digit(_display, 8, 8 + 7*5, 4, this->date_color, 3, 4);
      date6 = new Digit(_display, 8, 8 + 7*6, 4, this->date_color, 3, 4);
      date7 = new Digit(_display, 8, 8 + 7*7, 4, this->date_color, 3, 4);

    }

    void ZClock::loop() {

    }

    void ZClock::setTimeColor(esphome::Color color) {
      ESP_LOGD(TAG, "Set Time color to (%d, %d, %d)", color.red, color.green, color.blue);
      if (color.red == 0 && color.green == 0 && color.blue == 0) {
        this->time_color = Color(0, 255, 255);
      } else {
        this->time_color = color;
      }
      digit0->setColor(this->time_color);
      digit1->setColor(this->time_color);
      digit2->setColor(this->time_color);
      digit3->setColor(this->time_color);
      digit4->setColor(this->time_color);
      digit5->setColor(this->time_color);
    }

    void ZClock::setDateColor(esphome::Color color) {
      ESP_LOGD(TAG, "Set Date color to (%d, %d, %d)", color.red, color.green, color.blue);
      if (color.red == 0 && color.green == 0 && color.blue == 0) {
        this->date_color = Color(0, 255, 0);
      } else {
        this->date_color = color;
      }
      date0->setColor(this->date_color);
      date1->setColor(this->date_color);
      date2->setColor(this->date_color);
      date3->setColor(this->date_color);
      date4->setColor(this->date_color);
      date5->setColor(this->date_color);
      date6->setColor(this->date_color);
      date7->setColor(this->date_color);
    }

    void ZClock::display() {
      //_display->filled_rectangle(2, 2, 4, 5, this->time_color);
      //_display->draw_pixel_at(1, 1, esphome::Color(255, 0, 0));
      //_display->line(7, 8, 10, 11, esphome::Color(0, 255, 0));

      uint8_t s0 = 8;
      uint8_t s1 = 8;
      uint8_t m0 = 8;
      uint8_t m1 = 8;
      uint8_t h0 = 8;
      uint8_t h1 = 8;
      uint8_t D0 = 8;
      uint8_t D1 = 8;
      uint8_t M0 = 8;
      uint8_t M1 = 8;
      uint16_t Y0 = 8;
      uint16_t Y1 = 8;
      uint16_t Y2 = 8;
      uint16_t Y3 = 8;
      if (this->_rtc != NULL) {
        auto now = this->_rtc->now();
        if (now.is_valid()) {
          int hh = now.hour;
          int mm = now.minute;
          int ss = now.second;
          int DD = now.day_of_month;
          int MM = now.month;
          int YY = now.year;


          s0 = (uint8_t) ss % 10;
          s1 = (uint8_t) ss / 10;
          m0 = (uint8_t) mm % 10;
          m1 = (uint8_t) mm / 10;
          h0 = (uint8_t) hh % 10;
          h1 = (uint8_t) hh / 10;
          D0 = (uint8_t) DD % 10;
          D1 = (uint8_t) DD / 10;
          M0 = (uint8_t) MM % 10;
          M1 = (uint8_t) MM / 10;
          Y0 = (uint16_t) YY % 10;
          Y1 = (uint16_t) (YY % 100) / 10;
          Y2 = (uint16_t) (YY % 1000) / 100;
          Y3 = (uint16_t) YY / 1000;

          if (DD != this->_prevDD) {
            this->_prevDD = DD;
            this->xo = random(0, 2);
            this->yo = random(0, 5);
            digit0->setOffset(63 + 1 - 10*1 + this->xo, 15 - this->yo);
            digit1->setOffset(63 + 1 - 10*2 + this->xo, 15 - this->yo);
            digit2->setOffset(63 - 1 - 10*3 + this->xo, 15 - this->yo);
            digit3->setOffset(63 - 1 - 10*4 + this->xo, 15 - this->yo);
            digit4->setOffset(63 - 3 - 10*5 + this->xo, 15 - this->yo);
            digit5->setOffset(63 - 3 - 10*6 + this->xo, 15 - this->yo);
            date0->setOffset(0 + 7*0 + this->xo, 4 - this->yo);
            date1->setOffset(0 + 7*1 + this->xo, 4 - this->yo);
            date2->setOffset(4 + 7*2 + this->xo, 4 - this->yo);
            date3->setOffset(4 + 7*3 + this->xo, 4 - this->yo);
            date4->setOffset(8 + 7*4 + this->xo, 4 - this->yo);
            date5->setOffset(8 + 7*5 + this->xo, 4 - this->yo);
            date6->setOffset(8 + 7*6 + this->xo, 4 - this->yo);
            date7->setOffset(8 + 7*7 + this->xo, 4 - this->yo);
          }

          this->_date_string = (DD < 10 ? "0": "") + String(DD) + "." + (MM < 10 ? "0": "") + String(MM) + "." + String(YY);
        }
      }

      digit0->Draw(s0);
      if (digit0->Morphing()) {
        s1 = m0 = m1 = h0 = h1 = 99;
      }
      digit1->Draw(s1);
      digit1->DrawColon();
      if (digit1->Morphing()) {
        m0 = m1 = h0 = h1 = 99;
      }
      digit2->Draw(m0);
      if (digit2->Morphing()) {
        m1 = h0 = h1 = 99;
      }
      digit3->Draw(m1);
      digit3->DrawColon();
      if (digit3->Morphing()) {
        h0 = h1 = 99;
      }
      digit4->Draw(h0);
      if (digit4->Morphing()) {
        h1 = 99;
      }
      digit5->Draw(h1);

      date0->Draw(D1);
      date1->Draw(D0);
      date2->DrawDot();
      date2->Draw(M1);
      date3->Draw(M0);
      date4->DrawDot();
      date4->Draw(Y3);
      date5->Draw(Y2);
      date6->Draw(Y1);
      date7->Draw(Y0);
    }

    void ZClock::dump_config(){
      ESP_LOGCONFIG(TAG, "ZClock component");
    }
  }
}
