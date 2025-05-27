#include "esphome/core/log.h"
#include "zmatrix.h"
#include <Arduino.h>
#include "esphome/components/light/esp_hsv_color.h"

namespace esphome {
  namespace zmatrix {

    static const char *TAG = "zmatrix.component";



    void ZMatrix::setup() {
      ESP_LOGI("ZMatrix", "Setup");
    }

    void ZMatrix::dump_config(){
      ESP_LOGCONFIG(TAG, "ZMatrix component");
      ESP_LOGCONFIG(TAG, "width %d", this->width);
      ESP_LOGCONFIG(TAG, "height %d", this->height);
    }

    void ZMatrix::showTime(Color color, const int hour, const int minute, const int pos) {
      int h1 = hour/10;
      int h2 = hour%10;
      int m1 = minute/10;
      int m2 = minute%10;
      int x = 0;
      int y = 0;
      int segmentSize = 1;
      int space = 1;
      int maxX = this->width + 1 - 2*this->segmentSize - space - 4;
      int maxY = this->height + 1 - 4*this->segmentSize - space - 6;
      if (pos > 0) {
        if (pos < 75) {
          x = this->mapint(pos, 0, 74, 0, maxX);
          y = 0;
        } else if (pos < 150) {
          x = maxX;
          y = this->mapint(pos, 75, 149, 0, maxY);
        } else if (pos < 225) {
          x = this->mapint(pos, 150, 224, maxX, 0);
          y = maxY;
        } else if (pos < 300) {
          x = 0;
          y = this->mapint(pos, 225, 299, maxY, 0);
        }
      } else {
        if(this->prevmin != minute) {
          this->cx = random(0,maxX);
          this->cy = random(0,maxY);
          this->prevmin = minute;
        }
        x = cx;
        y = cy;
      }
      this->displayDigit(color, h1, x, y);
      this->displayDigit(color, h2, x+this->segmentSize+2+space, y);
      //leds[XY(x+(this->segmentSize+2+1)*2 - 1, y+1)] = color;
      //leds[XY(x+(this->segmentSize+2+1)*2 - 1, y+3)] = color;
      this->displayDigit(color, m1, x, y+2*this->segmentSize+3+space);
      this->displayDigit(color, m2, x+this->segmentSize+2+space, y+2*this->segmentSize+3+space);
      //displayDigit(color, m1, x+(segmentSize+2+1)*2 + 1, y, segmentSize);
      //displayDigit(color, m2, x+(segmentSize+2+1)*3 + 1, y, segmentSize);
    }

    void ZMatrix::randColor(uint8_t hue, uint8_t var, int time) {
      int totTime = time + time * this->animSkipValue;
      if (skiped < this->animSkipValue) {
        skiped++;
      } else {
        skiped = 0;
      }
      if(this->previousTime == 0) {
        for(int i = 0; i < this->num_leds; i++) {
          this->end[i] = this->leds[i];
        }
      }
      if(millis() - this->previousTime >= totTime) {

        //Serial.print("hue:");
        //Serial.println(hue);
        for(int i = 0; i < this->num_leds; i++) {
          esphome::light::ESPHSVColor hsv((uint8_t)random(hue-var, hue+var), random(128, 255), random(0, animBrightnessValue));
          this->start[i] = this->end[i];
          this->end[i] = hsv.to_rgb();
          this->leds[i] = this->start[i];
        }
        this->previousTime = millis();
      } else {
        int diff = millis() - this->previousTime;
        for(int i = 0; i < this->num_leds; i++) {
          if ((i % (animSkipValue + 1)) == skiped) {
            this->leds[i] = Color((int)(this->start[i].red+(float)(this->end[i].red - this->start[i].red)*(float)diff/(float)totTime),
                                  (int)(this->start[i].green+(float)(this->end[i].green - this->start[i].green)*(float)diff/(float)totTime),
                                  (int)(this->start[i].blue+(float)(this->end[i].blue - this->start[i].blue)*(float)diff/(float)totTime));
          }
        }
      }
    }

