# 🌲🚨 ECO-SYN – Deforestation & Illegal Log Detector

### 🌍🔊 **Smart Forest Monitoring & Early Warning System**

<p align="center">

🌲 **Protect Forests**   •   📡 **Long-Range Monitoring**   •   📍 **GPS Tracking**   •   📱 **Instant Alerts**

</p>

---

## 🏆✨ PROJECT HIGHLIGHTS

| 🏆 Achievement               | 🎓 Institution                              | 👥 Team    |
| ---------------------------- | ------------------------------------------- | ---------- |
| **CMR ThinkFest 3.0 Winner** | **CMR College of Engineering & Technology** | **TFT002** |

> 🌱 **ECO-SYN is an embedded forest-monitoring system designed to help detect suspicious acoustic activity associated with illegal logging and other potentially harmful activities in remote forest areas.**

---

# 🎯🔴 PROBLEM STATEMENT

🌳 Forests cover large and difficult-to-monitor areas, making continuous human surveillance challenging.

🚨 Illegal logging and unauthorized activities can happen in remote locations where immediate human intervention is difficult.

### ⚠️ Key Challenges

* 🌲 Large forest coverage
* 🏞️ Remote monitoring locations
* 👂 Continuous acoustic monitoring
* 📡 Long-distance communication
* ⏱️ Rapid alert delivery
* 📍 Locating the detected event

---

# 💡🟢 OUR SOLUTION

**ECO-SYN** uses a distributed embedded-system architecture consisting of two main units:

### 📡🌲 1. LISTENING NODE — TRANSMITTER

The transmitter is deployed in the forest monitoring area.

**It performs:**

🎙️ Acoustic activity sensing
🧠 Threshold-based event classification
📍 GPS location acquisition
📦 Data packet formation
📡 Long-range LoRa transmission
☀️ Solar-powered operation

### 📥🏠 2. GATEWAY BASE STATION — RECEIVER

The receiver acts as the central gateway.

**It performs:**

📡 LoRa data reception
🔎 Event identification
📺 LCD alert display
🔔 Local alert indication
📱 GSM/SMS notification
📍 GPS coordinate reporting

---

# 🔄⚙️ SYSTEM WORKFLOW

```text
                    🌲 FOREST AREA
                         │
                         ▼
                🎙️ ACOUSTIC SENSOR
                         │
                         ▼
                   🟢 ESP32 TX
                         │
              ┌──────────┴──────────┐
              │                     │
              ▼                     ▼
       🔊 SOUND ANALYSIS         📍 NEO-6M GPS
              │                     │
              └──────────┬──────────┘
                         │
                         ▼
                  📡 SX1278 LoRa
                         │
                  ~~~~~~~~📡~~~~~~~~
                         │
                         ▼
                  🟢 ESP32 RX
                         │
              ┌──────────┼──────────┐
              │          │          │
              ▼          ▼          ▼
           📺 LCD     🔔 BUZZER   📱 GSM
                                   │
                                   ▼
                              🚨 SMS ALERT
                                   │
                                   ▼
                              📍 GPS LOCATION
```

---

# 🔧🛠️ HARDWARE COMPONENTS

## 📡🌲 TRANSMITTER — LISTENING NODE

| 🔧 Component                        | 🎯 Purpose                                |
| ----------------------------------- | ----------------------------------------- |
| 🟢 ESP32 Development Board          | Main controller & processing              |
| 🎙️ Acoustic Microphone Sensor      | Detects acoustic activity                 |
| 🎚️ Potentiometer                   | Sound-threshold simulation during testing |
| 📍 NEO-6M GPS                       | Provides location coordinates             |
| 📡 SX1278 Ra-02 LoRa                | Long-range wireless transmission          |
| ☀️ Solar Panel                      | Renewable power source                    |
| 🔋 Battery Charging/Management Unit | Power management                          |

---

## 📥🏠 RECEIVER — GATEWAY BASE STATION

