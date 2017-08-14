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
      changed |= all_devices_state[all_devices_num].addr != i;
      all_devices_state[all_devices_num].addr = i;
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
    if(all_devices_num == 0){
      delay(1000);
    }
  }

  // device(s) connected
  else{
  const char TABLE_LINE[] PROGMEM = {"|-----------------------------|"};
    
    Serial.println(F("_____ All Devices found: ______"));
    Serial.println(F("|  Address  | Def Settings OK |"));
    Serial.println(TABLE_LINE);

    for(uint8_t i = 0; i < all_devices_num; i++){
      Serial.print("| ");
      SerialPrintFillLeft(String(all_devices_state[i].address), 10);
      Serial.print("| ");
      SerialPrintFillLeft(all_devices_state[i].isOK ? "OK" : "NOT OK", 16);
      Serial.println("|");
    }
    Serial.println(TABLE_LINE);
    
    if(all_devices_num == 1){
      // Only one device => connect directly
      device_addr = all_devices_addr[0];
      Serial.print(F("Device found at addr: "));
      Serial.println(device_addr);
    }else{
      // Multiple devices => Enable list mode
      for(uint8_t i = 0; i < all_devices_num; i++){
        if(!checkDeviceAvailable(all_devices_addr[i])){
          Serial.println(F("Connection lost to at least one device"));
          Serial.println();
          all_devices_num = 0;
          break;
        }
      }
      if(all_devices_num > 0){
        if(!device_list_displayed){
          device_list_displayed = true;
          Serial.print(F("Multiple devices found: "));
          Serial.println(all_devices_num);
          Serial.println(F("On Addresses:"));
          for(uint8_t i = 0; i < all_devices_num; i++){
            if(i != 0){
              Serial.print(',');
            }
            Serial.print(all_devices_addr[i]);
          }
          Serial.println();
          Serial.println();
          Serial.println(F("Waiting for input... (h for help)"));
        }
        
        readCommand(false);
        delay(1);
      }
    }
  }
}


void readCommand(bool single){
  while (Serial.available()) {
      char c = Serial.read();
      if(c == '\n'){
        line = true;
        break;
      }
      commandIn += c;
  }
  if(line){
    line = false;
    Serial.println();
    Serial.print(">  ");
    Serial.println(commandIn);

    if(! (single ? parseSingleCommand(commandIn) : parseListCommand(commandIn))){
      Serial.println(F("Unknown command"));
      Serial.println(F("type h for help"));
    }
    
    commandIn = "";
  }
}


