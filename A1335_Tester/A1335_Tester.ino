#include <Wire.h>
#include <avr/wdt.h>
#include "Definitions.h"



void setup() {
  
  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);

  // Reset the Arduino if it hangs for more than 2s
  wdt_enable(WDTO_2S);
  
  Serial.println();
  Serial.println(F("Reboot, please reconnect any devices (I2C might be blocked)"));
  Serial.println();
  Serial.println(F("Allegro A1335 Rotary Encoder Tester"));
  Serial.println(F("Daniel Schubert / Roboy (2017)"));
  Serial.println();
  Serial.println();
}


bool searchAddressSpace(){
  bool changed = false;
  all_devices_num = 0;
  for(uint8_t i=0; i < 128; i++){ // search all possible addresses
    if(readDeviceState(i, &all_devices_state[all_devices_num])){
      // If nothing changed, we will read the same addresses again
      changed |= all_devices_state[all_devices_num].address != i;
      all_devices_state[all_devices_num].address = i;
      all_devices_num++;
      if(all_devices_num >= all_devices_num_max)
        break;
    }
  }
  return changed;
}



/* Modes:
 * Not connected -> Will keep searching
 * Multi connected -> Will show list and check list until one of them disconnects
 * Single connected -> Will show single until disconnect
 */


void loop() {
  wdt_reset();

  // No device connected
  if(all_devices_num == 0){
    Serial.println(F("Scanning..."));
    searchAddressSpace();
  }

  // device(s) connected
  if(all_devices_num > 0){
    const char TABLE_LINE[] PROGMEM = {"|-----------------------------|"};


    // Print overview of all devices
    Serial.println(F("_____ All Devices found: ______"));
    Serial.println(F("|  Address  | Def Settings OK |"));
    Serial.println(TABLE_LINE);

    for(uint8_t i = 0; i < all_devices_num; i++){
      Serial.print("| ");
      SerialPrintAlignLeft(String(all_devices_state[i].address), 10);
      Serial.print("| ");
      SerialPrintAlignLeft(all_devices_state[i].isOK ? "OK" : "NOT OK", 16);
      Serial.println("|");
    }
    Serial.println(TABLE_LINE);
    Serial.println();
    Serial.println();


    // Pretty-print each device state
    for(uint8_t i = 0; i < all_devices_num; i++){
      A1335State s = all_devices_state[i];

      Serial.print(F("___Sensor:  "));
      Serial.print(s.address);
      Serial.print(F("________"));
      Serial.print(s.isOK ? "OK" : "NOT OK");
      Serial.println(F("____"));
      
      Serial.print(F("    Angle:  "));
      Serial.print(s.angle);
      Serial.print(F("° ("));
      SerialPrintFlags(s.angle_flags, ANGLE_FLAGS, 2);
      Serial.print(F(")"));
      
      Serial.println();
      Serial.println();
    }
  }

  
  delay(1000);
}



