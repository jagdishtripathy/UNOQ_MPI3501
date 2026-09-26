/**
 * @file LCD_Test.ino
 * @brief BARE MINIMUM LCD test — colors only, no fonts/touch/graphics.
 *
 * ============================================================================
 * WIRING — Arduino UNO Q to MPI3501 3.5" RPi Display
 * ============================================================================
 *
 * VERIFIED against: LCDWiki, TFT_eSPI Setup11, ianlunam/esp32-mpi3501
 *
 *   Display Pin 1  (3.3V)       --> UNO Q 3.3V
 *   Display Pin 2  (5V)         --> UNO Q 5V
 *   Display Pin 4  (5V)         --> UNO Q 5V
 *   Display Pin 6  (GND)        --> UNO Q GND
 *   Display Pin 17 (3.3V)       --> UNO Q 3.3V
 *   Display Pin 18 (LCD DC/RS)  --> UNO Q D2    *** THIS IS DC, NOT RST ***
 *   Display Pin 19 (SPI MOSI)   --> UNO Q D11
 *   Display Pin 21 (SPI MISO)   --> UNO Q D12
 *   Display Pin 22 (LCD RST)    --> UNO Q D3    *** THIS IS LCD RESET ***
 *   Display Pin 23 (SPI SCK)    --> UNO Q D13
 *   Display Pin 24 (LCD CS)     --> UNO Q D10
 *   Display Pin 26 (Touch CS)   --> UNO Q D4   (not used in this test)
 *   Display Pin 11 (Touch IRQ)  --> UNO Q D5   (not used in this test)
 *
 * DO NOT CONNECT:
 *   Display Pin 3, 5, 7, 8, 10, 12, 13, 14, 15, 16
 *   (these are GPIO/I2C/UART pins not used by this display)
 *
 * POWER — BOTH 3.3V and 5V are required:
 *   Pin 1, 17 = 3.3V (logic rail)
 *   Pin 2, 4  = 5V   (backlight + regulator input)
 *   Pin 6     = GND
 *
 * ============================================================================
 * TEST SEQUENCE
 * ============================================================================
 *
 *   1. Initialize GPIO
 *   2. Initialize SPI
 *   3. Reset the display
 *   4. Initialize ILI9486
 *   5. Set landscape 480x320
 *   6. Fill RED   — wait 2 seconds
 *   7. Fill GREEN — wait 2 seconds
 *   8. Fill BLUE  — wait 2 seconds
 *   9. Fill WHITE — hold
 *
 * ============================================================================
 */

#include <SPI.h>
#include "UNOQ_MPI3501.h"

// Create LCD driver: CS=D10, DC=D2, RST=D3
UNOQ_MPI3501 lcd;

void setup() {
    // Step 1: Serial for diagnostics
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    Serial.println(F(""));
    Serial.println(F("=== UNOQ_MPI3501 BARE MINIMUM TEST ==="));
    Serial.println(F(""));

    // Steps 2-5: Initialize SPI + GPIO + reset + init + landscape
    Serial.println(F("Initializing LCD..."));
    lcd.begin();  // Does: pinMode, SPI.begin, reset, initSequence, setRotation(1)
    Serial.println(F("LCD initialized."));
    Serial.println(F(""));

    // Print diagnostics
    lcd.printDiagnostics();
    Serial.println(F(""));

    // Step 6: Fill RED
    Serial.println(F("Filling RED..."));
    lcd.fillScreen(COLOR_RED);
    Serial.println(F("RED done. Waiting 2s..."));
    delay(2000);

    // Step 9: Fill WHITE
    Serial.println(F("Filling WHITE..."));
    lcd.fillScreen(COLOR_WHITE);
    Serial.println(F("WHITE done."));
    
    // Step 7: Fill GREEN
    Serial.println(F("Filling GREEN..."));
    lcd.fillScreen(COLOR_GREEN);
    Serial.println(F("GREEN done. Waiting 2s..."));
    delay(2000);

    // Step 8: Fill BLUE
    Serial.println(F("Filling BLUE..."));
    lcd.fillScreen(COLOR_BLUE);
    Serial.println(F("BLUE done. Waiting 2s..."));
    delay(2000);



    Serial.println(F(""));
    Serial.println(F("=== TEST COMPLETE ==="));
    Serial.println(F(""));
    Serial.println(F("EXPECTED: Display should have shown RED, GREEN, BLUE,"));
    Serial.println(F("          and now be solid WHITE."));
    Serial.println(F(""));
    Serial.println(F("IF BLANK/WHITE THE WHOLE TIME:"));
    Serial.println(F("  1. Check DC wiring:  Pin 18 -> D2"));
    Serial.println(F("  2. Check RST wiring: Pin 22 -> D3"));
    Serial.println(F("  3. Check CS wiring:  Pin 24 -> D10"));
    Serial.println(F("  4. Check 5V power:   Pin 2,4 -> 5V"));
    Serial.println(F("  5. Check 3.3V power: Pin 1,17 -> 3.3V"));
    Serial.println(F("  6. Try: lcd.invertDisplay(true) after begin()"));
    Serial.println(F("  7. Try: reduce LCD_SPI_FREQ to 8000000"));
    Serial.println(F(""));
    Serial.println(F("IF COLORS ARE WRONG/INVERTED:"));
    Serial.println(F("  Add lcd.invertDisplay(true) after lcd.begin()"));
}

void loop() {
    // Nothing — display holds content.
}
