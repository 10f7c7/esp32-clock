#include "driver/i2s.h"
#include <math.h>

// I2S pin configuration for ESP32-S3


#define I2S_DOUT      2
#define I2S_BCLK      1
#define I2S_LRC       8

#define SAMPLE_RATE 44100
#define PI 3.14159265

// Notes and durations
typedef struct {
    float frequency;
    int duration_ms;
} Note;

Note melody[] = {
  { 440.0, 500 },  // A4
  { 494.0, 500 },  // B4
  { 523.3, 500 },  // C5
  { 587.3, 500 },  // D5
  { 659.3, 500 },  // E5
  { 698.5, 500 },  // F5
  { 784.0, 500 },  // G5
  { 880.0, 700 },  // A5 (longer)
  { 0, 300 }       // pause
};

void setupI2S() {
    const i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
      .sample_rate = SAMPLE_RATE,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = 64,
      .use_apll = false,
      .tx_desc_auto_clear = true,
      .fixed_mclk = 0
    };

    const i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_BCLK,
      .ws_io_num = I2S_LRC,
      .data_out_num = I2S_DOUT,
      .data_in_num = I2S_PIN_NO_CHANGE
    };

    i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_NUM_0, &pin_config);
    i2s_zero_dma_buffer(I2S_NUM_0);
}

void playTone(float freq, int duration_ms) {
    const int samples = 256;
    int16_t buffer[samples];
    float phase = 0.0;
    float phaseIncrement = 2 * PI * freq / SAMPLE_RATE;

    unsigned long total_samples = (SAMPLE_RATE * duration_ms) / 1000;
    size_t bytes_written;

    for (unsigned long i = 0; i < total_samples; i += samples) {
        for (int j = 0; j < samples; j++) {
            if (freq == 0) {
                buffer[j] = 0;  // pause
            }
            else {
                buffer[j] = (int16_t)(sin(phase) * 32767);
                phase += phaseIncrement;
                if (phase >= 2 * PI) phase -= 2 * PI;
            }
        }
        i2s_write(I2S_NUM_0, buffer, sizeof(buffer), &bytes_written, portMAX_DELAY);
    }
}

void play() {
    // Serial.begin(115200);
    setupI2S();
    // Serial.println("Playing melody...");

    int numNotes = sizeof(melody) / sizeof(melody[0]);
    for (int i = 0; i < numNotes; i++) {
        playTone(melody[i].frequency, melody[i].duration_ms);
    }

    // Serial.println("Done!");
}