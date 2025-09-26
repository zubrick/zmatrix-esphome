#include "Digit.h"

#include "esphome/core/log.h"
#include "esphome/components/display/display.h"

namespace esphome {
  namespace zclock {
    static const char *TAG = "zclock.component.digit";

    const uint8_t sA = 0;
    const uint8_t sB = 1;
    const uint8_t sC = 2;
    const uint8_t sD = 3;
    const uint8_t sE = 4;
    const uint8_t sF = 5;
    const uint8_t sG = 6;
    const uint16_t height = 31;
    const uint16_t width = 63;

    byte digitBits[] = {
      0b11111100, // 0 ABCDEF--
        0b01100000, // 1 -BC-----
        0b11011010, // 2 AB-DE-G-
        0b11110010, // 3 ABCD--G-
        0b01100110, // 4 -BC--FG-
        0b10110110, // 5 A-CD-FG-
        0b10111110, // 6 A-CDEFG-
        0b11100000, // 7 ABC-----
        0b11111110, // 8 ABCDEFG-
        0b11110110, // 9 ABCD_FG-
    };

//byte sunBitmap[] {
//  B100100100,
//  B010001000,
//  B001110000,
//  B101110100,
//  B001110000,
//  B010001000,
//  B100100100
//};

    Digit::Digit(esphome::display::Display* d, uint8_t value, uint16_t xo, uint16_t yo, esphome::Color color) {
      this->_display = d;
      this->_value = value;
      this->_xOffset = xo;
      this->_yOffset = yo;
      this->_color = color;
    }

    Digit::Digit(esphome::display::Display* d, uint8_t value, uint16_t xo, uint16_t yo, esphome::Color color, int segHeight, int segWidth) {
      this->_display = d;
      this->_value = value;
      this->_xOffset = xo;
      this->_yOffset = yo;
      this->_color = color;
      this->segHeight = segHeight;
      this->segWidth = segWidth;
    }

    void Digit::setOffset(uint16_t  xo, uint16_t yo) {
      this->_xOffset = xo;
      this->_yOffset = yo;
    }

    void Digit::setColor(esphome::Color color) {
      int red = color.red;
      int green = color.green;
      int blue = color.blue;
      if (red >= green && red >= blue) {
        this->_color = Color(255, 255 * green / red, 255 * blue / red);
      } else if (green >= red && green >= blue) {
        this->_color = Color(255 * red / green, 255, 255 * blue / green);
      } else {
        this->_color = Color(255 * red / blue, 255 * green / blue, 255);
      }
    }

    uint8_t Digit::Value() {
      return _value;
    }
    void Digit::drawPixel(uint16_t x, uint16_t y, esphome::Color c)
    {
      this->_display->draw_pixel_at(_xOffset + x, height - (y + _yOffset), c);
    }

    void Digit::drawLine(int16_t x, int16_t y, int16_t x2, int16_t y2, esphome::Color c)
    {
      this->_display->line(_xOffset + x, height - (y + _yOffset), _xOffset + x2, height - (y2 + _yOffset), c);
    }

    void Digit::drawFillRect(int16_t x, int16_t y, uint16_t w, uint16_t h, esphome::Color c)
    {
      this->_display->filled_rectangle(this->_xOffset + x, height - (y + this->_yOffset), w,h, c);
    }

    void Digit::DrawColon()
    {
      // Colon is drawn to the left of this digit
      drawFillRect(-2, segHeight-1, 1,2, this->_color);
      drawFillRect(-2, segHeight+1+3, 1,2, this->_color);
    }

    void Digit::DrawDot()
    {
      // Colon is drawn to the left of this digit
      drawFillRect(-3, 1, 1,2, this->_color);
    }

    void Digit::drawSeg(uint8_t seg)
    {
      switch (seg) {
        case sA: drawLine(1, segHeight * 2 + 2, segWidth, segHeight * 2 + 2, _color); break;
        case sB: drawLine(segWidth + 1, segHeight * 2 + 1, segWidth + 1, segHeight + 2, _color); break;
        case sC: drawLine(segWidth + 1, 1, segWidth + 1, segHeight, _color); break;
        case sD: drawLine(1, 0, segWidth, 0, _color); break;
        case sE: drawLine(0, 1, 0, segHeight, _color); break;
        case sF: drawLine(0, segHeight * 2 + 1, 0, segHeight + 2, _color); break;
        case sG: drawLine(1, segHeight + 1, segWidth, segHeight + 1, _color); break;
      }
    }

