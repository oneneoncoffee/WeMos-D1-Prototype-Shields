///
// 8x8 LED Matris Shield version
// WeatherStation D1 Mini project. 
// With AHT10 custom Sensor breakout shield.  
// Updates: 
// From Basic http server to https weird redirect bugfix.
// 
///

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServerSecure.h> // #include <ESP8266WebServer.h>
#include "SAT_data.h"
#include <umm_malloc/umm_malloc.h>
#include <umm_malloc/umm_heap_select.h> 
#include <ESP8266mDNS.h>
#include "MLEDScroll.h" 

#include <Arduino.h>
#include <Wire.h>
#include <BMP180I2C.h>
#include <SimpleDHT.h> 
#include <DHTStable.h>
#include <DHT.h> 
#include <Adafruit_AHT10.h> 

Adafruit_AHT10 aht; 
MLEDScroll matrix; 
int pinDHT11 = 2;
SimpleDHT11 dht11(pinDHT11);
DHTStable DHTx; 
#define I2C_ADDRESS 0x77
BMP180I2C bmp180(I2C_ADDRESS);
const char* ssid = STASSID;
const char* password = STAPSK;
const uint8_t seedPin = A0;  
BearSSL::ESP8266WebServerSecure server(443); //ESP8266WebServer server(80);
BearSSL::ServerSessions serverCache(5); 
DHT dht(2,DHT11); 
float Temperature; 
float Humidity; 
float bmp180Temp; 
float bmp180Pressure; 
float randoms; 
float randoms1;
float randoms2;
float randoms3;

void setup() {
Serial.begin(9600);
  Serial.println(" ");  
  Serial.print("Starting 8x8 LED Matris shield.."); 
  matrix.begin();
  Serial.println("Start [OK]..");
  matrix.flip=false; 
  
while (!Serial); 
Wire.begin();
pinMode(2, INPUT); 
dht.begin(); 
Serial.println("\n\n"); 
Serial.println("Running self tests...Searching.."); 
if (! aht.begin()) {
    Serial.println("Could not find AHT10 chipset.");
} else { Serial.println("Found AHT10 chipset."); } 

if (!bmp180.begin()) {
Serial.println("BMP180 Sensor check failed and/or not found.."); 
while (1);   
}
bmp180.resetToDefaults();
bmp180.setSamplingMode(BMP180MI::MODE_UHR);
byte error, address;
int nDevices;
nDevices=0;
 for(address = 1; address < 127; address++ )
  {
     Wire.beginTransmission(address);
    error = Wire.endTransmission();
 
    if (error == 0)
    {
      Serial.print("\nI2C device found at address 0x");
      if (address<16)
        Serial.print("0");
      Serial.print(address,HEX); 
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
  Serial.println("...done\n");
delay(5000);   
matrix.setIntensity(1);
matrix.message("Starting server in 5 seconds..  ");
while(matrix.scroll()!=SCROLL_ENDED) { delay(50); yield(); }
  letterBlink("5", 50);
  letterScroll("5",SCROLL_UP); 
  Serial.print("Starting network connection.");
  WiFi.begin(ssid, password); 
    // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(200);
    Serial.print(".");
  }
  letterBlink("4", 50);
  letterScroll("4",SCROLL_UP); 
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("Local IP address: ");
  Serial.println(WiFi.localIP());
  // Hope we get lucky?!?!? 
  configTime(3 * 3600, 0, "pool.ntp.org", "time.nist.gov");

    if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
  
  server.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));

  letterBlink("3", 50);
  letterScroll("3",SCROLL_UP); 
  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  Serial.printf("MAC ADDRESS:%s\n", WiFi.softAPmacAddress().c_str());
  letterBlink("2", 50);
  letterScroll("2",SCROLL_UP); 
  server.getServer().setCache(&serverCache);

  server.on("/", handle_OnConnect);
  server.onNotFound(handle_NotFound);
  server.on("/tempget", [](){
    server.send(200, "text/html", "<h1>D1 Mini Weather Server</h1><p>[Serial output] Sensor Triggered<br>Sensors Triggered by this action are DHT11 & bmp180. DHT11 is active on digital pin 2 of D1 mini mainboard. If you are reading this the server is active and running without errors. </p>");
    tempget(); 
    delay(1000);
  });
  letterBlink("1", 50);
  letterScroll("1",SCROLL_UP); 

  server.on("/gettemp", gettemp_OnConnect); 
  server.on("/random", OnConnect_random); 
  server.on("/help", OnConnect_help);    
  server.begin();
  Serial.println("HTTP server started");
  tempget(); 
  letterBlink("0", 50);
  letterScroll("0",SCROLL_UP); 
  Serial.println("Serial debugger enabled.\n[Type ? + Enter for Serial help menu.]"); 
}

