/**
 * @file Text_Test.ino
 * @brief Text rendering test for MPI3501 display showing simulated sensor data.
 *
 * This example draws a static UI and updates simulated sensor values (AQI, Temp,
 * Humidity, PM2.5) every 5 seconds.
 */

#include <SPI.h>
#include "UNOQ_MPI3501.h"

UNOQ_MPI3501 lcd;

unsigned long lastUpdate = 0;

void setup() {
    Serial.begin(115200);
    
    Serial.println(F("Initializing LCD..."));
    lcd.begin();
    
    // Draw static UI background
    lcd.fillScreen(COLOR_BLACK);
    
    // Draw header (blue background, white text)
    lcd.fillRect(0, 0, 480, 50, COLOR_BLUE);
    lcd.drawString(20, 15, "ENVIRONMENT MONITOR", COLOR_WHITE, COLOR_BLUE, 3);
    
    // Draw static labels
    // Format: x, y, text, foreground color, background color, text size
    lcd.drawString(20, 80,  "AQI      :", COLOR_CYAN,    COLOR_BLACK, 3);
    lcd.drawString(20, 140, "TEMP     :", COLOR_YELLOW,  COLOR_BLACK, 3);
    lcd.drawString(20, 200, "HUMIDITY :", COLOR_GREEN,   COLOR_BLACK, 3);
    lcd.drawString(20, 260, "PM 2.5   :", COLOR_MAGENTA, COLOR_BLACK, 3);
    
    Serial.println(F("Setup complete. Updating values..."));
}

void loop() {
    // Update every 5000 milliseconds (5 seconds)
    if (millis() - lastUpdate > 5000) {
        lastUpdate = millis();
        
        // Generate dummy sensor values
        int aqi = random(30, 150);
        float temp = 20.0 + random(0, 150) / 10.0;
        float hum = 40.0 + random(0, 300) / 10.0;
        int pm25 = random(10, 80);
        
        // Convert to strings with extra padding spaces to overwrite old text cleanly
        String aqiStr = String(aqi) + "    "; 
        String tempStr = String(temp, 1) + " C   ";
        String humStr = String(hum, 1) + " %   ";
        String pmStr = String(pm25) + " ug/m3   ";
        
        // Draw the updated values to the screen
        lcd.drawString(240, 80,  aqiStr.c_str(), COLOR_WHITE, COLOR_BLACK, 3);
        lcd.drawString(240, 140, tempStr.c_str(), COLOR_WHITE, COLOR_BLACK, 3);
        lcd.drawString(240, 200, humStr.c_str(), COLOR_WHITE, COLOR_BLACK, 3);
        lcd.drawString(240, 260, pmStr.c_str(), COLOR_WHITE, COLOR_BLACK, 3);
        
        Serial.println("Updated values on screen.");
    }
}
