#include "WiFi.h"
#include "HTTPClient.h"
#include "ArduinoJson.h"
#include<time.h>

String pushTemp = "NULL-Value";
String pushRH = "NULL-Value";
String pushPower1 = "NULL-Value";
String pushPower2 = "NULL-Value";
String pushEner1 = "NULL-Value";
String pushEner2 = "NULL-Value";

int fg=0;
int flg[7];
int PushOnce=1;
int DELAY_val=60000;
//  =======================================================================================================
//  ---------------------------------------------- Energy Meter -------------------------------------------
//  =======================================================================================================

#include <Arduino.h>
#include "esp32ModbusRTU.h"
#include <algorithm>  // for std::reverse
String energyBuff = "NULL-Value";
esp32ModbusRTU modbus(&Serial1, 25);  // use Serial1 and pin 16 as RTS

/* 
  SUCCES                = 0x00,
  ILLEGAL_FUNCTION      = 0x01,
  ILLEGAL_DATA_ADDRESS  = 0x02,
  ILLEGAL_DATA_VALUE    = 0x03,
  SERVER_DEVICE_FAILURE = 0x04,
  ACKNOWLEDGE           = 0x05,
  SERVER_DEVICE_BUSY    = 0x06,
  NEGATIVE_ACKNOWLEDGE  = 0x07,
  MEMORY_PARITY_ERROR   = 0x08,
  TIMEOUT               = 0xE0,
  INVALID_SLAVE         = 0xE1,
  INVALID_FUNCTION      = 0xE2,
  CRC_ERROR             = 0xE3,  // only for Modbus-RTU
  COMM_ERROR            = 0xE4  // general communication error
 */
/*
 Using address 0x01 for Energy Meter 1
 Using address 0x02 for Energy Meter 2
 Using Register 100 for Power values(Reading 2 registers)
 Using Register 158 for Energy Received Values (Reading 2 registers)
*/

String energyMeasure() {

    Serial.print("sending Modbus request...\n");
    String retstr = "";
    
    fg=1;
    delay(100);
//  reading Power1(PowerIN)
    modbus.readHoldingRegisters(0x01, 100, 2);
//  wait till modbus request is completed
    while(fg==1)
    {
      delay(10);
    }

    fg=2;
    delay(100);
//  reading Power2(PowerOUT)
    modbus.readHoldingRegisters(0x02, 100, 2);
//  wait till modbus request is completed
    while(fg==2)
    {
      delay(10);
    }
    
    fg=3;
    delay(100);
//  reading Energy1(EnergyIN)
    modbus.readHoldingRegisters(0x01, 158, 2);
//  wait till modbus request is completed
    while(fg==3)
    {
      delay(10);
    }

    
    fg=4;
    delay(100);
//  reading Energy2(EnergyOUT)
    modbus.readHoldingRegisters(0x02, 158, 2);
//  wait till modbus request is completed
    while(fg==4)
    {
      delay(10);
    }    
    
    delay(1000);
    return retstr;
}

//  =======================================================================================================
//  ---------------------------------------------- Temp sensor --------------------------------------------
//  =======================================================================================================
#include <Wire.h>;

// sda 21
// scl 22

#define si7021Addr 0x40
String tempStr;

void tempMeasure()
{
  unsigned int data[2];
  
  Wire.beginTransmission(si7021Addr);
  //Send humidity measurement command
  Wire.write(0xF5);
  Wire.endTransmission();
  delay(500);
    
  // Request 2 bytes of data
  Wire.requestFrom(si7021Addr, 2);
  // Read 2 bytes of data to get humidity
  if(Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }
    
  // Convert the data
  float humidity  = ((data[0] * 256.0) + data[1]);
  humidity = ((125 * humidity) / 65536.0) - 6;

  Wire.beginTransmission(si7021Addr);
  // Send temperature measurement command
  Wire.write(0xF3);
  Wire.endTransmission();
  delay(500);
    
  // Request 2 bytes of data
  Wire.requestFrom(si7021Addr, 2);
  
  // Read 2 bytes of data for temperature
  if(Wire.available() == 2)
  {
    data[0] = Wire.read();
    data[1] = Wire.read();
  }

// Convert the data
 
  float temp  = ((data[0] * 256.0) + data[1]);
  float celsTemp = ((175.72 * temp) / 65536.0) - 46.85;
  float fahrTemp = celsTemp * 1.8 + 32;
   
// Output data to serial monitor
  Serial.print("Humidity : ");
  Serial.print(humidity);
  Serial.println(" % RH");
  Serial.print("Celsius : ");
  Serial.print(celsTemp);
  Serial.println(" C");
  Serial.print("Fahrenheit : ");
  Serial.print(fahrTemp);
  Serial.println(" F");
  
  tempStr = "Humidity : " + (String)humidity + " % RH" + "\n";
  tempStr += "Celsius : " + (String)celsTemp + " C" + "\n" + "Fahrenheit : " + (String)fahrTemp + " F" + "\n";
  pushRH = (String)humidity;
  // Using Celsius value
  pushTemp = (String)celsTemp;
  if(pushTemp=="NULL-Value")
    flg[1]=0;
  else
    flg[1]=1;
  if(pushRH=="NULL-Value")
    flg[2]=0;
  else
    flg[2]=1;
}
//  -------------------------------------------------------------------------------------------------------
//  ========================================================================================================

char* wifi_ssid = "esw-m19@iiith";
char* wifi_pwd = "e5W-eMai@3!20hOct";
String cse_ip = "onem2m.iiit.ac.in";

int ledFlag=0;
int LED_R = 4;
int LED_G = 5; 

//  =======================================================================================================
//  ---------------------------------------------- One M2M ------------------------------------------------
//  =======================================================================================================

