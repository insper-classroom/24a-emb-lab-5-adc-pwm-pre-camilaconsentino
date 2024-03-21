#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <queue.h>

#include "pico/stdlib.h"
#include <stdio.h>

#include "data.h"
QueueHandle_t xQueueData;

// não mexer! Alimenta a fila com os dados do sinal
void data_task(void *p) {
    vTaskDelay(pdMS_TO_TICKS(400));

    int data_len = sizeof(sine_wave_four_cycles) / sizeof(sine_wave_four_cycles[0]);
    for (int i = 0; i < data_len; i++) {
        xQueueSend(xQueueData, &sine_wave_four_cycles[i], 1000000);
    }

    while (true) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}


// void process_task(void *p) {
//     int data = 0;
//     int window[5] = {0}; // Array para armazenar a janela
//     int window_index = 0; // Índice da janela
//     int window_count = 0; // Número de valores na janela

//     // Fila circular para armazenar os últimos 5 valores recebidos
//     int circular_buffer[5] = {0};
//     int circular_index = 0; // Índice da fila circular

//     while (true) {
//         if (xQueueReceive(xQueueData, &data, 100)) {
//             printf("Recebido: %d\n", data); // Depuração: Imprime o valor recebido

//             // Adiciona o novo valor à janela
//             window[window_index] = data;
//             window_index = (window_index + 1) % 5;
//             window_count++;

//             // Adiciona o novo valor à fila circular
//             circular_buffer[circular_index] = data;
//             circular_index = (circular_index + 1) % 5;

//             // Se a janela estiver completa, calcula a média móvel
//             if (window_count == 5) {
//                 printf("Janela completa.\n"); // Depuração: Indica que a janela está completa

//                 // Calcula a média móvel
//                 int sum = 0;
//                 for (int i = 0; i < 5; i++) {
//                     sum += circular_buffer[i];
//                 }
//                 int filtered_data = sum / 5;

//                 // Imprime o dado filtrado na UART
//                 printf("%d\n", filtered_data);

//                 // Deixa este atraso
//                 vTaskDelay(pdMS_TO_TICKS(50));

//                 // Resetar window_count após o cálculo da média móvel
//                 window_count = 0;
//             }
//         }
//     }
// }


void process_task(void *p) {
    int data = 0;
    int window[5] = {0}; // Array para armazenar a janela
    int window_index = 0; // Índice da janela
    int window_count = 0; // Número de valores na janela

    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // Adiciona o novo valor à janela
            window[window_index] = data;
            window_index = (window_index + 1) % 5;
            window_count++;

            // Verifica se a janela está completa
            if (window_count == 5) {
                // Calcula a média móvel
                int sum = 0;
                for (int i = 0; i < 5; i++) {
                    sum += window[i];
                }
                int filtered_data = sum / 5;

                // Imprime o dado filtrado na UART
                printf("%d\n", filtered_data);

                // Deixa este atraso
                vTaskDelay(pdMS_TO_TICKS(50));
                window_count = 0;
            }
        }
    }
}

int main() {
    stdio_init_all();

    xQueueData = xQueueCreate(64, sizeof(int));

    xTaskCreate(data_task, "Data task ", 4096, NULL, 1, NULL);
    xTaskCreate(process_task, "Process task", 4096, NULL, 1, NULL);

    vTaskStartScheduler();

    while (true)
        ;
}
