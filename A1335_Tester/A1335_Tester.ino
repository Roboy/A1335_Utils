#include <Wire.h>
#include <avr/wdt.h>
#include "Definitions.h"

#define LED_PIN 53

void setup() {
  
  Wire.begin();
  Wire.setClock(400000);
  Serial.begin(115200);

  pinMode(LED_PIN, OUTPUT);

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
  uint8_t all_devices_num_old = all_devices_num;
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
  return changed || all_devices_num != all_devices_num_old;
}



/* Modes:
 * Not connected -> Will keep searching
 * Multi connected -> Will show list and check list until one of them disconnects
 * Single connected -> Will show single until disconnect
 */


uint16_t ledTime;
void loop() {
  wdt_reset();

  ledTime = 0;
  digitalWrite(LED_PIN, HIGH);

  // No device connected
  if(all_devices_num == 0){
    Serial.println(F("Scanning..."));
  }
  bool changed = searchAddressSpace();

  // device(s) connected
  if(all_devices_num > 0){
    const char TABLE_LINE[] PROGMEM = {"|-----------------------------|"};


    if(changed){
      // Print overview of all devices
      Serial.println();
      Serial.println(F("_______ Devices found: ________"));
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
    }


    bool allOK = true;
    for(uint8_t i = 0; i < all_devices_num; i++){
      A1335State s = all_devices_state[i];
      allOK &= s.isOK;

      // Pretty-print each device state
      if(changed){
        Serial.print(F("==== Sensor: "));
        Serial.print(s.address);
        Serial.print(F(" ==== ("));
        Serial.print(s.isOK ? "OK" : "NOT OK");
        Serial.println(F(") ===="));
        
        Serial.print(F("    Angle:  "));
        Serial.print(s.angle);
        Serial.print(F("° ("));
        SerialPrintFlags(s.angle_flags, ANGLE_FLAGS, 2);
        Serial.println(F(")"));

        Serial.print(F("   Status:  "));
        SerialPrintFlags(s.status_flags, STATUS_FLAGS, 4);
        Serial.println();
        
        Serial.print(F("   Errors:  "));
        SerialPrintFlags(s.err_flags, ERROR_FLAGS, 2);
        Serial.println();

        Serial.print(F("     Temp:  "));
        Serial.print(s.temp);
        Serial.println(F("°C"));

        Serial.print(F("    Field:  "));
        Serial.print(s.fieldStrength);
        Serial.println(F("mT"));

        
        Serial.println();
        Serial.println(F("Raw Register Values:"));
        char buf[30];
        for(uint8_t i=0; i < 8; i++){
          // Register numbers
          uint16_t reg = (i<<1) < num_registers
                  ? start_register +(i<<1)
                  : start_register2 +(i<<1)-num_registers; // (second half)
          sprintf(buf, "0x%02x: %02x %02x", reg, s.rawData[i<<1], s.rawData[i<<1 +1]);
          Serial.println(buf);
        }
        
        Serial.println();
        Serial.println();
      }
    }

    ledTime = allOK ? 1000 : 500;
  }

  
  delay(ledTime);
  digitalWrite(LED_PIN, LOW);
  delay(1000-ledTime);
}



