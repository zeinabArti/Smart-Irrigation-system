/*
    Based on Neil Kolban example for IDF: https://github.com/nkolban/esp32-snippets/blob/master/cpp_utils/tests/BLE%20Tests/SampleServer.cpp
    Ported to Arduino ESP32 by Evandro Copercini
    updates by chegewara
*/

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"
#include <Wire.h>

#define temperatureCelsius

Adafruit_BMP3XX bmp;

float temp;
float tempF;
float pressure;

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 3000;

bool deviceConnected = false;
//"4fafc201-1fb5-459e-8fcc-c5c9c331914b" "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SERVICE_UUID         "84f6b8d6-ae43-11ed-afa1-0242ac120002"

// Temperature Characteristic and Descriptor
//#ifdef temperatureCelsius
  BLECharacteristic bmpTemperatureCelsiusCharacteristics("a34376c6-ae43-11ed-afa1-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
  BLEDescriptor bmpTemperatureCelsiusDescriptor(BLEUUID((uint16_t)0x2902));
//#else
  //BLECharacteristic bmpTemperatureFahrenheitCharacteristics("f78ebbff-c8b7-4107-93de-889a6a06d408", BLECharacteristic::PROPERTY_NOTIFY);
  //BLEDescriptor bmpTemperatureFahrenheitDescriptor(BLEUUID((uint16_t)0x2902));
//#endif

// pressure Characteristic and Descriptor
BLECharacteristic bmpPressureCharacteristics("ae9530f0-ae43-11ed-afa1-0242ac120002", BLECharacteristic::PROPERTY_NOTIFY);
BLEDescriptor bmpPressureDescriptor(BLEUUID((uint16_t)0x2903));

//Setup callbacks onConnect and onDisconnect
class MyServerCallbacks: public BLEServerCallbacks {
  void onConnect(BLEServer* pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer* pServer) {
    deviceConnected = false;
  }
};
void initBMP(){
  if (!bmp.begin_I2C()) {   
    Serial.println("Could not find a valid BMP3 sensor, check wiring!");
    while (1);
  }
  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}
void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  Wire.begin(23, 19);
  initBMP();
//create BLE device
  BLEDevice::init("BMP388_ESP32");
//create BLE server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
//create BLE service  
  BLEService *bmpService = pServer->createService(SERVICE_UUID);
 // Create BLE Characteristics and Create a BLE Descriptor
  // Temperature  
   //#ifdef temperatureCelsius
    bmpService->addCharacteristic(&bmpTemperatureCelsiusCharacteristics);
    bmpTemperatureCelsiusDescriptor.setValue("BMP temperature Celsius");
    bmpTemperatureCelsiusCharacteristics.addDescriptor(&bmpTemperatureCelsiusDescriptor);
  // #else
  //   bmpService->addCharacteristic(&bmpTemperatureFahrenheitCharacteristics);
  //   bmpTemperatureFahrenheitDescriptor.setValue("BMP temperature Fahrenheit");
  //   bmpTemperatureFahrenheitCharacteristics.addDescriptor(&bmpTemperatureFahrenheitDescriptor);
  // #endif  
    // Pressure
  bmpService->addCharacteristic(&bmpPressureCharacteristics);
  bmpPressureDescriptor.setValue("BMP Pressure");
  bmpPressureCharacteristics.addDescriptor(new BLE2902());
// start the service
  bmpService->start();
//start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");
}

void loop() {
  // put your main code here, to run repeatedly:
  if (! bmp.performReading()) {
    Serial.println("Failed to perform reading :(");
    return;
  }
  if (deviceConnected) {
    if ((millis() - lastTime) > timerDelay) {
      // Read temperature as Celsius (the default)
      temp = bmp.temperature;
      // Fahrenheit
      //tempF = 1.8*temp +32;
      // Read pressure
      pressure = bmp.pressure / 100.0;
  
      //Notify temperature reading from BMP sensor
      //#ifdef temperatureCelsius
        static char temperatureCTemp[6];
        dtostrf(temp, 6, 2, temperatureCTemp);
        //Set temperature Characteristic value and notify connected client
        bmpTemperatureCelsiusCharacteristics.setValue(temperatureCTemp);
        bmpTemperatureCelsiusCharacteristics.notify();
        Serial.print("Temperature Celsius: ");
        Serial.print(temp);
        Serial.print(" ºC");
      //#else
      //   static char temperatureFTemp[6];
      //   dtostrf(tempF, 6, 2, temperatureFTemp);
      //   //Set temperature Characteristic value and notify connected client
      //   bmpTemperatureFahrenheitCharacteristics.setValue(temperatureFTemp);
      //   bmpTemperatureFahrenheitCharacteristics.notify();
      //   Serial.print("Temperature Fahrenheit: ");
      //   Serial.print(tempF);
      //   Serial.print(" ºF");
      // #endif
      
      //Notify pressure reading from BMp
      static char pressureTemp[6];
      dtostrf(pressure, 6, 2, pressureTemp);
      //Set humidity Characteristic value and notify connected client
      bmpPressureCharacteristics.setValue(pressureTemp);
      bmpPressureCharacteristics.notify();   
      Serial.print(" - Pressure: ");
      Serial.print(pressure);
      Serial.println(" hPa");
      
      lastTime = millis();
    }
  }
}