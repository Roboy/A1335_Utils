#define FS(x) (__FlashStringHelper*)(x)


bool readMemory(uint8_t deviceaddress, uint8_t eeaddress, byte* rdata)
{
  // Address 0 hangs if read "manually" but we can read it by reading without a request
  if(eeaddress > 0){
    Wire.beginTransmission(deviceaddress);
    Wire.write(eeaddress);
    if(Wire.endTransmission(false) != 0){ // Restart Line for request
      Wire.endTransmission(true);
      return false;
    }
  }
  
  Wire.requestFrom(deviceaddress, (uint8_t) 2, (uint8_t) true);
  rdata[0] = Wire.read();
  rdata[1] = Wire.read();
  return true;
}

bool writeMemory(uint8_t deviceaddress, uint8_t eeaddress, byte* wdata)
{
  Wire.beginTransmission(deviceaddress);
  Wire.write(eeaddress); // LSB
  Wire.write(wdata, 2); // LSB
  if(Wire.endTransmission(true) != 0){
    return false;
  }
  return true;
}

bool writeMemoryCheck(uint8_t deviceaddress,uint8_t eeaddress, byte* wdata)
{
  if(!writeMemory(deviceaddress, eeaddress, wdata)){
    return false;
  }
  byte rdata[2];
  delay(30);
  if(!readMemory(deviceaddress, eeaddress, rdata)){
    return false;
  }
  return (rdata[0] == wdata[0] && rdata[1] == wdata[1]);
}



bool readDeviceSettings(uint8_t deviceaddress){
  byte r[2];
  for(uint8_t i = 0; i < 4; i++){
    if(!readMemory(deviceaddress, i, device_settings[i])){
      return false;
    }
  }
}

bool checkDefaultSettings(){
  for(uint8_t i = 0; i < 4; i++){
    if((device_settings[i][0] & default_settings_mask[i][0]) != default_settings[i][0])
      return false;
    if((device_settings[i][1] & default_settings_mask[i][1]) != default_settings[i][1])
      return false;
  }
  return true;
}
bool writeDefaultSettings(uint8_t deviceaddress){
  bool ok = true;
  for(uint8_t i = 0; i < 4; i++){
    ok = true;
    if((device_settings[i][0] & default_settings_mask[i][0]) != default_settings[i][0]){
      device_settings[i][0] = (device_settings[i][0] & ~default_settings_mask[i][0]) | default_settings[i][0];
      ok = false;
    }
    if((device_settings[i][1] & default_settings_mask[i][1]) != default_settings[i][1]){
      device_settings[i][1] = (device_settings[i][1] & ~default_settings_mask[i][1]) | default_settings[i][1];
      ok = false;
    }
    if(!ok){
      if(!writeMemoryCheck(deviceaddress, i, device_settings[i])){
        return false;
      }
    }
  }
  return true;
}

bool readDeviceStatus(uint8_t deviceaddress){
  byte r[2];
  
  if(!readMemory(deviceaddress, 32, r))
    return false;
  device_status.data_valid = ~((r[0] >> 7) & 0b1); // 0 = data valid
  device_status.rel_pos = ((r[0] & 0xf) << 8) | r[1];
  
  if(!readMemory(deviceaddress, 33, r))
    return false;
  device_status.data_valid = ~((r[0] >> 7) & 0b1); // 0 = data valid
  device_status.abs_pos = ((r[0] & 0xf) << 8) | r[1];
  
  if(!readMemory(deviceaddress, 34, r))
    return false;
  device_status.magnet_too_far = (r[0] >> 6) & 0b1;
  device_status.magnet_too_close = (r[0] >> 5) & 0b1;
  
  if(!readMemory(deviceaddress, 35, r))
    return false;
  device_status.AGC_gain = (r[0] >> 4) & 0xf;
  device_status.tacho_overflow = (r[0] >> 2) & 0b1;
  device_status.tacho = ((r[0] & 0b11) << 8) | r[1];

  return true;
}


const char DISPLAY_SEPARATOR[] PROGMEM = {"-------------------------------"};
void SerialPrintFillLeft(String s, uint16_t l){
  Serial.print(s);
  l -= s.length();
  for(; l > 0; l--){
    Serial.print(' ');
  }
}


void displayDevicePropertiesTableHeader(){
  Serial.println(F("________ All Devices Connected: ________"));
  Serial.println(F("| Address | Def Settings OK | Rotation |"));
  Serial.println(F("|--------------------------------------|"));
}
void displayDevicePropertiesTableRow(uint8_t deviceaddress){
  Serial.print("| ");
  SerialPrintFillLeft(String(deviceaddress), 8);
  Serial.print("| ");
  SerialPrintFillLeft(checkDefaultSettings() ? "OK" : "NOT OK", 16);
  Serial.print("| ");
  SerialPrintFillLeft((device_settings[1][0] & 0b10000) ? F("CCW") : F("CW"), 9);
  Serial.print("|");
  Serial.println();
}


void displayDeviceProperties(){
  Serial.println(FS(DISPLAY_SEPARATOR));
  SerialPrintFillLeft(F("Rotary Encoder on addr:"), 24);
  Serial.println(device_addr);
  SerialPrintFillLeft(F("Default settings:"), 24);
  Serial.println(checkDefaultSettings() ? "OK" : "NOT APPLIED");
  SerialPrintFillLeft(F("Positive Rotation:"), 24);
  Serial.println((device_settings[1][0] & 0b10000) ? F("Counter-Clockwise") : F("Clockwise"));
  SerialPrintFillLeft(F("Zero Value:"), 24);
  Serial.println((uint16_t) (((device_settings[1][0] & 0b1111) << 8) | device_settings[1][1]));
  Serial.println(FS(DISPLAY_SEPARATOR));
  Serial.println();
  Serial.flush();
}

void displayDeviceStatus(){
  Serial.println(FS(DISPLAY_SEPARATOR));
  SerialPrintFillLeft(F("Rotary Encoder status:"), 24);
  Serial.println(device_status.data_valid ? F("(valid)") : F("(invalid data)"));
  SerialPrintFillLeft(F("Magnet:"), 24);
  if(device_status.magnet_too_close || device_status.magnet_too_far){
    if(device_status.magnet_too_close)
      Serial.print(F("TOO CLOSE  "));
    if(device_status.magnet_too_far)
      Serial.print(F("TOO FAR  "));
    Serial.println("");
  }else{
    Serial.println("OK");
  }
  SerialPrintFillLeft(F("Current AGC gain:"), 24);
  Serial.println(device_status.AGC_gain);
  SerialPrintFillLeft(F("Rotation Rel:"), 24);
  Serial.println(device_status.rel_pos);
  SerialPrintFillLeft(F("Rotation Abs:"), 24);
  Serial.println(device_status.abs_pos);
  SerialPrintFillLeft(F("Tacho Value:"), 24);
  Serial.println(device_status.tacho);
  if(device_status.tacho_overflow)
    Serial.print(F(" (overflow)"));
  Serial.println(FS(DISPLAY_SEPARATOR));
  Serial.println();
  Serial.flush();
}

