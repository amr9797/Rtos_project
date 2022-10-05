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
#include "queue.h " 

/* Peripheral includes. */
#include "serial.h"
#include "GPIO.h"
BaseType_t xTaskCreatePeriodic( TaskFunction_t pxTaskCode,
                            const char * const pcName, /*lint !e971 Unqualified char types are allowed for strings and single characters only. */
                            const configSTACK_DEPTH_TYPE usStackDepth,
                            void * const pvParameters,
                            UBaseType_t uxPriority,
                            TaskHandle_t * const pxCreatedTask , TickType_t period ) ; 

unsigned long int IDLEstart , task1start ,task2start ,task3start=0,task4start , task5start , task6start ; 
unsigned long int IDLEend , task1end ,task2end ,task3end=0,task4end , task5end , task6end ; 
unsigned long int IDLEtotal , task1total , task2total ,task3total=0 ,task4total ,task5total ,task6total ; 
 float IDLEperc , task1perc , task2perc , task3perc, task4perc, task5perc,task6perc ;
 float totalcpuperc ; 
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
typedef struct /*struct type for the queue message */ 
 {
    char Data[40];
 }str_message ;
 
 /********the messages tasks will send *****/
 str_message rising1 = {"Button1_rising_edge_detected\n"} ; 
 str_message falling1 = {"Button1_falling_edge_detected\n"} ;
str_message rising2 = {"Button2_rising_edge_detected\n"} ; 
 str_message falling2 = {"Button2_falling_edge_detected\n"} ;  
 str_message task3 = {"task3_periodic_string\n"} ; 
 
   
 QueueHandle_t xQueue; /*the queue will be used for inter task communications */
 
static void prvSetupHardware( void );
 /* the rtos tasks prototypes */ 
void Load_1_Simulation (void) ;
void Load_2_Simulation (void) ;
 void Button_1_Monitor (void) ; 
 void Button_2_Monitor (void) ; 
 void Periodic_Transmitter (void) ; 
 void Uart_Receiver (void) ; 
/*-----------------------------------------------------------*/


/*
 * Application entry point:
 * Starts all the other tasks, then starts the scheduler. 
 */
 
int main( void )
{ 
	

    /* Create a queue capable of containing 10 str_messages. */
    xQueue = xQueueCreate( 15, sizeof( str_message ) );
	BaseType_t xReturned ;
TaskHandle_t xHandle = NULL;
	/* Setup the hardware for use with the Keil demo board. */
	prvSetupHardware();
 
  
    /* Create Tasks here */
	xReturned = xTaskCreatePeriodic(
                    &Button_1_Monitor ,       /* Function that implements the task. */
                    "task1",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    2,/* Priority at which the task is created. */
                    &xHandle , 50);  	/* Used to pass out the created task's handle. */
	xReturned = xTaskCreatePeriodic(
                    &Button_2_Monitor ,       /* Function that implements the task. */
                    "task2",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    2,/* Priority at which the task is created. */
                    &xHandle , 50 );  	/* Used to pass out the created task's handle. */
	xReturned = xTaskCreatePeriodic(
                    &Periodic_Transmitter ,       /* Function that implements the task. */
                    "task3",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &xHandle ,100 );  	/* Used to pass out the created task's handle. */
  xReturned = xTaskCreatePeriodic(
                    &Uart_Receiver ,       /* Function that implements the task. */
                    "task4",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    3,/* Priority at which the task is created. */
                    &xHandle ,20 );  	/* Used to pass out the created task's handle. */
	 
	xReturned = xTaskCreatePeriodic(
                    Load_1_Simulation ,       /* Function that implements the task. */
                    "task5",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &xHandle ,10);  	/* Used to pass out the created task's handle. */
	xReturned = xTaskCreatePeriodic(
                    Load_2_Simulation ,       /* Function that implements the task. */
                    "task6",          /* Text name for the task. */
                    100,      /* Stack size in words, not bytes. */
                    NULL,    /* Parameter passed into the task. */
                    1,/* Priority at which the task is created. */
                    &xHandle ,100);  	/* Used to pass out the created task's handle. */
										      
	
							

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


/*********************************application tasks ***************************************/

void Button_1_Monitor (void)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = 50;
	int prev=1 , current=1 ;
	    vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );
  for ( ;; ) 
{
	/*detect the rising or falling edges*/
	current = GPIO_read(PORT_0 , PIN0) ;
	if (current==0 && prev==1 )
	{
		prev=0 ; 
		xQueueSend( xQueue, ( void * ) &falling1 , ( TickType_t ) 0 );/*send the event to the queue*/
	}
	if (current==1 && prev==0 )
	{
		prev=1 ; 
		xQueueSend( xQueue, ( void * ) &rising1 , ( TickType_t ) 0 );
	}
	
   vTaskDelayUntil( &xLastWakeTime, xFrequency );
	 
}
}
void Button_2_Monitor (void)
{
 TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = 50;
	int prev=1 , current=1 ;
	    vTaskSetApplicationTaskTag( NULL, ( void * ) 2);
  for (;;)
{ /*detect the rising or falling edges*/
	current = GPIO_read(PORT_0 , PIN1) ;
	if (current==0 && prev==1 )
	{
		prev=0 ; 
		xQueueSend( xQueue, ( void * ) &falling2, ( TickType_t ) 0 ); /*send the event to the queue*/
	}
	if (current==1 && prev==0 )
	{
		prev=1 ; 
		xQueueSend( xQueue, ( void * ) &rising2 , ( TickType_t ) 0 );
	}
	
   vTaskDelayUntil( &xLastWakeTime, xFrequency );
	 
}
}

