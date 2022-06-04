#include <Arduino.h>
const uint8_t seedPin = A0; 
 
void setup() {
Serial.begin(9600);     
randomSeed(generateRandomSeed()); 
}

void loop() {
for (int y = 0; y < 5; y++) {  
  randomSeed(generateRandomSeed()); 
  for (int x = 0; x < 5; x++) {
  int Value = random(generateRandomSeed()); 
    Serial.println(Value); 
  }
}  
}

uint32_t generateRandomSeed() {
  uint8_t seedBitValue = 0; 
  uint8_t seedByteValue = 0; 
  uint32_t seedWordValue = 0; 
int randomNumber; 
  randomNumber = random(0, 3); 
  switch(randomNumber) { 
  case 0:   
 // Serial.print("Case 0:");   
  for(uint8_t wordShift = 0; wordShift < 4; wordShift++) {
   for(uint8_t byteShift = 0; byteShift < 8; byteShift++) {
    for(uint8_t bitSum = 0; bitSum <= 8; bitSum++)  {
    seedBitValue = seedBitValue + (analogRead(seedPin) & 0x01); // default seed pin is A0   
    }
    delay(2); 
    seedByteValue = seedByteValue | ((seedBitValue & 0x01) << byteShift); 
    seedBitValue = 0; 
   }
   seedWordValue = seedWordValue | (uint32_t)seedByteValue << (8*wordShift);
   seedByteValue = 0; 
  }
  break;
case 1:
//  Serial.print("Case 1:"); 
  for(uint8_t wordShift = 0; wordShift < 4; wordShift++) {
   for(uint8_t byteShift = 0; byteShift < 8; byteShift++) {
    for(uint8_t bitSum = 0; bitSum <= 8; bitSum++)  {
    seedBitValue = seedBitValue + (analogRead(seedPin)^2 & 0x01); // default seed pin is A0   
    }
    delay(2); 
    seedByteValue = seedByteValue | ((seedBitValue & 0x01) << byteShift); 
    seedBitValue = 0; 
   }
   seedWordValue = seedWordValue | (uint32_t)seedByteValue << (8*wordShift);
   seedByteValue = 0; 
  }
  break;
case 2:
//   Serial.print("case 2:");
   for(uint8_t wordShift = 0; wordShift < 4; wordShift++) {
   for(uint8_t byteShift = 0; byteShift < 8; byteShift++) {
    for(uint8_t bitSum = 0; bitSum <= 8; bitSum++)  {
    seedBitValue = seedBitValue + (analogRead(seedPin)^3 & 0x01); // default seed pin is A0   
    }
    delay(2); 
    seedByteValue = seedByteValue | ((seedBitValue & 0x01) << byteShift); 
    seedBitValue = 0; 
   }
   seedWordValue = seedWordValue | (uint32_t)seedByteValue << (8*wordShift);
   seedByteValue = 0; 
  }
  break;
  Default: Serial.println(" "); 
  break;
  }  
 return (seedWordValue);  
 Serial.println(" "); 
}
