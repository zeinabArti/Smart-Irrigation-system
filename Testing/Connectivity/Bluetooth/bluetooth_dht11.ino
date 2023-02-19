#include "BluetoothSerial.h"
#include "DHT.h"
#define DHTPIN 4
#define DHTTYPE DHT11 // DHT 11#include <LiquidCrystal_I2C.h>
DHT dht(DHTPIN, DHTTYPE);
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif 
BluetoothSerial SerialBT;
String message;
void setup() {
dht.begin();
Serial.begin(115200);
SerialBT.begin("DEVICE_ESP32"); //Bluetooth device name
Serial.println("The device started, now you can pair it with bluetooth!");
}
void loop() {
while(SerialBT.available() && SerialBT.read() != '\n'){
char incomingChar = SerialBT.read();
message += String(incomingChar);
}

//SENSOR
float h = dht.readHumidity();
// Read temperature as Celsius (the default)
float t = dht.readTemperature();
if (isnan(h) || isnan(t)) {
SerialBT.println(F("Failed to read from DHT sensor!"));
return;
}
SerialBT.print(F("Humidity: "));
SerialBT.print(h);
SerialBT.println(F("%"));

delay(1000);
SerialBT.print(F("Temperature: "));
SerialBT.print(t);
SerialBT.println(F(" C"));
delay(1000);
}
