#pragma once

#include "esphome/core/component.h"
#include "esphome/components/time/real_time_clock.h"

namespace esphome {
  namespace ds3231 {

    struct Time {
          int year;
          int month;
          int day;
          int hour;
          int minute;
          int second;
          int day_of_week;
        };

    class DS3231 : public Component {
    public:
      void setup() override;
      void dump_config() override;

      void sync();

      void set_time(int year, int month, int day, int hour, int minute, int second, int day_of_week);
      Time get_time();
      void set_i2c_address(const int address) { this->i2c_address = address;};
      void set_reftime(esphome::time::RealTimeClock *rtc) { this->rtc_ = rtc; }

      float get_setup_priority() const override { return esphome::setup_priority::PROCESSOR; }

      void set_update_interval(uint32_t val) { /* ignore */
      }
      void set_update_interval_min(uint8_t update_interval_min) {this->update_interval_min_ = update_interval_min;}

    protected:
      void loop() override;

    private:
      int i2c_address = 0x68;
      uint8_t update_interval_min_ = 0;
      int lastUpdate = 0;
      bool found = false;

      esphome::time::RealTimeClock *rtc_ = NULL;
      uint8_t dec_to_bcd(int val);

      int bcd_to_dec(uint8_t val);

    };


  }  // namespace empty_component
}  // namespace esphome