void loop() {
  matrix.setIntensity(5); 
  matrix.message("Server now running..  "); 
  while (matrix.scroll()!=SCROLL_ENDED) {
  server.handleClient();
  MDNS.update();
  if (Serial.available()) {
    int hotKey = Serial.read();
    processKey(Serial, hotKey);
  }  
  yield(); 
  }
}


void handle_OnConnect() {
 bmp180Temp = bmp180.getTemperature();
 bmp180Pressure = bmp180.getPressure(); 
 Temperature = dht.readTemperature(); // Gets the values of the temperature
 Humidity = dht.readHumidity(); // Gets the values of the humidity 
  server.send(200, "text/html", SendHTML(bmp180Pressure,bmp180Temp,Temperature,Humidity)); 
}

void OnConnect_help() {
  server.send(200, "text/html", sendHELP()); 
}

int OnConnect_random() {
  randomSeed(generateRandomSeed()); 
  int Value = random(generateRandomSeed()); 
  int Value1 = random(generateRandomSeed());
  int Value2 = random(generateRandomSeed());
  int Value3 = random(generateRandomSeed());
  Serial.println(Value); 
  randoms = Value;
  randomSeed(generateRandomSeed());
  randoms1 = Value1;
  randomSeed(generateRandomSeed());
  randoms2 = Value2;
  randomSeed(generateRandomSeed());
  randoms3 = Value3;  
  server.send(200, "text/html", SendRandom(randoms,randoms1,randoms2,randoms3)); 
}

uint32_t generateRandomSeed() {
  uint8_t seedBitValue = 0; 
  uint8_t seedByteValue = 0; 
  uint32_t seedWordValue = 0; 
  for(uint8_t wordShift = 0; wordShift < 4; wordShift++) {
   for(uint8_t byteShift = 0; byteShift < 8; byteShift++) {
    for(uint8_t bitSum = 0; bitSum <= 8; bitSum++)  {
    seedBitValue = seedBitValue + (analogRead(seedPin) & 0x01);   
    }
    delay(2); 
    seedByteValue = seedByteValue | ((seedBitValue & 0x01) << byteShift); 
    seedBitValue = 0; 
   }
   seedWordValue = seedWordValue | (uint32_t)seedByteValue << (8*wordShift);
   seedByteValue = 0; 
  }
  
 return (seedWordValue);  
}

void gettemp_OnConnect() {
 bmp180Temp = bmp180.getTemperature();
 bmp180Pressure = bmp180.getPressure(); 
 Temperature = dht.readTemperature(); // Gets the values of the temperature
 Humidity = dht.readHumidity(); // Gets the values of the humidity 
 server.send(200, "text/html", SendHTML2(bmp180Pressure,bmp180Temp,Temperature,Humidity)); 
}


