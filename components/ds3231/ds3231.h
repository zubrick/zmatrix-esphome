#pragma once

#include "esphome/core/component.h"
#include "esphome/components/time/real_time_clock.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/hal.h"

namespace esphome {
  namespace ds3231 {

    struct Time {
          int second;
          int minute;
          int hour;
          int day_of_week;
          int day;
          int month;
          int year;
        };

    class DS3231 : public time::RealTimeClock, public i2c::I2CDevice {
    public:
      void setup() override;
      void dump_config() override;
      void update() override;
      ESPTime now();

      bool read_rtc_();

      void sync();

      void set_time(int year, int month, int day, int hour, int minute, int second, int day_of_week);
      Time get_time();
      void set_reftime(esphome::time::RealTimeClock *rtc) { this->rtc_ = rtc; }

      float get_setup_priority() const override { return esphome::setup_priority::PROCESSOR; }

      void set_update_interval(uint32_t val) { /* ignore */
      }
      void set_update_interval_min(uint32_t update_interval_min) {this->update_interval_min_ = update_interval_min;}
      void set_read_interval_min(uint32_t read_interval_min) {this->read_interval_min_ = read_interval_min;}

    protected:
      void loop() override;

    private:
      uint32_t update_interval_min_ = 0;
      int lastUpdate = 0;
      uint32_t read_interval_min_ = 0;
      int lastRead = 0;
      bool found = false;
      uint8_t databuffer[7];

      esphome::time::RealTimeClock *rtc_ = NULL;
      uint8_t dec_to_bcd(int val);

      int bcd_to_dec(uint8_t val);

    };


  }  // namespace empty_component
}  // namespace esphome
