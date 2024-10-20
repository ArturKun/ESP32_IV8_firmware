#ifndef MAIN_WIFI_TCP_SERVER_H_
#define MAIN_WIFI_TCP_SERVER_H_

#include <stdio.h>
#include <string.h>

#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_check.h"
#include "esp_mac.h"
#include "esp_eth.h"
#include "esp_netif.h"
#include "esp_http_server.h"
#include "esp_http_client.h"
#include "esp_event.h"
#include "esp_system.h"

#include "lwip/inet.h"
#include "lwip/netdb.h"
#include "lwip/sockets.h"
#include "lwip/ip_addr.h"

#include "nvs_flash.h"
#include "ping/ping_sock.h"

void tcp_server_task(void *pvParameters);

void wifi_event_handler(void *event_handler_arg,
		esp_event_base_t event_base,
		int32_t event_id,
		void *event_data);

void wifi_connection();

#endif /* MAIN_WIFI_TCP_SERVER_H_ */
