
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "switch_driver.h"


static QueueHandle_t gpio_evt_queue = NULL;

static switch_func_pair_t *switch_func_pair;

static esp_switch_callback_t func_ptr;

static uint8_t switch_num;
static const char *TAG = "ESP_ZB_SWITCH";

static void IRAM_ATTR gpio_isr_handler(void *arg)
{
    xQueueSendFromISR(gpio_evt_queue, (switch_func_pair_t *)arg, NULL);
}

static void switch_driver_gpios_intr_enabled(bool enabled)
{
    for (int i = 0; i < switch_num; ++i) {
        if (enabled) {
            gpio_intr_enable((switch_func_pair + i)->pin);
        } else {
            gpio_intr_disable((switch_func_pair + i)->pin);
        }
    }
}


static void switch_driver_button_detected(void *arg)
{
    gpio_num_t io_num = GPIO_NUM_NC;
    switch_func_pair_t button_func_pair;
    static switch_state_t switch_state = SWITCH_IDLE;
    bool evt_flag = false;

    for (;;) {
        
        if (xQueueReceive(gpio_evt_queue, &button_func_pair, portMAX_DELAY)) {
            io_num =  button_func_pair.pin;
            switch_driver_gpios_intr_enabled(false);
            evt_flag = true;
        }
        while (evt_flag) {
            bool value = gpio_get_level(io_num);
            switch (switch_state) {
            case SWITCH_IDLE:
                switch_state = (value == GPIO_INPUT_LEVEL_ON) ? SWITCH_PRESS_DETECTED : SWITCH_IDLE;
                break;
            case SWITCH_PRESS_DETECTED:
                switch_state = (value == GPIO_INPUT_LEVEL_ON) ? SWITCH_PRESS_DETECTED : SWITCH_RELEASE_DETECTED;
                break;
            case SWITCH_RELEASE_DETECTED:
                switch_state = SWITCH_IDLE;
                /* callback to button_handler */
                (*func_ptr)(&button_func_pair);
                break;
            default:
                break;
            }
            if (switch_state == SWITCH_IDLE) {
                switch_driver_gpios_intr_enabled(true);
                evt_flag = false;
                break;
            }
            vTaskDelay(10 / portTICK_PERIOD_MS);
        }
    }
}


static bool switch_driver_gpio_init(switch_func_pair_t *button_func_pair, uint8_t button_num)
{
    gpio_config_t io_conf = {};
    switch_func_pair = button_func_pair;
    switch_num = button_num;
    uint64_t pin_bit_mask = 0;

    
    for (int i = 0; i < button_num; ++i) {
        pin_bit_mask |= (1ULL << (button_func_pair + i)->pin);
    }
    
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    io_conf.pin_bit_mask = pin_bit_mask;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pull_up_en = 1;
    
    gpio_config(&io_conf);
    
    gpio_evt_queue = xQueueCreate(10, sizeof(switch_func_pair_t));
    if ( gpio_evt_queue == 0) {
        ESP_LOGE(TAG, "Queue was not created and must not be used");
        return false;
    }
    
    xTaskCreate(switch_driver_button_detected, "button_detected", 2048, NULL, 10, NULL);
    
    gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
    for (int i = 0; i < button_num; ++i) {
        gpio_isr_handler_add((button_func_pair + i)->pin, gpio_isr_handler, (void *) (button_func_pair + i));
    }
    return true;
}

bool switch_driver_init(switch_func_pair_t *button_func_pair, uint8_t button_num, esp_switch_callback_t cb)
{
    if (!switch_driver_gpio_init(button_func_pair, button_num)) {
        return false;
    }
    func_ptr = cb;
    return true;
}
