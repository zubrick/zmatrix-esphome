#define NUM_LEDS      256
Color leds[NUM_LEDS];
Color finalleds[NUM_LEDS];
Color start[NUM_LEDS];
Color end[NUM_LEDS];
int skiped = 1;
int previousTime = 0;
int animBrightnessValue = 250;
int animSkipValue = 0;
int gifanim = 0;
int gifanimstart = 0;
int gifanimlastframe = 0;
size_t gifanimPrevMode = 0;

float mapfloat(float x, float in_min, float in_max, float out_min, float out_max) {
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}


float RGB_to_H(float r, float g, float b) {
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

uint16_t XY( uint8_t x, uint8_t y) {
  if ( (y & 0x01)) {
    return ((15-y) * 16) + x;
  } else {
    return ((15-y) * 16) + 15 - x;
  }
}

int segmentSize = 1;
void displayDigit(Color color, const int digit, const int x, const int y, const int segmentSize) {
  //Serial.print("displayDigit ");
  //Serial.print(digit);
  //Serial.print(" at ");
  //Serial.print(x);
  //Serial.print(",");
  //Serial.println(y);
  if(digit == 0 || digit == 1 || digit == 2 || digit == 3 || digit == 4 || digit == 7 || digit == 8 || digit == 9) { // Seg A
    for (int i = 0; i < segmentSize+2; i++) {
      finalleds[XY(x+segmentSize+1, y+i)] = color;
    }
  }
  if(digit == 0 || digit == 1 || digit == 3 || digit == 4 || digit == 5 || digit == 6 || digit == 7 || digit == 8 || digit == 9) { // Seg B
    for (int i = 0; i < segmentSize+2; i++) {
      finalleds[XY(x+segmentSize+1, y+segmentSize+1+i)] = color;
    }
  }
  if(digit == 0 || digit == 2 || digit == 3 || digit == 5 || digit == 6 || digit == 8 || digit == 9) { // Seg C
    for (int i = 0; i < segmentSize+2; i++) {
      finalleds[XY(x+i, y+2*segmentSize+2)] = color;
    }
  }
  if(digit == 0 || digit ==2 || digit == 6 || digit == 8) { // Seg D
    for (int i = 0; i < segmentSize+2; i++) {
      finalleds[XY(x, y+segmentSize+1+i)] = color;
    }
  }
  if(digit == 0 || digit ==4 || digit ==5 || digit ==6 || digit == 8 || digit == 9) { // Seg E
    for (int i = 0; i < segmentSize+2; i++) {
      finalleds[XY(x, y+i)] = color;
    }
  }
  if(digit == 0 || digit == 2 || digit == 3 || digit == 5 || digit == 6 || digit == 7 || digit == 8 || digit == 9) { // Seg F
    for (int i = 0; i < segmentSize+2; i++) {
      finalleds[XY(x+i, y)] = color;
    }
  }
  if(digit == 2 || digit == 3 || digit == 4 || digit == 5 || digit == 6 || digit == 8 || digit == 9) { // Seg G
    for (int i = 0; i < segmentSize+2; i++) {
      finalleds[XY(x+i, y+segmentSize+1)] = color;
    }
  }
}

int prevmin = -1;
int cx = 0;
int cy = 0;
void showTime(Color color, const int hour, const int minute, const int pos) {
  int h1 = hour/10;
  int h2 = hour%10;
  int m1 = minute/10;
  int m2 = minute%10;
  int x = 0;
  int y = 0;
  int segmentSize = 1;
  int space = 1;
  int maxX = 17 - 2*segmentSize - space - 4;
  int maxY = 17 - 4*segmentSize - space - 6;
  if (pos > 0) {
    if (pos < 75) {
      x = map(pos, 0, 74, 0, maxX);
      y = 0;
    } else if (pos < 150) {
      x = maxX;
      y = map(pos, 75, 149, 0, maxY);
    } else if (pos < 225) {
      x = map(pos, 150, 224, maxX, 0);
      y = maxY;
    } else if (pos < 300) {
      x = 0;
      y = map(pos, 225, 299, maxY, 0);
    }
  } else {
    if(prevmin != minute) {
      cx = random(0,maxX);
      cy = random(0,maxY);
      prevmin = minute;
    }
    x = cx;
    y = cy;
  }
  displayDigit(color, h1, x, y, segmentSize);
  displayDigit(color, h2, x+segmentSize+2+space, y, segmentSize);
  //leds[XY(x+(segmentSize+2+1)*2 - 1, y+1)] = color;
  //leds[XY(x+(segmentSize+2+1)*2 - 1, y+3)] = color;
  displayDigit(color, m1, x, y+2*segmentSize+3+space,segmentSize);
  displayDigit(color, m2, x+segmentSize+2+space, y+2*segmentSize+3+space, segmentSize);
  //displayDigit(color, m1, x+(segmentSize+2+1)*2 + 1, y, segmentSize);
  //displayDigit(color, m2, x+(segmentSize+2+1)*3 + 1, y, segmentSize);
}

void randColor(uint8_t hue, uint8_t var, int time) {
  int totTime = time + time*animSkipValue;
  if (skiped < animSkipValue) {
    skiped++;
  } else {
    skiped = 0;
  }
  if(previousTime == 0) {
    for(int i = 0; i < NUM_LEDS; i++) {
      end[i] = leds[i];
    }
  }
  if(millis() - previousTime >= totTime) {

    //Serial.print("hue:");
    //Serial.println(hue);
    for(int i = 0; i < NUM_LEDS; i++) {
      ESPHSVColor hsv((uint8_t)random(hue-var, hue+var), random(128, 255), random(0, animBrightnessValue));
      start[i] = end[i];
      end[i] = hsv.to_rgb();
      leds[i] = start[i];
    }
    previousTime = millis();
  } else {
    int diff = millis() - previousTime;
    for(int i = 0; i < NUM_LEDS; i++) {
      if ((i % (animSkipValue + 1)) == skiped) {
        leds[i] = Color((int)(start[i].red+(float)(end[i].red - start[i].red)*(float)diff/(float)totTime),
                        (int)(start[i].green+(float)(end[i].green - start[i].green)*(float)diff/(float)totTime),
                        (int)(start[i].blue+(float)(end[i].blue - start[i].blue)*(float)diff/(float)totTime));
      }
    }
  }
}

void matrix() {
  if(millis() - previousTime >= 75) {
    // Move bright spots downward
    for (int row = 15; row >= 0; row--) {
      for (int col = 0; col < 16; col++) {
        if (leds[XY(col, row)] == Color(175,255,175)) {
          leds[XY(col, row)] = Color(27,200,39); // create trail
          if (row < 15) leds[XY(col, row + 1)] = Color(175,255,175);
        }
      }
    }

    // Fade all leds
    for(int i = 0; i < NUM_LEDS; i++) {
      if (leds[i].green != 255) leds[i] = leds[i].fade_to_black(50); // only fade trail
    }

    // Spawn new falling spots
    if (random(0,2) == 0) // lower number == more frequent spawns
    {
      int8_t spawnX = random(0,16);
      leds[XY(spawnX, 0)] = Color(175,255,175 );
    }

    previousTime = millis();
  }
}

void nationalDay() {
  int time = 5000;
  int var = 10;
  int totTime = time;
  if(previousTime == 0) {
    for(int i = 0; i < NUM_LEDS; i++) {
      end[i] = leds[i];
    }
  }
  if(millis() - previousTime >= totTime) {
    for (int y = 0; y < 16; y++) {
      for (int x = 0; x < 16; x++) {
        int c = 0;
        if (y > 1 && y <= 5 && x > 5 && x <= 9) {
          c = 1;
        } else if (y > 5 && y <= 9 && x > 1 && x <= 13) {
          c = 1;
        } else if (y > 9 && y <= 13 && x > 5 && x <= 9) {
          c = 1;
        }

        ESPHSVColor newcolor;
        if (c) {
          newcolor = ESPHSVColor(0, 0, random(40, 90));
        } else {
          newcolor = ESPHSVColor(0, random(128, 255), random(40, 90));
        }
        start[XY(x, y)] = end[XY(x, y)];
        end[XY(x, y)] = newcolor.to_rgb();
        leds[XY(x, y)] = start[XY(x, y)];
      }
    }
    previousTime = millis();
  } else {
    int diff = millis() - previousTime;
    for(int i = 0; i < NUM_LEDS; i++) {
      leds[i] = Color((int)(start[i].red+(float)(end[i].red - start[i].red)*(float)diff/(float)totTime),
                      (int)(start[i].green+(float)(end[i].green - start[i].green)*(float)diff/(float)totTime),
                      (int)(start[i].blue+(float)(end[i].blue - start[i].blue)*(float)diff/(float)totTime));
    }
  }
}

void fire() {
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
        leds[phy_x + phy_y * phy_w] = ESPColor(colors[pix[row][col]]);
      }
    }
  }
}