void handle_NotFound(){
  server.send(404, "text/plain", "I am sorry your request is not found...");
}
String sendHELP() {
String ptr ="<!DOCTYPE html>"; 
       ptr +="<html><head>";
       ptr +="<html>";
       ptr +="<head>";
       ptr +="<title>Quick help server commands list</title>"; 
       ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
       ptr +="body{margin-top: 50px; bgcolor: black; background:black;} ";
       ptr +="h1 {color: blue;margin: 50px auto 30px;}\n";
       ptr +="p {font-size: 24px; color: #444444; margin-left: 10px; margin-right: 10px; margin-bottom: 10px;}\n";
       ptr +="table {";
       ptr +="    border-collapse:separate;";
       ptr +="    border:solid white 5px;";
       ptr +="    border-radius:10px;";
       ptr +="    width: 100%;";
       ptr +="}";
       ptr +="td, th {";
       ptr +="    border-left:solid black 1px;";
       ptr +="    border-top:solid black 1px;";
       ptr +="    bg_color: white; background: white;"; 
       ptr +="}";
       ptr +="";
       ptr +="th {";
       ptr +="    background-color: blue;";
       ptr +="    border-top: none;";
       ptr +="}"; 
       ptr +="";
       ptr +="td:first-child, th:first-child {";  
       ptr +="     border-left: none;";
       ptr +="} ";  
       ptr +=".h3 { font-size: 24px; font-color: white; color:white}";
       ptr +=".tg  {border-collapse:collapse;border-color:#C44D58;border-spacing:0;}";
       ptr +=".tg td{background-color:#F9CDAD;border-color:#C44D58;border-style:solid;border-width:1px;color:#002b36;";
       ptr +="font-family:Arial, sans-serif;font-size:14px;overflow:hidden;padding:10px 5px;word-break:normal;}";
       ptr +=".tg th{background-color:#FE4365;border-color:#C44D58;border-style:solid;border-width:1px;color:#fdf6e3;";
       ptr +="font-family:Arial, sans-serif;font-size:14px;font-weight:normal;overflow:hidden;padding:10px 5px;word-break:normal;}";
       ptr +=".tg .tg-zv4m{border-color:#ffffff;text-align:left;vertical-align:top}";
       ptr +=".tg .tg-baqh{text-align:center;vertical-align:top}\n";
       ptr +="</style>\n";
       ptr +="</head>\n";
       ptr +="<body>\n"; 
       ptr +="<table class='tg'>";
       ptr +="<thead>";
       ptr +="<tr>";
       ptr +="<th class='tg-baqh'><H3>D1 mini server table of contents</H3></th>";
       ptr +="</tr>";
       ptr +="</thead>";
       ptr +="<tbody>";
       ptr +=" <tr>";
       ptr +="<td class='tg-zv4m'>";
       ptr +="<p>Sensors attached to the D1 are DHT11 and BMP180. Note: this is not a portable network and is used with a basic home WiFi network.";
       ptr +="Programming by Johnny B Stroud 2022 Public free and open source."; 
       ptr +="</p>";
       ptr +="<ol>";
       ptr +="<li> /help  [You are here]</li>";
       ptr +="<li>/  [The main index site]</li>";
       ptr +="<li>/tempget [Serial refresh]</li>";
       ptr +="<li>/gettemp [Serial refresh / display temps]</li>";
       ptr +="<li>/random  [Display random numbers and lucky numbers]</li>";
       ptr +="</ol>";
       ptr +="</td>";
       ptr +="</tr>"; 
       ptr +="</tbody></table>";
       ptr +="</body></html>";       
return ptr;   
}


String  SendRandom(float randoms, float randoms1, float randoms2, float randoms3) {
String ptr = "<!DOCTYPE html>";
       ptr +="<html>";
       ptr +="<head>";
       ptr +="<title>Random super lucky numbers</title>"; 
       ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
       ptr +="body{margin-top: 50px; bgcolor: black; background:black;} ";
       ptr +="h1 {color: blue;margin: 50px auto 30px;}\n";
       ptr +="p {font-size: 24px; color: #444444; margin-left: 10px; margin-right: 10px; margin-bottom: 10px;}\n";
       ptr +="table {";
       ptr +="    border-collapse:separate;";
       ptr +="    border:solid white 5px;";
       ptr +="    border-radius:6px;";
       ptr +="}";
       ptr +="td, th {";
       ptr +="    border-left:solid black 1px;";
       ptr +="    border-top:solid black 1px;";
       ptr +="    bg_color: white; background: white;"; 
       ptr +="}";
       ptr +="";
       ptr +="th {";
       ptr +="    background-color: blue;";
       ptr +="    border-top: none;";
       ptr +="}"; 
       ptr +="";
       ptr +="td:first-child, th:first-child {";  
       ptr +="     border-left: none;";
       ptr +="} ";  
       ptr +="</style>\n";
       ptr +="</head>\n";
       ptr +="<body>\n";
       ptr +="<table><tr><td><div id=\"webpage\">\n";
       ptr +="<h1>Todays random Numbers</h1>\n";
       ptr +="<p><font color='green'>Todays Lucky Number:</font>\n";
       ptr +=(int)randoms;
       ptr +="<BR/>\n"; 
       ptr +="<font color='red'>Random number sets:</font>"; 
       ptr +=(int)randoms1;
       ptr +="\t"; 
       ptr +=(int)randoms2;
       ptr +="\t";
       ptr +=(int)randoms3;
       ptr +="<BR/>\n"; 
  ptr +="<font color='gray'></font> <br/></p>";
  ptr +="</div></t></td></table>\n";
       ptr +="</body></html>\n";
       return ptr; 
}

