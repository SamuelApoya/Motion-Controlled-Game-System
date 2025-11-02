/**
Samuel Apoya
**/

#define FACTORYRESET_ENABLE 0  // Set to 1 to enable factory reset. It is currently disabled (0)

#include <Arduino.h>
#include <SPI.h>
#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_LSM303_U.h>

volatile bool isSending = false;
const byte interruptPin = 3;

// Create Bluefruit LE SPI instance
#define BLUEFRUIT_SPI_CS 8
#define BLUEFRUIT_SPI_IRQ 7
#define BLUEFRUIT_SPI_RST 4

Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// Timing for data output
unsigned long lastSendTime = 0;
const unsigned long sendInterval = 300;

// Sensor instance
Adafruit_LSM303_Accel_Unified accel = Adafruit_LSM303_Accel_Unified(12345);
Adafruit_LSM303_Mag_Unified mag = Adafruit_LSM303_Mag_Unified(12345);

// linear interpolation (map x, y, and z readings from accelerometer to new ranges)
float linearInterpolation(float x, float x0, float x1, float y0, float y1){
  return (x - x0) * (y1 - y0) / (x1 - x0) + y0; 
}

void error(const __FlashStringHelper *err) {
  Serial.println(err);
  while (1);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Initializing...");

  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), toggleState, RISING);

  if (!ble.begin(false)) {
    error(F("Couldn't find  it's in Command mode!"));
  }
  
  if (FACTORYRESET_ENABLE) {
    if (!ble.factoryReset()) {
      error(F("Couldn't perform factory reset"));
    }
  }

  ble.echo(false);
  ble.info();

  // Set device name
  if (!ble.sendCommandCheckOK(F("AT+GAPDEVNAME=Samuels_bluetooth"))) {
    error(F("Could not set device name"));
  }

  // Enable HID service
    if (!ble.sendCommandCheckOK(F("AT+BleHIDEn=On"))) {
      error(F("Could not enable HID service"));
    }

    if (!ble.sendCommandCheckOK(F("AT+BleKeyboardEn=On"))) {
      error(F("Could not enable keyboard"));
    }

  // Reset after enabling services
  if (!ble.reset()) {
    error(F("Couldn't reset the module"));
  }

  // Enable Bluetooth advertising
  ble.sendCommandCheckOK(F("AT+GAPADVERTISING=1"));
  
  // Initialize sensors
  if (!accel.begin() || !mag.begin()) {
    Serial.println("Failed to initialize sensors");
    while (1);
  }
  Serial.println("Sensors initialized.");
}

void loop() {
  if (isSending && millis() - lastSendTime >= sendInterval) {

    sensors_event_t accelEvent;
    accel.getEvent(&accelEvent);

    sensors_event_t magEvent;
    mag.getEvent(&magEvent);

  // calculate heading for yaw control (z - axis control)
    float Pi = 3.14159;
    float heading = atan2(magEvent.magnetic.y, magEvent.magnetic.x)*180/ Pi;

    // call linear interpolation to find new ranges for X, Y, and Z
    float cameraX = linearInterpolation(accelEvent.acceleration.x, -10, 10, -100, 100);
    float cameraY = linearInterpolation(accelEvent.acceleration.y, -10, 10, -100, 100);
    float cameraZ = linearInterpolation(accelEvent.acceleration.z, -10, 10, -100, 100);


    // controls roll, pitch, and yaw
    float roll = linearInterpolation(accelEvent.acceleration.x, -10, 10, -45, 45);
    float pitch = linearInterpolation(accelEvent.acceleration.y, -10, 10, -45, 45);
    float yaw = linearInterpolation(heading, 0, 360, -180, 180);


    if(heading < 0){
      heading += 360;
    }

  // this block controls translational movements
  String output = "";
  if (cameraZ < -20) output = "w"; // forward
  else if (cameraZ > 20) output = "x"; // backward
  else if (cameraX > -20) output = "a"; // left
  else if (cameraX > 20) output = "d"; // right
  else if (cameraY > 20) output = "e"; // up
  else if (cameraY > -20) output = "c"; // down

  // this part controls rotations (pitch, roll, and yaw)
  // row
  else if (roll > 20) output  = "z"; // row right (z)
  else if (roll > -20) output  = "q"; // row left (q)

  // pitch
  else if (pitch > 20) output  = "0x1F"; // up arrow (picth up)
  else if (pitch > -20) output  = "0x1E"; // down arrow (picth down)

  //yaw
  else if (yaw > 20) output = "0x15"; // yaw right (right arrow)
  else if (yaw > -20) output = "0x1D"; // yaw left (left arrow)

  
  // send key press when movement detected
  if (output.length() > 0){
    ble.print("AT+BleKeyboard=");
    ble.println(output);
    Serial.println("Data sent: " + output);

    if (ble.waitForOK()){
      Serial.println("Data sent successfully");
    } else{
      Serial.println("Failed to send data");
    }
  }


  }
}

void toggleState() {
  isSending = !isSending;
  Serial.println(isSending ? "Started sending data" : "Stopped sending data");
}



