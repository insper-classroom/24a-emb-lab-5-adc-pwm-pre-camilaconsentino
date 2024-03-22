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

//     while (true) {
//         if (xQueueReceive(xQueueData, &data, 100)) {
//             // Adiciona o novo valor à janela
//             window[window_index] = data;
//             window_index = (window_index + 1) % 5;
//             window_count++;

//             // Verifica se a janela está completa
//             if (window_count == 5) {
//                 // Calcula a média móvel
//                 int sum = 0;
//                 for (int i = 0; i < 5; i++) {
//                     sum += window[i];
//                 }
//                 int filtered_data = sum / 5;

//                 // Imprime o dado filtrado na UART
//                 printf("%d\n", filtered_data);

//                 // Deixa este atraso
//                 vTaskDelay(pdMS_TO_TICKS(50));
//                 window_count = 0;
//             }
//         }
//     }
// }

void process_task(void *p) {
    int data = 0;
    int v[5];
    int media=0;
    while (true) {
        if (xQueueReceive(xQueueData, &data, 100)) {
            // implementar filtro aqui!
            v[0]=v[1];
            v[1]=v[2];
            v[2]=v[3];
            v[3]=v[4];
            v[4]=data;
            
            media=((v[4]+v[3]+v[2]+v[1]+v[0])/5);
            printf("%d\n",media);
        
            // deixar esse delay!
           
            vTaskDelay(pdMS_TO_TICKS(50));

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
