/*
  ====================================================================================
  Project: Smart Driver Drowsiness Detection System (Transmitter - Goggles)
  Hardware: Arduino Nano, Eye Blink IR Sensor, NRF24L01+ 2.4GHz Transceiver, Power LED
  License: MIT
  ====================================================================================
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// NRF24L01 Configuration: CE = Pin 9, CSN = Pin 10
RF24 radio(9, 10);
const byte rfAddress[6] = "00001";

// Pin Definitions
const int PIN_EYE_SENSOR = 2;  // Digital OUT from IR Eye Sensor
const int PIN_POWER_LED  = 4;  // Constant ON Green Power Indicator
const int PIN_STATUS_LED = 13; // Onboard LED feedback

void setup() {
  Serial.begin(9600);
  
  pinMode(PIN_EYE_SENSOR, INPUT);
  pinMode(PIN_POWER_LED, OUTPUT);
  pinMode(PIN_STATUS_LED, OUTPUT);

  // Turn ON Green Power Indicator LED permanently
  digitalWrite(PIN_POWER_LED, HIGH);
  digitalWrite(PIN_STATUS_LED, LOW);

  // Initialize NRF24L01+ Transceiver
  if (!radio.begin()) {
    Serial.println(F("[ERROR] NRF24L01 transceiver not responding. Check 3.3V supply."));
    while (1) {
      digitalWrite(PIN_STATUS_LED, HIGH);
      delay(100);
      digitalWrite(PIN_STATUS_LED, LOW);
      delay(100);
    }
  }

  radio.openWritingPipe(rfAddress);
  radio.setPALevel(RF24_PA_MIN);    // Low RF power for cabin range
  radio.setDataRate(RF24_250KBPS);  // Maximum sensitivity and reliability
  radio.stopListening();

  Serial.println(F("[INFO] Goggles Transmitter Ready. Telemetry stream started."));
}

void loop() {
  int eyeState = digitalRead(PIN_EYE_SENSOR);

  // Active-LOW: Obstacle/eyelid reflection pulls pin LOW
  if (eyeState == LOW) {
    char command = 'S'; // 'S' = STOP and sound alarm
    radio.write(&command, sizeof(command));

    digitalWrite(PIN_STATUS_LED, HIGH);
    Serial.println(F("[ALERT] Eye CLOSED -> Sent 'S' (Emergency Brake)"));
  } else {
    char command = 'F'; // 'F' = FORWARD (Normal driving)
    radio.write(&command, sizeof(command));

    digitalWrite(PIN_STATUS_LED, LOW);
    Serial.println(F("[NORMAL] Eye OPEN -> Sent 'F' (Cruising)"));
  }

  delay(30);
}