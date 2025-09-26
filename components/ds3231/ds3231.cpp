#include "esphome/core/log.h"
#include "esphome/core/time.h"
#include "ds3231.h"
#include <Wire.h>

namespace esphome {
  namespace ds3231 {

    static const char *TAG = "ds3231.component";

    void DS3231::setup() {
      if (!Wire.requestFrom(this->i2c_address, 1)) {
        ESP_LOGE("DS3231", "RTC not found at I2C address 0x%x", this->i2c_address);
        return;
      }
      this->found = true;
      ESP_LOGI("DS3231", "RTC found at I2C address 0x%x", this->i2c_address);
    }

    void DS3231::loop() {
      if (this->update_interval_min_ > 0 && (this->lastUpdate + this->update_interval_min_ * 60000 < millis())) {
        this->sync();
      }
      if (this->read_interval_min_ > 0 && (this->lastRead + this->read_interval_min_ * 60000 < millis())) {
        time::RealTimeClock::synchronize_epoch_(this->now().timestamp);
        this->lastRead = millis();
      }
    }

    void DS3231::update() {
      //time::RealTimeClock::synchronize_epoch_(this->now());
    }

    void DS3231::dump_config(){
      ESP_LOGCONFIG(TAG, "DS3231 component");
      //ESP_LOGCONFIG(TAG, "  timezone: %s", this->timezone_.c_str());
    }

    void DS3231::sync() {
      this->lastUpdate= millis();
      if (this->rtc_ != NULL) {
        auto now = this->rtc_->now();
        if (now.is_valid()) {
          this->set_time(now.year, now.month, now.day_of_month, now.hour, now.minute, now.second, now.day_of_week);
          ESP_LOGI("DS3231", "RTC time set to %04d-%02d-%02d %02d:%02d:%02d",
                   now.year, now.month, now.day_of_month, now.hour, now.minute, now.second);
        } else {
          ESP_LOGE("DS3231", "SNTP time is not valid; cannot set RTC");
        }
        auto time = this->get_time();
        if (time.year > 0) {
          ESP_LOGI("DS3231", "Current RTC Time: %04d-%02d-%02d %02d:%02d:%02d",
                   time.year, time.month, time.day, time.hour, time.minute, time.second);
        } else {
          ESP_LOGE("DS3231", "Failed to read time from RTC");
        }
      }
    }

    void DS3231::set_time(int year, int month, int day, int hour, int minute, int second, int day_of_week) {
      Wire.beginTransmission(this->i2c_address);
      Wire.write(0);  // Start at register 0
      Wire.write(this->dec_to_bcd(second));        // Seconds
      Wire.write(this->dec_to_bcd(minute));        // Minutes
      Wire.write(this->dec_to_bcd(hour));          // Hours
      Wire.write(this->dec_to_bcd(day_of_week));   // Day of the week
      Wire.write(this->dec_to_bcd(day));           // Day of the month
      Wire.write(this->dec_to_bcd(month));         // Month
      Wire.write(this->dec_to_bcd(year - 2000));   // Year
      Wire.endTransmission();
    }

    Time DS3231::get_time() {
      if (!this->found) {
        ESP_LOGE("DS3231", "No RTC module found to get time from");
        return Time{0, 0, 0, 0, 0, 0, 0};
      }

      Wire.beginTransmission(this->i2c_address);
      Wire.write(0);  // Start at register 0
      Wire.endTransmission();

      if (Wire.requestFrom(0x68, 7) != 7) {
        ESP_LOGE("DS3231", "Failed to read time registers");
        return Time{0, 0, 0, 0, 0, 0, 0};
      }

      Time t{
        .second = this->bcd_to_dec(Wire.read()),
        .minute = this->bcd_to_dec(Wire.read()),
        .hour = this->bcd_to_dec(Wire.read()),
        .day_of_week = this->bcd_to_dec(Wire.read()),
        .day = this->bcd_to_dec(Wire.read()),
        .month = this->bcd_to_dec(Wire.read()),
        .year = this->bcd_to_dec(Wire.read()) + 2000
      };

      return t;
    }

    ESPTime DS3231::now() {
      Time t = this->get_time();
      ESPTime rtc_time{
        .second = (uint8_t)t.second,
        .minute = (uint8_t)t.minute,
        .hour = (uint8_t)t.hour,
        .day_of_week = (uint8_t)t.day_of_week,
        .day_of_month = (uint8_t)t.day,
        .day_of_year = 1,
        .month = (uint8_t)t.month,
        .year = (uint16_t)t.year,
        .is_dst = false,  // not used
        .timestamp = 0    // overwritten by recalc_timestamp_utc(false)
      };
      rtc_time.recalc_timestamp_utc(false);
      if (!rtc_time.is_valid()) {
        ESP_LOGE(TAG, "Invalid RTC time, not syncing to system clock.");
        return ESPTime{};
      }
      return rtc_time;
    }

    uint8_t DS3231::dec_to_bcd(int val) {
      return ((val / 10 * 16) + (val % 10));
    }

    int DS3231::bcd_to_dec(uint8_t val) {
      return ((val / 16 * 10) + (val % 16));
    }

  }  // namespace empty_component
}  // namespace esphome
