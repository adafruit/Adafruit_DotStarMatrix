/*-------------------------------------------------------------------------
  Arduino library to control single and tiled matrices of APA102-based
  RGB LED devices such as displays assembled from DotStar strips, making
  them compatible with the Adafruit_GFX graphics library.  Requires both
  the Adafruit_DotStar and Adafruit_GFX libraries.

  Written by Phil Burgess / Paint Your Dragon for Adafruit Industries.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing products
  from Adafruit!

  -------------------------------------------------------------------------
  This file is part of the Adafruit DotStarMatrix library.

  DotStarMatrix is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of
  the License, or (at your option) any later version.

  DotStarMatrix is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with DotStarMatrix.  If not, see
  <http://www.gnu.org/licenses/>.
  -------------------------------------------------------------------------*/

#include "Adafruit_DotStarMatrix.h"
#include "gamma.h"
#include <Adafruit_DotStar.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#elif defined(ESP8266)
#include <pgmspace.h>
#else
#ifndef pgm_read_byte
#define pgm_read_byte(addr) (*(const unsigned char *)(addr))
#endif
#endif

#ifndef _swap_uint16_t
#define _swap_uint16_t(a, b)                                                   \
  {                                                                            \
    uint16_t t = a;                                                            \
    a = b;                                                                     \
    b = t;                                                                     \
  }
#endif

//
/**
 * @brief Constructor for single matrix w/hardware SPI
 *
 * @param w The width of the matrix
 * @param h The height of the matrix
 * @param matrixType matrix layout flags. **Add together as needed**:
 *
 *  Position flags for the **FIRST LED** in the matrix **Pick two**, e.g.
 * **DS_MATRIX_TOP** + **DS_MATRIX_LEFT** for the top-left corner:
 *  - **DS_MATRIX_TOP**:         Pixel 0 is at top of matrix
 *  - **DS_MATRIX_LEFT**:        Pixel 0 is at left of matrix
 *  - **DS_MATRIX_BOTTOM**:      Pixel 0 is at bottom of matrix
 *  - **DS_MATRIX_RIGHT**:       Pixel 0 is at right of matrix
 *
 *  LED arrangement flags: **Pick one**
 *  - **DS_MATRIX_ROWS**:        Matrix is row major (horizontal)
 *  - **DS_MATRIX_COLUMNS**:     Matrix is column major (vertical)
 *  - **DS_MATRIX_RIGHT**:       Pixel 0 is at right of matrix
 *
 *  Pixel routing: **Pick one**
 *  - **DS_MATRIX_PROGRESSIVE**: Same pixel order across each line
 *  - **DS_MATRIX_ZIGZAG**:      Pixel order reverses between lines
 *
 * @param ledType DotStar type -- one of the `DOTSTAR_*` constants defined in
 * `Adafruit_DotStar.h`, for example DOTSTAR_BRG for DotStars expecting color
 * bytes expressed in blue, red, green order per pixel.
 *
 *  * **DOTSTAR_RGB**: Transmit as RED,GREEN,BLUE
 *  * **DOTSTAR_RBG**: Transmit as RED,BLUE,GREEN
 *  * **DOTSTAR_GRB**: Transmit as GREEN,RED,BLUE
 *  * **DOTSTAR_GBR**: Transmit as GREEN,BLUE,RED
 *  * **DOTSTAR_BRG**: Transmit as BLUE,RED,GREEN
 *  * **DOTSTAR_BGR**: Transmit as BLUE,GREEN,RED
 */
Adafruit_DotStarMatrix::Adafruit_DotStarMatrix(int w, int h, uint8_t matrixType,
                                               uint8_t ledType)
    : Adafruit_GFX(w, h), Adafruit_DotStar(w * h, ledType), type(matrixType),
      matrixWidth(w), matrixHeight(h), tilesX(0), tilesY(0), remapFn(NULL) {}

