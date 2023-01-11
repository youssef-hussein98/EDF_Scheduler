/*
 * FreeRTOS Kernel V10.2.0
 * Copyright (C) 2019 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://www.FreeRTOS.org
 * http://aws.amazon.com/freertos
 *
 * 1 tab == 4 spaces!
 */

/* 
	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used.
*/


/*
 * Creates all the demo application tasks, then starts the scheduler.  The WEB
 * documentation provides more details of the demo application tasks.
 * 
 * Main.c also creates a task called "Check".  This only executes every three 
 * seconds but has the highest priority so is guaranteed to get processor time.  
 * Its main function is to check that all the other tasks are still operational.
 * Each task (other than the "flash" tasks) maintains a unique count that is 
 * incremented each time the task successfully completes its function.  Should 
 * any error occur within such a task the count is permanently halted.  The 
 * check task inspects the count of each task to ensure it has changed since
 * the last time the check task executed.  If all the count variables have 
 * changed all the tasks are still executing error free, and the check task
 * toggles the onboard LED.  Should any task contain an error at any time 
 * the LED toggle rate will change from 3 seconds to 500ms.
 *
 */

/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "lpc21xx.h"
#include "queue.h"

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"


/*-----------------------------------------------------------*/

/* Constants to setup I/O and processor. */
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )

/* Constants for the ComTest demo application tasks. */
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )


/*
 * Configure the processor for use with the Keil demo board.  This is very
 * minimal as most of the setup is managed by the settings in the project
 * file.
 */
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/

BaseType_t xReturned;
/* Tasks' Handlers */
TaskHandle_t xButton_1_Monitor_Handle 			= NULL;
TaskHandle_t xButton_2_Monitor_Handle 			= NULL;
TaskHandle_t xPeriodic_Transmitter_Handle 	= NULL;
TaskHandle_t xUart_Receiver_Handle 					= NULL;
TaskHandle_t xLoad_1_Simulation_Handle 			=	NULL;
TaskHandle_t xLoad_2_Simulation_Handle 			= NULL;

/* Tasks Prototype */
void Button_1_Monitor(void * pvParameters);
void Button_2_Monitor(void * pvParameters);
void Periodic_Transmitter(void * pvParameters);
void Uart_Receiver(void * pvParameters);
void Load_1_Simulation(void * pvParameters);
void Load_2_Simulation(void * pvParameters);

int IDLE_IN_time = 0, IDLE_OUT_time	= 0, IDLE_total_time = 0;
int Button_1_Monitor_IN_time = 0,Button_1_Monitor_OUT_time = 0,Button_1_Monitor_total_time = 0;
int Button_2_Monitor_IN_time = 0,Button_2_Monitor_OUT_time = 0,Button_2_Monitor_total_time = 0;
int Periodic_Transmitter_IN_time = 0,Periodic_Transmitter_OUT_time = 0,Periodic_Transmitter_total_time = 0;
int Uart_Receiver_IN_time = 0,Uart_Receiver_OUT_time = 0,Uart_Receiver_total_time = 0;
int Load_1_Simulation_IN_time = 0,Load_1_Simulation_OUT_time = 0,Load_1_Simulation_total_time = 0;
int Load_2_Simulation_IN_time = 0,Load_2_Simulation_OUT_time = 0,Load_2_Simulation_total_time = 0;

int system_time = 0;
float cpu_load = 0;


typedef struct 
{
   char ucMessageID;
   char ucData[ 20 ];
} xMessage;

QueueHandle_t xQueue = NULL;

