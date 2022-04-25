/*including all the libraries we need to work with*/
#include "Arduino.h"
#include "string.h"
#include "WiFi.h"
#include"WiFiClient.h"
#include "PubSubClient.h"
#include <HTTPClient.h>
#include "Adafruit_PM25AQI.h"
#include <Adafruit_Sensor.h>
#include "DFRobot_CCS811.h"
#include "Adafruit_BME680.h"
#include "MQ135.h"
#include "SPI.h"

/*Defining pins to use */

/**********LEDS***************/

#define LED_ORANGE_MQTT_ESTABLISHING_CONNECTION 26
//#define LED_RED_MQTT_ESTABLISHING_CONNECTION_FAILED
#define LED_ORANGE_WiFi_ESTABLISHING_CONNECTION 27
//#define LED_RED_WiFi_ESTABLISHING_CONNECTION_FAILED   
#define LED_GREEN_DATA_SENT_SUCCESSFULLY  0

/*****************************/



/*----------------------------------------------------------*/
/* Here i used a differents pins for an usual i2c pins in order to work with two sensors using i2c*/
#define I2C_SDA_pma300i 33
#define I2C_SCL_pma300i 32


/*----------------------------------------------------------*/
/*For the CCS811 i worked with the dedicated pins for i2c on 
board (it depend for every board so make sure to check your 
pinsout schema)                                              */
/*----------------------------------------------------------*/


/********************************************/
/*For the BME680 its used the SPI protocol */ 
/********************************************/ 
   //SCL 	Broche SCL pour la communication I2C
  //Broche SCK pour la communication SPI
  //SDA 	Broche SDA pour la communication I2C
  //Broche SDI (MOSI) pour la communication SPI
   //SDO 	Broche SDO (MISO) pour la communication SPI
  //CS 	Broche de sélection de puce pour la communication SPI
#define BME_SCK 18
#define BME_MISO 19
#define BME_MOSI 23
#define BME_CS 5

/*----------------------------------------------------------*/
#define PIN_MQ135 34
/*----------------------------------------------------------*/

#define SEALEVELPRESSURE_HPA (1013.25)


/*-------------------------------------------------------------------*/
long lastMsg = 0;
/*-------------------------------------------------------------------*/
//Create a new TwoWire instance. In this case, it’s called PMA003i. This simply creates an I2C bus.
TwoWire PMA003i = TwoWire(1);
/*-----------------------------------------------------------------------------------------------*/

/******************WIFI & MQTT Broker info**************************/
const char* ssid = "DJAWEB_Pepsi";
const char* password = "google012345";
const char* mqttServer = "192.168.1.16";
const int mqttPort = 1883;
const char* mqttUser = "iot_enst";
const char* mqttPassword = "cherfianadir";

WiFiClient espClient;
PubSubClient client(espClient);
/******************************************************************/

/***********HTTP***************************************************/
String HOST_NAME ="http://192.168.1.5:3000"; // change to PC's IP
String PATH_NAME   = "/insert";
/*******************************************************************/
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
} 
void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
      // Subscribe
      client.subscribe("esp32/output");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }}
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

Adafruit_BME680 bme(BME_CS, BME_MOSI, BME_MISO,  BME_SCK);

DFRobot_CCS811 CCS811;

MQ135 mq135_sensor = MQ135(PIN_MQ135);

void setup() {
  Serial.begin(115200);
/*******PIN_MODE************/
pinMode(LED_ORANGE_MQTT_ESTABLISHING_CONNECTION,OUTPUT);
pinMode(LED_ORANGE_WiFi_ESTABLISHING_CONNECTION,OUTPUT);
pinMode(LED_GREEN_DATA_SENT_SUCCESSFULLY,OUTPUT);
/**************************/


// Wait for serial monitor to open
while (!Serial) delay(100);
/*initiating the data transfere throw the additional bus with a frequency : 100000 -Default for i2c-*/
PMA003i.begin(I2C_SDA_pma300i, I2C_SCL_pma300i,100000);
delay(1000);
Serial.println("Adafruit PMSA003I Air Quality Sensor");
// Wait one second for sensor to boot up!
delay(1000);

/*Checking if sensor is dected by its Master*/
if (!aqi.begin_I2C(&PMA003i)) { 
Serial.println("Could not find PM 2.5 sensor!");
while (1) delay(10);
}
delay(10000);
Serial.println("PM25 found!");
while (!Serial);

/*Checking if sensor can establish a connection using SPI*/

Serial.println(F("BME680 test"));
delay(10000);
  if (!bme.begin()) {
    Serial.println("Could not find a valid BME680 sensor, check wiring!");
    while (1);}
    /************************************************************/
  
// Set up oversampling and filter initialization
bme.setTemperatureOversampling(BME680_OS_8X);
bme.setHumidityOversampling(BME680_OS_2X);
bme.setPressureOversampling(BME680_OS_4X);
bme.setIIRFilterSize(BME680_FILTER_SIZE_3);
bme.setGasHeater(320, 150); // 320*C for 150 ms
/****************************************************/
/* Verifiying i2c SCLK & SDA */
 while(CCS811.begin() != 0){
        Serial.println("failed to init chip, please check if the chip connection is fine");
        delay(1000);
    }
/******************WIFI & MQTT Broker info**************************/
 WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
    digitalWrite(LED_ORANGE_WiFi_ESTABLISHING_CONNECTION,HIGH);
  }
 
  Serial.println("Connected to the WiFi network");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
 digitalWrite(LED_ORANGE_WiFi_ESTABLISHING_CONNECTION,LOW);
  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    digitalWrite(LED_ORANGE_MQTT_ESTABLISHING_CONNECTION,HIGH);
 
    if (client.connect("ESP32Client", mqttUser, mqttPassword )) {
 
      Serial.println("connected");
      digitalWrite(LED_ORANGE_MQTT_ESTABLISHING_CONNECTION,LOW);
    } else {
 
      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
 
    }}
 

