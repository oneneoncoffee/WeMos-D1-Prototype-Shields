// Arduino Board: WeMos D1 
// Weather Station prototype Shield sketch
// Library used with shield: BMP180I2C.h/BMP180MI, Arduino.h, Wire.h, Adafruit_bmp280.h
// WeMos D1 chip ESP8266 (MASTER) 
// D3 ------ SCL 
// D4 ------ SDA
//
// Arduino prototype shield 5.0
// 1 bmp180 Sensor (SLAVE) 
// I2C address : 0x77
// 5V ------ VCC
// GND ----- GND
// SDA ----- SDA
// SCL ----- SCL
// 1 bmp280 Ssensor (SLAVE) 
// I2C address : 0x76 
// 5V ------ VCC
// GND ----- GND
// SDA ----- SDA
// SCL ----- SDA 

#include <Arduino.h>
#include <Wire.h>
#include <BMP180I2C.h>
#include <Adafruit_BMP280.h>
#define I2C_ADDRESS 0x77
#define I2C_ADDRESS2 0x76 
BMP180I2C bmp180(I2C_ADDRESS); 
Adafruit_BMP280 bmp280; 

void setup() {
Serial.begin(9600);

delay(5000);
Serial.println("Starting Serial to Wire interface..");  
while(!Serial); 
Wire.begin(); 
Serial.println("Initialize Starting, check sensor:");
if(!bmp180.begin()) {
  Serial.println("Error: check fail "); 
  while (1);  
}
bmp180.resetToDefaults(); 
bmp180.setSamplingMode(BMP180MI::MODE_UHR); 
if(!bmp280.begin(0x76)) {
   Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
   while (1); 
}

  /* Default settings from datasheet. */
  bmp280.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
Serial.println("Sensor BMP180 Active.\nSensor BMP280 Active."); 
}

void loop() {
  delay(1000);
  if (!bmp180.measureTemperature()){
    Serial.println("could not start temperature measurement.");
    return;
  }
  do
  {
    delay(100);
  } while (!bmp180.hasValue());

  Serial.print("Temperature: "); 
  Serial.print(bmp180.getTemperature()); 
  Serial.println(" degC");
  if (!bmp180.measurePressure()){
    Serial.println("could not start perssure measurement..");
    return;
  }
 do
  {
    delay(100);
  } while (!bmp180.hasValue());

    Serial.print("Pressure: "); 
    Serial.print(bmp180.getPressure());
    Serial.println(" Pa");
    Serial.print(F("Temperature = "));
    Serial.print(bmp280.readTemperature());
    Serial.println(" *C");
    Serial.print(F("Pressure = "));
    Serial.print(bmp280.readPressure());
    Serial.println(" Pa");
    Serial.print(F("Approx altitude = "));
    Serial.print(bmp280.readAltitude(3658));
    Serial.println(" m");

    Serial.println();
    delay(2000);  
byte error, address;
int nDevices;
 
  Serial.println("Scanning...");
 
  nDevices = 0;
  for(address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("I2C device found, Address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX);
      Serial.println("");
 
      nDevices++;
    }
    else if (error==4)
    {
      Serial.print("Unknown error at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.println(address,HEX);
    }    
  }
  if (nDevices == 0)
    Serial.println("No I2C devices found\n");
  else
    Serial.println("\n");
}
