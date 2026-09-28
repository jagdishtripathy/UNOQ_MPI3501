# UNOQ_MPI3501 — Arduino UNO Q Driver for 3.5" RPi Display

Native Arduino UNO Q display driver for the MPI3501 / GoodTFT 3.5-inch
Raspberry Pi TFT LCD (480×320, ILI9486 controller, XPT2046 touch).

---

## Table of Contents

1. [Hardware Overview](#hardware-overview)
2. [Wiring Guide](#wiring-guide)
3. [Installation](#installation)
4. [Quick Start](#quick-start)
5. [LCD API Reference](#lcd-api-reference)
6. [Touch API Reference](#touch-api-reference)
7. [How the Protocol Works](#how-the-protocol-works)
8. [Troubleshooting](#troubleshooting)
9. [Pin Reference Table](#pin-reference-table)

---

## Hardware Overview

### What You Need

| Component             | Details                                  |
|-----------------------|------------------------------------------|
| Microcontroller       | Arduino UNO Q (3.3V logic)               |
| Display               | 3.5" RPi Display (MPI3501)               |
| Display Resolution    | 480 × 320 pixels                         |
| LCD Controller        | ILI9486                                  |
| Touch Controller      | XPT2046 / HR2046                         |
| Color Format          | RGB565 (16-bit, 65536 colors)            |
| Interface             | SPI (shared bus for LCD and touch)        |
| Jumper Wires          | 12 female-to-female (or female-to-male)  |

### ICs on the Display PCB

| Marking | IC         | Function                                     |
|---------|------------|----------------------------------------------|
| U1      | 74HC04D    | Hex inverter — generates clock/control signals |
| U2      | 74HC4040   | 12-stage ripple counter — serial-to-parallel  |
| U3      | 74HC4040   | 12-stage ripple counter — serial-to-parallel  |
| U4      | 74HC4040   | 12-stage ripple counter — serial-to-parallel  |
| U5      | HR2046     | Touch controller (XPT2046 compatible)         |
| U6      | AMS1117-3.3| 3.3V voltage regulator                        |

---

## Wiring Guide

### Overview Diagram

```
    Arduino UNO Q                       MPI3501 Display (26-pin header)
   ┌─────────────┐                     ┌─────────────────────────┐
   │             │                     │  (pin 1 is top-left     │
   │        3.3V ├────────────────────►│   when connector faces  │
   │             ├────────────────────►│   you, display facing   │
   │          5V ├────────────────────►│   away)                 │
   │             ├────────────────────►│                         │
   │         GND ├────────────────────►│                         │
   │             │                     │                         │
   │          D2 ├────── DC/RS ───────►│  Pin 18 (LCD DC)        │
   │          D3 ├────── RESET ───────►│  Pin 22 (LCD RST)       │
   │          D4 ├────── T_CS ────────►│  Pin 26 (Touch CS)      │
   │          D5 ├────── T_IRQ ───────►│  Pin 11 (Touch IRQ)     │
   │             │                     │                         │
   │   D10 (SS) ├────── LCD_CS ──────►│  Pin 24 (LCD CS)        │
   │   D11 (MOSI)├───── MOSI ────────►│  Pin 19 (SPI MOSI)      │
   │   D12 (MISO)├───── MISO ────────►│  Pin 21 (SPI MISO)      │
   │   D13 (SCK) ├───── SCK ─────────►│  Pin 23 (SPI SCK)       │
   │             │                     │                         │
   └─────────────┘                     └─────────────────────────┘
```

### Detailed Wire-by-Wire Connections

Wire these connections one at a time. Use the table below.

#### Power Wires (3 wires)

| # | UNO Q Pin | Display Pin | Signal | Wire Color (suggested) | Notes |
|---|-----------|-------------|--------|------------------------|-------|
| 1 | **5V**    | **Pin 2**   | 5V     | Red                    | Powers the backlight and AMS1117 regulator |
| 2 | **5V**    | **Pin 4**   | 5V     | Red                    | Second 5V supply — connect both |
| 3 | **GND**   | **Pin 6**   | Ground | Black                  | Common ground |

> **IMPORTANT:** The display also needs 3.3V on Pin 1 and Pin 17.
> On the Arduino UNO Q, connect:

| # | UNO Q Pin | Display Pin | Signal | Notes |
|---|-----------|-------------|--------|-------|
| 4 | **3.3V**  | **Pin 1**   | 3.3V   | Logic power rail |
| 5 | **3.3V**  | **Pin 17**  | 3.3V   | Second 3.3V supply |

#### SPI Bus Wires (3 wires — shared between LCD and touch)

| # | UNO Q Pin     | Display Pin | Signal    | Notes |
|---|---------------|-------------|-----------|-------|
| 6 | **D11 (MOSI)**| **Pin 19**  | SPI MOSI  | Data out from UNO Q to display |
| 7 | **D12 (MISO)**| **Pin 21**  | SPI MISO  | Data back from touch controller |
| 8 | **D13 (SCK)** | **Pin 23**  | SPI Clock | Clock signal |

#### LCD Control Wires (3 wires)

| #  | UNO Q Pin | Display Pin | Signal   | Notes |
|----|-----------|-------------|----------|-------|
| 9  | **D10**   | **Pin 24**  | LCD CS   | LCD chip select (active LOW) |
| 10 | **D2**    | **Pin 18**  | LCD DC   | Data/Command select (HIGH=data, LOW=command) |
| 11 | **D3**    | **Pin 22**  | LCD RST  | Hardware reset (active LOW) |

#### Touch Control Wires (2 wires)

| #  | UNO Q Pin | Display Pin | Signal    | Notes |
|----|-----------|-------------|-----------|-------|
| 12 | **D4**    | **Pin 26**  | Touch CS  | Touch chip select (active LOW) |
| 13 | **D5**    | **Pin 11**  | Touch IRQ | Pen-down interrupt (active LOW) |

**Total: 13 wires** (5 power + 3 SPI + 3 LCD control + 2 touch control)

### Display 26-Pin Header Layout

Looking at the display from the BACK (connector facing you):

```
         ┌─────────────────────────┐
         │   26-Pin Header Layout  │
         │                         │
         │  [1]  [2]               │
         │  3.3V  5V               │
         │                         │
         │  [3]  [4]               │
         │   -    5V               │
         │                         │
         │  [5]  [6]               │
         │   -   GND               │
         │                         │
         │  [7]  [8]               │
         │   -    -                │
         │                         │
         │  [9]  [10]              │
         │   -    -                │
         │                         │
         │ [11]  [12]              │
         │ T_IRQ  -                │
         │                         │
         │ [13]  [14]              │
         │   -    -                │
         │                         │
         │ [15]  [16]              │
         │   -    -                │
         │                         │
         │ [17]  [18]              │
         │ 3.3V  LCD_DC            │
         │                         │
         │ [19]  [20]              │
         │ MOSI   -                │
         │                         │
         │ [21]  [22]              │
         │ MISO  LCD_RST           │
         │                         │
         │ [23]  [24]              │
         │  SCK  LCD_CS            │
         │                         │
         │ [25]  [26]              │
         │   -   T_CS              │
         └─────────────────────────┘
```

Pins marked with `-` are NOT connected.

### DO NOT CONNECT These Pins

| Display Pin | RPi GPIO     | Reason                         |
|-------------|--------------|--------------------------------|
| Pin 3       | GPIO2 (SDA)  | I2C — not used by this display |
| Pin 5       | GPIO3 (SCL)  | I2C — not used                 |
| Pin 7       | GPIO4        | Not used                       |
| Pin 8       | GPIO14 (TXD) | UART — not used                |
| Pin 9       | GND          | Use Pin 6 only                 |
| Pin 10      | GPIO15 (RXD) | UART — not used                |
| Pin 12      | GPIO18       | Not used                       |
| Pin 13      | GPIO27       | Not used                       |
| Pin 14      | GND          | Use Pin 6 only                 |
| Pin 15      | GPIO22       | Not used                       |
| Pin 16      | GPIO23       | Not used                       |
| Pin 20      | GND          | Use Pin 6 only                 |
| Pin 25      | GND          | Use Pin 6 only                 |

### Voltage Safety

- Arduino UNO Q GPIO operates at **3.3V**.
- The display's logic ICs (74HC series) run at **3.3V** (regulated by AMS1117).
- The 5V power (Pin 2, Pin 4) feeds only the AMS1117 regulator and the backlight.
- **No level shifters are needed.**
- **Do NOT** apply 5V to any signal wire — only to Pin 2 and Pin 4.

---

## Installation

### Method 1 — Copy to Arduino Libraries Folder

1. Copy the entire `UNOQ_MPI3501` folder to your Arduino libraries directory:

   ```
   C:\Users\<YourName>\Documents\Arduino\libraries\UNOQ_MPI3501\
   ```

2. Restart Arduino IDE.

3. The library will appear under **Sketch → Include Library → UNOQ_MPI3501**.

### Method 2 — Use Directly in Your Sketch Folder

1. Place the `src/` files alongside your `.ino` sketch.
2. Include with relative paths:
   ```cpp
   #include "UNOQ_MPI3501.h"
   #include "UNOQ_XPT2046.h"
   ```

---

## Quick Start

### Step 1 — Test the LCD First (No Touch)

Wire ONLY the LCD pins (power + SPI + LCD control — wires 1–11 from the table above).
Do NOT connect touch wires yet.

Upload this sketch:

```cpp
#include <SPI.h>
#include "UNOQ_MPI3501.h"

UNOQ_MPI3501 lcd;  // CS=D10, DC=D2, RST=D3

void setup() {
    Serial.begin(115200);
    lcd.begin();          // Init SPI + reset + ILI9486 + landscape mode
    lcd.printDiagnostics();

    lcd.fillScreen(COLOR_RED);    delay(2000);
    lcd.fillScreen(COLOR_GREEN);  delay(2000);
    lcd.fillScreen(COLOR_BLUE);   delay(2000);
    lcd.fillScreen(COLOR_WHITE);
}

void loop() {}
```

**Expected result:** Display cycles RED → GREEN → BLUE → WHITE.

### Step 2 — Add Touch (After LCD Works)

Add wires 12–13 (Touch CS → D4, Touch IRQ → D5, and MISO → D12 if not already connected).

```cpp
#include <SPI.h>
#include "UNOQ_MPI3501.h"
#include "UNOQ_XPT2046.h"

UNOQ_MPI3501 lcd;
UNOQ_XPT2046 touch;

void setup() {
    Serial.begin(115200);
    lcd.begin();
    touch.begin();
    touch.setRotation(lcd.getRotation());
    touch.setDisplaySize(lcd.width(), lcd.height());

    lcd.fillScreen(COLOR_WHITE);
}

void loop() {
    if (touch.isTouched()) {
        TouchPoint tp = touch.read();
        if (tp.pressed) {
            lcd.fillRect(tp.x - 1, tp.y - 1, 3, 3, COLOR_RED);
            Serial.print("x="); Serial.print(tp.x);
            Serial.print(" y="); Serial.println(tp.y);
        }
    }
    delay(20);
}
```

---

## LCD API Reference

### Include and Create

```cpp
#include <SPI.h>
#include "UNOQ_MPI3501.h"

// Default pins: CS=D10, DC=D2, RST=D3
UNOQ_MPI3501 lcd;

// Or specify custom pins:
UNOQ_MPI3501 lcd(10, 2, 3);  // CS, DC, RST
```

### Initialization

```cpp
lcd.begin();   // Sets up SPI, resets display, runs ILI9486 init, sets landscape mode
```

### Screen Orientation

```cpp
lcd.setRotation(0);  // Portrait     320×480
lcd.setRotation(1);  // Landscape    480×320  (default after begin)
lcd.setRotation(2);  // Portrait     320×480  (upside down)
lcd.setRotation(3);  // Landscape    480×320  (upside down)

int16_t w = lcd.width();       // Current width  (depends on rotation)
int16_t h = lcd.height();      // Current height (depends on rotation)
uint8_t r = lcd.getRotation(); // Current rotation (0-3)
```

### Drawing — Fills and Rectangles

```cpp
lcd.fillScreen(COLOR_RED);                      // Fill entire screen
lcd.fillRect(10, 10, 100, 50, COLOR_BLUE);      // Filled rectangle at (10,10), 100×50
lcd.drawRect(10, 10, 100, 50, COLOR_WHITE);     // Outlined rectangle
```

### Drawing — Lines

```cpp
lcd.drawFastHLine(0, 160, 480, COLOR_GREEN);    // Horizontal line
lcd.drawFastVLine(240, 0, 320, COLOR_GREEN);    // Vertical line
lcd.drawLine(0, 0, 479, 319, COLOR_RED);        // Arbitrary diagonal line
```

### Drawing — Pixels

```cpp
lcd.drawPixel(100, 100, COLOR_WHITE);           // Single pixel
```

### Drawing — Bitmaps

```cpp
// Array of RGB565 pixel values, stored in RAM
const uint16_t myImage[] = { 0xF800, 0x07E0, 0x001F, ... };
lcd.drawBitmap(0, 0, 16, 16, myImage);  // Draw 16×16 bitmap at (0,0)
```

### Drawing — Text

```cpp
// drawChar(x, y, character, foreground, background, size)
lcd.drawChar(10, 10, 'A', COLOR_WHITE, COLOR_BLACK, 2);

// drawString(x, y, string, foreground, background, size)
lcd.drawString(10, 10, "Hello!", COLOR_WHITE, COLOR_BLACK, 2);

// Size 1 = 5×7 pixels per character (tiny)
// Size 2 = 10×14 pixels per character (readable)
// Size 3 = 15×21 pixels per character (large)
```

### Display Control

```cpp
lcd.invertDisplay(true);   // Invert colors (try if colors look wrong)
lcd.invertDisplay(false);  // Normal colors
lcd.displayOn();           // Turn display on
lcd.displayOff();          // Turn display off (backlight stays on)
```

### Diagnostics

```cpp
lcd.printDiagnostics();    // Print all pin/SPI/display info to Serial
```

### Color Constants

```cpp
COLOR_BLACK        // 0x0000
COLOR_WHITE        // 0xFFFF
COLOR_RED          // 0xF800
COLOR_GREEN        // 0x07E0
COLOR_BLUE         // 0x001F
COLOR_CYAN         // 0x07FF
COLOR_MAGENTA      // 0xF81F
COLOR_YELLOW       // 0xFFE0
COLOR_ORANGE       // 0xFD20
COLOR_DARK_GREEN   // 0x03E0
COLOR_DARK_GREY    // 0x7BEF
COLOR_LIGHT_GREY   // 0xC618

// Create custom color from 8-bit R, G, B:
uint16_t myColor = COLOR565(255, 128, 0);  // Orange
```

---

## Touch API Reference

### Include and Create

```cpp
#include "UNOQ_XPT2046.h"

// Default pins: Touch CS=D4, IRQ=D5, LCD CS=D10
UNOQ_XPT2046 touch;

// Or specify custom pins:
UNOQ_XPT2046 touch(4, 5, 10);  // touchCS, irqPin, lcdCS
```

### Initialization

```cpp
touch.begin();
touch.setRotation(lcd.getRotation());      // Match LCD rotation
touch.setDisplaySize(lcd.width(), lcd.height());  // Set pixel dimensions
```

### Reading Touch

```cpp
if (touch.isTouched()) {
    TouchPoint tp = touch.read();

    if (tp.pressed) {
        int16_t  x    = tp.x;       // Mapped pixel X (0 to width-1)
        int16_t  y    = tp.y;       // Mapped pixel Y (0 to height-1)
        uint16_t rawX = tp.rawX;    // Raw ADC value (0-4095)
        uint16_t rawY = tp.rawY;    // Raw ADC value (0-4095)
        uint16_t z    = tp.z;       // Pressure (higher = more pressure)
    }
}
```

### Raw ADC Readings (for debugging)

```cpp
uint16_t rx = touch.readRawX();   // 8-sample averaged, 0-4095
uint16_t ry = touch.readRawY();   // 8-sample averaged, 0-4095
uint16_t rz = touch.readRawZ();   // Pressure reading
```

### Calibration

The default calibration values (300–3800) are approximate.
For accurate touch, run the 4-corner calibration in `Touch_Test.ino`
and use the values it prints:

```cpp
// Set custom calibration (from calibration routine output)
touch.setCalibration(250, 3850, 280, 3780);  // xMin, xMax, yMin, yMax

// Or define before including the header:
#define TOUCH_CAL_X_MIN 250
#define TOUCH_CAL_X_MAX 3850
#define TOUCH_CAL_Y_MIN 280
#define TOUCH_CAL_Y_MAX 3780
#include "UNOQ_XPT2046.h"
```

### Diagnostics

```cpp
touch.printDiagnostics();  // Print pin config, calibration, live readings
```

---

## How the Protocol Works

### Why This Display is Special

This is NOT a standard SPI display. The MPI3501 PCB has an onboard
**serial-to-parallel bridge** made from three 74HC4040 ripple counters
and a 74HC04D inverter. This bridge converts the serial SPI data into
a 16-bit parallel bus for the ILI9486 controller.

### What This Means for SPI Communication

**Every SPI transfer must be 16 bits wide:**

- **Sending a command (8-bit):** The byte is sent TWICE to fill 16 clocks.
  ```
  DC = LOW (command mode)
  SPI.transfer(0x2C);   // First copy of byte
  SPI.transfer(0x2C);   // Second copy — 16 clocks total
  ```

- **Sending a parameter (8-bit):** Same — byte sent twice.
  ```
  DC = HIGH (data mode)
  SPI.transfer(0x55);   // First copy
  SPI.transfer(0x55);   // Second copy
  ```

- **Sending a pixel (16-bit RGB565):** Sent as two bytes, high then low.
  ```
  DC = HIGH (data mode)
  SPI.transfer(0xF8);   // High byte of RED (0xF800)
  SPI.transfer(0x00);   // Low byte
  ```

- **Sending a 16-bit coordinate (e.g., X or Y limit):** A 16-bit coordinate is first split into its High Byte and Low Byte. Because of the bridge, EACH of those 8-bit bytes must be sent twice. This means sending a single 16-bit coordinate takes 4 byte transfers (4 chunks).
  ```
  DC = HIGH (data mode)
  // Example: Sending coordinate X = 319 (Hex: 0x013F)
  SPI.transfer(0x01);   // High byte (chunk 1)
  SPI.transfer(0x01);   // High byte (chunk 2)
  SPI.transfer(0x3F);   // Low byte (chunk 3)
  SPI.transfer(0x3F);   // Low byte (chunk 4)
  ```

This is proven by the TFT_eSPI library source code:
```cpp
// From Processors/TFT_eSPI_Generic.h:
#if defined (RPI_DISPLAY_TYPE)
  #define tft_Write_8(C)  spi.transfer(C); spi.transfer(C)
  #define tft_Write_16(C) spi.transfer((C)>>8); spi.transfer((C)>>0)
#endif
```

### SPI Configuration

| Setting   | LCD (ILI9486)    | Touch (XPT2046)  |
|-----------|------------------|------------------|
| Mode      | SPI Mode 0       | SPI Mode 0       |
| Bit Order | MSB First        | MSB First        |
| Frequency | 16 MHz (default) | 1 MHz            |
| Word Size | 16-bit (always)  | 8-bit (standard) |
| CS Pin    | D10              | D4               |

The LCD and touch share MOSI, MISO, and SCK.
Only ONE chip select can be LOW at a time.

---

## Troubleshooting

### Display is completely blank / stays white

| Check | What to do |
|-------|------------|
| DC pin wiring | Verify Display Pin 18 → UNO Q D2 |
| RST pin wiring | Verify Display Pin 22 → UNO Q D3 |
| CS pin wiring | Verify Display Pin 24 → UNO Q D10 |
| 5V power | Verify 5V on Display Pin 2 AND Pin 4 |
| 3.3V power | Verify 3.3V on Display Pin 1 AND Pin 17 |
| GND | Verify GND on Display Pin 6 |
| SPI wires | Verify MOSI (Pin 19→D11), SCK (Pin 23→D13) |
| SPI speed | Try `#define LCD_SPI_FREQ 8000000UL` before including header |
| Inversion | Try `lcd.invertDisplay(true);` after `lcd.begin();` |

### Colors look wrong / inverted

Add `lcd.invertDisplay(true);` after `lcd.begin();`

### Display shows garbage / noise

- Check SPI wiring (MOSI and SCK especially)
- Reduce SPI speed to 8 MHz
- Check that GND is connected

### Image is mirrored or rotated

Try different `lcd.setRotation()` values (0, 1, 2, 3).

### Touch coordinates are wrong

1. Run the 4-corner calibration in `Touch_Test.ino`
2. Check Serial output for raw ADC values (should be 200–3900 range)
3. If raw values are always 0 or 4095: check MISO (Pin 21→D12) and Touch CS (Pin 26→D4)

### Touch not responding at all

| Check | What to do |
|-------|------------|
| Touch CS | Verify Pin 26 → D4 |
| Touch IRQ | Verify Pin 11 → D5 |
| MISO | Verify Pin 21 → D12 |
| LCD CS conflict | The driver auto-deselects LCD CS when reading touch |

---

## Pin Reference Table

### Complete Arduino UNO Q Pin Usage

| UNO Q Pin | Used For      | Display Pin | Required? |
|-----------|---------------|-------------|-----------|
| **3.3V**  | Logic power   | Pin 1, 17   | YES       |
| **5V**    | Backlight/reg | Pin 2, 4    | YES       |
| **GND**   | Ground        | Pin 6       | YES       |
| **D2**    | LCD DC/RS     | Pin 18      | YES       |
| **D3**    | LCD Reset     | Pin 22      | YES       |
| **D4**    | Touch CS      | Pin 26      | For touch |
| **D5**    | Touch IRQ     | Pin 11      | For touch |
| **D10**   | LCD CS (SS)   | Pin 24      | YES       |
| **D11**   | SPI MOSI      | Pin 19      | YES       |
| **D12**   | SPI MISO      | Pin 21      | For touch |
| **D13**   | SPI SCK       | Pin 23      | YES       |

### Available UNO Q Pins (Not Used by Display)

These pins are free for other use in your project:

```
D0, D1 (Serial RX/TX)
D6, D7, D8, D9
A0, A1, A2, A3, A4, A5
```

---

## Library Files

```
UNOQ_MPI3501/
├── library.properties          Arduino library metadata
├── README.md                   This file
├── src/
│   ├── UNOQ_MPI3501.h         LCD driver header
│   ├── UNOQ_MPI3501.cpp       LCD driver implementation
│   ├── UNOQ_XPT2046.h         Touch driver header
│   └── UNOQ_XPT2046.cpp       Touch driver implementation
└── examples/
    ├── LCD_Test/
    │   └── LCD_Test.ino        Bare minimum LCD color test
    └── Touch_Test/
        └── Touch_Test.ino      Touch test with calibration
```

---

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.
