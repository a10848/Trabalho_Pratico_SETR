#include <Arduino_FreeRTOS.h>

// task
void Task_Led(void* param);
void Task_Pir(void* param);

// handle
TaskHandle_t Task_Led_Handle;
TaskHandle_t Task_Pir_Handle;

// pin
#define led1 12
#define led2 13
#define pir1 2
#define pir2 3

// variable
bool pirSensorActive = false;

void setup()
{
	Serial.begin(9600);

	pinMode(led1, OUTPUT);
	pinMode(led2, OUTPUT);
	pinMode(pir1, INPUT);
	pinMode(pir2, INPUT);

	// task create
	xTaskCreate(Task_Led, "TASK_LED", 100, NULL, 1, &Task_Led_Handle);
	xTaskCreate(Task_Pir, "TASK_PIR", 100, NULL, 1, &Task_Pir_Handle);
}

void loop() {}

void Task_Led(void* param) {
	(void)param;

	while (1) {
		if (pirSensorActive == true) {
			digitalWrite(led1, HIGH);
			digitalWrite(led2, LOW);
		}
		else {
			digitalWrite(led1, LOW);
			digitalWrite(led2, HIGH);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void Task_Pir(void* param) {
	(void)param;

	while (1) {
		if (digitalRead(pir1) == HIGH || digitalRead(pir2) == HIGH) {
			pirSensorActive = true;
		}
		else {
			pirSensorActive = false;
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}