//
/**
 * @brief Constructor for single matrix w/bitbang SPI
 *

 * @param w The width of the matrix
 * @param h The height of the matrix
 * @param d Data Pin
 * @param c Clock Pin
 * @param matrixType matrix layout flags. **Add together as needed**:
*
*  Position flags for the **FIRST LED** in the matrix **Pick two**, e.g.
**DS_MATRIX_TOP** + **DS_MATRIX_LEFT** for the top-left corner:
*  - **DS_MATRIX_TOP**:         Pixel 0 is at top of matrix
*  - **DS_MATRIX_LEFT**:        Pixel 0 is at left of matrix
*  - **DS_MATRIX_BOTTOM**:      Pixel 0 is at bottom of matrix
*  - **DS_MATRIX_RIGHT**:       Pixel 0 is at right of matrix
*
*  LED arrangement flags: **Pick one**
*  - **DS_MATRIX_ROWS**:        Matrix is row major (horizontal)
*  - **DS_MATRIX_COLUMNS**:     Matrix is column major (vertical)
*  - **DS_MATRIX_RIGHT**:       Pixel 0 is at right of matrix
*
*  Pixel routing: **Pick one**
*  - **DS_MATRIX_PROGRESSIVE**: Same pixel order across each line
*  - **DS_MATRIX_ZIGZAG**:      Pixel order reverses between lines
*
* @param ledType DotStar type -- one of the `DOTSTAR_*` constants defined in
`Adafruit_DotStar.h`, for example DOTSTAR_BRG for DotStars expecting color bytes
expressed in blue, red, green order per pixel.
*
*  * **DOTSTAR_RGB**: Transmit as RED,GREEN,BLUE
*  * **DOTSTAR_RBG**: Transmit as RED,BLUE,GREEN
*  * **DOTSTAR_GRB**: Transmit as GREEN,RED,BLUE
*  * **DOTSTAR_GBR**: Transmit as GREEN,BLUE,RED
*  * **DOTSTAR_BRG**: Transmit as BLUE,RED,GREEN
*  * **DOTSTAR_BGR**: Transmit as BLUE,GREEN,RED
*/
Adafruit_DotStarMatrix::Adafruit_DotStarMatrix(int w, int h, uint8_t d,
                                               uint8_t c, uint8_t matrixType,
                                               uint8_t ledType)
    : Adafruit_GFX(w, h), Adafruit_DotStar(w * h, d, c, ledType),
      type(matrixType), matrixWidth(w), matrixHeight(h), tilesX(0), tilesY(0),
      remapFn(NULL) {}

/**
 * @brief Constructor for tiled matrices w/hardware SPI
 *

 * @param mW width of **each dotstar matrix**, not the total display
 * @param mH height of each matrix
 * @param tX number of matrices arranged horizontally
 * @param tY number of matrices arranged vertically
 * @param matrixType matrix layout flags. **Add together as needed**:
*
*  Position flags for the **FIRST LED** in the matrix **Pick two**, e.g.
**DS_MATRIX_TOP** + **DS_MATRIX_LEFT** for the top-left corner:
*  - **DS_MATRIX_TOP**:         Pixel 0 is at top of matrix
*  - **DS_MATRIX_LEFT**:        Pixel 0 is at left of matrix
*  - **DS_MATRIX_BOTTOM**:      Pixel 0 is at bottom of matrix
*  - **DS_MATRIX_RIGHT**:       Pixel 0 is at right of matrix
*
*  LED arrangement flags: **Pick one**
*  - **DS_MATRIX_ROWS**:        Matrix is row major (horizontal)
*  - **DS_MATRIX_COLUMNS**:     Matrix is column major (vertical)
*  - **DS_MATRIX_RIGHT**:       Pixel 0 is at right of matrix
*
*  Pixel routing: **Pick one**
*  - **DS_MATRIX_PROGRESSIVE**: Same pixel order across each line
*  - **DS_MATRIX_ZIGZAG**:      Pixel order reverses between lines
*
* @param ledType DotStar type -- one of the `DOTSTAR_*` constants defined in
`Adafruit_DotStar.h`, for example DOTSTAR_BRG for DotStars expecting color bytes
expressed in blue, red, green order per pixel.
*
*  * **DOTSTAR_RGB**: Transmit as RED,GREEN,BLUE
*  * **DOTSTAR_RBG**: Transmit as RED,BLUE,GREEN
*  * **DOTSTAR_GRB**: Transmit as GREEN,RED,BLUE
*  * **DOTSTAR_GBR**: Transmit as GREEN,BLUE,RED
*  * **DOTSTAR_BRG**: Transmit as BLUE,RED,GREEN
*  * **DOTSTAR_BGR**: Transmit as BLUE,GREEN,RED
*/
Adafruit_DotStarMatrix::Adafruit_DotStarMatrix(uint8_t mW, uint8_t mH,
                                               uint8_t tX, uint8_t tY,
                                               uint8_t matrixType,
                                               uint8_t ledType)
    : Adafruit_GFX(mW * tX, mH * tY),
      Adafruit_DotStar(mW * mH * tX * tY, ledType), type(matrixType),
      matrixWidth(mW), matrixHeight(mH), tilesX(tX), tilesY(tY), remapFn(NULL) {
}

