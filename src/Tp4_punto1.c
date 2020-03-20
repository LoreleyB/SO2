/*
 * Productor_Consumidor.c
 *
 *  Created on: 2 mar. 2020
 *      Author: loreley
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>
#include <stdlib.h>
/* Kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//Frecuencia en que se envian los datos a la cola 50milisegundos
#define QUEUE_PERIODO_MS (50 / portTICK_RATE_MS)

//Limite de elementos de la cola
#define MAX_QUEUE_LENGTH (5)
#define QUEUE_SIZE sizeof(char)

//Prioridades
#define CONSUMIDOR_PRIORIDAD (tskIDLE_PRIORITY + 1)
#define PRODUCTOR_PRIORIDAD (tskIDLE_PRIORITY + 2)

static void pConsumidor(void *pvParameters);
static void pProductor(void *pvParameters);
void vConfigureTimerForRunTimeStats(void);



static xQueueHandle xQueue = NULL;


int main(void)
{


	//Debe ser la primer línea y es necesaria para comenzar a grabar
	vTraceEnable(TRC_START);

	xQueue = xQueueCreate(MAX_QUEUE_LENGTH, QUEUE_SIZE);

	if (xQueue != NULL)
	{
		//Se crean las tareas
		xTaskCreate(pConsumidor, "Consumidor", configMINIMAL_STACK_SIZE, NULL, CONSUMIDOR_PRIORIDAD, NULL);
		xTaskCreate(pProductor, "Productor", configMINIMAL_STACK_SIZE, NULL, PRODUCTOR_PRIORIDAD, NULL);


		vTaskStartScheduler();
	}

	for (;;)
		;
}



/**
 * @brief Agrega un elemento a la cola compartida por las tareas.
 * Espera un tiempo mainQUEUE_SEND_FREQUENCY_MS para activarse.
 *
 * @param pvParameters
 */
static void pProductor(void *pvParameters)
{

	//Dato a enviar
	char msg[20];
	/*tarea que realiza una acción cada 50 milliseconds. */


	/** En este caso, xLastWakeTime retiene el tiempo en que la tarea salió por última vez
		 * del estado Bloqueado (se 'despertó')y se usa como punto de referencia para calcular
		 * el tiempo en el que la tarea debe salir del estado Bloqueado.. */
		portTickType xLastWakeTime;
		/* Initialize the variable used by the call to vTaskDelayUntil(). */
		xLastWakeTime = xTaskGetTickCount();
		/**The xLastWakeTime variable needs to be initialized with the current tick
		 * count. Note that this is the only time the variable is explicitly written to.
		 * After this xLastWakeTime is managed automatically by the vTaskDelayUntil() API function*/


	for(;;){
		strcpy(msg, "Hola Mundo!");
		vTaskDelayUntil(&xLastWakeTime, QUEUE_PERIODO_MS);
		/*The parameters to vTaskDelayUntil() specify, instead, the exact tick count value at which the
		calling task should be moved from the Blocked state into the Ready state.*/
			if( xQueueSendToBack( xQueue, &msg, 0 ) != pdPASS )
			{
				printf("No se pudo escribir en la cola\n");
			}
			printf("Guardo en la cola: %s\n", &msg);

	    }
}



static void pConsumidor(void *pvParameters)
{

	char msg[20];

	for (;;)
		{
			/*Lee e imprime error en caso de falla*/
			if( xQueueReceive( xQueue, &msg, portMAX_DELAY ) != pdPASS ){
						printf("No se leyó nada de la cola\n");
					} /*Setting xTicksToWait to portMAX_DELAY will cause the task to
						wait indefinitely (without timing out)*/
			printf("Saco mensaje de la cola \n");
		}
}


/**The LPC17xx demo application does not include the high frequency interrupt test,
 *  so portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() was used to configure the timer 0 peripheral
 *   to generate the time base. */

void vConfigureTimerForRunTimeStats(void)
{
	const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

	/* Power up and feed the timer. */
	LPC_SC->PCONP |= 0x02UL;
	LPC_SC->PCLKSEL0 = (LPC_SC->PCLKSEL0 & (~(0x3 << 2))) | (0x01 << 2);

	/* Reset Timer 0 */
	LPC_TIM0->TCR = TCR_COUNT_RESET;

	/* Just count up. */
	LPC_TIM0->CTCR = CTCR_CTM_TIMER;

	/* Prescale to a frequency that is good enough to get a decent resolution,
	but not too fast so as to overflow all the time. */
	LPC_TIM0->PR = (configCPU_CLOCK_HZ / 10000UL) - 1UL;

	/* Start the counter. */
	LPC_TIM0->TCR = TCR_COUNT_ENABLE;
}

