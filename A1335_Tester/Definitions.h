#include "Rotary_Encoder_Settings.h"

bool readMemory(uint8_t deviceaddress, uint8_t eeaddress, byte* rdata);
bool writeMemory(uint8_t deviceaddress, uint8_t eeaddress, byte* wdata);
bool writeMemoryCheck(uint8_t deviceaddress, uint8_t eeaddress, byte* wdata);

bool readDeviceSettings(uint8_t deviceaddress);
bool checkDefaultSettings();
bool writeDefaultSettings(uint8_t deviceaddress);
bool readDeviceStatus(uint8_t deviceaddress);
void displayDeviceProperties();
void displayDeviceStatus();
void displayDevicePropertiesTableHeader();
void displayDevicePropertiesTableRow(uint8_t deviceaddress);

void readCommand();
bool parseListCommand(String commandIn);
bool parseSingleCommand(String commandIn);

struct encoderState {
  bool data_valid;
  bool magnet_too_close;
  bool magnet_too_far;
  uint16_t abs_pos; // 12 bit
  uint16_t rel_pos; // relative to zeroed data
  uint16_t tacho; // 10 bit
  bool tacho_overflow;
  uint8_t AGC_gain; // 0 - 16 
};


// Number and list of all devices connected
const uint8_t all_devices_num_max = 50;
uint8_t all_devices_addr[all_devices_num_max];
uint8_t all_devices_num = 0;
bool device_list_displayed = false;

// Address of currently connected device
uint8_t device_addr = 255;
bool device_settings_read = false;
byte device_settings[4][2];
struct encoderState device_status;


String commandIn = "";
bool line = false;