    void ZMatrix::matrix() {
      if(millis() - this->previousTime >= 75) {
        // Move bright spots downward
        for (int row = this->height - 1; row >= 0; row--) {
          for (int col = 0; col < this->width; col++) {
            if (this->leds[XY(col, row)] == Color(175,255,175)) {
              this->leds[XY(col, row)] = Color(27,200,39); // create trail
              if (row < 15) this->leds[XY(col, row + 1)] = Color(175,255,175);
            }
          }
        }

        // Fade all leds
        for(int i = 0; i < this->num_leds; i++) {
          if (this->leds[i].green != 255) this->leds[i] = this->leds[i].fade_to_black(50); // only fade trail
        }

        // Spawn new falling spots
        if (random(0,2) == 0) // lower number == more frequent spawns
        {
          int8_t spawnX = random(0,16);
          this->leds[XY(spawnX, 0)] = Color(175,255,175 );
        }

        this->previousTime = millis();
      }
    }

    void ZMatrix::nationalDay() {
      int time = 5000;
      int var = 10;
      int totTime = time;
      if(this->previousTime == 0) {
        for(int i = 0; i < this->num_leds; i++) {
          this->end[i] = this->leds[i];
        }
      }
      if(millis() - this->previousTime >= totTime) {
        for (int y = 0; y < this->height; y++) {
          for (int x = 0; x < this->width; x++) {
            int c = 0;
            if (y > 1 && y <= (this->height - 1)/3 && x > (this->width - 1)/3 && x <= this->width - (this->width - 1)/3) {
              c = 1;
            } else if (y > (this->height - 1)/3 && y <= this->height - (this->height - 1)/3  && x > 1 && x <= this->width - 2) {
              c = 1;
            } else if (y > this->height - (this->height - 1)/3 && y <= this->height - 2 && x > (this->width - 1)/3 && x <= this->width - (this->width - 1)/3) {
              c = 1;
            }

            esphome::light::ESPHSVColor newcolor;
            if (c) {
              newcolor = esphome::light::ESPHSVColor(0, 0, random(40, 90));
            } else {
              newcolor = esphome::light::ESPHSVColor(0, random(128, 255), random(40, 90));
            }
            this->start[XY(x, y)] = this->end[XY(x, y)];
            this->end[XY(x, y)] = newcolor.to_rgb();
            this->leds[XY(x, y)] = this->start[XY(x, y)];
          }
        }
        this->previousTime = millis();
      } else {
        int diff = millis() - this->previousTime;
        for(int i = 0; i < this->num_leds; i++) {
          this->leds[i] = Color((int)(this->start[i].red+(float)(this->end[i].red - this->start[i].red)*(float)diff/(float)totTime),
                                (int)(this->start[i].green+(float)(this->end[i].green - this->start[i].green)*(float)diff/(float)totTime),
                                (int)(this->start[i].blue+(float)(this->end[i].blue - this->start[i].blue)*(float)diff/(float)totTime));
        }
      }
    }

