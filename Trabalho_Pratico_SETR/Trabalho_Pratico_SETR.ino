/*
 Name:		SETR_ALARM_V2.ino
 Created:	22/04/2021 18:54:02
 Author:	vitor
*/

#include <Arduino.h>

/*Bibliotecas FreeRTOS */
#include <Arduino_FreeRTOS.h>

/*mapeamento de pinos*/
#define PIR1 2
#define PIR2 3
// #define MAG1 5
// #define MAG2 5
// #define MAG3 6
#define LED_ON 12
#define LED_OFF 13

/* Variáveis para armazenamento do handle das tasks*/
TaskHandle_t taskPir1Handle = NULL;
TaskHandle_t taskPir2Handle = NULL;
// TaskHandle_t taskMag1Handle = NULL;
// TaskHandle_t taskMag2Handle = NULL;
// TaskHandle_t taskMag3Handle = NULL;
// TaskHandle_t taskLedOnHandle = NULL;
// TaskHandle_t taskLedOffHandle = NULL;

/*protítipos das Tasks*/
void vTaskPir1(void* pvParameters);
void vTaskPir2(void* pvParameters);
// void vTaskMag1(void* pvParameters);
// void vTaskMag2(void* pvParameters);
// void vTaskMag3(void* pvParameters);
// void vTaskLedOn(void* pvParameters);
// void vTaskLedOff(void* pvParameters);

/*variáveis auxiliares*/
bool alarmArmed = false;
int valueSensorTicksTime = 1000;

// the setup function runs once when you press reset or power the board
void setup()
{
	Serial.begin(9600);

	xTaskCreate(vTaskPir1, "TASK_PIR_1", configMINIMAL_STACK_SIZE, (void*)PIR1, 1, &taskPir1Handle);
	xTaskCreate(vTaskPir2, "TASK_PIR_2", configMINIMAL_STACK_SIZE, (void*)PIR2, 1, &taskPir2Handle);
	// xTaskCreate(vTaskMag1, "TASK_MAG_1", configMINIMAL_STACK_SIZE, (void*)MAG1, 1, &taskMag1Handle);
	// xTaskCreate(vTaskMag2, "TASK_MAG_2", configMINIMAL_STACK_SIZE, (void*)MAG2, 1, &taskMag2Handle);
	// xTaskCreate(vTaskMag3, "TASK_MAG_3", configMINIMAL_STACK_SIZE, (void*)MAG3, 1, &taskMag3Handle);
	// xTaskCreate(vTaskLedOn, "TASK_LED_ON", configMINIMAL_STACK_SIZE, (void*)LED_ON, 1, &taskLedOnHandle);
	// xTaskCreate(vTaskLedOff, "TASK_LED_OFF", configMINIMAL_STACK_SIZE, (void*)LED_OFF, 1, &taskLedOffHandle);
}

// the loop function runs over and over again until power down or reset
void loop()
{
	if (alarmArmed == true)
	{
		digitalWrite(LED_ON, HIGH);
		digitalWrite(LED_OFF, LOW);
	}
	else
	{
		digitalWrite(LED_ON, LOW);
		digitalWrite(LED_OFF, HIGH);
	}
}

/// <summary>
/// le o valor do sensor PIR 1
/// </summary>
/// <param name="pvParameters"></param>
void vTaskPir1(void* pvParameters)
{
	pinMode(PIR1, INPUT);

	while (1)
	{
		int value = digitalRead(PIR1);

		if (value == HIGH)
		{
			alarmArmed = true;
			Serial.println("PIR_1 Reading..." + (String)value);
		}
		else
		{
			alarmArmed = false;
		}
		pdMS_TO_TICKS(valueSensorTicksTime);
	}
}

/// <summary>
/// le o valor do sensor PIR 2
/// </summary>
/// <param name="pvParameters"></param>
void vTaskPir2(void* pvParameters)
{
	pinMode(PIR2, INPUT);

	while (1)
	{
		int value = digitalRead(PIR2);

		if (value == HIGH)
		{
			alarmArmed = true;
			Serial.println("PIR_2 Reading...");
		}
		else
		{
			alarmArmed = false;
		}
		pdMS_TO_TICKS(valueSensorTicksTime);
	}
}

/*void vTaskMag2(void* pvParameters)
{
	pinMode(MAG1, INPUT);

	while (1)
	{
		int value = digitalRead(MAG1);

		Serial.println("MAG_1: " + (String)value);

		pdMS_TO_TICKS(valueSensorTicksTime);
	}
}

void vTaskLedOn(void* pvParameters)
{
	pinMode(LED_ON, OUTPUT);

	while (1)
	{
		if (alarmArmed == true)
		{
			digitalWrite(LED_ON, HIGH);
		}
		else
		{
			digitalWrite(LED_ON, LOW);
		}
		pdMS_TO_TICKS(valueSensorTicksTime);
	}
}

void vTaskLedOff(void* pvParameters)
{
	pinMode(LED_OFF, OUTPUT);

	while (1)
	{
		if (alarmArmed == false)
		{
			digitalWrite(LED_OFF, HIGH);
		}
		else
		{
			digitalWrite(LED_OFF, LOW);
		}
		pdMS_TO_TICKS(valueSensorTicksTime);
	}
}*/
