/*--------------------------------------------------------------------
  This file is part of the Adafruit DotStarMatrix library.

  DotStarMatrix is free software: you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public License
  as published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  DotStarMatrix is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with DotStarMatrix.  If not, see
  <http://www.gnu.org/licenses/>.
  --------------------------------------------------------------------*/

#ifndef _ADAFRUIT_DSMATRIX_H_
#define _ADAFRUIT_DSMATRIX_H_

#if ARDUINO >= 100
 #include <Arduino.h>
#else
 #include <WProgram.h>
 #include <pins_arduino.h>
#endif
#include <Adafruit_GFX.h>
#include <Adafruit_DotStar.h>

// Matrix layout information is passed in the 'matrixType' parameter for
// each constructor (the parameter immediately following is the LED type
// from Adafruit_DotStar.h).

// These define the layout for a single 'unified' matrix (e.g. one made
// from DotStar strips), or for the pixels within each matrix of a tiled
// display.

#define DS_MATRIX_TOP          0x00 // Pixel 0 is at top of matrix
#define DS_MATRIX_BOTTOM       0x01 // Pixel 0 is at bottom of matrix
#define DS_MATRIX_LEFT         0x00 // Pixel 0 is at left of matrix
#define DS_MATRIX_RIGHT        0x02 // Pixel 0 is at right of matrix
#define DS_MATRIX_CORNER       0x03 // Bitmask for pixel 0 matrix corner
#define DS_MATRIX_ROWS         0x00 // Matrix is row major (horizontal)
#define DS_MATRIX_COLUMNS      0x04 // Matrix is column major (vertical)
#define DS_MATRIX_AXIS         0x04 // Bitmask for row/column layout
#define DS_MATRIX_PROGRESSIVE  0x00 // Same pixel order across each line
#define DS_MATRIX_ZIGZAG       0x08 // Pixel order reverses between lines
#define DS_MATRIX_SEQUENCE     0x08 // Bitmask for pixel line order

// These apply only to tiled displays (multiple matrices):

#define DS_TILE_TOP            0x00 // First tile is at top of matrix
#define DS_TILE_BOTTOM         0x10 // First tile is at bottom of matrix
#define DS_TILE_LEFT           0x00 // First tile is at left of matrix
#define DS_TILE_RIGHT          0x20 // First tile is at right of matrix
#define DS_TILE_CORNER         0x30 // Bitmask for first tile corner
#define DS_TILE_ROWS           0x00 // Tiles ordered in rows
#define DS_TILE_COLUMNS        0x40 // Tiles ordered in columns
#define DS_TILE_AXIS           0x40 // Bitmask for tile H/V orientation
#define DS_TILE_PROGRESSIVE    0x00 // Same tile order across each line
#define DS_TILE_ZIGZAG         0x80 // Tile order reverses between lines
#define DS_TILE_SEQUENCE       0x80 // Bitmask for tile line order

class Adafruit_DotStarMatrix : public Adafruit_GFX, public Adafruit_DotStar {

 public:

  // Constructor for single matrix w/hardware SPI:
  Adafruit_DotStarMatrix(int w, int h,
    uint8_t matrixType, uint8_t ledType);

  // Constructor for single matrix w/bitbang SPI:
  Adafruit_DotStarMatrix(int w, int h, uint8_t d, uint8_t c,
    uint8_t matrixType, uint8_t ledType);

  // Constructor for tiled matrices w/hardware SPI:
  Adafruit_DotStarMatrix(
    uint8_t matrixW, uint8_t matrixH, uint8_t tX, uint8_t tY,
    uint8_t matrixType, uint8_t ledType);

  // Constructor for tiled matrices w/bitbang SPI:
  Adafruit_DotStarMatrix(uint8_t matrixW, uint8_t matrixH,
    uint8_t tX, uint8_t tY, uint8_t d, uint8_t c,
    uint8_t matrixType, uint8_t ledType);

  void
    drawPixel(int16_t x, int16_t y, uint16_t color),
    fillScreen(uint16_t color),
    setPassThruColor(uint32_t c),
    setPassThruColor(void),
    setRemapFunction(uint16_t (*fn)(uint16_t, uint16_t));

  static uint16_t
    Color(uint8_t r, uint8_t g, uint8_t b);

 private:

  const uint8_t
    type;
  const uint8_t
    matrixWidth, matrixHeight, tilesX, tilesY;
  uint16_t
    (*remapFn)(uint16_t x, uint16_t y);

  uint32_t passThruColor;
  boolean  passThruFlag = false;
};

#endif // _ADAFRUIT_DSMATRIX_H_
