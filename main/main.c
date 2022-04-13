#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_log.h"
#include "esp_mac.h"
#include "esp_partition.h"
#include "esp_spi_flash.h"
#include "esp_spiffs.h"
#include "esp_system.h"
#include "esp_vfs.h"
#include "esp_wifi.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/err.h"
#include "lwip/sys.h"
#include "nvs_flash.h"
#include "sdkconfig.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/unistd.h>

#include "log_remote_control.h"

static const char *TAG = "MAIN";

#define ESP_WIFI_SSID "ESP_Server"
#define ESP_WIFI_PASS "123456789"
#define ESP_WIFI_CHANNEL 1
#define MAX_STA_CONN 5

extern const unsigned char index_start[] asm("_binary_index_html_start");
extern const unsigned char index_end[] asm("_binary_index_html_end");

static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  if (event_id == WIFI_EVENT_AP_STACONNECTED) {
    wifi_event_ap_staconnected_t *event =
        (wifi_event_ap_staconnected_t *)event_data;
    ESP_LOGI(TAG, "station " MACSTR " join, AID=%d", MAC2STR(event->mac),
             event->aid);
  } else if (event_id == WIFI_EVENT_AP_STADISCONNECTED) {
    wifi_event_ap_stadisconnected_t *event =
        (wifi_event_ap_stadisconnected_t *)event_data;
    ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d", MAC2STR(event->mac),
             event->aid);
  }
}

void wifi_init_softap(void) {
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(
      WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

  wifi_config_t wifi_config = {
      .ap =
          {
              .ssid = ESP_WIFI_SSID,
              .ssid_len = strlen(ESP_WIFI_SSID),
              .channel = ESP_WIFI_CHANNEL,
              .password = ESP_WIFI_PASS,
              .max_connection = MAX_STA_CONN,
              .authmode = WIFI_AUTH_WPA_WPA2_PSK,
          },
  };
  if (strlen(ESP_WIFI_PASS) == 0) {
    wifi_config.ap.authmode = WIFI_AUTH_OPEN;
  }

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &wifi_config));
  ESP_ERROR_CHECK(esp_wifi_start());

  ESP_LOGI(TAG, "wifi_init_softap finished. SSID:%s password:%s channel:%d",
           ESP_WIFI_SSID, ESP_WIFI_PASS, ESP_WIFI_CHANNEL);
}