String SendHTML(float bmp180Pressure,float bmp180Temp,float Temperaturestat,float Humiditystat){
  String ptr = "<!DOCTYPE html> <html>\n";
  ptr +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr +="<title>D1 mini - Weather Report</title>\n";
  ptr +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr +="body{margin-top: 50px; bgcolor: black; background:black;} ";
  ptr +="h1 {color: blue;margin: 50px auto 30px;}\n";
  ptr +="p {font-size: 24px; color: #444444; margin-left: 10px; margin-right: 10px; margin-bottom: 10px;}\n";
  ptr +="table {";
  ptr +="    border-collapse:separate;";
  ptr +="    border:solid white 5px;";
  ptr +="    border-radius:6px;";
  ptr +="}";
  ptr +="td, th {";
  ptr +="    border-left:solid black 1px;";
  ptr +="    border-top:solid black 1px;";
  ptr +="    bg_color: white; background: white;"; 
  ptr +="}";
  ptr +="";
  ptr +="th {";
  ptr +="    background-color: blue;";
  ptr +="    border-top: none;";
  ptr +="}";
  ptr +="";
  ptr +="td:first-child, th:first-child {";
  ptr +="     border-left: none;";
  ptr +="} ";
  ptr +="</style>\n";
  ptr +="</head>\n";
  ptr +="<body>\n";
  ptr +="<table><tr><td><div id=\"webpage\">\n";
  ptr +="<h1>Weather Report</h1>\n";
  ptr +="<p><font color='green'>Temperature:</font> ";
  ptr +=(int)Temperaturestat;
  ptr +="<font color='gray'> C / </font>"; 
  ptr +=""; 
  ptr +=(int)bmp180Temp;
  ptr +="<font color='gray'> C</font><br/>"; 
  ptr +="<font color='green'>Pressure:</font> "; 
  ptr +=(int)bmp180Pressure; 
  ptr +="<font color='gray'> Ph <br/></font>";
  ptr +="<font color='green'>Humidity: </font>";
  ptr +=(int)Humiditystat;
  ptr +="<font color='gray'> %</font> <br/></p>";
  ptr +="</div></t></td></table>\n";
  ptr +="</body>\n";
  ptr +="</html>\n";
  return ptr;
}

