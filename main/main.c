#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "wifi_tcp_server.h"

void app_main(void)
{
	wifi_connection();
	vTaskDelay(1000 / portTICK_PERIOD_MS);
	xTaskCreate(tcp_server_task, "tcp_server", 4096, (void *)AF_INET, 5, NULL);
}


