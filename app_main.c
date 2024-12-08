/* MQTT over SSL Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_system.h"
#include "esp_partition.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "esp_tls.h"
#include "esp_ota_ops.h"
#include <sys/param.h>

#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
int timerId = 1;
int actuals;
int interval = 0;
TimerHandle_t xTimers;
#define estado1 1
#define estado2 2
#define estado3 3
#define estado4 4
#define estado5 5
int msj_mqtt;
unsigned int btn;
unsigned int LEVEL;
#define true 1
#define false 0

#define ledR 44

#define STACK_SIZE 4028

char TAG[5];

// Semáforos para controlar las tareas

// Funciones de inicialización
esp_err_t init_led(void);
esp_err_t create_tasks(void);
esp_err_t create_timer(void);
// Prototipos de tareas
void vTaskR(void *pvParameters);
void vTaskG(void *pvParameters);
esp_mqtt_client_handle_t client;
static void mqtt_event_handler();

void vTimerCall(TimerHandle_t pxtimer)
{
    {

    if (actuals == estado1)
    {
        LEVEL = false;
        gpio_set_level(ledR, false);
    }
    if (actuals != estado1)
    {
        LEVEL = !LEVEL;
        gpio_set_level(ledR, LEVEL);
    }
    printf("El estado del led es: %i\n", ledR);
    ESP_LOGI(TAG, "El valor de intervalos es %i", interval);
}
}
// Función de callback del evento MQTT
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%" PRIi32, base, event_id);
    esp_mqtt_event_handle_t event = event_data;
    client = event->client;
    int msg_id;
    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED:
       ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        msg_id = esp_mqtt_client_subscribe(client, "UNO", 0);
        ESP_LOGI(TAG, "Enviado subscribe exitoso, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "DOS", 0);
        ESP_LOGI(TAG, "Enviado subscribe exitoso, msg_id=%d", msg_id);

        msg_id = esp_mqtt_client_subscribe(client, "TRES", 0);
        ESP_LOGI(TAG, "Enviado subscribe exitoso, msg_id=%d", msg_id);
        break;
         
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;

    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:

        char topico_MQTT[100];
        strncpy(topico_MQTT, event->topic, event->topic_len);
        topico_MQTT[event->topic_len] = '\0';

        char dato_MQTT[100];
        strncpy(dato_MQTT, event->data, event->data_len);
        dato_MQTT[event->data_len] = '\0';

       
        break;

    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT)
        {
            ESP_LOGI(TAG, "Last error code reported from esp-tls: 0x%x", event->error_handle->esp_tls_last_esp_err);
            ESP_LOGI(TAG, "Last tls stack error number: 0x%x", event->error_handle->esp_tls_stack_err);
            ESP_LOGI(TAG, "Last captured errno : %d (%s)", event->error_handle->esp_transport_sock_errno,
                     strerror(event->error_handle->esp_transport_sock_errno));
        }
        else if (event->error_handle->error_type == MQTT_ERROR_TYPE_CONNECTION_REFUSED)
        {
            ESP_LOGI(TAG, "Connection refused error: 0x%x", event->error_handle->connect_return_code);
        }
        else
        {
            ESP_LOGW(TAG, "Unknown error type: 0x%x", event->error_handle->error_type);
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}
// Configuración e inicio del cliente MQTT

static void mqtt_app_start(void)
{
    const esp_mqtt_client_config_t mqtt_cfg = {
        .broker = {
            .address.uri = CONFIG_BROKER_URI},
    };

ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
/* The last argument may be used to pass data to the event handler, in this example mqtt_event_handler */
esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
esp_mqtt_client_start(client);
}