/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
int main( void )
{
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
	
		xQueue = xQueueCreate( 3, sizeof( xMessage) );
	
    /* Create Tasks here */


    /* Create the task, storing the handle. */
    xReturned = xTaskPeriodicCreate(
                    Button_1_Monitor,     /* Function that implements the task. **********/
                    "Button_1_Monitor",   /* Text name for the task. *********************/
                     200,            			/* Stack size in words, not bytes. *************/
                    ( void * ) 1,    			/* Parameter passed into the task. *************/
                    50,										/* Deadline at which the task is created. ******/
                    &xButton_1_Monitor_Handle );      /* Used to pass out the created task's handle. */
    
    xReturned = xTaskPeriodicCreate(
                    Button_2_Monitor,     /* Function that implements the task. **********/
                    "Button_2_Monitor",   /* Text name for the task. *********************/
                     200,            			/* Stack size in words, not bytes. *************/
                    ( void * ) 1,    			/* Parameter passed into the task. *************/
                    50,										/* Deadline at which the task is created. ******/
                    &xButton_2_Monitor_Handle );      /* Used to pass out the created task's handle. */
 
  	xReturned = xTaskPeriodicCreate(
                    Periodic_Transmitter,     /* Function that implements the task. **********/
                    "Periodic_Transmitter",   /* Text name for the task. *********************/
                     100,            					/* Stack size in words, not bytes. *************/
                    ( void * ) 1,    					/* Parameter passed into the task. *************/
                    100,											/* Deadline at which the task is created. ******/
                    &xPeriodic_Transmitter_Handle );      /* Used to pass out the created task's handle. */					
										
  	xReturned = xTaskPeriodicCreate(
                    Uart_Receiver,     	/* Function that implements the task. **********/
                    "Uart_Receiver",   	/* Text name for the task. *********************/
                     100,            		/* Stack size in words, not bytes. *************/
                    ( void * ) 1,    		/* Parameter passed into the task. *************/
                    20,									/* Deadline at which the task is created. ******/
                    &xUart_Receiver_Handle );      /* Used to pass out the created task's handle. */					

  	xReturned = xTaskPeriodicCreate(
                    Load_1_Simulation,     	/* Function that implements the task. **********/
                    "Load_1_Simulation",   	/* Text name for the task. *********************/
                     100,            				/* Stack size in words, not bytes. *************/
                    ( void * ) 1,    				/* Parameter passed into the task. *************/
                    10,											/* Deadline at which the task is created. ******/
                    &xLoad_1_Simulation_Handle );      /* Used to pass out the created task's handle. */		

  	xReturned = xTaskPeriodicCreate(
                    Load_2_Simulation,     	/* Function that implements the task. **********/
                    "Load_2_Simulation",   	/* Text name for the task. *********************/
                     100,            				/* Stack size in words, not bytes. *************/
                    ( void * ) 1,    				/* Parameter passed into the task. *************/
                    100,										/* Deadline at which the task is created. ******/
                    &xLoad_2_Simulation_Handle );      /* Used to pass out the created task's handle. */		

										
	/* Now all the tasks have been started - start the scheduler.

	NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
	The processor MUST be in supervisor mode when vTaskStartScheduler is 
	called.  The demo applications included in the FreeRTOS.org download switch
	to supervisor mode prior to main being called.  If you are not using one of
	these demo application projects then ensure Supervisor mode is used here. */
	vTaskStartScheduler();

	/* Should never reach here!  If you do then there was not enough heap
	available for the idle task to be created. */
	for( ;; );
}
/*-----------------------------------------------------------*/

/* Function to reset timer 1 */
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}

/* Function to initialize and start timer 1 */
static void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}

static void prvSetupHardware( void )
{
	/* Perform the hardware setup required.  This is minimal as most of the
	setup is managed by the settings in the project file. */

	/* Configure UART */
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);

	/* Configure GPIO */
	GPIO_init();
	
	/* Config trace timer 1 and read T1TC to get current tick */
	configTimer1();

	/* Setup the peripheral bus to be the same as the PLL output. */
	VPBDIV = mainBUS_CLK_FULL;
}

