#include <SoftwareSerial.h>
#include <DHT.h>

#define DHTPIN 13 // DHT11 data pin
#define DHTTYPE DHT11 // DHT11 sensor type

DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial zigbee(4, 5); // set up a software serial port for the Zigbee module

void setup() {
  Serial.begin(9600); // initialize serial communication for debugging
  zigbee.begin(9600); // initialize software serial communication for the Zigbee module
  dht.begin(); // initialize the DHT11 sensor
}

void loop() {
  // read temperature and humidity data from the DHT11 sensor and store it in variables
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // create a string to store the sensor data
  String data = String(temp) + "," + String(hum);

  // send the sensor data to the Zigbee module
  String msg="temperature and humidity";
  zigbee.print(msg);
  zigbee.println(data);

  // print the sensor data for debugging
  Serial.println(data);

  // delay for a short time before sending the next data point
  delay(5000);
}