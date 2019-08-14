// Adafruit_DotStarMatrix example for single DotStar LED matrix.
// Scrolls 'Adafruit' across the matrix.

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>
#include <Fonts/TomThumb.h>

#if defined(ESP8266)
  #define DATAPIN    13
  #define CLOCKPIN   14
#elif defined(__AVR_ATmega328P__)
  #define DATAPIN    2
  #define CLOCKPIN   4
#elif defined(ARDUINO_NRF52832_FEATHER)
  #define DATAPIN    7
  #define CLOCKPIN   16
#elif defined(TEENSYDUINO)
  #define DATAPIN    9
  #define CLOCKPIN   5
#elif defined(ARDUINO_ARCH_WICED)
  #define DATAPIN    PA4
  #define CLOCKPIN   PB5
#elif defined(ESP32)
  #define DATAPIN    27
  #define CLOCKPIN   13
#else // // 32u4, M0, M4, nrf52840 and 328p
  #define DATAPIN    11
  #define CLOCKPIN   13
#endif

#define SHIFTDELAY 100
#define BRIGHTNESS 20

// MATRIX DECLARATION:
// Parameter 1 = width of DotStar matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   DS_MATRIX_TOP, DS_MATRIX_BOTTOM, DS_MATRIX_LEFT, DS_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     DS_MATRIX_TOP + DS_MATRIX_LEFT for the top-left corner.
//   DS_MATRIX_ROWS, DS_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   DS_MATRIX_PROGRESSIVE, DS_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type:
//   DOTSTAR_BRG  Pixels are wired for BRG bitstream (most DotStar items)
//   DOTSTAR_GBR  Pixels are wired for GBR bitstream (some older DotStars)
//   DOTSTAR_BGR  Pixels are wired for BGR bitstream (APA102-2020 DotStars)

Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
                                  12, 6, DATAPIN, CLOCKPIN,
                                  DS_MATRIX_BOTTOM     + DS_MATRIX_LEFT +
                                  DS_MATRIX_ROWS + DS_MATRIX_PROGRESSIVE,
                                  DOTSTAR_BGR);

const uint16_t primaryColors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255)
};

const uint16_t adaColors[] = {
  matrix.Color(255, 0, 0),   //A red
  matrix.Color(255, 125, 0), //D orange
  matrix.Color(200, 255, 0), //A yellowish
  matrix.Color(0, 255, 0),   //F green
  matrix.Color(0, 255, 225), //R blue
  matrix.Color(150, 0, 255), //U purple
  matrix.Color(255, 0, 220), //I pink
  matrix.Color(255, 65, 0),  //T reddish
  matrix.Color(255, 220, 0)  //! orange/yellow
};

char adafruit[] = "ADAFRUIT!";

void setup() {
  Serial.begin(115200);
 
  // uncomment to have wait
  //while (!Serial) delay(500); 

  Serial.println("\nDotstar Matrix Wing");
  matrix.begin();
  matrix.setFont(&TomThumb);
  matrix.setTextWrap(false);
  matrix.setBrightness(BRIGHTNESS);

  for (byte i = 0; i < 3; i++) {
    matrix.fillScreen(primaryColors[i]);
    matrix.show();
    delay(500);
  }
}

int x = matrix.width();
int pass = 0;

void loop() {
  matrix.fillScreen(0);
  matrix.setCursor(x, 5);
  for (byte i = 0; i < strlen(adafruit); i++) {
    // set the color
    matrix.setTextColor(adaColors[i]);
    // write the letter
    matrix.print(adafruit[i]);
  }

  if (--x < -50) {
    x = matrix.width();
  }

  matrix.show();
  delay(SHIFTDELAY);
}
