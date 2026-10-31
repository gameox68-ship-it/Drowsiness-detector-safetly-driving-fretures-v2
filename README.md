[readme (1).md](https://github.com/user-attachments/files/34220878/readme.1.md)
# Smart Autonomous Safety Vehicle with Driver Drowsiness Detection & Collision Avoidance

An embedded IoT safety platform combining **Biometric Driver Drowsiness Monitoring (Smart Goggles)** with an **Autonomous 4WD Smart Vehicle**. Using real-time 2.4 GHz RF telemetry, the system continuously tracks the driver's eye state while scanning the road ahead with ultrasonic sonar, activating dual-layer emergency braking and acoustic alarms whenever risk is detected.

---

## Key Features

- **Biometric Eye-Blink & Drowsiness Tracking:** Infrared reflectance sensor mounted on safety goggles monitors cornea/eyelid movements in real time.
- **Wireless Telemetry (2.4 GHz NRF24L01+):** Low-latency RF communication link transmitting driving telemetry packets at ~30 updates per second.
- **Dual-Safety Emergency Braking Architecture:**
  - **Layer 1 (Driver Impairment):** Instantaneous vehicle halt and 1.5 kHz wake-up siren upon eye closure.
  - **Layer 2 (Obstacle Collision Prevention):** Autonomous emergency braking (AEB) and 2.4 kHz collision alarm if an obstacle enters the 20 cm critical perimeter, even with eyes wide open.
- **Fail-Safe Watchdog Protection:** Automatic vehicle stop if wireless communication is lost for more than 1,000 ms.
- **Permanent Power & System Health Feedback:** Visual power-on indicator LED and onboard status LEDs for instant diagnostics.

---

## Hardware Bill of Materials (BOM)

### Unit 1: Driver Safety Goggles (Transmitter)
| Component | Specification | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | Arduino Nano (ATmega328P) | 1 | Telemetry processing & sensor sampling |
| **Eye Sensor** | LM393 Optical IR Obstacle Module | 1 | Detects eyelid closure vs. corneal reflection |
| **RF Transceiver** | NRF24L01+ 2.4 GHz Module | 1 | Transmits drive/stop control packets |
| **Indicator LED** | 5mm Green LED + 220Ω Resistor | 1 | Continuous power & unit activity indicator |
| **Power Supply** | 2S 18650 Li-ion Battery (7.4V) | 1 | Connected to `VIN` and `GND` |
| **Safety Frame** | Protective Laboratory Goggles | 1 | Structural mount for sensor assembly |

### Unit 2: Smart Vehicle Chassis (Receiver)
| Component | Specification | Quantity | Purpose |
| :--- | :--- | :--- | :--- |
| **Microcontroller** | Arduino Nano (ATmega328P) | 1 | Motor logic, ultrasonic sonar & RF reception |
| **Motor Driver** | L298N Dual H-Bridge Module | 1 | Powers and drives 4 DC gear motors |
| **Drive Motors** | 3V-6V Dual Shaft TT Gear Motors | 4 | 4WD chassis propulsion |
| **Sonar Sensor** | HC-SR04 Ultrasonic Module | 1 | 2 cm – 400 cm frontal obstacle detection |
| **Acoustic Alarm** | Active / Passive Piezo Buzzer | 1 | Dual-frequency acoustic warning siren |
| **RF Transceiver** | NRF24L01+ 2.4 GHz Module | 1 | Receives wireless goggle telemetry |
| **Chassis** | 4WD Acrylic Robot Car Kit | 1 | Vehicle mechanical frame & wheels |
| **Power Supply** | 5V USB Power Bank or 7.4V Li-ion | 1 | Independent high-current power for motors |

---

## Pinout & Connection Tables

### Goggles Transmitter Unit

| Module Pin | Wire Color | Arduino Nano Pin | Technical Note |
| :--- | :--- | :--- | :--- |
| **IR Sensor VCC** | Red | `5V` | Standard 5V regulated rail |
| **IR Sensor GND** | Black | `GND` | System common ground |
| **IR Sensor OUT** | Green | `D2` | Active-LOW digital interrupt/input |
| **NRF24L01 VCC** | Orange | `3.3V` (3V3) | **CRITICAL: 5V will permanently burn NRF24L01** |
| **NRF24L01 GND** | Brown | `GND` | Ground plane |
| **NRF24L01 CE** | Yellow | `D9` | Chip Enable control |
| **NRF24L01 CSN** | Blue | `D10` | SPI Chip Select Not |
| **NRF24L01 SCK** | Purple | `D13` | SPI Bus Serial Clock |
| **NRF24L01 MOSI**| Gray | `D11` | SPI Master Out Slave In |
| **NRF24L01 MISO**| White | `D12` | SPI Master In Slave Out |
| **Green LED (+)** | Red | `D4` | Powered via 220Ω resistor (Always ON) |
| **Green LED (-)** | Black | `GND` | Common ground |
| **Battery (+)** | Red | `VIN` | 7.4V input stepped down by onboard LDO |
| **Battery (-)** | Black | `GND` | Common ground return |

---

### Smart Vehicle Receiver Unit

| Module Pin | Wire Color | Arduino Nano Pin | Technical Note |
| :--- | :--- | :--- | :--- |
| **HC-SR04 VCC** | Red | `5V` | 5V power rail |
| **HC-SR04 GND** | Black | `GND` | Common ground |
| **HC-SR04 Trig** | Yellow | `D2` | 10 µs trigger pulse output |
| **HC-SR04 Echo** | White | `D4` | Echo pulse measurement input |
| **Buzzer (+)** | Red | `D3` | Supports frequency modulation (`tone()`) |
| **Buzzer (-)** | Black | `GND` | Common ground |
| **L298N IN1** | Blue | `D5` | Left Motor Group Forward |
| **L298N IN2** | Gray | `D6` | Left Motor Group Reverse |
| **L298N IN3** | Purple | `D7` | Right Motor Group Forward |
| **L298N IN4** | Brown | `D8` | Right Motor Group Reverse |
| **L298N 12V** | Red | Battery / Power Bank (+) | Main DC motor power input |
| **L298N GND** | Black | Battery (-) & Nano `GND` | **MANDATORY: Must bridge to Nano GND** |
| **NRF24L01 VCC** | Orange | `3.3V` (3V3) | Regulated 3.3V output |
| **NRF24L01 GND** | Brown | `GND` | Common ground |
| **NRF24L01 CE** | Yellow | `D9` | Chip Enable |
| **NRF24L01 CSN** | Blue | `D10` | SPI Chip Select Not |
| **NRF24L01 SCK** | Purple | `D13` | SPI Serial Clock |
| **NRF24L01 MOSI**| Gray | `D11` | SPI Master Out Slave In |
| **NRF24L01 MISO**| White | `D12` | SPI Master In Slave Out |

---

## Schematic Blueprints

### Goggles Transmitter Schematic

```
        +----------------------------------------------------+
        |             2S Li-ion Battery (7.4V)               |
        |             (+) Red           (-) Black            |
        +----------------+------------------+----------------+
                         |                  |
                         v                  v
                 +-------+------------------+-------+
                 |  VIN                    GND      |
                 |                                  |
                 |           ARDUINO NANO           |
                 |           (Transmitter)          |
                 |                                  |
                 |   5V    D2    D4    3V3    D9-D13|
                 +----+-----+-----+-----+---+---+---+
                      |     |     |     |   |   |
          +-----------+     |     |     |   |   +----------+
          | VCC             |     |     |   |              |
          v                 |     |     |   |              |
    +-----------+           |     |     |   |              |
    |  IR EYE   | OUT-------+     |     |   |              |
    |  SENSOR   |                 |     |   |              |
    |  (LM393)  | GND             |     |   |              |
    +-----+-----+                 |     |   |              |
          |                       |     |   |              |
          +-> GND                 |     |   |              |
                                  v     |   |              |
                        +-----------+   |   |              |
                        | GREEN LED |   |   |              |
                        | Indicator |   |   |              |
                        +-----+-----+   |   |              |
                              |         |   |              |
                              +-> GND   |   |              |
                                        v   v              v
                                      +-----------------------+
                                      |   NRF24L01+ 2.4GHz    |
                                      |   Wireless Telemetry  |
                                      |   (VCC, CE, CSN, SPI) |
                                      +-----------------------+
```

---

### Vehicle Receiver & Collision Braking Schematic

```
                                  +------------------------------------+
                                  |    DC Motor Power (5V - 8.4V)      |
                                  |    (+) Red               (-) Black |
                                  +-------+----------------------+-----+
                                          |                      |
                                          v                      v
                                  +-------+----------------------+-----+
                                  |  12V Term                GND Term  |
                                  |                                    |
                                  |      L298N MOTOR DRIVER            |
                                  |                                    |
                                  | OUT1/2 (Left)       OUT3/4 (Right) |
                                  +---+----+-----------------+----+----+
                                      |    |                 |    |
                                      v    v                 v    v
                                    [Left Motors]          [Right Motors]
                                      (Parallel)             (Parallel)
                                          ^                      |
                                          | IN1-IN4              | Common
                                          |                      | Ground
                                          |                      v
                 +------------------------+----------------------+-----+
                 | D5-D8 (Motor Logic)                      GND        |
                 |                                                     |
                 |                    ARDUINO NANO                     |
                 |                    (Car Receiver)                   |
                 |                                                     |
                 |  5V         D2, D4          D3         3V3   D9-D13 |
                 +---+-----------+--------------+----------+------+----+
                     |           |              |          |      |
        +------------+           |              |          |      |
        | VCC                    |              |          |      |
        v                        v              v          |      |
  +-----------+            +-----------+  +-----------+    |      |
  |  HC-SR04  | Trig -> D2 |  SAFETY   |  | NRF24L01+ |<---+------+
  | ULTRASONIC| Echo -> D4 |  BUZZER   |  | TRANSCEIVER          |
  |  SENSOR   |            |  ALARM    |  | (2.4GHz Telemetry)   |
  +-----+-----+            +-----+-----+  +----------------------+
        |                        |
        +-> GND                  +-> GND
```

---

## Dual-Safety Decision Matrix

| Wireless Command | Front Sonar Distance | Vehicle Motor State | Safety Buzzer Alarm | Primary Hazard / Status |
| :---: | :---: | :---: | :---: | :--- |
| `'S'` (Eye Closed) | Any Distance | **EMERGENCY STOP** | **ACTIVE (1500 Hz Siren)** | Driver Drowsiness / Sleep detected |
| `'F'` (Eye Open) | $\le 20\text{ cm}$ | **EMERGENCY BRAKE** | **ACTIVE (2400 Hz Pitch)** | Impending Frontal Collision |
| `'F'` (Eye Open) | $> 20\text{ cm}$ | **CRUISING FORWARD** | **SILENT** | Normal, safe driving conditions |
| Signal Lost (>1s) | Any Distance | **FAILSAFE STOP** | Intermittent alert | Telemetry link severed |

---

## Installation & Deployment

1. **Clone the Repository:**
   ```bash
   git clone https://github.com/your-username/smart-drowsiness-car.git
   cd smart-drowsiness-car
   ```
2. **Install Required Arduino Libraries:**
   - Open Arduino IDE -> `Tools` -> `Manage Libraries...`
   - Search for **`RF24`** by *TMRh20, Avamander* and click **Install**.
3. **Flashing the Goggles (Transmitter):**
   - Open `Goggles_Transmitter.ino`.
   - Connect the Goggles Arduino Nano via mini-USB.
   - Go to `Tools` -> `Board: Arduino Nano`.
   - Select `Tools` -> `Processor: ATmega328P (Old Bootloader)`.
   - Select the assigned COM port and click **Upload**.
4. **Flashing the Vehicle (Receiver):**
   - Open `SmartCar_Receiver.ino`.
   - Connect the Car Arduino Nano via mini-USB.
   - Select the appropriate COM port and click **Upload**.

---

## Calibration & Tuning Guide

1. **IR Eye Blink Sensor Threshold:**
   - Put on the goggles and observe the onboard signal LED on the LM393 comparator board.
   - Using a precision screwdriver, rotate the blue potentiometer until the LED is **OFF** when looking straight ahead with eyes open.
   - Blink and close your eyes: the LED must reliably latch **ON** every time eyelids cover the cornea.
2. **HC-SR04 Sonar Alignment:**
   - Mount the ultrasonic transducer perpendicular to the floor, at least 4 cm above ground level to avoid ground reflections.
   - Adjust `SAFE_DISTANCE_CM` (default: 20 cm) in `SmartCar_Receiver.ino` depending on vehicle top speed and braking distance.
3. **RF Antenna Stability:**
   - If communication drops intermittently, solder a **10 µF – 100 µF electrolytic capacitor** directly across `VCC` and `GND` of each NRF24L01 module to eliminate voltage ripple.

---

## License

This project is open-source under the [MIT License](https://www.google.com/search?q=LICENSE). Contributions, issues, and feature requests are welcome!