| 🔧 Component                | 🎯 Purpose                 |
| --------------------------- | -------------------------- |
| 🟢 ESP32 Development Board  | Central gateway controller |
| 📡 SX1278 Ra-02 LoRa        | Receives transmitter data  |
| 📺 16×2 I2C LCD             | Displays event information |
| 📱 GSM/SMS Module           | Sends alert notifications  |
| 🔔 Buzzer / Alert Indicator | Local warning              |

---

# 📡🚀 TRANSMITTER

### 🌲 Listening Node

The transmitter continuously monitors acoustic activity.

### 🔄 Process

🎙️ **Sense**

⬇️

📊 **Measure Sound Level**

⬇️

🔎 **Classify Event**

⬇️

📍 **Get GPS Coordinates**

⬇️

📦 **Create Data Packet**

⬇️

📡 **Transmit through LoRa**

---

# 📥🚨 RECEIVER

### 🏠 Gateway Base Station

The receiver waits for incoming LoRa packets.

### 🔄 Process

📡 **Receive LoRa Packet**

⬇️

🔎 **Identify Event**

⬇️

📺 **Display on LCD**

⬇️

🔔 **Activate Local Alert**

⬇️

📱 **Send SMS**

⬇️

📍 **Provide GPS Location**

---

# 🔊📊 EVENT CLASSIFICATION

The current prototype uses **predefined acoustic thresholds** for event classification.

|  🔢 Code | 🚨 Event            | 📱 Response    |
| -------: | ------------------- | -------------- |
| 🟢 **0** | Ambient / Heartbeat | No SMS         |
| 🔴 **1** | Gunshot             | 🚨 Alert + SMS |
| 🟠 **2** | Chainsaw            | 🚨 Alert + SMS |
| 🟡 **3** | Vehicle             | 🚨 Alert + SMS |
| 🟤 **4** | Tree Cutting        | 🚨 Alert + SMS |

> ℹ️ **Note:** The current prototype uses threshold-based acoustic classification. It does **not** use TinyML or a machine-learning model.

---

# 📍🌍 GPS LOCATION TRACKING

The transmitter uses the **NEO-6M GPS module** to obtain geographical coordinates.

When an alert is received:

📍 Latitude
📍 Longitude
🗺️ Location information

can be included in the SMS notification.

This helps monitoring personnel identify the approximate location of the detected event.

---

# 📡📶 LoRa COMMUNICATION

ECO-SYN uses the:

### **SX1278 Ra-02 LoRa Module — 433 MHz**

for communication between the listening node and gateway.

### ⭐ Advantages

📡 Long-range communication
🔋 Low-power operation
🌲 Suitable for remote monitoring
📦 Small data packet transmission
🔗 Wireless node-to-gateway communication

---

# 📱🚨 GSM SMS ALERT

When a threat event is detected, the receiver can route an SMS notification through the GSM module.

### 📩 Example Alert

```text
🚨 ECO-SYN ALERT

Event: Tree Cutting
Node ID: 101
Peak: XXXX

📍 Location:
Latitude: XX.XXXXXX
Longitude: XX.XXXXXX

🗺️ Map Location:
Google Maps Link
```

This provides monitoring personnel with a quick notification and location information.

---

# ☀️🔋 POWER SYSTEM

The listening node is designed with:

☀️ **Solar Panel**

*

🔋 **Battery**

*

⚡ **Battery Charging / Management Unit**

This architecture is intended to support long-term operation in remote monitoring locations.

---

# 🔬💻 WOKWI SIMULATION

The virtual prototype was developed and tested using **Wokwi**.

### 📡 Transmitter Simulation

