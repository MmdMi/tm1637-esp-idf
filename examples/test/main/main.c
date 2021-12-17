#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_log.h"
#include "soc/timer_group_struct.h"
#include "soc/timer_group_reg.h"
#include "tm1637.h"

void app_main(void)
{
    // Initialize 7-segment display
    tm1637__7seg_t *lcd = tm1637_init(CONFIG_TM1637_CLK, CONFIG_TM1637_DIO);

    // Turn on all segments
    tm1637_set_value(lcd, 8888);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    // Print [ 0 ~ 9999 ]
    for (int i = 0; i < 10000; i++)
    {
        tm1637_set_value(lcd, i);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }

    // Change brightness [ 0 ~ 7 ]
    for (int i = 0; i < 7; i++)
    {
        tm1637_set_brightness(lcd, i);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }

    // Test done
    while (1)
    {
        tm1637_turnoff(lcd);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        tm1637_set_value(lcd, 9999);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}