    void Digit::clearSeg(uint8_t seg)
    {
      switch (seg) {
        case sA: drawLine(1, segHeight * 2 + 2, segWidth, segHeight * 2 + 2, esphome::Color::BLACK); break;
        case sB: drawLine(segWidth + 1, segHeight * 2 + 1, segWidth + 1, segHeight + 2, esphome::Color::BLACK); break;
        case sC: drawLine(segWidth + 1, 1, segWidth + 1, segHeight, esphome::Color::BLACK); break;
        case sD: drawLine(1, 0, segWidth, 0, esphome::Color::BLACK); break;
        case sE: drawLine(0, 1, 0, segHeight, esphome::Color::BLACK); break;
        case sF: drawLine(0, segHeight * 2 + 1, 0, segHeight + 2, esphome::Color::BLACK); break;
        case sG: drawLine(1, segHeight + 1, segWidth, segHeight + 1, esphome::Color::BLACK); break;
      }
    }

    void Digit::Draw(uint8_t value) {
      if (value != this->_value && value != 99) {
        this->_prev_value = this->_value;
        this->_value = value;
        this->morph_step = 0;
      }

      if (this->morph_step >= 0 && this->morph_step <= this->nb_step) {
        Morph(this->_value);
        this->morph_step++;
      } else {
        this->morph_step = -1;
         byte pattern = digitBits[this->_value];
         if (bitRead(pattern, 7)) {
           drawSeg(sA);
         } else {
           clearSeg(sA);
         }
         if (bitRead(pattern, 6)) {
           drawSeg(sB);
         } else {
           clearSeg(sB);
         }
         if (bitRead(pattern, 5)) {
           drawSeg(sC);
         } else {
           clearSeg(sC);
         }
         if (bitRead(pattern, 4)) {
           drawSeg(sD);
         } else {
           clearSeg(sD);
         }
         if (bitRead(pattern, 3)) {
           drawSeg(sE);
         } else {
           clearSeg(sE);
         }
         if (bitRead(pattern, 2)) {
           drawSeg(sF);
         } else {
           clearSeg(sF);
         }
         if (bitRead(pattern, 1)) {
           drawSeg(sG);
         } else {
           clearSeg(sG);
         }
      }
    }

    bool Digit::Morphing() {
      if (this->morph_step == -1) {
        return false;
      } else {
        return true;
      }
    }

    void Digit::Morph1() {
      drawSeg(sB);
      drawSeg(sC);
      drawSeg(sA);
      drawSeg(sD);
      int i = (segWidth + 1) * morph_step / nb_step;
      // Move E left to right
      drawLine(0 + i - 1, 1, 0 + i - 1, segHeight, esphome::Color::BLACK);
      drawLine(0 + i, 1, 0 + i, segHeight, _color);

      // Move F left to right
      drawLine(0 + i - 1, segHeight * 2 + 1, 0 + i - 1, segHeight + 2, esphome::Color::BLACK);
      drawLine(0 + i, segHeight * 2 + 1, 0 + i, segHeight + 2, _color);

      // Gradually Erase A, G, D
      for (int j = 0; j <= i-1; j++) {
        drawPixel(1 + j, segHeight * 2 + 2, esphome::Color::BLACK); // A
        drawPixel(1 + j, 0, esphome::Color::BLACK); // D
        drawPixel(1 + j, segHeight + 1, esphome::Color::BLACK); // G
      }
    }

    void Digit::Morph2() {
      // TWO
      drawSeg(sB);
      int i = segWidth * morph_step / nb_step;
      for (int j = 0; j <= i; j++) {
        if (j < segWidth) {
          drawPixel(segWidth - j, segHeight * 2 + 2, _color);
          drawPixel(segWidth - j, segHeight + 1, _color);
          drawPixel(segWidth - j, 0, _color);
        }
      }

      drawLine(segWidth + 1 - i, 1, segWidth + 1 - i, segHeight, esphome::Color::BLACK);
      drawLine(segWidth - i, 1, segWidth - i, segHeight, _color);
    }