🔗 **[Open Transmitter Simulation](https://wokwi.com/projects/470135193191559169)**

### 📥 Receiver Simulation

🔗 **[Open Receiver Simulation](https://wokwi.com/projects/470132338565567489)**

### 🧪 Simulation Includes

🟢 ESP32
📺 I2C LCD
📍 GPS parsing
🎚️ Potentiometer / analog input
📊 Sound threshold simulation

---

# 📷✨ PROJECT IMAGES

Project photographs and testing images are available in:

```text
📁 images/
```

Suggested images:

📸 Prototype
🔌 Hardware setup
📡 Transmitter node
📥 Receiver gateway
🧪 Testing
🏆 CMR ThinkFest achievement

---

# 📄📚 DOCUMENTATION

### 📘 Project Report

The complete project report is available in:

```text
📁 documentation/
```

### 📊 Project Presentation

The project presentation is also available in:

```text
📁 documentation/
```

---

# 🏆🎉 ACHIEVEMENT

## 🥇 CMR ThinkFest 3.0 — Winner

🏫 **Institution:**
CMR College of Engineering & Technology

🔬 **Centre:**
Centre for Engineering Education Research

👥 **Team ID:**
**TFT002**

🏆 **Achievement:**
**Winner – CMR ThinkFest 3.0**

---

# 👥💚 TEAM ECO-SYN

| 👤 Team Member              |
| --------------------------- |
| 🧑‍💻 **INIYAN S**          |
| 👩‍💻 **HENCY SHIPPORAL H** |
| 👩‍💻 **MOHANA PRIYA K**    |

---

# 📁🗂️ REPOSITORY STRUCTURE

```text
🌲 ECO-SYN-Deforestation-and-illegal-LogDetector/
│
├── 🔧 hardware/
│   └── 📐 Circuit Diagram
│
├── 💻 firmware/
│   │
│   ├── 📡 transmitter/
│   │   └── transmitter.ino
│   │
│   └── 📥 receiver/
│       └── receiver.ino
│
├── 📚 documentation/
│   ├── 📘 Project Report
│   └── 📊 Presentation
│
├── 📷 images/
│   └── Project Images
│
└── 📄 README.md
```

---

# 🚀🌱 FUTURE SCOPE

Future improvements can include:

🌲 **Multiple Listening Nodes**
📡 **Larger Forest Coverage**
🔋 **Improved Power Optimization**
☀️ **Long-Term Solar Operation**
📊 **Centralized Monitoring Dashboard**
📱 **Improved Alert Management**
🎙️ **Advanced Acoustic Classification**
🌦️ **Additional Environmental Sensors**
🧪 **Extensive Real-World Field Testing**

---

# 🛠️💻 TECHNOLOGIES USED

| 💻 Technology       | 🔧 Application         |
| ------------------- | ---------------------- |
| 🟢 ESP32            | Embedded controller    |
| 📡 SX1278 LoRa      | Wireless communication |
| 📍 NEO-6M GPS       | Location tracking      |
| 📱 GSM              | SMS alerts             |
| 📺 I2C LCD          | Local display          |
| 🎙️ Acoustic Sensor | Sound monitoring       |
| ⚡ Arduino Framework | Firmware development   |
| 🔬 Wokwi            | Virtual prototyping    |

---

# 📌🔐 SECURITY NOTE

⚠️ **Do not publish your personal phone number in this public repository.**

If your receiver code contains your real phone number, replace it before publishing the repository with:

```cpp
const String OWNER_PHONE_NUMBER = "YOUR_PHONE_NUMBER";
```

Keep the real number only in your local version.

---

# ⚠️📋 DISCLAIMER

ECO-SYN is a prototype developed for **academic, research, and demonstration purposes**.

The acoustic threshold values and detection performance may require further calibration and real-world field validation before deployment in operational forest environments.

---

# 🌍🌲 ECO-SYN

### 💚 **Technology for Smarter Forest Monitoring**

### 📡 **Detect • Locate • Alert • Protect** 🚨🌲

---

<p align="center">

🌲 🌳 🌲 🌳 🌲 🌳 🌲

**ECO-SYN — Protecting Forests Through Technology**

🌲 🌳 🌲 🌳 🌲 🌳 🌲

</p>