String SendHTML2(float bmp180Pressure,float bmp180Temp,float Temperaturestat,float Humiditystat){
  String ptr1 = "<!DOCTYPE html> <html>\n";
  ptr1 +="<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0, user-scalable=no\">\n";
  ptr1 +="<title>D1 mini - Weather Report</title>\n";
  ptr1 +="<style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}\n";
  ptr1 +="body{margin-top: 50px; bgcolor: black; background:black;} ";
  ptr1 +="h1 {color: blue;margin: 50px auto 30px;}\n";
  ptr1 +="p {font-size: 24px; color: #444444; margin-left: 10px; margin-right: 10px; margin-bottom: 10px;}\n";
  ptr1 +="subtitle { font-color:red; color: red; margin: 40px auto 40px; }";
  ptr1 +="table {";
  ptr1 +="   border-collapse:separate;";
  ptr1 +="   border:solid white 6px;";
  ptr1 +="   border-radius:6px;";
  ptr1 +="}";
  ptr1 +="td, th {";
  ptr1 +="    border-left:solid black 1px;";
  ptr1 +="    border-top:solid black 1px;";
  ptr1 +="    background: white;";
  ptr1 +="}";
  ptr1 +="";
  ptr1 +="th {";
  ptr1 +="    background-color: blue;";
  ptr1 +="    border-top: none;";
  ptr1 +="}";
  ptr1 +="";
  ptr1 +="td:first-child, th:first-child {";
  ptr1 +="     border-left: none;";
  ptr1 +="} ";
  ptr1 +="</style>\n";
  ptr1 +="</head>\n";
  ptr1 +="<body>\n";
  ptr1 +="<table><tr><td><div id=\"webpage\">\n";
  ptr1 +="<h1>Weather Report</h1>\n";
  ptr1 +="<subtitle> [Serial output] Sensor Triggered! <br/>Sensors Triggered by this action are DHT11 & bmp180.<br/> DHT11 is active on digital pin 2 of D1 mini mainboard.<br/> If you are reading this the server is active and running without errors. </subtitle>";
  ptr1 +="<p><font color='green'>Temperature:</font> ";
  ptr1 +=(int)Temperaturestat;
  ptr1 +="<font color='gray'> C / </font>"; 
  ptr1 +=""; 
  ptr1 +=(int)bmp180Temp;
  ptr1 +="<font color='gray'> C</font><br/>"; 
  ptr1 +="<font color='green'>Pressure:</font> "; 
  ptr1 +=(int)bmp180Pressure; 
  ptr1 +="<font color='gray'> Ph <br/></font>";
  ptr1 +="<font color='green'>Humidity: </font>";
  ptr1 +=(int)Humiditystat;
  ptr1 +="<font color='gray'> %</font> <br/></p>";
  ptr1 +="</tr></td></table></div>\n";
  ptr1 +="</body>\n";
  ptr1 +="</html>\n";
  return ptr1;
}

void tempget() { 
  if (!bmp180.measureTemperature())
  {
    Serial.println("could not start temperature measurement, is a measurement already running?");
    return;
  }
  do
  {
    delay(100);
  } while (!bmp180.hasValue());

  Serial.print("[bmp180]\tTemperature: "); 
  Serial.print(bmp180.getTemperature()); 
  Serial.println(" degC");

  if (!bmp180.measurePressure())
  {
    Serial.println("could not start perssure measurement, is a measurement already running?");
    return;
  }
  do
  {
    delay(100);
  } while (!bmp180.hasValue());

  Serial.print("[bmp180]\tPressure: "); 
  Serial.print(bmp180.getPressure());
  Serial.println(" Pa");
   Serial.print("DHT11,\t");
   
  int chk = DHTx.read11(pinDHT11);
  switch (chk)
  {
    case DHTLIB_OK:  
      Serial.print("OK,\t"); 
      break;
    case DHTLIB_ERROR_CHECKSUM: 
      Serial.print("Checksum error,\t"); 
      break;
    case DHTLIB_ERROR_TIMEOUT: 
      Serial.print("Time out error,\t"); 
      break;
    default: 
      Serial.print("Unknown error,\t"); 
      break;
  }
  sensors_event_t humidity, temp;
  aht.getEvent(&humidity, &temp);
  // DISPLAY DATA
  Serial.print("Humidity:"); 
  Serial.print(DHTx.getHumidity(), 1);
  Serial.print(",\t");
  Serial.print("Temperature:");
  Serial.println(DHTx.getTemperature(), 1);
  Serial.print("AHT10 Temperature,\t "); Serial.print(temp.temperature); Serial.println(" degrees C");
  Serial.print("AHT10 Relative humidity,\t "); Serial.print(humidity.relative_humidity); Serial.println("% rH");

  server.handleClient(); 
}

