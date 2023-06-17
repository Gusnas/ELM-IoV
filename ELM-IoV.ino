#include "BluetoothSerial.h"
#include "ELMduino.h"
#include <HTTPClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>

BluetoothSerial SerialBT;

#define DEBUG_PORT Serial

String macAdd = "00:10:cc:4f:36:03";
String btName = "OBDII";

const char* ssid = "iPhone de Gustavo";
const char* password = "";

const char* serverName =

  uint8_t address[6] = { 0x00, 0x10, 0xCC, 0x4F, 0x36, 0x03 };

ELM327 myELM327;

float gas = 0;
int32_t kph = 0;
uint16_t runTime = 0;
uint32_t km = 0;


void setup() {
  DEBUG_PORT.begin(115200);

  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());


  SerialBT.begin("iov", true);  //inicia o bluetooth
  SerialBT.setPin("1234");

  Serial.println("Attempting to connect to ELM327...");

  if (!SerialBT.connect(address))  //tenta conectar ao elm via bluetooth
  {
    DEBUG_PORT.println("Couldn't connect to OBD scanner - Phase 1");
    while (1)
      ;
  }

  if (!myELM327.begin(SerialBT, true, 2000))  //tenta iniciar a conexão do elm a ECU
  {
    Serial.println("Couldn't connect to OBD scanner - Phase 2");
    while (1)
      ;
  }

  Serial.println("Connected to ELM327");
}


void loop() {

  if (WiFi.status() == WL_CONNECTED) {
    WiFiClient client;
    HTTPClient http;

    http.begin(client, serverName);

    uint8_t standards = myELM327.obdStandards();

    Serial.println(standards);

    float tempGas = myELM327.fuelLevel();
    if (myELM327.nb_rx_state == ELM_SUCCESS) {

      gas = tempGas;
      Serial.print("Gas: ");
      Serial.println(gas);
    }
    else{    
    myELM327.printError();
    Serial.println("status: " + myELM327.nb_rx_state);
    }

    int32_t tempKph = myELM327.kph();
    if (myELM327.nb_rx_state == ELM_SUCCESS) {
      kph = tempKph;
      Serial.print("Km/h: ");
      Serial.println(kph);
    }
    else{    
    myELM327.printError();
    Serial.println("status: " + myELM327.nb_rx_state);
    }

    uint16_t tempRunTime = myELM327.runTime();
    if (myELM327.nb_rx_state == ELM_SUCCESS) {
      runTime = tempRunTime;
      Serial.print("Run time: ");
      Serial.println(runTime);
    }
    else{    
    myELM327.printError();
    Serial.println("status: " + myELM327.nb_rx_state);
    }

    float tempKm = myELM327.Km_scc();
    if (myELM327.nb_rx_state == ELM_SUCCESS) {
      km = tempKm;
      Serial.print("Kilometrage: ");
      Serial.println(km);
    }
    else{    
    myELM327.printError();
    Serial.println("status: " + myELM327.nb_rx_state);
    }

    //If you need an HTTP request with a content type: application/json, use the following:
    http.addHeader("Content-Type", "application/json");
    int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);

    http.end();
  } else {
    Serial.println("WiFi Disconnected");
  }
}