    void ZMatrix::fire() {
      const bool colmajor = false;
      const bool mattop = false;
      const bool matleft = true;
      const bool zigzag = true;
      const uint16_t rows = 16;
      const uint16_t cols = 16;
      const uint16_t offsetx = 0;
      const uint16_t offsety = 0;
      const uint8_t maxflare = 3;
      const uint8_t flarerows = 7;
      const uint8_t flarechance = 30;
      const uint8_t flaredecay = 14;
      const uint32_t colors[] = {0x000000,0x100000,0x300000,0x600000,0x800000,0xA00000,0xC02000,0xC04000,0xC06000,0xC08000,0x807080};
      const uint8_t NCOLORS = (sizeof(colors)/sizeof(colors[0]));

      static uint8_t nflare = 0;
      static uint32_t flare[maxflare];
      static uint8_t pix[rows][cols];
      static bool needsinit = true;
      static long t = 0;

      uint16_t b, d, i, j, k, l, n, x, y, z;
      uint16_t phy_w = cols;
      uint16_t phy_h = rows;
      uint16_t phy_x = 0;
      uint16_t phy_y = 0;

      if(millis() - previousTime >= 75) {
        previousTime = millis();
        if ( needsinit == true ) {
          needsinit = false;
          for ( i=0; i<rows; ++i ) {
            for ( j=0; j<cols; ++j ) {
              if ( i == 0 ) pix[i][j] = NCOLORS - 1;
              else pix[i][j] = 0;
            }
          }
        }

        // First, move all existing heat points up the display and fade
        for ( i=rows-1; i>0; --i ) {
          for ( j=0; j<cols; ++j ) {
            uint8_t n = 0;
            if ( pix[i-1][j] > 0 )
              n = pix[i-1][j] - 1;
            pix[i][j] = n;
          }
        }

        // Heat the bottom row
        for ( j=0; j<cols; ++j ) {
          i = pix[0][j];
          if ( i > 0 ) {
            pix[0][j] = random(NCOLORS-6, NCOLORS-2);
          }
        }

        // Update existing flares
        for ( i=0; i<nflare; ++i ) {
          x = flare[i] & 0xff;
          y = (flare[i] >> 8) & 0xff;
          z = (flare[i] >> 16) & 0xff;
          b = z * 10 / flaredecay + 1;
          for ( k=(y-b); k<(y+b); ++k ) {
            for ( int l=(x-b); l<(x+b); ++l ) {
              if ( k >=0 && l >= 0 && k < rows && l < cols ) {
                d = ( flaredecay * sqrt((x-l)*(x-l) + (y-k)*(y-k)) + 5 ) / 10;
                n = 0;
                if ( z > d ) n = z - d;
                if ( n > pix[k][l] ) { // can only get brighter
                  pix[k][l] = n;
                }
              }
            }
          }
          if ( z > 1 ) {
            flare[i] = (flare[i] & 0xffff) | ((z-1)<<16);
          } else {
            // This flare is out
            for ( j=i+1; j<nflare; ++j ) {
              flare[j-1] = flare[j];
            }
            --nflare;
          }
        }
        // New Flare
        if ( nflare < maxflare && random(1,101) <= flarechance ) {
          x = random(0, cols);
          y = random(0, flarerows);
          z = NCOLORS - 1;
          b = z * 10 / flaredecay + 1;
          flare[nflare++] = (z<<16) | (y<<8) | (x&0xff);
          for ( k=(y-b); k<(y+b); ++k ) {
            for ( int l=(x-b); l<(x+b); ++l ) {
              if ( k >=0 && l >= 0 && k < rows && l < cols ) {
                d = ( flaredecay * sqrt((x-l)*(x-l) + (y-k)*(y-k)) + 5 ) / 10;
                n = 0;
                if ( z > d ) n = z - d;
                if ( n > pix[k][l] ) { // can only get brighter
                  pix[k][l] = n;
                }
              }
            }
          }
        }
        // Draw
        if ( colmajor == true ) {
          phy_w = rows;
          phy_h = cols;
        }
        for ( uint16_t row=0; row<rows; ++row ) {
          for ( uint16_t col=0; col<cols; ++col ) {
            if ( colmajor == true ) {
              phy_x = offsetx + (uint16_t) row;
              phy_y = offsety + (uint16_t) col;
            } else {
              phy_x = offsetx + (uint16_t) col;
              phy_y = offsety + (uint16_t) row;
            }
            if ( matleft == true && zigzag == true ) {
              if ( ( phy_y & 1 ) == 1 ) {
                phy_x = phy_w - phy_x - 1;
              }
            } else if ( matleft == false && zigzag == true ) {
              if ( ( phy_y & 1 ) == 0 ) {
                phy_x = phy_w - phy_x - 1;
              }
            } else if ( matleft == false ) {
              phy_x = phy_w - phy_x - 1;
            }
            if ( mattop == true && colmajor == true ) {
              phy_x = phy_w - phy_x - 1;
            } else if (mattop) {
              phy_y = phy_h - phy_y - 1;
            }
            this->leds[phy_x + phy_y * phy_w] = Color(colors[pix[row][col]]);
          }
        }
      }
    }