void Periodic_Transmitter (void)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = 100;
	    vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );
	 
  for (;;)
{
   xQueueSend( xQueue, ( void * ) &task3, ( TickType_t ) 0 ); /*send periodic string to ready queue*/	
   vTaskDelayUntil( &xLastWakeTime, xFrequency );
	
}
}
void Uart_Receiver (void)
{
	TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = 20;
	    vTaskSetApplicationTaskTag( NULL, ( void * ) 4 );
	
	 str_message buffer ; 
  for (;;)
{	
	if(xQueueReceive( xQueue, &buffer ,( TickType_t ) 0 )) /*recieve a string from the queue*/
	{		
	   vSerialPutString((const signed char * const)&(buffer.Data) , strlen((const char * const)&(buffer.Data))) ;  /*send the string via uart*/
	}
	
		
   vTaskDelayUntil( &xLastWakeTime, xFrequency );
	
}

}

void Load_1_Simulation (void)
{
	 TickType_t xLastWakeTime;
	xLastWakeTime = xTaskGetTickCount();
   const TickType_t xFrequency = 10;
   int i ; 
     // Initialise the xLastWakeTime variable with the current time.
     xLastWakeTime = xTaskGetTickCount();
	  vTaskSetApplicationTaskTag( NULL, ( void * ) 5);

     for( ;; )
     { 
			 /*calculate the tasks , idle , cpu loads*/
		totalcpuperc = ((float)(task1total + task2total + task3total + task4total + task5total + task6total) /(float) T1TC)*100 ; 
	
	   task1perc = ((float) task1total /( float)T1TC)*100 ; 
	   task2perc = ((float) task2total /( float)T1TC)*100 ; 
     task3perc = ((float) task3total /( float)T1TC)*100 ; 	
	   task4perc = ((float) task4total /( float)T1TC)*100 ; 
	   task5perc = ((float) task5total /( float)T1TC)*100 ; 
	   task6perc = ((float) task6total /( float)T1TC)*100 ; 
		 IDLEperc = ((float) IDLEtotal /( float)T1TC)*100 ; 
			 
			
        for(i=0 ; i<36200  ;i++) ; /*delay 5ms*/
			 
         vTaskDelayUntil( &xLastWakeTime, xFrequency );
			 
         // Perform action here.
     }
	 }

	 
	 
 void Load_2_Simulation (void)
 {
	 TickType_t xLastWakeTime;
	 xLastWakeTime = xTaskGetTickCount();
   const TickType_t xFrequency = 100;
   int i ; 
     // Initialise the xLastWakeTime variable with the current time.
     xLastWakeTime = xTaskGetTickCount();
     vTaskSetApplicationTaskTag( NULL, ( void * ) 6);
     for( ;; )
     {
      
	    /*xQueueSend( xQueue, ( void * ) &task3 , ( TickType_t ) 100 );*/

	
        for(i=0 ; i<86900 ;i++) ; /*delay for 12ms*/
			
         vTaskDelayUntil( &xLastWakeTime, xFrequency );
			 
        
     }
 }
/*tick hook */
 void vApplicationTickHook( void )
 {
	 GPIO_write(PORT_0,PIN8 ,PIN_IS_HIGH);
	 GPIO_write(PORT_0,PIN8 ,PIN_IS_LOW);
 }
 







