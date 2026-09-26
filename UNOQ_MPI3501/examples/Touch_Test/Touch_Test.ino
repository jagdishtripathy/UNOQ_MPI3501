/**
 * @file Touch_Test.ino
 * @brief Touch test and calibration — run AFTER LCD_Test works.
 *
 * ============================================================================
 * WIRING (same as LCD_Test + touch pins)
 * ============================================================================
 *
 *   Display Pin 26 (TP_CS)  --> UNO Q D4
 *   Display Pin 11 (TP_IRQ) --> UNO Q D5
 *   (all LCD pins same as LCD_Test.ino)
 *
 * NOTE: There is NO separate touch reset pin to connect.
 *       Pin 22 is LCD RST (already connected to D3).
 *       The XPT2046 does not need a hardware reset.
 *
 * ============================================================================
 */

#include <SPI.h>
#include "UNOQ_MPI3501.h"
#include "UNOQ_XPT2046.h"

UNOQ_MPI3501 lcd;
UNOQ_XPT2046 touch;

uint16_t brushColor = COLOR_RED;
unsigned long lastTouchTime = 0;

// ============================================================================
// Four-corner calibration
// ============================================================================

void runCalibration() {
    Serial.println(F(""));
    Serial.println(F("=== CALIBRATION ==="));
    Serial.println(F("Touch each crosshair on the LCD."));

    CalibrationPoint points[4];
    const int16_t margin = 30;
    int16_t targets[4][2] = {
        { margin, margin },
        { lcd.width() - margin, margin },
        { margin, lcd.height() - margin },
        { lcd.width() - margin, lcd.height() - margin }
    };
    const char *labels[4] = { "Top-Left", "Top-Right", "Bot-Left", "Bot-Right" };

    for (int i = 0; i < 4; i++) {
        lcd.fillScreen(COLOR_WHITE);

        int16_t tx = targets[i][0];
        int16_t ty = targets[i][1];

        // Draw crosshair
        lcd.drawFastHLine(tx - 10, ty, 21, COLOR_RED);
        lcd.drawFastVLine(tx, ty - 10, 21, COLOR_RED);

        lcd.drawString(120, 150, "Touch crosshair", COLOR_BLACK, COLOR_WHITE, 2);
        lcd.drawString(140, 180, labels[i], COLOR_BLUE, COLOR_WHITE, 2);

        Serial.print(F("Touch ")); Serial.print(labels[i]); Serial.println(F("..."));

        // Wait for touch
        while (!touch.isTouched()) delay(10);
        delay(100);

        // Read averaged raw values
        uint32_t sumX = 0, sumY = 0;
        for (int s = 0; s < 16; s++) {
            sumX += touch.readRawX();
            sumY += touch.readRawY();
            delay(5);
        }
        points[i].screenX = tx;
        points[i].screenY = ty;
        points[i].rawX = sumX / 16;
        points[i].rawY = sumY / 16;

        Serial.print(F("  Raw X=")); Serial.print(points[i].rawX);
        Serial.print(F(" Y=")); Serial.println(points[i].rawY);

        lcd.fillRect(tx - 5, ty - 5, 11, 11, COLOR_GREEN);

        // Wait for release
        while (touch.isTouched()) delay(10);
        delay(200);
    }

    // Compute calibration
    uint16_t xMin = min(min(points[0].rawX, points[2].rawX), min(points[1].rawX, points[3].rawX));
    uint16_t xMax = max(max(points[0].rawX, points[2].rawX), max(points[1].rawX, points[3].rawX));
    uint16_t yMin = min(min(points[0].rawY, points[1].rawY), min(points[2].rawY, points[3].rawY));
    uint16_t yMax = max(max(points[0].rawY, points[1].rawY), max(points[2].rawY, points[3].rawY));

    // Extend margins slightly
    uint16_t xRange = xMax - xMin;
    uint16_t yRange = yMax - yMin;
    xMin = (xMin > xRange / 10) ? xMin - xRange / 10 : 0;
    xMax += xRange / 10;
    yMin = (yMin > yRange / 10) ? yMin - yRange / 10 : 0;
    yMax += yRange / 10;

    touch.setCalibration(xMin, xMax, yMin, yMax);

    Serial.println(F("Calibration complete!"));
    Serial.print(F("  X: ")); Serial.print(xMin); Serial.print(F("-")); Serial.println(xMax);
    Serial.print(F("  Y: ")); Serial.print(yMin); Serial.print(F("-")); Serial.println(yMax);
    Serial.println(F("Add to your code:"));
    Serial.print(F("  #define TOUCH_CAL_X_MIN ")); Serial.println(xMin);
    Serial.print(F("  #define TOUCH_CAL_X_MAX ")); Serial.println(xMax);
    Serial.print(F("  #define TOUCH_CAL_Y_MIN ")); Serial.println(yMin);
    Serial.print(F("  #define TOUCH_CAL_Y_MAX ")); Serial.println(yMax);
}

