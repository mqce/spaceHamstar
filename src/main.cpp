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
#define AXIS_TRANSLATION_X 0
#define AXIS_TRANSLATION_Y 1
#define AXIS_TRANSLATION_Z 2
#define AXIS_ROTATION_Y 3
#define AXIS_ROTATION_X 4
#define AXIS_ROTATION_Z 5

/// hardware 
#define DEAD_THRESH 2 // Deadzone for ignoring small movement
#define SPEED_PARAM 40 // larger is slower

// Joystick A ports (for 2D movement: translation X, translation Y)
#define JOYSTICK_A_X_PORT A0  // translation X
#define JOYSTICK_A_Y_PORT A1  // translation Y

int joystick_a_origin[2]; // initial sensor values for joystick A (x, y)

void setup() {
  static HIDSubDescriptor node(_hidReportDescriptor, sizeof(_hidReportDescriptor));
  HID().AppendDescriptor(&node);

  // Initialize origin values for joystick A (A0, A1)
  joystick_a_origin[0] = analogRead(JOYSTICK_A_X_PORT); // translation X
  joystick_a_origin[1] = analogRead(JOYSTICK_A_Y_PORT); // translation Y
}

void send_command(int16_t rotation_x, int16_t rotation_y, int16_t rotation_z, 
                  int16_t translation_x, int16_t translation_y, int16_t translation_z) {
  uint8_t translation_data[6] = { 
    translation_x & 0xFF, translation_x >> 8,
    translation_z & 0xFF, translation_z >> 8,
    translation_y & 0xFF, translation_y >> 8
  };
  HID().SendReport(1, translation_data, 6);
  
  uint8_t rotation_data[6] = { 
    rotation_x & 0xFF, rotation_x >> 8, 
    rotation_y & 0xFF, rotation_y >> 8, 
    rotation_z & 0xFF, rotation_z >> 8 
  };
  HID().SendReport(2, rotation_data, 6);
}

int apply_deadzone_and_limit(int raw_value) {
  int processed_value = raw_value / SPEED_PARAM;
  
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
  int motion_vector[NUM_AXES]; // (translation_x, translation_y, translation_z, rotation_y, rotation_x, rotation_z)
  
  // Read joystick A values and calculate translation X and Y
  int sensor_value_x = analogRead(JOYSTICK_A_X_PORT) - joystick_a_origin[0];
  int sensor_value_y = analogRead(JOYSTICK_A_Y_PORT) - joystick_a_origin[1];
  
  // Calculate translation X (movement in X direction)
  motion_vector[AXIS_TRANSLATION_X] = apply_deadzone_and_limit(sensor_value_x);
  
  // Calculate translation Y (movement in Y direction)
  motion_vector[AXIS_TRANSLATION_Y] = apply_deadzone_and_limit(sensor_value_y);
  
  // Set other axes to 0 (translation_z, rotation_x, rotation_y, rotation_z)
  motion_vector[AXIS_TRANSLATION_Z] = 0;
  motion_vector[AXIS_ROTATION_X] = 0;
  motion_vector[AXIS_ROTATION_Y] = 0;
  motion_vector[AXIS_ROTATION_Z] = 0;

  // Check if there's any movement
  bool has_movement = (motion_vector[AXIS_TRANSLATION_X] != 0 || 
                       motion_vector[AXIS_TRANSLATION_Y] != 0);

  if(has_movement){
    send_command(
      motion_vector[AXIS_ROTATION_X], 
      -motion_vector[AXIS_ROTATION_Y], 
      -motion_vector[AXIS_ROTATION_Z], 
      motion_vector[AXIS_TRANSLATION_X], 
      motion_vector[AXIS_TRANSLATION_Y], 
      motion_vector[AXIS_TRANSLATION_Z]
    );
  }
}