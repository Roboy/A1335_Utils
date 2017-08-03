#include "Rotary_Encoder_Settings.h"

bool readMemory(uint8_t deviceaddress, uint8_t eeaddress, byte* rdata, uint8_t num);
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

struct A1335State {
  uint8_t address;
  bool isOK;
  uint16_t angle : 12;
  uint8_t angle_flags : 2; // error, new
  uint8_t status_flags : 4;
  uint16_t err_flags : 12;
  uint16_t xerr_flags : 12;
  float temp; // in °C
  float fieldStrength; // in mT

  byte rawData[16];
};


// Number and list of all devices connected
const uint8_t all_devices_num_max = 50;
A1335State all_devices_state[all_devices_num_max];
uint8_t all_devices_num = 0;



//String commandIn = "";
//bool line = false;
