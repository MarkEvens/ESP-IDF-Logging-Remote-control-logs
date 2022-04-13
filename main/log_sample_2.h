#ifndef LOG_SAMPLE_2_H
#define LOG_SAMPLE_2_H

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"

static const char *SAMPLE_2_TAG = "SAMPLE 2";

void print_sample_2() {
  ESP_LOGE(SAMPLE_2_TAG, "This is error log");
  ESP_LOGW(SAMPLE_2_TAG, "This is warning log");
  ESP_LOGI(SAMPLE_2_TAG, "This is info log");
  ESP_LOGD(SAMPLE_2_TAG, "This is debug log");
  ESP_LOGV(SAMPLE_2_TAG, "This is verbos log");
}

#endif