void letterBlink(String _letter, uint8_t _pause) {
  matrix.setIntensity(0);
  matrix.character(_letter);
  for(uint8_t i=0;i<=8;i++) {
    matrix.setIntensity(i);
    delay(_pause);  
  }
  delay(_pause*10);
  for(uint8_t i=8;i<=0;i--) {
    matrix.setIntensity(i);
    delay(_pause);  
  }   
}

void letterScroll(String _letter, uint8_t _direction) {
  matrix.setIntensity(0);
  matrix.character(_letter);
  for(uint8_t i=0;i<=8;i++) {
    matrix.setIntensity(i);
    delay(50);  
  }
  delay(500);
  uint8_t scrRes;
  do {
    scrRes=matrix.scroll(_direction);
    if (scrRes==1)
      matrix.setIntensity(matrix.getIntensity()-1);
  } while (scrRes<=SCROLL_MOVED);
}



extern "C" void stack_thunk_dump_stack();

void processKey(Print& out, int hotKey) {
  switch (hotKey) {
    case 'd': {
        HeapSelectDram ephemeral;
        umm_info(NULL, true);
        break;
      }
    case 'i': {
        HeapSelectIram ephemeral;
        umm_info(NULL, true);
        break;
      }
    case 'h': {
        {
          HeapSelectIram ephemeral;
          Serial.printf(PSTR("IRAM ESP.getFreeHeap:  %u\n"), ESP.getFreeHeap());
        }
        {
          HeapSelectDram ephemeral;
          Serial.printf(PSTR("DRAM ESP.getFreeHeap:  %u\n"), ESP.getFreeHeap());
        }
        break;
      }
      
#ifdef DEBUG_ESP_PORT
    // From this context stack_thunk_dump_stack() will only work when Serial
    // debug is enabled.
    case 'p':
      out.println(F("Calling stack_thunk_dump_stack();"));
      stack_thunk_dump_stack();
      break;
#endif
    case 'R':
      out.printf_P(PSTR("Restart, ESP.restart(); ...\r\n"));
      ESP.restart();
      break;
    case '\r':
      out.println();
    case '\n':
      break;
    case 'Z': 
      out.println(F("Firmware Made for the D1 Mini"));
      out.println(F("+--------+------------+------+"));
      out.print(F("Systems_get_time() CALL=")); 
      out.println(system_get_time());
      out.print(F("WiFi_get_broadcase_if() CALL=")); 
      out.println(wifi_get_broadcast_if());
      out.print(F("wifi_get_opmode() CALL=")); 
      out.println(wifi_get_opmode());
      out.print(F("system_get_cpu_freq() CALL=")); 
      out.println(system_get_cpu_freq());
      out.print(F("WiFi_get_channel() CALL="));
      out.println(wifi_get_channel()); 
      out.print(F("system_get_chip_id(): 0x"));
      out.println(system_get_chip_id(), HEX);
      out.print(F("system_get_sdk_version(): "));
      out.println(system_get_sdk_version());
      out.print(F("system_get_boot_version(): "));
      out.println(system_get_boot_version());
      out.print(F("system_get_userbin_addr(): 0x"));
      out.println(system_get_userbin_addr(), HEX);
      out.print(F("system_get_boot_mode(): "));
      out.println(system_get_boot_mode() == 0 ? F("SYS_BOOT_ENHANCE_MODE") : F("SYS_BOOT_NORMAL_MODE"));
      break;  
    case '?':
      out.println();
      out.println(F("~Main menu")); 
      out.println(F("Press a key + <enter>"));
      out.println(F("  h    - Free Heap Report;"));
      out.println(F("  i    - iRAM umm_info(null, true);"));
      out.println(F("  d    - dRAM umm_info(null, true);"));
      out.println(F("  Z    - System information")); 
#ifdef DEBUG_ESP_PORT
      out.println(F("  p    - call stack_thunk_dump_stack();"));
#endif
      out.println(F("  R    - Restart, ESP.restart();"));
      out.println(F("  ?    - Print Help"));
      out.println();
      break;
    default:
      out.printf_P(PSTR("\"%c\" - Not an option?  / ? - help"), hotKey);
      out.println();
      processKey(out, '?');
      break;
  }
}
