#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_timer.h"
#include <stdint.h>
#include <stdio.h>

#define PLAYERS 4
#define START_TIME 840000 //ms -> 14minutes
#define INCREMENT 10000

bool buttonPressed = false;

void timer_task(void* args) {
    int32_t playerTime[PLAYERS] = {START_TIME, START_TIME, START_TIME, START_TIME};
    int currentPlayer = 0;
    int64_t last_time = esp_timer_get_time();

    while(1) {
        playerTime[currentPlayer] -= 10;

        if(playerTime[currentPlayer] <= 0) {
            playerTime[currentPlayer] = INCREMENT;
            currentPlayer = (currentPlayer + 1) % PLAYERS;
        } else if(buttonPressed) {
            playerTime[currentPlayer] += INCREMENT;
            currentPlayer = (currentPlayer + 1) % PLAYERS;
        }

        printf("Current Player: %d\n", currentPlayer);
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

void app_main(void) {
    xTaskCreate(timer_task, "timer_task", 2048, NULL, 1, NULL);
}