// Adafruit_DotStarMatrix example for tiled DotStar matrices.  Scrolls
// 'Howdy' across three 10x8 DotStar grids that were created using
// DotStar 60 LEDs per meter flex strip.

#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_DotStarMatrix.h>
#include <Adafruit_DotStar.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define DATAPIN  4
#define CLOCKPIN 5

// MATRIX DECLARATION:
// Parameter 1 = width of EACH DOTSTAR MATRIX (not total display)
// Parameter 2 = height of each matrix
// Parameter 3 = number of matrices arranged horizontally
// Parameter 4 = number of matrices arranged vertically
// Parameter 5 = pin number (most are valid)
// Parameter 6 = matrix layout flags, add together as needed:
//   DS_MATRIX_TOP, DS_MATRIX_BOTTOM, DS_MATRIX_LEFT, DS_MATRIX_RIGHT:
//     Position of the FIRST LED in the FIRST MATRIX; pick two, e.g.
//     DS_MATRIX_TOP + DS_MATRIX_LEFT for the top-left corner.
//   DS_MATRIX_ROWS, DS_MATRIX_COLUMNS: LEDs WITHIN EACH MATRIX are
//     arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   DS_MATRIX_PROGRESSIVE, DS_MATRIX_ZIGZAG: all rows/columns WITHIN
//     EACH MATRIX proceed in the same order, or alternate lines reverse
//     direction; pick one.
//   DS_TILE_TOP, DS_TILE_BOTTOM, DS_TILE_LEFT, DS_TILE_RIGHT:
//     Position of the FIRST MATRIX (tile) in the OVERALL DISPLAY; pick
//     two, e.g. DS_TILE_TOP + DS_TILE_LEFT for the top-left corner.
//   DS_TILE_ROWS, DS_TILE_COLUMNS: the matrices in the OVERALL DISPLAY
//     are arranged in horizontal rows or in vertical columns, respectively;
//     pick one or the other.
//   DS_TILE_PROGRESSIVE, DS_TILE_ZIGZAG: the ROWS/COLUMS OF MATRICES
//     (tiles) in the OVERALL DISPLAY proceed in the same order for every
//     line, or alternate lines reverse direction; pick one.  When using
//     zig-zag order, the orientation of the matrices in alternate rows
//     will be rotated 180 degrees (this is normal -- simplifies wiring).
//   See example below for these values in action.
// Parameter 7 = pixel type:
//   DOTSTAR_BRG  Pixels are wired for BRG bitstream (most DotStar items)
//   DOTSTAR_GBR  Pixels are wired for GBR bitstream (some older DotStars)

// Example with three 10x8 matrices (created using DotStar flex strip --
// these grids are not a ready-made product).  In this application we'd
// like to arrange the three matrices side-by-side in a wide display.
// The first matrix (tile) will be at the left, and the first pixel within
// that matrix is at the top left.  The matrices use zig-zag line ordering.
// There's only one row here, so it doesn't matter if we declare it in row
// or column order.  The pixels expect BRG color data.
// IMPORTANT: when using tiled matrices, cast the first two arguments to
// (uint8_t) as shown here.  This avoids ambiguity between certain combos
// of single/tiled hard/soft SPI.  Do NOT cast for single matrices.
Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
  (uint8_t)10, (uint8_t)8, 3, 1, DATAPIN, CLOCKPIN,
  DS_TILE_TOP   + DS_TILE_LEFT   + DS_TILE_ROWS   + DS_TILE_PROGRESSIVE +
  DS_MATRIX_TOP + DS_MATRIX_LEFT + DS_MATRIX_ROWS + DS_MATRIX_ZIGZAG,
  DOTSTAR_BRG);
//Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
//  (uint8_t)10, (uint8_t)8, 3, 1,
//  DS_TILE_TOP   + DS_TILE_LEFT   + DS_TILE_ROWS   + DS_TILE_PROGRESSIVE +
//  DS_MATRIX_TOP + DS_MATRIX_LEFT + DS_MATRIX_ROWS + DS_MATRIX_ZIGZAG,
//  DOTSTAR_BRG);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

void setup() {
  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);
}

int x    = matrix.width();
int pass = 0;

void loop() {
  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(F("Howdy"));
  if(--x < -36) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(100);
}