/**
 * @brief Constructor for tiled matrices w/bitbang SPI
 *
 * @param mW width of **each dotstar matrix**, not the total display
 * @param mH height of each matrix
 * @param tX number of matrices arranged horizontally
 * @param tY number of matrices arranged vertically
 * @param d Data Pin
 * @param c Clock Pin
 * @param matrixType matrix layout flags. **Add together as needed**:
 *
 *  Position flags for the **FIRST LED** in the matrix **Pick two**, e.g.
 * **DS_MATRIX_TOP** + **DS_MATRIX_LEFT** for the top-left corner:
 *  - **DS_MATRIX_TOP**:         Pixel 0 is at top of matrix
 *  - **DS_MATRIX_LEFT**:        Pixel 0 is at left of matrix
 *  - **DS_MATRIX_BOTTOM**:      Pixel 0 is at bottom of matrix
 *  - **DS_MATRIX_RIGHT**:       Pixel 0 is at right of matrix
 *
 *  LED arrangement flags: **Pick one**
 *  - **DS_MATRIX_ROWS**:        Matrix is row major (horizontal)
 *  - **DS_MATRIX_COLUMNS**:     Matrix is column major (vertical)
 *  - **DS_MATRIX_RIGHT**:       Pixel 0 is at right of matrix
 *
 *  Pixel routing: **Pick one**
 *  - **DS_MATRIX_PROGRESSIVE**: Same pixel order across each line
 *  - **DS_MATRIX_ZIGZAG**:      Pixel order reverses between lines
 *
 * @param ledType DotStar type -- one of the `DOTSTAR_*` constants defined in
 * `Adafruit_DotStar.h`, for example DOTSTAR_BRG for DotStars expecting color
 * bytes expressed in blue, red, green order per pixel.
 *
 *  * **DOTSTAR_RGB**: Transmit as RED,GREEN,BLUE
 *  * **DOTSTAR_RBG**: Transmit as RED,BLUE,GREEN
 *  * **DOTSTAR_GRB**: Transmit as GREEN,RED,BLUE
 *  * **DOTSTAR_GBR**: Transmit as GREEN,BLUE,RED
 *  * **DOTSTAR_BRG**: Transmit as BLUE,RED,GREEN
 *  * **DOTSTAR_BGR**: Transmit as BLUE,GREEN,RED
 */
Adafruit_DotStarMatrix::Adafruit_DotStarMatrix(uint8_t mW, uint8_t mH,
                                               uint8_t tX, uint8_t tY,
                                               uint8_t d, uint8_t c,
                                               uint8_t matrixType,
                                               uint8_t ledType)
    : Adafruit_GFX(mW * tX, mH * tY),
      Adafruit_DotStar(mW * mH * tX * tY, d, c, ledType), type(matrixType),
      matrixWidth(mW), matrixHeight(mH), tilesX(tX), tilesY(tY), remapFn(NULL) {
}

/**
 * @brief Expand 16-bit input color (Adafruit_GFX colorspace) to 24-bit (DotStar
 * w/gamma adjustment)
 *
 * @param color
 * @return uint32_t
 */
static uint32_t expandColor(uint16_t color) {
  return ((uint32_t)pgm_read_byte(&gamma5[color >> 11]) << 16) |
         ((uint32_t)pgm_read_byte(&gamma6[(color >> 5) & 0x3F]) << 8) |
         pgm_read_byte(&gamma5[color & 0x1F]);
}

/**
 * @brief  Downgrade 24-bit color to 16-bit
 *
 * @param r The 8-bit Red Value
 * @param g The 8-bit Green Value
 * @param b The 8-bit Blue Value
 * @return uint16_t
 */
uint16_t Adafruit_DotStarMatrix::Color(uint8_t r, uint8_t g, uint8_t b) {
  return ((uint16_t)(r & 0xF8) << 8) | ((uint16_t)(g & 0xFC) << 3) | (b >> 3);
}

/**
 * @brief Pass a 'raw' color value to set/enable passthrough
 *
 * Pass-through is a kludge that lets you override the current drawing
 *  color with a 'raw' RGB value that's issued directly to pixel(s),
 *  side-stepping the 16-bit color limitation of Adafruit_GFX.
 *  This is not without some limitations of its own -- for example, it
 *  won't work in conjunction with the background color feature when
 *  drawing text or bitmaps (you'll just get a solid rect of color),
 *  only 'transparent' text/bitmaps.  Also, no gamma correction.
 *  Remember to UNSET the passthrough color immediately when done with
 *  it (call with no value)!
 *
 * @param c  A 'raw' 32-bit color value (e.g. no gamma correction)
 */
void Adafruit_DotStarMatrix::setPassThruColor(uint32_t c) {
  passThruColor = c;
  passThruFlag = true;
}

/**
 * @brief Call without a value to disable passthrough
 *
 */
void Adafruit_DotStarMatrix::setPassThruColor(void) { passThruFlag = false; }

/**
 * @brief Draw a pixel of the given color at the specified X, Y position
 *
 * @param x The X coordinate to draw the pixel at
 * @param y The Y coordinate to draw the pixel at
 * @param color The 16 bit color to set
 */
