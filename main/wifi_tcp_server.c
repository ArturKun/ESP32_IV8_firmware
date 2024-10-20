#include "wifi_tcp_server.h"

#define PORT 3333
static const char *TAG = "TCP_SOCKET";

void tcp_server_task(void *pvParameters)
{
	char addr_str[128];
	char rx_buffer[128];
	int keepAlive = 1;
	int keepIdle = 5;
	int keepInterval = 5;
	int keepCount = 3;
	struct sockaddr_storage dest_addr;

	struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
	dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
	dest_addr_ip4->sin_family = AF_INET;
	dest_addr_ip4->sin_port = htons(PORT);

	// Open socket
	int listen_sock = socket(AF_INET, SOCK_STREAM, 0); // 0 for TCP Protocol
	int opt = 1;
	setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

	ESP_LOGI(TAG, "Socket created");

	bind(listen_sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
	ESP_LOGI(TAG, "Socket bound, port %d", PORT);

	// Listen to the socket
	listen(listen_sock, 1);

	while (1)
	{
		ESP_LOGI(TAG, "Socket listening");

		struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
		socklen_t addr_len = sizeof(source_addr);

		// Accept socket
		int sock = accept(listen_sock, (struct sockaddr *)&source_addr, &addr_len);
		if (sock < 0)
		{
			ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
			break;
		}

		// Set tcp keepalive option
		setsockopt(sock, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
		setsockopt(sock, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
		setsockopt(sock, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
		setsockopt(sock, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
		// Convert ip address to string
		if (source_addr.ss_family == PF_INET)
		{
			inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
		}

		ESP_LOGI(TAG, "Socket accepted ip address: %s", addr_str);

		int len;
		do
		{
			len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);

			int to_write = len;

			while (to_write > 0)
			{
				int written = send(sock, rx_buffer + (len - to_write), to_write, 0);
				if (written < 0)
				{
					ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
					// Failed to retransmit, giving up
					return;
				}
				to_write -= written;
			}
		} while(len > 0);

		shutdown(sock, 0);
		close(sock);
	}

	close(listen_sock);
	vTaskDelete(NULL);
}

void wifi_event_handler(void *event_handler_arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
	switch (event_id)
	{
	case WIFI_EVENT_STA_START:
		printf("WiFi connecting WIFI_EVENT_STA_START ... \n");
		break;
	case WIFI_EVENT_STA_CONNECTED:
		printf("WiFi connected WIFI_EVENT_STA_CONNECTED ... \n");
		break;
	case WIFI_EVENT_STA_DISCONNECTED:
		printf("WiFi lost connection WIFI_EVENT_STA_DISCONNECTED ... \n");
		break;
	case IP_EVENT_STA_GOT_IP:
		printf("WiFi got IP ... \n\n");
		break;
	default:
		break;
	}
}

void wifi_connection()
{
	nvs_flash_init();
	esp_netif_init();
	esp_event_loop_create_default();
	esp_netif_create_default_wifi_sta();
	wifi_init_config_t wifi_initiation = WIFI_INIT_CONFIG_DEFAULT();
	esp_wifi_init(&wifi_initiation);
	esp_event_handler_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler, NULL);
	esp_event_handler_register(IP_EVENT, IP_EVENT_STA_GOT_IP, wifi_event_handler, NULL);
	wifi_config_t wifi_configuration = {
			.sta = {
					.ssid = CONFIG_SSID,
					.password = CONFIG_PASSWORD}};
	esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_configuration);
	esp_wifi_set_mode(WIFI_MODE_STA);
	esp_wifi_start();
	esp_wifi_connect();
}