void Button_1_Monitor(void * pvParameters)
{
	pinState_t button_1State;
	xMessage Button1Data;
	TickType_t xLastWakeTime;
	
	const TickType_t xFrequency = 50;
	Button1Data.ucMessageID = '1';
	xLastWakeTime = xTaskGetTickCount();

	vTaskSetApplicationTaskTag( NULL, ( TaskHookFunction_t ) 1 );

	
	for( ;; )
	{
		button_1State = GPIO_read(PORT_0 ,PIN7);

		vTaskDelayUntil(&xLastWakeTime,xFrequency);
		if(button_1State != GPIO_read(PORT_0 ,PIN7))
		{
			if(button_1State == 0)	/* GPIO_read == 1 -> rising*/
			{
				strncpy(Button1Data.ucData,"Button 1: Rising\n",20);
				if( xQueueSend( xQueue, ( void * ) &Button1Data, ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
        }
			}
			else									/* falling */
			{
				strncpy(Button1Data.ucData,"Button 1: Falling\n",20);
				if( xQueueSend( xQueue, ( void * ) &Button1Data, ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
        }
			}
		}
		
	}
}
void Button_2_Monitor(void * pvParameters)
{
	pinState_t button_2State;
	xMessage Button2Data;
	TickType_t xLastWakeTime;
	

	const TickType_t xFrequency = 50;
	Button2Data.ucMessageID = '2';
	xLastWakeTime = xTaskGetTickCount();
	
	vTaskSetApplicationTaskTag( NULL, ( TaskHookFunction_t ) 2 );

	
	for( ;; )
	{
		button_2State = GPIO_read(PORT_0 ,PIN8);
		vTaskDelayUntil(&xLastWakeTime,xFrequency);
		if(button_2State != GPIO_read(PORT_0 ,PIN8))
		{
			if(button_2State == 0)	/* GPIO_read == 1 -> rising*/
			{
				strncpy(Button2Data.ucData,"Button 2: Rising\n",20);
				if( xQueueSend( xQueue, ( void * ) &Button2Data, ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
        }
			}
			else									/* falling */
			{
				strncpy(Button2Data.ucData,"Button 2: Falling\n",20);
				if( xQueueSend( xQueue, ( void * ) &Button2Data, ( TickType_t ) 10 ) != pdPASS )
        {
            /* Failed to post the message, even after 10 ticks. */
        }
			}
		}
	}
}
void Periodic_Transmitter(void * pvParameters)
{
	xMessage TransmitterData;
	TickType_t xLastWakeTime;
	
	const TickType_t xFrequency = 100;
	TransmitterData.ucMessageID = '3';
	xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( TaskHookFunction_t ) 3 );
	for( ;; )
	{
		strncpy(TransmitterData.ucData,"Periodic String\n",20);
		if( xQueueSend( xQueue, ( void * ) &TransmitterData, ( TickType_t ) 10 ) != pdPASS )
		{
			/* Failed to post the message, even after 10 ticks. */
		}
		vTaskDelayUntil(&xLastWakeTime,xFrequency);	
	}
}

void Uart_Receiver(void * pvParameters)
{
	TickType_t xLastWakeTime;
	xMessage ReceivedData;
	
	const TickType_t xFrequency = 20;
  xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( TaskHookFunction_t ) 4 );
	for( ;; )
	{
		if( xQueueReceive( xQueue, &( ReceivedData ),( TickType_t ) 10 ) == pdPASS )
      {
         /* xRxedStructure now contains a copy of xMessage. */
				vSerialPutString((signed char *)ReceivedData.ucData,20);
      }
		vTaskDelayUntil(&xLastWakeTime,xFrequency);
		
	}
}

void Load_1_Simulation(void * pvParameters)
{
	int i = 0;
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 10;
	xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( TaskHookFunction_t ) 5 );
	for( ;; )
	{
		for(i = 0;i <40000;i++){}
		vTaskDelayUntil(&xLastWakeTime,xFrequency);	
	}
}

void Load_2_Simulation(void * pvParameters)
{
	int i = 0;
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100;
	xLastWakeTime = xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL, ( TaskHookFunction_t ) 6 );
	for( ;; )
	{
		for(i = 0;i <90000;i++){}
		vTaskDelayUntil(&xLastWakeTime,xFrequency);	
	}
}
/*-----------------------------------------------------------*/
