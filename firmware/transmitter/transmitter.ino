#include <SPI.h>
#include <LoRa.h>
#include <TinyGPS++.h>
#include <driver/i2s.h>

// ============================================================
// ECO-SYN - DEFORESTATION & ILLEGAL LOG DETECTOR
// TRANSMITTER NODE
// ESP32 + INMP441 + GPS + SX1278 LoRa
// ============================================================

// ---------------- I2S MEMS MICROPHONE ----------------
#define I2S_WS          25
#define I2S_SD          33
#define I2S_SCK         26
#define I2S_PORT        I2S_NUM_0

// ---------------- LoRa SX1278 ----------------
#define LORA_SS         5
#define LORA_RST        14
#define LORA_DIO0       2
#define BAND            433E6

// ---------------- GPS ----------------
#define GPS_RX_PIN      16
#define GPS_TX_PIN      17

// ---------------- SOUND THRESHOLDS ----------------
// IMPORTANT:
// Keep these in DESCENDING order.

#define THRESHOLD_CHAINSAW       35000
#define THRESHOLD_GUNSHOT        20000
#define THRESHOLD_VEHICLE        18000
#define THRESHOLD_TREE_CUTTING   10000

// ---------------- NODE ID ----------------
#define NODE_ID 101

// ---------------- EVENT CODES ----------------
// 0 = Ambient / Heartbeat
// 1 = Gunshot
// 2 = Chainsaw
// 3 = Vehicle
// 4 = Tree Cutting

#define EVENT_AMBIENT      0
#define EVENT_GUNSHOT      1
#define EVENT_CHAINSAW     2
#define EVENT_VEHICLE      3
#define EVENT_TREE_CUTTING 4

HardwareSerial gpsSerial(2);
TinyGPSPlus gps;

uint32_t lastHeartbeatTime = 0;

// ============================================================
// PACKET STRUCTURE
// Total size = 16 bytes
// ============================================================

struct __attribute__((__packed__)) AcousticStreamPacket {

  uint8_t node_id;

  uint16_t sound_peak;

  uint8_t event_type;

  float latitude;

  float longitude;

  uint32_t timestamp_sec;
};

// ============================================================
// FUNCTION DECLARATIONS
// ============================================================

void setupI2SMic();

uint16_t readMEMSPeakAudio(uint16_t durationMs);

uint8_t classifySound(uint16_t peak);

void sendLoRaPacket(uint8_t eventType, uint16_t peak);

// ============================================================
// SETUP
// ============================================================

void setup() {

  Serial.begin(115200);

  delay(1000);

  Serial.println();
  Serial.println("==========================================");
  Serial.println("      ECO-SYN TRANSMITTER NODE");
  Serial.println(" Deforestation & Illegal Log Detection");
  Serial.println("==========================================");

  // Initialize microphone
  setupI2SMic();

  // Initialize GPS
  gpsSerial.begin(
    9600,
    SERIAL_8N1,
    GPS_RX_PIN,
    GPS_TX_PIN
  );

  // Initialize LoRa
  LoRa.setPins(
    LORA_SS,
    LORA_RST,
    LORA_DIO0
  );

  if (!LoRa.begin(BAND)) {

    Serial.println("ERROR: LoRa initialization failed!");

    while (1) {
      delay(1000);
    }
  }

  // LoRa configuration
  LoRa.setSpreadingFactor(7);
  LoRa.setSignalBandwidth(125E3);
  LoRa.setSyncWord(0xF3);

  Serial.println("LoRa initialized successfully.");
  Serial.println("Transmitter ready.");
}

// ============================================================
// MAIN LOOP
// ============================================================

void loop() {

  // ----------------------------------------------------------
  // 1. Update GPS parser
  // ----------------------------------------------------------

  while (gpsSerial.available() > 0) {

    gps.encode(gpsSerial.read());
  }

  // ----------------------------------------------------------
  // 2. Read acoustic signal
  // ----------------------------------------------------------

  uint16_t peakValue =
    readMEMSPeakAudio(100);

  // ----------------------------------------------------------
  // 3. Classify sound
  // ----------------------------------------------------------

  uint8_t eventType =
    classifySound(peakValue);

  // ----------------------------------------------------------
  // 4. Send threat immediately
  // ----------------------------------------------------------

  if (eventType != EVENT_AMBIENT) {

    sendLoRaPacket(
      eventType,
      peakValue
    );

    // Prevent repeated alerts
    delay(3000);
  }

  // ----------------------------------------------------------
  // 5. Send heartbeat every 10 seconds
  // ----------------------------------------------------------

  else if (
    millis() - lastHeartbeatTime >= 10000
  ) {

    sendLoRaPacket(
      EVENT_AMBIENT,
      peakValue
    );

    lastHeartbeatTime = millis();
  }
}

// ============================================================
// I2S MICROPHONE INITIALIZATION
// ============================================================