String cse_port = "443";
String server = "http://"+cse_ip+":"+cse_port+"/~/in-cse/in-name/";
void ledUpdate(){
  if(ledFlag==1){
    digitalWrite(LED_R,0);
    digitalWrite(LED_G,1);
  }
  else{
    digitalWrite(LED_R,1);
    digitalWrite(LED_G,0);
  }
}

String createCI(String server, String ae, String cnt, String val)
{
  HTTPClient http;
  http.begin(server + ae + "/" + cnt + "/");
  http.addHeader("X-M2M-Origin", "admin:admin");
  http.addHeader("Content-Type", "application/json;ty=4");
  http.addHeader("Content-Length", "100");
  http.addHeader("Connection", "close");
  int code = http.POST("{\"m2m:cin\": {\"cnf\": \"text/plain:0\",\"con\": "+ String(val) +"}}");
  http.end();
  Serial.println(code);
  if(code==-1){
    Serial.println("UNABLE TO CONNECT TO THE SERVER");
    ledFlag=0;
    ledUpdate();
  }
  delay(300);
}

void pushMyData(String pathh, String val){

  Serial.print("in pushmydata");
  Serial.println(val);
  val = "\"" + val + "\"";
  pathh = "pr_5_esp32_1/"+pathh;
  createCI(server, "Team43_UPS_performance_monitoring", pathh, val);  
}
//  -------------------------------------------------------------------------------------------------------
//  =======================================================================================================


void connect_to_WIFI(){
  WiFi.mode(WIFI_STA);// Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.disconnect();
  delay(100);
  WiFi.begin(wifi_ssid, wifi_pwd);
  Serial.println("Connecting to WiFi..");
  while (WiFi.status() != WL_CONNECTED || WiFi.status()==WL_CONNECT_FAILED){
    delay(500);
    Serial.print(".");
  }
  if(WiFi.status()==WL_CONNECTED){
    Serial.println("Connected to the WiFi network");
    ledFlag=1;
    ledUpdate();
  }
  Serial.println("Connected to the WiFi network");
}

void setup()
{
  Serial.begin(115200);
  ledUpdate();
  connect_to_WIFI();

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G,OUTPUT);
  Serial.println("Setup done");
  
//  ========================================================================================================
//  ---------------------------------------------- Energy Meter --------------------------------------------
//  ========================================================================================================

  Serial1.begin(9600, SERIAL_8E1, 26, 27);  // Modbus connection

  modbus.onData([](uint8_t serverAddress, esp32Modbus::FunctionCode fc, uint8_t* data, size_t length) {
//    Serial.printf("id 0x%02x fc 0x%02x len %u: 0x", serverAddress, fc, length);
    
    for (size_t i = 0; i < length; ++i) {
      //Serial.printf("%02x", data[i]);
//      Serial.printf("\n/%.2f\n",data[i]);
    }
    
    uint8_t data2[4];
    data2[0] = data[1];
    data2[1] = data[0];
    data2[2] = data[3];
    data2[3] = data[2];

//    Serial.printf("\nval: %.2f", *reinterpret_cast<float*>(data2));
    energyBuff = (String)(*reinterpret_cast<float*>(data2));
    Serial.println();
    Serial.print("ebuff, fg=  ");
    Serial.print(energyBuff);
    Serial.print(" ");
    Serial.println(fg);
    if(fg == 1)
    {
      pushPower1=energyBuff;
      flg[3]=1;
    }
    else if(fg == 2)
    {
      pushPower2=energyBuff;
      flg[4]=1;
    }
    else if(fg == 3 )
    {
      pushEner1=energyBuff;
      flg[5]=1;
    }
    else if(fg == 4)
    {
      pushEner2=energyBuff;
      flg[6]=1;
    }
    
    fg++;  
  });
  modbus.onError([](esp32Modbus::Error error) {
    
    
    Serial.printf("Got error: 0x%02x\n\n", static_cast<uint8_t>(error));
    Serial.println();
    Serial.print("ebuff, fg=  ");
    Serial.print(energyBuff);
    Serial.print(" ");
    Serial.println(fg);
    energyBuff= "NULL-Value";
    if(fg == 1)
    {
      flg[3]=0;
    }
    else if(fg == 2)
    {
      flg[4]=0;
    }
    else if(fg == 3 )
    {
      flg[5]=0;
    }
    else if(fg == 4)
    {
      flg[6]=0;
    }
    fg++;
  });
  modbus.begin();

//  =======================================================================================================
//  ---------------------------------------------- Temp sensor --------------------------------------------
//  =======================================================================================================
  Wire.begin();
  Wire.beginTransmission(si7021Addr);
  Wire.endTransmission();
  delay(300);
//  -------------------------------------------------------------------------------------------------------
//  =======================================================================================================

}

void loop()
{
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Connection lost.. trying to reconnect");
    ledFlag=0;ledUpdate();
    connect_to_WIFI();
  }
  for( int i = 1 ; i < 7 ; i++ )
  {
    flg[i]=0;
  }
  tempMeasure();
  String energyStr = energyMeasure();

  // Send data to OneM2M server

  String masterstr = "(" + pushTemp + "," + pushRH + "," + pushPower1 + "," + pushPower2 + "," + pushEner1 + ","+ pushEner2 + ")";
  int sum=0;
  for(int i = 1; i < 7 ; i++)
  {
    sum+=flg[i];
  }

  if(PushOnce == 1 && sum==6)
  {
    pushMyData("em/em_2_vll_avg", masterstr);
    PushOnce=0;
  }
  else if(PushOnce==0 && sum>=4)
  {
    pushMyData("em/em_2_vll_avg", masterstr);
  }

  delay(DELAY_val); // DO NOT CHANGE THIS LINE
}
