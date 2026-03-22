/*
 * SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: CC0-1.0
 */

 
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "driver/i2c_master.h"
#include "esp_err.h"
#include "esp_log.h"
#include "lvgl.h"
#include "esp_lvgl_port.h"

#include "driver/gpio.h"

static lv_obj_t *label;
static const char *TAG = "example";

// Timer constants
#define PLAYERS 4
#define START_TIME 840000
#define INCREMENT 10000
#define BUTTON_PIN 4

volatile int32_t playerTime[PLAYERS] = {START_TIME, START_TIME, START_TIME, START_TIME};
volatile int currentPlayer = 0;
volatile bool buttonPressed = false;

static void lvgl_update_timer(lv_timer_t *timer);

void timer_task(void* args) {
    int last_button_state = 1;
    while(1) {
        int button_state = gpio_get_level(BUTTON_PIN);
        if(last_button_state == 1 && button_state == 0) {
            buttonPressed = true;
        }
        last_button_state = button_state;
        playerTime[currentPlayer] -= 10;

        if(buttonPressed) {
            playerTime[currentPlayer] += INCREMENT;
            currentPlayer = (currentPlayer + 1) % PLAYERS;
            buttonPressed = false;
        }
        else if(playerTime[currentPlayer] <= 0) {
            playerTime[currentPlayer] = INCREMENT;
            currentPlayer = (currentPlayer + 1) % PLAYERS;
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void example_lvgl_demo_ui(lv_disp_t *disp)
{

    ESP_LOGI(TAG, "Running LVGL demo UI");
//     lv_obj_t *scr = lv_disp_get_scr_act(disp);
//     lv_obj_t *label = lv_label_create(scr);
//     lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR); /* Circular scroll */
//     lv_label_set_text(label, "Hello Espressif, Hello LVGL.");
//     /* Size of the screen (if you use rotation 90 or 270, please set disp->driver->ver_res) */
// #if LVGL_VERSION_MAJOR >= 9
//     lv_obj_set_width(label, lv_display_get_physical_horizontal_resolution(disp));
// #else
//     lv_obj_set_width(label, disp->driver->hor_res);
// #endif
//     lv_obj_align(label, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_t *scr = lv_disp_get_scr_act(disp);

    label = lv_label_create(scr);
    lv_label_set_text(label, "Initializing...");
    ESP_LOGI(TAG, "Label created");
    lv_obj_align(label, LV_ALIGN_CENTER, 0, 0);

    lv_timer_create(lvgl_update_timer, 100, NULL);  // update every 100 ms
}

static void lvgl_update_timer(lv_timer_t *timer)
{
    char buf[64];

    int player = currentPlayer;
    int time_ms = playerTime[player];

    int seconds = time_ms / 1000;
    int minutes = seconds / 60;
    seconds %= 60;

    snprintf(buf, sizeof(buf),
             "P%d %02d:%02d",
             player,
             minutes,
             seconds);

    lv_label_set_text(label, buf);
}