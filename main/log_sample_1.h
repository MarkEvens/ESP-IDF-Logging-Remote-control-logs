#ifndef LOG_SAMPLE_1_H
#define LOG_SAMPLE_1_H

#define LOG_LOCAL_LEVEL ESP_LOG_DEBUG
#include "esp_log.h"

static const char *SAMPLE_1_TAG = "SAMPLE 1";

void print_sample_1() {
  ESP_LOGE(SAMPLE_1_TAG, "This is error log");
  ESP_LOGW(SAMPLE_1_TAG, "This is warning log");
  ESP_LOGI(SAMPLE_1_TAG, "This is info log");
  ESP_LOGD(SAMPLE_1_TAG, "This is debug log");
  ESP_LOGV(SAMPLE_1_TAG, "This is verbos log");
}

#endif






