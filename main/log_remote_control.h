#ifndef LOG_REMOTE_CONTROL_H
#define LOG_REMOTE_CONTROL_H

#include "log_sample_1.h"
#include "log_sample_2.h"

void disable_sample_1_log() 
{ 
    esp_log_level_set(SAMPLE_1_TAG, ESP_LOG_NONE);
}

void disable_sample_2_log() 
{ 
    esp_log_level_set(SAMPLE_2_TAG, ESP_LOG_NONE);
}

void set_sample_1_log_level(esp_log_level_t level) 
{
    esp_log_level_set(SAMPLE_1_TAG, level);
}

void set_sample_2_log_level(esp_log_level_t level)
{
    esp_log_level_set(SAMPLE_2_TAG, level);
}

#endif