    void Digit::Morph3() {
      // THREE
      drawSeg(sA);
      drawSeg(sB);
      drawSeg(sD);
      drawSeg(sG);
      int i = segWidth * morph_step / nb_step;
      drawLine(0 + i, 1, 0 + i, segHeight, esphome::Color::BLACK);
      drawLine(1 + i, 1, 1 + i, segHeight, _color);
    }

    void Digit::Morph4() {
      // FOUR
      drawSeg(sA);
      drawSeg(sB);
      drawSeg(sC);
      drawSeg(sD);
      drawSeg(sG);
      int i = segWidth * morph_step / nb_step;
      for (int j = 0; j <= i; j++) {
        if (j < segWidth) {
          drawPixel(segWidth - j, segHeight * 2 + 2, esphome::Color::BLACK); // Erase A
          drawPixel(0, segHeight * 2 + 1 - j, _color); // Draw as F
          drawPixel(1 + j, 0, esphome::Color::BLACK); // Erase D
        }
      }

    }

    void Digit::Morph5() {
      // FIVE
      drawSeg(sB);
      drawSeg(sC);
      drawSeg(sF);
      drawSeg(sG);
      int i = segWidth * morph_step / nb_step;
      for (int j = 0; j <= i; j++) {
        if (j < segWidth) {
          drawPixel(segWidth + 1, segHeight + 2 + j, esphome::Color::BLACK); // Erase B
          drawPixel(segWidth - j, segHeight * 2 + 2, _color); // Draw as A
          drawPixel(segWidth - j, 0, _color); // Draw D
        }
      }
    }

    void Digit::Morph6() {
      // SIX
      drawSeg(sA);
      drawSeg(sC);
      drawSeg(sD);
      drawSeg(sF);
      drawSeg(sG);
      int i = segWidth * morph_step / nb_step;
      // Move C right to left
      drawLine(segWidth - i, 1, segWidth - i, segHeight, _color);
      //if (i > 0) drawLine(segWidth - i + 1, 1, segWidth - i + 1, segHeight, esphome::Color::BLACK);
      /*for (int j = 1; j < i; j++) {
        if (j <= segWidth) {
          drawPixel(0, j, _color); // Draw as E
        }
        }*/

    }

    void Digit::Morph7() {
      // SEVEN
      drawSeg(sA);
      drawSeg(sC);
      drawSeg(sD);
      drawSeg(sG);
      int i = (segWidth + 1) * morph_step / nb_step;
      // Move E left to right
      drawLine(0 + i - 1, 1, 0 + i - 1, segHeight, esphome::Color::BLACK);
      drawLine(0 + i, 1, 0 + i, segHeight, _color);

      // Move F left to right
      drawLine(0 + i - 1, segHeight * 2 + 1, 0 + i - 1, segHeight + 2, esphome::Color::BLACK);
      drawLine(0 + i, segHeight * 2 + 1, 0 + i, segHeight + 2, _color);

      // Erase D and G gradually
      for (int j = 0; j <= i; j++) {
        drawPixel(1 + j, 0, esphome::Color::BLACK); // D
        drawPixel(1 + j, segHeight + 1, esphome::Color::BLACK); // G
      }
    }

    void Digit::Morph8() {
      // EIGHT
      drawSeg(sA);
      drawSeg(sB);
      drawSeg(sC);
      int i = segWidth * morph_step / nb_step;

      // Move B right to left
      drawLine(segWidth - i, segHeight * 2 + 1, segWidth - i, segHeight + 2, _color);
      if (i > 0) drawLine(segWidth - i + 1, segHeight * 2 + 1, segWidth - i + 1, segHeight + 2, esphome::Color::BLACK);

      // Move C right to left
      drawLine(segWidth - i, 1, segWidth - i, segHeight, _color);
      if (i > 0) drawLine(segWidth - i + 1, 1, segWidth - i + 1, segHeight, esphome::Color::BLACK);

      // Gradually draw D and G
      for (int j = 0; j <= i; j++) {
        if (j < segWidth) {
          drawPixel(segWidth - j, 0, _color); // D
          drawPixel(segWidth - j, segHeight + 1, _color); // G
        }
      }
    }