void setupI2SMic() {

  i2s_config_t i2s_config = {

    .mode =
      (i2s_mode_t)(
        I2S_MODE_MASTER |
        I2S_MODE_RX
      ),

    .sample_rate = 16000,

    .bits_per_sample =
      I2S_BITS_PER_SAMPLE_32BIT,

    .channel_format =
      I2S_CHANNEL_FMT_ONLY_LEFT,

    .communication_format =
      I2S_COMM_FORMAT_STAND_I2S,

    .intr_alloc_flags =
      ESP_INTR_FLAG_LEVEL1,

    .dma_buf_count = 8,

    .dma_buf_len = 64,

    .use_apll = false
  };

  i2s_pin_config_t pin_config = {

    .bck_io_num = I2S_SCK,

    .ws_io_num = I2S_WS,

    .data_out_num =
      I2S_PIN_NO_CHANGE,

    .data_in_num = I2S_SD
  };

  i2s_driver_install(
    I2S_PORT,
    &i2s_config,
    0,
    NULL
  );

  i2s_set_pin(
    I2S_PORT,
    &pin_config
  );
}

// ============================================================
// READ MEMS MICROPHONE
// ============================================================

uint16_t readMEMSPeakAudio(
  uint16_t durationMs
) {

  uint32_t startMillis =
    millis();

  int32_t sampleBuffer[64];

  size_t bytesRead = 0;

  int32_t maxSample =
    -2147483647;

  int32_t minSample =
    2147483647;

  while (
    millis() - startMillis <
    durationMs
  ) {

    i2s_read(
      I2S_PORT,
      &sampleBuffer,
      sizeof(sampleBuffer),
      &bytesRead,
      portMAX_DELAY
    );

    int samplesRead =
      bytesRead /
      sizeof(int32_t);

    for (
      int i = 0;
      i < samplesRead;
      i++
    ) {

      int32_t rawSample =
        sampleBuffer[i] >> 14;

      if (
        rawSample >
        maxSample
      ) {
        maxSample =
          rawSample;
      }

      if (
        rawSample <
        minSample
      ) {
        minSample =
          rawSample;
      }
    }
  }

  int32_t peakToPeak =
    maxSample - minSample;

  if (peakToPeak < 0) {
    peakToPeak = 0;
  }

  uint32_t scaledPeak =
    peakToPeak >> 2;

  if (scaledPeak > 65535) {
    scaledPeak = 65535;
  }

  return (uint16_t)scaledPeak;
}

// ============================================================
// SOUND CLASSIFICATION
// ============================================================

uint8_t classifySound(
  uint16_t peak
) {

  // IMPORTANT:
  // Highest threshold must be checked first.

  if (
    peak >= THRESHOLD_CHAINSAW
  ) {
    return EVENT_CHAINSAW;
  }

  if (
    peak >= THRESHOLD_GUNSHOT
  ) {
    return EVENT_GUNSHOT;
  }

  if (
    peak >= THRESHOLD_VEHICLE
  ) {
    return EVENT_VEHICLE;
  }

  if (
    peak >= THRESHOLD_TREE_CUTTING
  ) {
    return EVENT_TREE_CUTTING;
  }

  return EVENT_AMBIENT;
}

// ============================================================
// SEND LORA PACKET
// ============================================================

void sendLoRaPacket(
  uint8_t eventType,
  uint16_t peak
) {

  AcousticStreamPacket packet;

  packet.node_id =
    NODE_ID;

  packet.sound_peak =
    peak;

  packet.event_type =
    eventType;

  // ----------------------------------------------------------
  // GPS
  // ----------------------------------------------------------

  if (
    gps.location.isValid()
  ) {

    packet.latitude =
      gps.location.lat();

    packet.longitude =
      gps.location.lng();
  }

  else {

    // 0,0 means GPS FIX NOT AVAILABLE
    packet.latitude =
      0.0;

    packet.longitude =
      0.0;
  }

  packet.timestamp_sec =
    millis() / 1000;

  // ----------------------------------------------------------
  // Send LoRa
  // ----------------------------------------------------------

  LoRa.beginPacket();

  LoRa.write(
    (uint8_t*)&packet,
    sizeof(packet)
  );

  LoRa.endPacket();

  // ----------------------------------------------------------
  // Serial Debug
  // ----------------------------------------------------------

  Serial.println();
  Serial.println("------------------------------------------");

  Serial.printf(
    "Node ID      : %d\n",
    packet.node_id
  );

  Serial.printf(
    "Event Code   : %d\n",
    packet.event_type
  );

  Serial.printf(
    "Sound Peak   : %u\n",
    packet.sound_peak
  );

  if (
    gps.location.isValid()
  ) {

    Serial.printf(
      "GPS          : %.6f, %.6f\n",
      packet.latitude,
      packet.longitude
    );
  }

  else {

    Serial.println(
      "GPS          : No Fix"
    );
  }

  Serial.printf(
    "Uptime       : %u sec\n",
    packet.timestamp_sec
  );

  Serial.println("------------------------------------------");
}
