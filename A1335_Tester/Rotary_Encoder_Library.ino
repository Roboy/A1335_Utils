

bool readMemory(uint8_t deviceaddress, uint8_t eeaddress, byte* rdata, uint8_t num)
{
  Wire.beginTransmission(deviceaddress);
  Wire.write(eeaddress);
  if(Wire.endTransmission(false) != 0){ // Restart Line for request
    Wire.endTransmission(true);
    return false;
  }
  
  Wire.requestFrom(deviceaddress, num, (uint8_t) true);
  for(uint8_t i = 0; i < num; i++){
    rdata[i] = Wire.read();
  }
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

bool writeMemoryCheck(uint8_t deviceaddress, uint8_t eeaddress, byte* wdata)
{
  if(!writeMemory(deviceaddress, eeaddress, wdata)){
    return false;
  }
  byte rdata[2];
  delay(30);
  if(!readMemory(deviceaddress, eeaddress, rdata, 2)){
    return false;
  }
  return (rdata[0] == wdata[0] && rdata[1] == wdata[1]);
}







bool checkDefaultSettings(A1335State* state){
  for(uint8_t i = 0; i < 8; i++){
    if((state->rawData[i>>1] & expected_registers_mask[i][0]) != expected_registers[i][0])
      return false;
    if((state->rawData[i>>1|1] & expected_registers_mask[i][1]) != expected_registers[i][1])
      return false;
  }
  return true;
}


bool readDeviceState(uint8_t deviceaddress, A1335State* state){
  
  if(!readMemory(deviceaddress, start_register, state->rawData, num_registers)){
    return false;
  }
  if(!readMemory(deviceaddress, start_register2, &state->rawData[num_registers], num_registers2)){
    return false;
  }

  state->isOK = checkDefaultSettings(state);
  
  state->angle = (float)((uint16_t)((state->rawData[0] & 0xf) << 8) | state->rawData[1]) * 360 / 4096 ;
  state->angle_flags = (state->rawData[0] >> 5) & 0b11;
  
  state->status_flags = state->rawData[2] & 0xf;
  
  state->err_flags = ((state->rawData[4] & 0xf) << 8) | state->rawData[5];
  state->xerr_flags = ((state->rawData[6] & 0xf) << 8) | state->rawData[7];
  
  state->temp = 
    (float)((uint16_t)((state->rawData[8] & 0xf) << 8) | state->rawData[9])
     / 8.0 - 273.145; // 8th Kelvin to °C
  state->fieldStrength = 
    (float)((uint16_t)((state->rawData[10] & 0xf) << 8) | state->rawData[11])
     / 10.0; // Gauss to milliTesla
  

  return true;
}


void SerialPrintFlags(uint16_t flags, char meanings[][FLAGS_STRLEN], uint8_t num){
  bool first = true;
  for(uint8_t i = 0; i < num; i++){
    if((flags >> i) & 1){
      if(!first){
        Serial.print(", ");
      }
      Serial.print(meanings[i]);
      first = false;
    }
  }
}

void SerialPrintAlignLeft(String s, uint16_t l){
  Serial.print(s);
  l -= s.length();
  for(; l > 0; l--){
    Serial.print(' ');
  }
}