// TAREAS AL SERIAL MQTT
void vTaskR(void *pvParameters)
{
    msj_mqtt = 0;

    if (actuals == estado1)
    {
        msj_mqtt = esp_mqtt_client_publish(client, "UNO", "estado1", 0, 0, 0);
    }

    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        if (btn == estado1)
        {
            if (actuals == estado2)
            {
                msj_mqtt = esp_mqtt_client_publish(client, "UNO", "estado2", 0, 0, 0);
            }

            if (actuals == estado3)
            {
                msj_mqtt = esp_mqtt_client_publish(client, "UNO", "estado3", 0, 0, 0);
            }

            if (actuals == estado4)
            {
                msj_mqtt = esp_mqtt_client_publish(client, "UNO", "estado4", 0, 0, 0);
            }

            if (actuals == estado5)
            {
                msj_mqtt = esp_mqtt_client_publish(client, "UNO", "estado5", 0, 0, 0);
            }
        }
    }
}

/*void vTaskG(void *pvParameters)
{
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
*/
// Inicialización de los LEDs
esp_err_t init_led(void)
{
    gpio_reset_pin(ledR);
    gpio_set_direction(ledR, GPIO_MODE_OUTPUT);

    return ESP_OK;
}

// Creación de las tareas
esp_err_t create_tasks(void)
{
    static uint8_t ucParameterToPass;
    TaskHandle_t xHandle = NULL;
    
    xTaskCreate(vTaskR,
                "vTaskR", 
                STACK_SIZE,
                &ucParameterToPass,
                2,
                &xHandle);

   /* xTaskCreate(vTaskG,
                "vTaskG",
                STACK_SIZE,
                &ucParameterToPass,
                2,
                &xHandle);
                */
    return ESP_OK;


}
void app_main(void)
{

    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %" PRIu32 " bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());
    esp_log_level_set("*", ESP_LOG_INFO);

    esp_log_level_set("esp-tls", ESP_LOG_VERBOSE);
    esp_log_level_set("mqtt_client", ESP_LOG_VERBOSE);
    esp_log_level_set("mqtt_example", ESP_LOG_VERBOSE);
    esp_log_level_set("transport_base", ESP_LOG_VERBOSE);
    esp_log_level_set("transport", ESP_LOG_VERBOSE);
    esp_log_level_set("outbox", ESP_LOG_VERBOSE);

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    ESP_ERROR_CHECK(example_connect());

    init_led();

    // Crear semáforos binarios

    // Crear tareas
    create_tasks();

    // Iniciar cliente MQTT
    mqtt_app_start();
    actuals = estado1;
    btn = false;
    ESP_LOGI(TAG, "SALIO");
    vTaskDelay(pdMS_TO_TICKS(2000));
    while (1)
    { // MACHINE

        if (btn == true)
        {
            actuals++;
            if (actuals > estado5)
            {
                actuals = estado1;
            }

            if (actuals == estado2)
            {
                interval = 500;
                create_timer();
                ESP_LOGI(TAG, "ESTADO DOS");
            }

            if (actuals == estado3)
            {
                interval = 100;
                create_timer();
                ESP_LOGI(TAG, "ESTADO TRES");
            }

            if (actuals == estado4)
            {
                interval = 1000;
                create_timer();
                ESP_LOGI(TAG, "ESTADO CUATRO");
            }

            if (actuals == estado5)
            {
                interval = 250;
                create_timer();
                ESP_LOGE(TAG, "ESTADO CINCO");
            }
        }
    }
}
// TIMER
esp_err_t create_timer(void)
{
    ESP_LOGI(TAG, "config timer");
    xTimers = xTimerCreate("Timer",                   // Just a text name, not used by the kernel.
                           (pdMS_TO_TICKS(interval)), // The timer period in ticks.
                           pdTRUE,                    // The timers will auto-reload themselves when they expire.
                           (void *)timerId,           // Assign each timer a unique id equal to its array index.
                           vTimerCall);
    if (xTimers == NULL)
    {
        // The timer was not created.
        ESP_LOGI(TAG, "The timer was not created.");
    }
    else
    {
        // Start the timer.  No block time is specified, and even if one was
        // it would be ignored because the scheduler has not yet been
        // started.
        if (xTimerStart(xTimers, 0) != pdPASS)
        {
            // The timer could not be set into the Active state.
            ESP_LOGI(TAG, "The timer could not be set into the Active st");
        }
    }
    return ESP_OK;
}