// ============================================================================
// Draw the drawing UI
// ============================================================================

void drawUI() {
    lcd.fillScreen(COLOR_WHITE);
    lcd.fillRect(0, 0, lcd.width(), 25, COLOR_BLUE);
    lcd.drawString(5, 5, "Touch Test", COLOR_WHITE, COLOR_BLUE, 2);

    // Color palette at bottom
    int16_t palY = lcd.height() - 30;
    int16_t palW = lcd.width() / 7;
    lcd.fillRect(0 * palW, palY, palW, 30, COLOR_RED);
    lcd.fillRect(1 * palW, palY, palW, 30, COLOR_GREEN);
    lcd.fillRect(2 * palW, palY, palW, 30, COLOR_BLUE);
    lcd.fillRect(3 * palW, palY, palW, 30, COLOR_YELLOW);
    lcd.fillRect(4 * palW, palY, palW, 30, COLOR_CYAN);
    lcd.fillRect(5 * palW, palY, palW, 30, COLOR_MAGENTA);
    lcd.fillRect(6 * palW, palY, palW, 30, COLOR_BLACK);
}

// ============================================================================
// Setup
// ============================================================================

void setup() {
    Serial.begin(115200);
    while (!Serial && millis() < 3000);

    Serial.println(F("=== Touch Test ==="));

    lcd.begin();
    touch.begin();
    touch.setRotation(lcd.getRotation());
    touch.setDisplaySize(lcd.width(), lcd.height());

    lcd.printDiagnostics();
    touch.printDiagnostics();

    // Offer calibration
    lcd.fillScreen(COLOR_BLACK);
    lcd.drawString(10, 140, "Touch NOW to calibrate", COLOR_YELLOW, COLOR_BLACK, 2);
    lcd.drawString(10, 170, "or wait 3s to skip", COLOR_DARK_GREY, COLOR_BLACK, 1);

    unsigned long t = millis();
    bool doCal = false;
    while (millis() - t < 3000) {
        if (touch.isTouched()) { doCal = true; break; }
        delay(50);
    }

    if (doCal) {
        while (touch.isTouched()) delay(10);
        delay(200);
        runCalibration();
    }

    drawUI();
    Serial.println(F("Ready. Touch to draw."));
}

// ============================================================================
// Loop
// ============================================================================

void loop() {
    if (millis() - lastTouchTime < 20) return;

    if (touch.isTouched()) {
        lastTouchTime = millis();
        TouchPoint tp = touch.read();

        if (tp.pressed) {
            Serial.print(F("x=")); Serial.print(tp.x);
            Serial.print(F(" y=")); Serial.print(tp.y);
            Serial.print(F(" raw(")); Serial.print(tp.rawX);
            Serial.print(F(",")); Serial.print(tp.rawY);
            Serial.print(F(") z=")); Serial.println(tp.z);

            // Check palette
            int16_t palY = lcd.height() - 30;
            if (tp.y >= palY) {
                int idx = tp.x / (lcd.width() / 7);
                uint16_t colors[] = { COLOR_RED, COLOR_GREEN, COLOR_BLUE,
                                      COLOR_YELLOW, COLOR_CYAN, COLOR_MAGENTA, COLOR_BLACK };
                if (idx >= 0 && idx < 7) brushColor = colors[idx];
            } else if (tp.y > 26) {
                lcd.fillRect(tp.x - 1, tp.y - 1, 3, 3, brushColor);
            }
        }
    }
}
