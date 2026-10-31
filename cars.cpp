/*
  ====================================================================================
  Project: Smart Driver Drowsiness Detection System (Receiver - Vehicle Car)
  Hardware: Arduino Nano, L298N Motor Driver, HC-SR04 Sonar, Piezo Buzzer, NRF24L01+
  License: MIT
  ====================================================================================
*/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// NRF24L01+ Configuration: CE = Pin 9, CSN = Pin 10
RF24 radio(9, 10);
const byte rfAddress[6] = "00001";

// L298N Motor Control Pins
const int PIN_IN1 = 5;
const int PIN_IN2 = 6;
const int PIN_IN3 = 7;
const int PIN_IN4 = 8;

// Sonar & Alarm Pins
const int PIN_BUZZER = 3;
const int PIN_TRIG   = 2;
const int PIN_ECHO   = 4;

// Safety Thresholds & Telemetry Watchdog
const int SAFE_DISTANCE_CM = 20; // Collision threshold (20 cm)
char currentCommand = 'S';
unsigned long lastPacketTime = 0;
const unsigned long FAILSAFE_TIMEOUT_MS = 1000;

void setup() {
  Serial.begin(9600);

  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  stopCar();
  silenceAlarm();

  if (!radio.begin()) {
    Serial.println(F("[ERROR] NRF24L01 receiver not responding."));
    while (1) {
      tone(PIN_BUZZER, 400);
      delay(200);
      noTone(PIN_BUZZER);
      delay(200);
    }
  }

  radio.openReadingPipe(0, rfAddress);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  radio.startListening();

  Serial.println(F("[INFO] Vehicle Receiver Active. Dual safety monitoring started."));
}

long measureDistanceCm() {
  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(2);
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  // 25ms timeout bounds reading to avoid freezing RF telemetry loops
  long durationUs = pulseIn(PIN_ECHO, HIGH, 25000);
  if (durationUs == 0) return 999;
  return (durationUs * 0.0343) / 2;
}

void soundAlarm(int frequencyHz) {
  digitalWrite(PIN_BUZZER, HIGH);
  tone(PIN_BUZZER, frequencyHz);
}

void silenceAlarm() {
  digitalWrite(PIN_BUZZER, LOW);
  noTone(PIN_BUZZER);
}

void moveForward() {
  digitalWrite(PIN_IN1, HIGH);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, HIGH);
  digitalWrite(PIN_IN4, LOW);
}

void stopCar() {
  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW);
}

void loop() {
  // 1. Ingest RF Telemetry Packet
  if (radio.available()) {
    radio.read(&currentCommand, sizeof(currentCommand));
    lastPacketTime = millis();
  }

  // 2. Failsafe: Emergency halt if RF link drops for > 1 second
  if (millis() - lastPacketTime > FAILSAFE_TIMEOUT_MS && lastPacketTime != 0) {
    currentCommand = 'S';
    Serial.println(F("[FAILSAFE] Goggle link lost. Vehicle halted."));
  }

  // 3. Measure Road Clearance
  long distanceCm = measureDistanceCm();

  // 4. Dual-Safety Decision Matrix
  if (currentCommand == 'S') {
    // Condition A: Driver drowsiness / closed eyes
    stopCar();
    soundAlarm(1500); // 1.5 kHz wake-up siren
  } 
  else if (currentCommand == 'F' && distanceCm <= SAFE_DISTANCE_CM) {
    // Condition B: Eye open, but front obstacle detected <= 20 cm
    stopCar();
    soundAlarm(2400); // 2.4 kHz collision warning pitch
  } 
  else if (currentCommand == 'F' && distanceCm > SAFE_DISTANCE_CM) {
    // Condition C: Eye open AND road ahead is clear
    moveForward();
    silenceAlarm();
  }

  delay(20);
}