    Color ZMatrix::get_finalled(uint8_t x, uint8_t y, float brightness) {
      return Color(this->finalleds[this->XY(x, y)].red * brightness, this->finalleds[this->XY(x, y)].green * brightness, this->finalleds[this->XY(x, y)].blue * brightness);
    }

    void ZMatrix::copy_leds() {
      for(int i = 0; i < this->num_leds; i++) {
        this->finalleds[i] = this->leds[i];
      }
    }

    void ZMatrix::select_index(size_t index) {
      this->previousTime = 0;
      if (index > 2) {
        this->gifanimstart = 0;
        this->gifanim = 1;
      } else {
        this->gifanim = 0;
        this->gifanimstart = 0;
        this->gifanimPrevMode = index;
      }
    }

    void ZMatrix::displayDigit(Color color, const int digit, const int x, const int y) {
      if(digit == 0 || digit == 1 || digit == 2 || digit == 3 || digit == 4 || digit == 7 || digit == 8 || digit == 9) { // Seg A
        for (int i = 0; i < this->segmentSize+2; i++) {
          this->finalleds[XY(x+this->segmentSize+1, y+i)] = color;
        }
      }
      if(digit == 0 || digit == 1 || digit == 3 || digit == 4 || digit == 5 || digit == 6 || digit == 7 || digit == 8 || digit == 9) { // Seg B
        for (int i = 0; i < this->segmentSize+2; i++) {
          this->finalleds[XY(x+this->segmentSize+1, y+this->segmentSize+1+i)] = color;
        }
      }
      if(digit == 0 || digit == 2 || digit == 3 || digit == 5 || digit == 6 || digit == 8 || digit == 9) { // Seg C
        for (int i = 0; i < this->segmentSize+2; i++) {
          this->finalleds[XY(x+i, y+2*this->segmentSize+2)] = color;
        }
      }
      if(digit == 0 || digit ==2 || digit == 6 || digit == 8) { // Seg D
        for (int i = 0; i < this->segmentSize+2; i++) {
          this->finalleds[XY(x, y+this->segmentSize+1+i)] = color;
        }
      }
      if(digit == 0 || digit ==4 || digit ==5 || digit ==6 || digit == 8 || digit == 9) { // Seg E
        for (int i = 0; i < this->segmentSize+2; i++) {
          this->finalleds[XY(x, y+i)] = color;
        }
      }
      if(digit == 0 || digit == 2 || digit == 3 || digit == 5 || digit == 6 || digit == 7 || digit == 8 || digit == 9) { // Seg F
        for (int i = 0; i < this->segmentSize+2; i++) {
          this->finalleds[XY(x+i, y)] = color;
        }
      }
      if(digit == 2 || digit == 3 || digit == 4 || digit == 5 || digit == 6 || digit == 8 || digit == 9) { // Seg G
        for (int i = 0; i < this->segmentSize+2; i++) {
          this->finalleds[XY(x+i, y+this->segmentSize+1)] = color;
        }
      }
    }

    float ZMatrix::RGB_to_H(float r, float g, float b) {
      float ma = max(r, max(g, b));
      float mi = min(r, min(g, b));
      float d = ma - mi;
      float h = 0.0;
      if (d < 0.0001) {
        return 0.0;
      } else if (r >= ma) {
        h = ( g - b ) / d;
      } else if (g >= ma) {
        h = ( b - r ) / d + 2.0 ;
      } else {
        h = ( r - g ) / d  + 4.0;
      }
      h *= 60;
      if (h < 0.0) {
        h += 360.0;
      }
      return h;
    }

  }  // namespace empty_component
}  // namespace esphome