/******************************************************************/
}

void loop() {
    if (!client.connected()) {
      digitalWrite(LED_ORANGE_MQTT_ESTABLISHING_CONNECTION,HIGH);
    reconnect();
  }
  digitalWrite(LED_ORANGE_MQTT_ESTABLISHING_CONNECTION,LOW);
/**********************MQ135********************************/
/*******TO_BE_USED_IN_CASE_WE_ARE_INTRESTED_IN_RZERO*******/
//float temperature = bme.temperature; //initialisation of MQ135 temperature with BME680 reading
//float humidity = bme.humidity;//initialisation of MQ135 humidity with BME680 reading
// float rzero = mq135_sensor.getRZero();
  //float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  //float resistance = mq135_sensor.getResistance();
  //float ppm = mq135_sensor.getPPM();
  //float correctedPPM = mq135_sensor.getCorrectedPPM(temperature, humidity);
/***********************************************************/
PM25_AQI_Data data;
if (!aqi.read(&data)) {
Serial.println("Could not read from AQI");
delay(1000); // try again in a bit!
return;
}
Serial.println("***********************************************");
Serial.println("*****PMA003i READING****************************");
Serial.println("***********************************************");
Serial.println("AQI reading success");
Serial.println();
Serial.println(F("---------------------------------------"));
Serial.println(F("Concentration Units (standard)"));
Serial.println(F("---------------------------------------"));
Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);
Serial.println(F("Concentration Units (environmental)"));
Serial.println(F("---------------------------------------"));
Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_env);
Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_env);
Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_env);
Serial.println(F("---------------------------------------"));
Serial.print(F("Particles > 0.3um / 0.1L air:"));
Serial.println(data.particles_03um);
Serial.print(F("Particles > 0.5um / 0.1L air:"));
Serial.println(data.particles_05um);
Serial.print(F("Particles > 1.0um / 0.1L air:"));
Serial.println(data.particles_10um);
Serial.print(F("Particles > 2.5um / 0.1L air:"));
Serial.println(data.particles_25um);
Serial.print(F("Particles > 5.0um / 0.1L air:"));
Serial.println(data.particles_50um);
Serial.print(F("Particles > 10 um / 0.1L air:"));
Serial.println(data.particles_100um);
delay(1000);
if (! bme.performReading()) {
Serial.println("Failed to perform reading :(");
return;
}
Serial.println(F("---------------------------------------"));
Serial.println("***********************************************");
Serial.println("****************BME680 READING******************");
Serial.println("***********************************************");
Serial.print("Temperature = ");
Serial.print(bme.temperature);
Serial.println(" *C");
Serial.print("Pressure = ");
Serial.print(bme.pressure / 100.0);
Serial.println(" hPa");
Serial.print("Humidity = ");
Serial.print(bme.humidity);
Serial.println(" %");
Serial.print("Gas = ");
Serial.print(bme.gas_resistance / 1000.0);
Serial.println(" KOhms");
Serial.print("Approx. Altitude = ");
Serial.print(bme.readAltitude(SEALEVELPRESSURE_HPA));
Serial.println(" m");
Serial.println(F("---------------------------------------"));
Serial.println();
delay(2000);
Serial.println("***********************************************");
Serial.println("*************CCS811 READING********************");
Serial.println("***********************************************");
 if(CCS811.checkDataReady() == true){
        Serial.print("CO2: ");
        Serial.print(CCS811.getCO2PPM());
        Serial.print("ppm, TVOC: ");
        Serial.print(CCS811.getTVOCPPB());
        Serial.println("ppb");

    } else {
        Serial.println("Data is not ready!");
    }
    CCS811.writeBaseLine(0xDEBE);
    //delay cannot be less than measurement cycle
    delay(1000);
Serial.println(F("---------------------------------------"));
Serial.println("***********************************************");
Serial.println("********************MQ135**********************");
Serial.println("***********************************************");
/********This data can be used to detected other gases as mentioned in datasheet
 * but in matter of fact its not that precise to take as a source of information**/
  //Serial.print("MQ135 RZero: ");
  //Serial.print(rzero);
  //Serial.print("\t Corrected RZero: ");
  //Serial.print(correctedRZero);
  //Serial.print("\t Resistance: ");
  //Serial.print(resistance);
  //Serial.print("\t PPM: ");
  //Serial.print(ppm);
  //Serial.print("\t Corrected PPM: ");
  //Serial.print(correctedPPM);
  //Serial.println("ppm");
  /*************Air quality ************************/
 int  sensorValue = analogRead(PIN_MQ135);       // read analog input pin 0