static esp_err_t print1error_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "print1error API called");
  set_sample_1_log_level(ESP_LOG_ERROR);
  print_sample_1();
  const size_t index_size = (index_end - index_start);
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send_chunk(req, (const char *)index_start, index_size);
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static esp_err_t print1warning_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "print1warning API called");
  set_sample_1_log_level(ESP_LOG_WARN);
  print_sample_1();
  const size_t index_size = (index_end - index_start);
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send_chunk(req, (const char *)index_start, index_size);
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static esp_err_t print2info_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "print2info API called");
  set_sample_2_log_level(ESP_LOG_INFO);
  print_sample_2();
  const size_t index_size = (index_end - index_start);
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send_chunk(req, (const char *)index_start, index_size);
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static esp_err_t print2debug_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "print2debug API called");
  set_sample_2_log_level(ESP_LOG_DEBUG);
  print_sample_2();
  const size_t index_size = (index_end - index_start);
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send_chunk(req, (const char *)index_start, index_size);
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static esp_err_t print1disable_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "print1disable API called");
  set_sample_1_log_level(ESP_LOG_NONE);
  set_sample_2_log_level(ESP_LOG_VERBOSE);
  print_sample_1();
  print_sample_2();
  const size_t index_size = (index_end - index_start);
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send_chunk(req, (const char *)index_start, index_size);
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static esp_err_t print2disable_get_handler(httpd_req_t *req) {
  ESP_LOGI(TAG, "print2disable API called");
  set_sample_1_log_level(ESP_LOG_VERBOSE);
  set_sample_2_log_level(ESP_LOG_NONE);
  print_sample_1();
  print_sample_2();
  const size_t index_size = (index_end - index_start);
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send_chunk(req, (const char *)index_start, index_size);
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static const httpd_uri_t print1error = {
    .uri = "/print1error",
    .method = HTTP_GET,
    .handler = print1error_get_handler,
    .user_ctx = NULL,
};

static const httpd_uri_t print1warning = {
    .uri = "/print1warning",
    .method = HTTP_GET,
    .handler = print1warning_get_handler,
    .user_ctx = NULL,
};

static const httpd_uri_t print2info = {
    .uri = "/print2info",
    .method = HTTP_GET,
    .handler = print2info_get_handler,
    .user_ctx = NULL,
};

static const httpd_uri_t print2debug = {
    .uri = "/print2debug",
    .method = HTTP_GET,
    .handler = print2debug_get_handler,
    .user_ctx = NULL,
};

static const httpd_uri_t print1disable = {
    .uri = "/print1disable",
    .method = HTTP_GET,
    .handler = print1disable_get_handler,
    .user_ctx = NULL,
};

static const httpd_uri_t print2disable = {
    .uri = "/print2disable",
    .method = HTTP_GET,
    .handler = print2disable_get_handler,
    .user_ctx = NULL,
};

static esp_err_t root_get_handler(httpd_req_t *req) {
  const size_t index_size = (index_end - index_start);
  httpd_resp_set_type(req, "text/html");
  httpd_resp_send_chunk(req, (const char *)index_start, index_size);
  httpd_resp_send_chunk(req, NULL, 0);
  return ESP_OK;
}

static const httpd_uri_t root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL,
};

static httpd_handle_t start_webserver(void) {
  httpd_handle_t server = NULL;
  httpd_config_t config = HTTPD_DEFAULT_CONFIG();
  config.max_resp_headers = 1024;
  config.lru_purge_enable = true;

  // Start the httpd server
  ESP_LOGI(TAG, "Starting server on port: '%d'", config.server_port);
  if (httpd_start(&server, &config) == ESP_OK) {
    // Set URI handlers
    ESP_LOGI(TAG, "Registering URI handlers");
    httpd_register_uri_handler(server, &root);
    httpd_register_uri_handler(server, &print1error);
    httpd_register_uri_handler(server, &print1warning);
    httpd_register_uri_handler(server, &print2info);
    httpd_register_uri_handler(server, &print2debug);
    httpd_register_uri_handler(server, &print1disable);
    httpd_register_uri_handler(server, &print2disable);
    return server;
  }

  ESP_LOGI(TAG, "Error starting server!");
  return NULL;
}

static void stop_webserver(httpd_handle_t server) {
  // Stop the httpd server
  httpd_stop(server);
}

static void disconnect_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data) {
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server) {
    ESP_LOGI(TAG, "Stopping webserver");
    stop_webserver(*server);
    *server = NULL;
  }
}

static void connect_handler(void *arg, esp_event_base_t event_base,
                            int32_t event_id, void *event_data) {
  httpd_handle_t *server = (httpd_handle_t *)arg;
  if (*server == NULL) {
    ESP_LOGI(TAG, "Starting webserver");
    *server = start_webserver();
  }
}

void app_main(void) {

  // Initialize NVS
  esp_err_t ret = nvs_flash_init();
  if (ret == ESP_ERR_NVS_NO_FREE_PAGES ||
      ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    ESP_ERROR_CHECK(nvs_flash_erase());
    ret = nvs_flash_init();
  }
  ESP_ERROR_CHECK(ret);

  ESP_LOGI(TAG, "ESP_WIFI_MODE_AP");
  wifi_init_softap();

  start_webserver();

  ESP_LOGE(TAG, "This is error log");
  ESP_LOGW(TAG, "This is warning log");
  ESP_LOGI(TAG, "This is info log");
  ESP_LOGD(TAG, "This is debug log");
  ESP_LOGV(TAG, "This is verbos log");

}
