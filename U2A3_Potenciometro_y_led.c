#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_adc/adc_oneshot.h"
#include "driver/ledc.h"

#define PIN_POTENCIOMETRO ADC_CHANNEL_4 
#define PIN_LED 5

void app_main(void) {
    // Inicializacion del ADC
    adc_oneshot_unit_handle_t control_adc;
    adc_oneshot_unit_init_cfg_t config_inicial_adc = {
        .unit_id = ADC_UNIT_1, //Unidad 1 del ADC
        .clk_src = 0, 
    };
    adc_oneshot_new_unit(&config_inicial_adc, &control_adc);
    //Configuracion del canal analogico especifico
    adc_oneshot_chan_cfg_t config_canal = {
        .bitwidth = ADC_BITWIDTH_DEFAULT, 
        .atten = ADC_ATTEN_DB_12,         
    };
    adc_oneshot_config_channel(control_adc, PIN_POTENCIOMETRO, &config_canal);
    // Configuracion del temporizador
    ledc_timer_config_t config_tiempo_led = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = LEDC_TIMER_12_BIT, //emparejamiento a los 12 bits del adc
        .freq_hz          = 5000, // frecuencia imperceptible
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ledc_timer_config(&config_tiempo_led);
    // Configuracion del canal PWM y conexion al pin del led
    ledc_channel_config_t config_canal_led = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = PIN_LED, //pin del esp
        .duty           = 0,  //se inicia con el led apagado
        .hpoint         = 0
    };
    ledc_channel_config(&config_canal_led);

    // guarda la posicion del potenciometro
    int valor_leido = 0;
    while (1) {
        // leer el valor entre 0 y 4095
        adc_oneshot_read(control_adc, PIN_POTENCIOMETRO, &valor_leido);
        //aplicar el valor leido al brillo del led
        ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, valor_leido);
        ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0); //hacer el cambio

        vTaskDelay(pdMS_TO_TICKS(20));
    }
}