    void Digit::Morph9() {
      // NINE
      drawSeg(sA);
      drawSeg(sB);
      drawSeg(sC);
      drawSeg(sD);
      drawSeg(sF);
      drawSeg(sG);
      int i = (segWidth + 1) * morph_step / nb_step;
      // Move E left to right
      drawLine(0 + i - 1, 1, 0 + i - 1, segHeight, esphome::Color::BLACK);
      drawLine(0 + i, 1, 0 + i, segHeight, _color);
    }

    void Digit::Morph0() {
      // ZERO
      int i = segWidth * morph_step / nb_step;
      if (this->_prev_value==1) { // If 1 to 0, slide B to F and E to C
        drawSeg(sB);
        drawSeg(sC);
        // slide B to F
        drawLine(segWidth - i, segHeight * 2+1 , segWidth - i, segHeight + 2, _color);
        if (i > 0) drawLine(segWidth - i + 1, segHeight * 2+1, segWidth - i + 1, segHeight + 2, esphome::Color::BLACK);

        // slide E to C
        drawLine(segWidth - i, 1, segWidth - i, segHeight, _color);
        if (i > 0) drawLine(segWidth - i + 1, 1, segWidth - i + 1, segHeight, esphome::Color::BLACK);

        for (int j = 0; j <= i; j++) {
          if (j<segWidth) drawPixel(segWidth - j, segHeight * 2 + 2 , _color); // Draw A
          if (j<segWidth) drawPixel(segWidth - j, 0, _color); // Draw D
        }
      }

      if (this->_prev_value==2) { // If 2 to 0, slide B to F and Flow G to C
        // slide B to F
        drawLine(segWidth - i, segHeight * 2+1 , segWidth - i, segHeight + 2, _color);
        if (i > 0) drawLine(segWidth - i + 1, segHeight * 2+1, segWidth - i + 1, segHeight + 2, esphome::Color::BLACK);

        drawPixel(1+i, segHeight + 1, esphome::Color::BLACK); // Erase G left to right
        if (i<=segWidth) drawPixel(segWidth + 1, segHeight + 1 - i, _color);// Draw C
      }

      if (this->_prev_value==3) { // B to F, C to E
        // slide B to F
        drawLine(segWidth - i, segHeight * 2+1 , segWidth - i, segHeight + 2, _color);
        if (i > 0) drawLine(segWidth - i + 1, segHeight * 2+1, segWidth - i + 1, segHeight + 2, esphome::Color::BLACK);

        // Move C to E
        drawLine(segWidth - i, 1, segWidth - i, segHeight, _color);
        if (i > 0) drawLine(segWidth - i + 1, 1, segWidth - i + 1, segHeight, esphome::Color::BLACK);

        // Erase G from right to left
        drawPixel(segWidth - i, segHeight + 1, esphome::Color::BLACK); // G
      }

      if (this->_prev_value==5 || this->_prev_value==9) { // If 9 or 5 to 0, Flow G into E
        drawSeg(sA);
        drawSeg(sB);
        drawSeg(sC);
        drawSeg(sD);
        if (this->_prev_value==9) drawSeg(sF);
        drawSeg(sG);
        for (int j = 0; j <= i; j++) {
          if (j<segWidth) drawPixel(segWidth - j, segHeight + 1, esphome::Color::BLACK);
          if (j<segWidth) drawPixel(0, segHeight - j, _color);
        }
      }

      if (this->_prev_value==5) { // If 5 to 0, we also need to slide F to B
        for (int j = 0; j <= i; j++) {
          if (j<segWidth) {
            if (j>0) drawLine(1 + j, segHeight * 2 + 1, 1 + j, segHeight + 2, esphome::Color::BLACK);
            drawLine(2 + j, segHeight * 2 + 1, 2 + j, segHeight + 2, _color);
          }
        }
      }
    }

    void Digit::Morph(uint8_t newValue) {
      switch (newValue) {
        case 2: Morph2(); break;
        case 3: Morph3(); break;
        case 4: Morph4(); break;
        case 5: Morph5(); break;
        case 6: Morph6(); break;
        case 7: Morph7(); break;
        case 8: Morph8(); break;
        case 9: Morph9(); break;
        case 0: Morph0(); break;
        case 1: Morph1(); break;
      }
    }

  }
}
