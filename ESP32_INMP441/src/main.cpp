#include <Arduino.h>
#include <driver/i2s.h>

// --- Pins ---
#define I2S_WS   25   // Word Select (LRCLK)
#define I2S_SD   32   // Serial Data (DOUT du micro)
#define I2S_SCK  33   // Serial Clock (BCLK)

#define SAMPLE_RATE     16000
#define I2S_PORT        I2S_NUM_0
#define BUFFER_LEN      512

int32_t samples[BUFFER_LEN];

// Seuil d'amplitude au-delà duquel on considère qu'il y a du son
// À ajuster selon ton environnement
const int32_t THRESHOLD = 2;

void setupI2S() {
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = BUFFER_LEN,
    .use_apll = false
  };

  i2s_pin_config_t pin_config = {
    .bck_io_num = I2S_SCK,
    .ws_io_num = I2S_WS,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_SD
  };

  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
}

void setup() {
  Serial.begin(115200);
  setupI2S();
  Serial.println("INMP441 pret.");
}

void loop() {
  size_t bytes_read = 0;
  i2s_read(I2S_PORT, samples, sizeof(samples), &bytes_read, portMAX_DELAY);

  int samples_read = bytes_read / sizeof(int32_t);

  int64_t sum = 0;
  for (int i = 0; i < samples_read; i++) {
    sum += abs(samples[i]);
  }
  int32_t avgAmplitude = sum / samples_read;

  // Decommente pour calibrer ton seuil :
  Serial.println(avgAmplitude);

  if (avgAmplitude > THRESHOLD) {
    Serial.println("SON DETECTE");
  }
}