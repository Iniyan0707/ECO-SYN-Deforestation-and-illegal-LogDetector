#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SPI.h>
#include <LoRa.h>

// ============================================================
// ECO-SYN - DEFORESTATION & ILLEGAL LOG DETECTOR
// RECEIVER NODE
// ESP32 + SX1278 + GSM + LCD + BUZZER
// ============================================================

// ---------------- GSM ----------------

HardwareSerial gsmSerial(1);

#define GSM_RX_PIN 16
#define GSM_TX_PIN 17

const String OWNER_PHONE_NUMBER =
  "+918807275985";

// ---------------- OUTPUTS ----------------

#define RX_LED_PIN 4
#define BUZZER_PIN 15

// ---------------- LoRa ----------------

#define SS_PIN 5
#define RST_PIN 14
#define DIO0_PIN 2

#define BAND 433E6

// ---------------- LCD ----------------

LiquidCrystal_I2C lcd(
  0x27,
  16,
  2
);

// ============================================================
// EVENT CODES
// MUST MATCH TRANSMITTER
// ============================================================

#define EVENT_AMBIENT      0
#define EVENT_GUNSHOT      1
#define EVENT_CHAINSAW     2
#define EVENT_VEHICLE      3
#define EVENT_TREE_CUTTING 4

// ============================================================
// PACKET STRUCTURE
// MUST MATCH TRANSMITTER EXACTLY
// ============================================================

struct __attribute__((__packed__))
AcousticStreamPacket {

  uint8_t node_id;

  uint16_t sound_peak;

  uint8_t event_type;

  float latitude;

  float longitude;

  uint32_t timestamp_sec;
};

// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("==========================================");
  Serial.println("       ECO-SYN RECEIVER NODE");
  Serial.println(" Deforestation & Illegal Log Detection");
  Serial.println("==========================================");

  // ----------------------------------------------------------
  // GSM
  // ----------------------------------------------------------

  gsmSerial.begin(
    9600,
    SERIAL_8N1,
    GSM_RX_PIN,
    GSM_TX_PIN
  );

  // ----------------------------------------------------------
  // Outputs
  // ----------------------------------------------------------

  pinMode(
    RX_LED_PIN,
    OUTPUT
  );

  pinMode(
    BUZZER_PIN,
    OUTPUT
  );

  digitalWrite(
    RX_LED_PIN,
    LOW
  );

  digitalWrite(
    BUZZER_PIN,
    LOW
  );

  // ----------------------------------------------------------
  // LCD
  // ----------------------------------------------------------

  lcd.init();

  lcd.backlight();

  lcd.setCursor(0, 0);

  lcd.print("ECO-SYN");

  lcd.setCursor(0, 1);

  lcd.print("Receiver");

  delay(1500);

  // ----------------------------------------------------------
  // GSM initialization
  // ----------------------------------------------------------

  initGSM();

  // ----------------------------------------------------------
  // LoRa
  // ----------------------------------------------------------

  LoRa.setPins(
    SS_PIN,
    RST_PIN,
    DIO0_PIN
  );

  if (!LoRa.begin(BAND)) {

    Serial.println(
      "ERROR: LoRa initialization failed!"
    );

    lcd.clear();

    lcd.setCursor(0, 0);

    lcd.print("LoRa Error!");

    while (1) {
      delay(1000);
    }
  }

  LoRa.setSpreadingFactor(7);

  LoRa.setSignalBandwidth(
    125E3
  );

  LoRa.setSyncWord(0xF3);

  lcd.clear();

  lcd.setCursor(0, 0);

  lcd.print("Listening...");

  Serial.println(
    "Receiver ready."
  );
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  int packetSize =
    LoRa.parsePacket();

  // ----------------------------------------------------------
  // Check packet size
  // ----------------------------------------------------------

  if (
    packetSize ==
    sizeof(AcousticStreamPacket)
  ) {

    AcousticStreamPacket receivedData;

    LoRa.readBytes(
      (uint8_t*)&receivedData,
      sizeof(receivedData)
    );

    digitalWrite(
      RX_LED_PIN,
      HIGH
    );

    // --------------------------------------------------------
    // THREAT DETECTED
    // --------------------------------------------------------

    if (
      receivedData.event_type >=
        EVENT_GUNSHOT
      &&
      receivedData.event_type <=
        EVENT_TREE_CUTTING
    ) {

      String alertText;

      String lcdAlertMsg;

      // ------------------------------------------------------
      // CORRECT EVENT MAPPING
      // ------------------------------------------------------

      switch (
        receivedData.event_type
      ) {

        case EVENT_GUNSHOT:

          alertText =
            "ALERT: Gunshot detected!";

          lcdAlertMsg =
            "Gunshot ALERT!";

          break;

        case EVENT_CHAINSAW:

          alertText =
            "ALERT: Chainsaw detected!";

          lcdAlertMsg =
            "Chainsaw ALERT!";

          break;

        case EVENT_VEHICLE:

          alertText =
            "ALERT: Vehicle detected!";

          lcdAlertMsg =
            "Vehicle ALERT!";

          break;

        case EVENT_TREE_CUTTING:

          alertText =
            "ALERT: Tree cutting detected!";

          lcdAlertMsg =
            "Tree Cut ALERT!";

          break;
      }

      // ------------------------------------------------------
      // LCD
      // ------------------------------------------------------

      lcd.clear();

      lcd.setCursor(0, 0);

      lcd.print(lcdAlertMsg);

      // ------------------------------------------------------
      // LOCATION
      // ------------------------------------------------------

      String locationText;

      if (
        receivedData.latitude != 0.0 ||
        receivedData.longitude != 0.0
      ) {

        locationText =
          "https://maps.google.com/?q=" +
          String(
            receivedData.latitude,
            6
          ) +
          "," +
          String(
            receivedData.longitude,
            6
          );
      }

      else {

        locationText =
          "GPS location unavailable";
      }

      // ------------------------------------------------------
      // SMS
      // ------------------------------------------------------

      String fullSMS =
        alertText +
        "\nNode ID: " +
        String(
          receivedData.node_id
        ) +
        "\nPeak: " +
        String(
          receivedData.sound_peak
        ) +
        "\nLocation: " +
        locationText;

      // ------------------------------------------------------
      // BUZZER
      // ------------------------------------------------------

      digitalWrite(
        BUZZER_PIN,
        HIGH
      );

      delay(500);

      digitalWrite(
        BUZZER_PIN,
        LOW
      );

      // ------------------------------------------------------
      // SEND SMS
      // ------------------------------------------------------

      sendSMS(
        OWNER_PHONE_NUMBER,
        fullSMS
      );

      // ------------------------------------------------------
      // SERIAL LOG
      // ------------------------------------------------------

      Serial.println();
      Serial.println(
        "=========================================="
      );

      Serial.println(
        "🚨 THREAT DETECTED"
      );

      Serial.println(
        alertText
      );

      Serial.printf(
        "Node ID : %d\n",
        receivedData.node_id
      );

      Serial.printf(
        "Peak    : %u\n",
        receivedData.sound_peak
      );

      Serial.printf(
        "GPS     : %.6f, %.6f\n",
        receivedData.latitude,
        receivedData.longitude
      );

      Serial.printf(
        "RSSI    : %d dBm\n",
        LoRa.packetRssi()
      );

      Serial.printf(
        "SNR     : %.2f dB\n",
        LoRa.packetSnr()
      );

      Serial.println(
        "=========================================="
      );

      delay(2000);

      lcd.clear();

      lcd.setCursor(0, 0);

      lcd.print("Listening...");
    }

    // --------------------------------------------------------
    // HEARTBEAT
    // --------------------------------------------------------

    else {

      Serial.printf(
        "Heartbeat | Node: %d | Peak: %u | RSSI: %d dBm\n",
        receivedData.node_id,
        receivedData.sound_peak,
        LoRa.packetRssi()
      );
    }

    digitalWrite(
      RX_LED_PIN,
      LOW
    );
  }
}

// ============================================================
// GSM INITIALIZATION
// ============================================================

void initGSM() {

  gsmSerial.println("AT");

  delay(500);

  gsmSerial.println(
    "AT+CMGF=1"
  );

  delay(500);

  Serial.println(
    "GSM initialized."
  );
}

// ============================================================
// SEND SMS
// ============================================================

void sendSMS(
  String phoneNumber,
  String message
) {

  gsmSerial.println(
    "AT+CMGF=1"
  );

  delay(300);

  gsmSerial.print(
    "AT+CMGS=\""
  );

  gsmSerial.print(
    phoneNumber
  );

  gsmSerial.println("\"");

  delay(500);

  gsmSerial.print(
    message
  );

  delay(100);

  // CTRL + Z
  gsmSerial.write(26);

  delay(5000);

  Serial.println(
    "SMS command sent."
  );
}
