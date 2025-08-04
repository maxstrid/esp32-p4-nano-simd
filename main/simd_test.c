#include "esp_log.h"
#include "esp_random.h"
#include "esp_timer.h"
#include "float.h"

// dsp includes (esp-dsp contains a simd frontend)
#include "dspm_mult.h"
#include "esp_dsp.h"

#define MAT_SIZE 12

static const char *TAG = "main";

// The idea here is that we're profiling the simd instructions by running the
// f32 ansi which runs a naive mat mul algorithm, then running the f32 arp4
// function which should be optimized for the p4.

void fill_matrix(float matrix[MAT_SIZE * MAT_SIZE]) {
  for (int i = 0; i < MAT_SIZE * MAT_SIZE; ++i) {
    // Scale random float to [0.0, FLT_MAX]
    matrix[i] = ((float)(esp_random() & 0xFFFFFF) / 0xFFFFFF) * 1000;
  }
}

void app_main(void) {
  __attribute__((aligned(16))) float A[MAT_SIZE * MAT_SIZE];
  __attribute__((aligned(16))) float B[MAT_SIZE * MAT_SIZE];

  fill_matrix(A);
  fill_matrix(B);

  int64_t ansi_time_accum = 0;
  for (int i = 0; i < 10; i++) {
    {
      __attribute__((aligned(16))) float C[MAT_SIZE * MAT_SIZE];

      int64_t start_us = esp_timer_get_time();

      dspm_mult_f32_ansi(A, B, C, MAT_SIZE, MAT_SIZE, MAT_SIZE);

      int64_t end_us = esp_timer_get_time();

      ansi_time_accum += end_us - start_us;
    }
  }

  // 101us
  ESP_LOGI(TAG, "dspm_mult_f32_ansi: %lld us average", ansi_time_accum / 10);

  int64_t arp4_time_accum = 0;
  for (int i = 0; i < 10; i++) {
    {
      __attribute__((aligned(16))) float C[MAT_SIZE * MAT_SIZE];

      int64_t start_us = esp_timer_get_time();

      dspm_mult_f32_arp4(A, B, C, MAT_SIZE, MAT_SIZE, MAT_SIZE);

      int64_t end_us = esp_timer_get_time();

      arp4_time_accum += end_us - start_us;
    }
  }

  // 35us
  ESP_LOGI(TAG, "dspm_mult_f32_arp4: %lld us average", arp4_time_accum / 10);
}
