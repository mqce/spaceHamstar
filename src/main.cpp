#include "HID.h"

static const uint8_t _hidReportDescriptor[] PROGMEM = {
  0x05, 0x01,           //  Usage Page (Generic Desktop)
  0x09, 0x08,           //  0x08: Usage (Multi-Axis)
  0xa1, 0x01,           //  Collection (Application)
  0xa1, 0x00,           // Collection (Physical)
  0x85, 0x01,           //  Report ID
  0x16, 0x00, 0x80,     //logical minimum (-500)
  0x26, 0xff, 0x7f,     //logical maximum (500)
  0x36, 0x00, 0x80,     //Physical Minimum (-32768)
  0x46, 0xff, 0x7f,     //Physical Maximum (32767)
  0x09, 0x30,           //    Usage (X)
  0x09, 0x31,           //    Usage (Y)
  0x09, 0x32,           //    Usage (Z)
  0x75, 0x10,           //    Report Size (16)
  0x95, 0x03,           //    Report Count (3)
  0x81, 0x02,           //    Input (variable,absolute)
  0xC0,                 //  End Collection
  0xa1, 0x00,           // Collection (Physical)
  0x85, 0x02,           //  Report ID
  0x16, 0x00, 0x80,     //logical minimum (-500)
  0x26, 0xff, 0x7f,     //logical maximum (500)
  0x36, 0x00, 0x80,     //Physical Minimum (-32768)
  0x46, 0xff, 0x7f,     //Physical Maximum (32767)
  0x09, 0x33,           //    Usage (RX)
  0x09, 0x34,           //    Usage (RY)
  0x09, 0x35,           //    Usage (RZ)
  0x75, 0x10,           //    Report Size (16)
  0x95, 0x03,           //    Report Count (3)
  0x81, 0x02,           //    Input (variable,absolute)
  0xC0,                 //  End Collection

  0xa1, 0x00,           // Collection (Physical)
  0x85, 0x03,           //  Report ID
  0x15, 0x00,           //   Logical Minimum (0)
  0x25, 0x01,           //    Logical Maximum (1)
  0x75, 0x01,           //    Report Size (1)
  0x95, 32,             //    Report Count (24)
  0x05, 0x09,           //    Usage Page (Button)
  0x19, 1,              //    Usage Minimum (Button #1)
  0x29, 32,             //    Usage Maximum (Button #24)
  0x81, 0x02,           //    Input (variable,absolute)
  0xC0,
  0xC0
};

#define NUM_AXES 6

// Axis indices for motion vector array
#define AXIS_TX 0
#define AXIS_TY 1
#define AXIS_TZ 2
#define AXIS_RY 3
#define AXIS_RX 4
#define AXIS_RZ 5

/// hardware 
#define DEAD_THRESH 10 // Deadzone for ignoring small movement
#define TRANSLATION_SPEED 10 // Sensitivity for translation (higher is faster)
#define ROTATION_SPEED    10 // Sensitivity for rotation (higher is faster)

// Joystick A ports (for Pan operation: translation X, translation Y)
#define PIN_TX A0  // translation X
#define PIN_TY A1  // translation Y
#define PIN_RX A8  // rotation X
#define PIN_RY A9  // rotation Y

void setup() {
  static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
  HID().AppendDescriptor(&node);
}

void send_command(int16_t rx, int16_t ry, int16_t rz, 
                  int16_t tx, int16_t ty, int16_t tz) {
  uint8_t translation_data[6] = { 
    tx & 0xFF, tx >> 8,
    tz & 0xFF, tz >> 8,
    ty & 0xFF, ty >> 8
  };
  HID().SendReport(1, translation_data, 6);
  
  uint8_t rotation_data[6] = {
    ry & 0xFF, ry >> 8,
    rz & 0xFF, rz >> 8,
    rx & 0xFF, rx >> 8
  };
  HID().SendReport(2, rotation_data, 6);
}

int process(int raw_value, int speed) {
  raw_value = raw_value - 512;

  // Apply speed (integrated multiplier where higher is faster)
  int processed_value = (long)raw_value * speed / 100;
  
  // Apply deadzone
  if((processed_value > -DEAD_THRESH) && (processed_value < DEAD_THRESH)){
    processed_value = 0;
  }
  // Limit to range [-500, 500]
  else if(processed_value > 500) {
    processed_value = 500;
  }
  else if(processed_value < -500) {
    processed_value = -500;
  }
  
  return processed_value;
}

void loop() {
  int tx = process(analogRead(PIN_TX), TRANSLATION_SPEED);
  int ty = process(analogRead(PIN_TY), TRANSLATION_SPEED);

  int rx = process(analogRead(PIN_RX), ROTATION_SPEED);
  int ry = process(analogRead(PIN_RY), ROTATION_SPEED);

  // Serial.print(    "rx: " + String(analogRead(PIN_RX)) + ", ry: " + String(analogRead(PIN_RY)));
  // Serial.println(", tx: " + String(analogRead(PIN_TX)) + ", ty: " + String(analogRead(PIN_TY)));
  
  // Calculate rotation X and Y
  bool has_movement = (tx != 0 || ty != 0 || rx != 0 || ry != 0);

  if(has_movement){
    send_command(
      -rx, 
      ry, 
      0, 
      tx, 
      ty, 
      0
    );
  }
}