void Adafruit_DotStarMatrix::drawPixel(int16_t x, int16_t y, uint16_t color) {

  if ((x < 0) || (y < 0) || (x >= _width) || (y >= _height))
    return;

  int16_t t;
  switch (rotation) {
  case 1:
    t = x;
    x = WIDTH - 1 - y;
    y = t;
    break;
  case 2:
    x = WIDTH - 1 - x;
    y = HEIGHT - 1 - y;
    break;
  case 3:
    t = x;
    x = y;
    y = HEIGHT - 1 - t;
    break;
  }

  int tileOffset = 0, pixelOffset;

  if (remapFn) { // Custom X/Y remapping function
    pixelOffset = (*remapFn)(x, y);
  } else { // Standard single matrix or tiled matrices

    uint8_t corner = type & DS_MATRIX_CORNER;
    uint16_t minor, major, majorScale;

    if (tilesX) { // Tiled display, multiple matrices
      uint16_t tile;

      minor = x / matrixWidth;           // Tile # X/Y; presume row major to
      major = y / matrixHeight,          // start (will swap later if needed)
          x = x - (minor * matrixWidth); // Pixel X/Y within tile
      y = y - (major * matrixHeight);    // (-* is less math than modulo)

      // Determine corner of entry, flip axes if needed
      if (type & DS_TILE_RIGHT)
        minor = tilesX - 1 - minor;
      if (type & DS_TILE_BOTTOM)
        major = tilesY - 1 - major;

      // Determine actual major axis of tiling
      if ((type & DS_TILE_AXIS) == DS_TILE_ROWS) {
        majorScale = tilesX;
      } else {
        _swap_uint16_t(major, minor);
        majorScale = tilesY;
      }

      // Determine tile number
      if ((type & DS_TILE_SEQUENCE) == DS_TILE_PROGRESSIVE) {
        // All tiles in same order
        tile = major * majorScale + minor;
      } else {
        // Zigzag; alternate rows change direction.  On these rows,
        // this also flips the starting corner of the matrix for the
        // pixel math later.
        if (major & 1) {
          corner ^= DS_MATRIX_CORNER;
          tile = (major + 1) * majorScale - 1 - minor;
        } else {
          tile = major * majorScale + minor;
        }
      }

      // Index of first pixel in tile
      tileOffset = tile * matrixWidth * matrixHeight;

    } // else no tiling (handle as single tile)

    // Find pixel number within tile
    minor = x; // Presume row major to start (will swap later if needed)
    major = y;

    // Determine corner of entry, flip axes if needed
    if (corner & DS_MATRIX_RIGHT)
      minor = matrixWidth - 1 - minor;
    if (corner & DS_MATRIX_BOTTOM)
      major = matrixHeight - 1 - major;

    // Determine actual major axis of matrix
    if ((type & DS_MATRIX_AXIS) == DS_MATRIX_ROWS) {
      majorScale = matrixWidth;
    } else {
      _swap_uint16_t(major, minor);
      majorScale = matrixHeight;
    }

    // Determine pixel number within tile/matrix
    if ((type & DS_MATRIX_SEQUENCE) == DS_MATRIX_PROGRESSIVE) {
      // All lines in same order
      pixelOffset = major * majorScale + minor;
    } else {
      // Zigzag; alternate rows change direction.
      if (major & 1)
        pixelOffset = (major + 1) * majorScale - 1 - minor;
      else
        pixelOffset = major * majorScale + minor;
    }
  }

  setPixelColor(tileOffset + pixelOffset,
                passThruFlag ? passThruColor : expandColor(color));
}

/**
 * @brief Fill the matrix with a given color
 *
 * @param color The 16-bit color to fill the matrix with
 */
void Adafruit_DotStarMatrix::fillScreen(uint16_t color) {
  uint16_t i, n;
  uint32_t c;

  c = passThruFlag ? passThruColor : expandColor(color);
  n = numPixels();
  for (i = 0; i < n; i++)
    setPixelColor(i, c);
}

/**
 * @brief Provide your own function to remap X/Y coordinates to DotStar strip
indices for matrices that are not uniformly tiled.
 *
 * @param fn A pointer to the remapping function. This function should accept
two unsigned 16-bit arguments (pixel X, Y coordinates) and return an unsigned
16-bit value (corresponding strip index). The simplest row-major progressive
function might resemble this:
 * \code{.cpp}
uint16_t myRemapFn(uint16_t x, uint16_t y) {
  return WIDTH * y + x;
}
 * \endcode
 */
void Adafruit_DotStarMatrix::setRemapFunction(uint16_t (*fn)(uint16_t,
                                                             uint16_t)) {
  remapFn = fn;
}
