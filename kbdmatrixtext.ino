/*
 * This program cmobines the Adafruit NeoMatrix matrixtest program with USB keyboard input.
 * The matrixtest program scrolls test on an LED grid. To change the text the program must
 * be changed and uploaded.
 *
 * This program add USB keyboard text entry so the scrolling text can be changed without
 * uploading a new version. The program has been tested on an Arduino Due.
 *
 */

#define SERIAL_MONITOR  Serial

// Require keyboard control library
#include <KeyboardController.h>

// Initialize USB Controller
USBHost usb;

// Attach keyboard controller to USB
KeyboardController keyboard(usb);

#define ASCII_CR  0x0d  // AKA carriage return AKA enter key
#define ASCII_BS  0x08  // AKA backspace key

#define MAX_MESSAGE 32
// This holds the current scrolling message
static char Message[MAX_MESSAGE+1] = "Howdy, USB"; // 1 extra for '\0'

// This holds the current keyboard input. When the user presses Enter, the
// contents are transferred to the Message buffer for display.
static char KeyboardBuffer[MAX_MESSAGE];
static uint8_t KeyboardLength = 0;

// This function intercepts key press
void keyPressed() {
  SERIAL_MONITOR.print("Pressed:  ");
  printKey();

  uint8_t k = keyboard.getKey();
  if (isprint(k)) {
    // Add printable characters to the keyboard buffer
    KeyboardBuffer[KeyboardLength] = (char)k;
    if (KeyboardLength < MAX_MESSAGE) KeyboardLength++;
  }
  else if ((k == ASCII_CR) && (KeyboardLength > 0)) {
    // Copy from keyboard buffer to display message buffer. NUL
    // terminate the display string!
    memcpy(Message, KeyboardBuffer, KeyboardLength);
    Message[KeyboardLength] = '\0';
    // Empty keyboard buffer for next message
    KeyboardLength = 0;
  }
  else if (k == ASCII_BS) {
    // Ignore last character
    if (KeyboardLength > 0) KeyboardLength--;
  }
}

void printKey() {
  // getOemKey() returns the OEM-code associated with the key
  SERIAL_MONITOR.print(" key:");
  SERIAL_MONITOR.print(keyboard.getOemKey());

  // getModifiers() returns a bits field with the modifiers-keys
  int mod = keyboard.getModifiers();
  SERIAL_MONITOR.print(" mod:");
  SERIAL_MONITOR.print(mod);

  SERIAL_MONITOR.print(" => ");

  if (mod & LeftCtrl) {
    SERIAL_MONITOR.print("L-Ctrl ");
  }
  if (mod & LeftShift) {
    SERIAL_MONITOR.print("L-Shift ");
  }
  if (mod & Alt) {
    SERIAL_MONITOR.print("Alt ");
  }
  if (mod & LeftCmd) {
    SERIAL_MONITOR.print("L-Cmd ");
  }
  if (mod & RightCtrl) {
    SERIAL_MONITOR.print("R-Ctrl ");
  }
  if (mod & RightShift) {
    SERIAL_MONITOR.print("R-Shift ");
  }
  if (mod & AltGr) {
    SERIAL_MONITOR.print("AltGr ");
  }
  if (mod & RightCmd) {
    SERIAL_MONITOR.print("R-Cmd ");
  }

  // getKey() returns the ASCII translation of OEM key
  // combined with modifiers.
  uint8_t k = keyboard.getKey();
  if (isprint(k)) {
    SERIAL_MONITOR.write('\'');
    SERIAL_MONITOR.write(k);
    SERIAL_MONITOR.write('\'');
  }
  else {
    SERIAL_MONITOR.print("0x");
    SERIAL_MONITOR.print(k, HEX);
  }
  SERIAL_MONITOR.println();
}

// Adafruit_NeoMatrix example for single NeoPixel Shield.
// Scrolls 'Howdy' across the matrix in a portrait (vertical) orientation.

#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#ifndef PSTR
 #define PSTR // Make Arduino Due happy
#endif

#define PIN 6

// MATRIX DECLARATION:
// Parameter 1 = width of NeoPixel matrix
// Parameter 2 = height of matrix
// Parameter 3 = pin number (most are valid)
// Parameter 4 = matrix layout flags, add together as needed:
//   NEO_MATRIX_TOP, NEO_MATRIX_BOTTOM, NEO_MATRIX_LEFT, NEO_MATRIX_RIGHT:
//     Position of the FIRST LED in the matrix; pick two, e.g.
//     NEO_MATRIX_TOP + NEO_MATRIX_LEFT for the top-left corner.
//   NEO_MATRIX_ROWS, NEO_MATRIX_COLUMNS: LEDs are arranged in horizontal
//     rows or in vertical columns, respectively; pick one or the other.
//   NEO_MATRIX_PROGRESSIVE, NEO_MATRIX_ZIGZAG: all rows/columns proceed
//     in the same order, or alternate lines reverse direction; pick one.
//   See example below for these values in action.
// Parameter 5 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)


// Example for NeoPixel Shield.  In this application we'd like to use it
// as a 5x8 tall matrix, with the USB port positioned at the top of the
// Arduino.  When held that way, the first pixel is at the top right, and
// lines are arranged in columns, progressive order.  The shield uses
// 800 KHz (v2) pixels that expect GRB color data.
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, PIN,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_COLUMNS + NEO_MATRIX_PROGRESSIVE,
  NEO_GRB            + NEO_KHZ800);

const uint16_t colors[] = {
  matrix.Color(255, 0, 0), matrix.Color(0, 255, 0), matrix.Color(0, 0, 255) };

void setup() {
  SERIAL_MONITOR.begin(115200);
  SERIAL_MONITOR.println("Program started");

  matrix.begin();
  matrix.setTextWrap(false);
  matrix.setBrightness(40);
  matrix.setTextColor(colors[0]);

  SERIAL_MONITOR.println("setup done");
}

int x    = matrix.width();
int pass = 0;

void loop() {
  // Process USB tasks
  usb.Task();

  matrix.fillScreen(0);
  matrix.setCursor(x, 0);
  matrix.print(Message);
  if(--x < (-6 * (int)(strlen(Message)+1))) {
    x = matrix.width();
    if(++pass >= 3) pass = 0;
    matrix.setTextColor(colors[pass]);
  }
  matrix.show();
  delay(100);
}