Serial.print("AirQua=");
float air_quality =sensorValue;
Serial.print(sensorValue, DEC);               // prints the value read
Serial.println(" PPM");         
Serial.println(F("---------------------------------------"));
  delay(300);



/**************MQTT_TEST**********************************/
   /**************MQTT_Publishing**********************************/
   long now = millis();
   if (now - lastMsg > 5000) {
     lastMsg=now; 
  char str_PM100[20];
  char str_PM25[20];
  char str_PM10[20];
  char str_03um[20];
  char str_05um[20];
  char str_10um[20];
  char str_25um[20];
  char str_50um[20];
  char str_100um[20];
  char str_temperature[20];
  char str_humidity[20];
  char str_pressure[20];
  char str_altitude[20];
  char str_gas_resistance[20];
  char str_TVOC[20];
  char str_CO2[20];
  char str_air_quality[20];
  dtostrf(data.pm100_standard, 1, 2,str_PM100);
  dtostrf(data.pm25_standard, 1, 2,str_PM25);
  dtostrf(data.pm10_standard, 1, 2,str_PM10);
  dtostrf(data.particles_03um, 1, 2,str_03um);
  dtostrf(data.particles_05um, 1, 2,str_05um);
  dtostrf(data.particles_10um, 1, 2,str_10um);
  dtostrf(data.particles_25um, 1, 2,str_25um);
  dtostrf(data.particles_50um, 1, 2,str_50um);
  dtostrf(data.particles_100um, 1, 2,str_100um); 
  dtostrf(bme.temperature, 1, 2,str_temperature);
  dtostrf(bme.humidity, 1, 2,str_humidity);     
  dtostrf(bme.readAltitude(SEALEVELPRESSURE_HPA), 1, 2,str_altitude);
  dtostrf(bme.pressure, 1, 2,str_pressure);
  dtostrf(bme.gas_resistance, 1, 2,str_gas_resistance);
  dtostrf(CCS811.getTVOCPPB(), 1, 2,str_TVOC);
  dtostrf(CCS811.getCO2PPM(), 1, 2,str_CO2);
  dtostrf(air_quality, 1, 2,str_air_quality);
  client.publish("esp/PMA003i/PM10",str_PM100);
  delay(500);
  client.publish("esp/PMA003i/PM25",str_PM25);
  delay(500);
  client.publish("esp/PMA003i/PM100",str_PM10);
  delay(500);
  client.publish("esp/PMA003i/03um",str_03um);
  delay(500);
  client.publish("esp/PMA003i/05um",str_05um);
  delay(500);
  client.publish("esp/PMA003i/10um",str_10um);
  delay(500);
  client.publish("esp/PMA003i/25um",str_25um);
  delay(500);
  client.publish("esp/PMA003i/50um",str_50um);
  delay(500);
  client.publish("esp/PMA003i/100um",str_100um);
  delay(500);
   client.publish("esp/BME680/temperature",str_temperature);
  delay(500);
  client.publish("esp/BME680/humidity",str_humidity);
  delay(500);
  client.publish("esp/BME680/pressure",str_pressure);
  delay(500);
  client.publish("esp/BME680/gas_resistance",str_gas_resistance);
  delay(500);
  client.publish("esp/BME680/altitude",str_altitude);
  delay(500);
  client.publish("esp/CCS811/TVOC",str_TVOC);
  delay(500);
  client.publish("esp/CCS811/CO2",str_CO2);
  delay(500);
  client.publish("esp/MQ135/air_quality",str_air_quality);
  delay(500);
  /****************HTTP**********(to be completed)*****************************/

  HTTPClient http;
  WiFiClient client;

  String serverPath=HOST_NAME+PATH_NAME+"?temperature="+str_temperature+"&humidity="+str_humidity+"&altitude="+str_altitude+"&pressure="+str_pressure+
  "&PM10="+str_PM10+"&PM25="+str_PM25+"&PM100="+str_PM100+"&P03um="+str_03um+"&P05um="+str_05um+"&P10um="+str_10um+"&P25um="+str_25um+
  "&P50um="+str_50um+"&P100um="+str_100um+"&CO2="+str_CO2+"&TVOC="+str_TVOC+"&AIR_QUALITY="+str_air_quality+"&GAS_RESISTANCE="+str_gas_resistance;
  
  http.begin(client,serverPath.c_str()); //HTTP
  int httpCode = http.GET();
    // httpCode will be negative on error
  if(httpCode > 0) {
    // file found at server
    if(httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      Serial.println(payload);
    } else {
      // HTTP header has been send and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);
    }
  } else {
    Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
/*********************************************************/
   }
      client.loop();
digitalWrite(LED_GREEN_DATA_SENT_SUCCESSFULLY,HIGH);
delay(1000);
digitalWrite(LED_GREEN_DATA_SENT_SUCCESSFULLY,LOW);
delay(10000);//Reading data delay
}