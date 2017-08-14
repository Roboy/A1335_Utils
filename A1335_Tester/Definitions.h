#include "Rotary_Encoder_Settings.h"
#define FS(x) (__FlashStringHelper*)(x)

bool readMemory(uint8_t deviceaddress, uint8_t eeaddress, byte* rdata, uint8_t num);
bool writeMemory(uint8_t deviceaddress, uint8_t eeaddress, byte* wdata);
bool writeMemoryCheck(uint8_t deviceaddress, uint8_t eeaddress, byte* wdata);



struct A1335State {
  uint8_t address;
  bool isOK;
  float angle; // in deg
  uint8_t angle_flags : 2; // error, new
  uint8_t status_flags : 4;
  uint16_t err_flags : 12;
  uint16_t xerr_flags : 12;
  float temp; // in °C
  float fieldStrength; // in mT

  byte rawData[8][2];
};

const uint8_t FLAGS_STRLEN = 10;


bool readDeviceState(uint8_t deviceaddress, A1335State* state);
bool checkDefaultSettings(A1335State* state);

void SerialPrintFlags(uint16_t flags, char meanings[][FLAGS_STRLEN], uint8_t num);
void SerialPrintAlignLeft(String s, uint16_t l);




// Number and list of all devices connected
const uint8_t all_devices_num_max = 50;
A1335State all_devices_state[all_devices_num_max];
uint8_t all_devices_num = 0;




const char ANGLE_FLAGS[][FLAGS_STRLEN] = {
  "NEW",
  "ERR"
};

const char STATUS_FLAGS[][FLAGS_STRLEN] = {
  "ERR",
  "NEW",
  "Soft_Rst",
  "PwON_Rst"
};

const char ERROR_FLAGS[][FLAGS_STRLEN] = {
  "MagLow",
  "MagHigh",
  "UnderVolt",
  "OverVolt",
  "AngleLow",
  "AngleHigh",
  "ProcError",
  "NoRunMode",
  "(CRC_Err)",
  "(SPI_Err)",
  "(XOV)",
  "XERR"
};




//String commandIn = "";
//bool line = false;
