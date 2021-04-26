#include <Arduino_FreeRTOS.h>
#include <Keypad.h>

// task
void Task_Led(void* param);
void Task_Blink_Led(void* param);
void Task_Pir(void* param);
void Task_Buzzer(void* param);
void Task_Keyboard(void* param);

// handle
TaskHandle_t Task_Led_Handle;
TaskHandle_t Task_Blink_Led_Handle;
TaskHandle_t Task_Pir_Handle;
TaskHandle_t Task_Buzzer_Handle;
TaskHandle_t Task_Keyboard_Handle;

// pin
#define ledBlinkRed 42
#define ledPir 43
#define ledMagnetic 44
#define ledWater 45
#define ledGas 46
#define ledExtra 47
#define ledRfidBlue 48
#define ledRfidGreen 49
#define ledRfidRed 50
#define ledOnOffBlue 51
#define ledOnOffGreen 52
#define ledOnOffRed 53
#define buzzer 31
#define pirA 13
#define pirB 12

// variable
bool pirSensorActive = false;
bool magneticSensorActive = false;
bool waterSensorActive = false;
bool gasSensorActive = false;
bool onOff = false;
bool blinkLed = false;
bool blinkLedActive = false;
bool buzzerPositive = false;
bool buzzerNegative = false;
bool buzzerStatus = false;
int alarmStatus = 0;
int countDown = 15;

// keyboard
char key = NO_KEY;
const byte rows = 5;
const byte cols = 4;
char key_map[rows][cols] = { {'A','B','#','*'},{'1','2','3','U'},{'4','5','6','D'},{'7','8','9','C'},{'L','0','R','E'} };
byte col_pins[cols] = { 29, 27, 25, 23 };
byte row_pins[rows] = { 30, 28, 26, 24, 22 };
Keypad my_key_pad = Keypad(makeKeymap(key_map), row_pins, col_pins, rows, cols);

// access codes
String codigo;
String autenticationAdmin = "5052";

void setup()
{
	Serial.begin(9600);

	pinMode(ledBlinkRed, OUTPUT);
	pinMode(ledPir, OUTPUT);
	pinMode(ledMagnetic, OUTPUT);
	pinMode(ledWater, OUTPUT);
	pinMode(ledGas, OUTPUT);
	pinMode(ledExtra, OUTPUT);
	pinMode(ledRfidGreen, OUTPUT);
	pinMode(ledRfidBlue, OUTPUT);
	pinMode(ledRfidRed, OUTPUT);
	pinMode(ledOnOffGreen, OUTPUT);
	pinMode(ledOnOffBlue, OUTPUT);
	pinMode(ledOnOffRed, OUTPUT);
	pinMode(pirA, INPUT);
	pinMode(pirB, INPUT);

	// task create
	xTaskCreate(Task_Led, "TASK_LED", 256, NULL, 1, &Task_Led_Handle);
	xTaskCreate(Task_Blink_Led, "TASK_BLINK_LED", 256, NULL, 1, &Task_Blink_Led_Handle);
	xTaskCreate(Task_Pir, "TASK_PIR", 256, NULL, 1, &Task_Pir_Handle);
	xTaskCreate(Task_Buzzer, "TASK_BUZZER", 256, NULL, 1, &Task_Buzzer_Handle);
	xTaskCreate(Task_Keyboard, "TASK_KEYBOARD", 2048, NULL, 1, &Task_Keyboard_Handle);
}

void loop() {}

void Task_Led(void* param) {
	(void)param;

	while (1) {
		// led pir
		if (pirSensorActive == true) digitalWrite(ledPir, HIGH);
		else digitalWrite(ledPir, LOW);

		// led on, off and input code
		if (alarmStatus == -1) {
			digitalWrite(ledOnOffGreen, LOW);
			digitalWrite(ledOnOffBlue, HIGH);
			digitalWrite(ledOnOffRed, LOW);
		}
		else if (alarmStatus == 0) {
			digitalWrite(ledOnOffGreen, LOW);
			digitalWrite(ledOnOffBlue, LOW);
			digitalWrite(ledOnOffRed, HIGH);
		}
		else if (alarmStatus == 1) {
			digitalWrite(ledOnOffGreen, HIGH);
			digitalWrite(ledOnOffBlue, LOW);
			digitalWrite(ledOnOffRed, LOW);
		}

		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

void Task_Buzzer(void* param) {
	(void)param;

	while (1) {
		// buzzer
		while (buzzerNegative == true) {
			if (buzzerStatus == false) {
				tone(buzzer, 4000);
				vTaskDelay(150 / portTICK_PERIOD_MS);
				noTone(buzzer);
				buzzerStatus = true;
			}
			else {
				tone(buzzer, 3000);
				vTaskDelay(150 / portTICK_PERIOD_MS);
				noTone(buzzer);
				buzzerStatus = false;
			}
		}

		if (buzzerPositive == true) {
			tone(buzzer, 4000);
			vTaskDelay(100 / portTICK_PERIOD_MS);
			noTone(buzzer);
			vTaskDelay(150 / portTICK_PERIOD_MS);
			tone(buzzer, 3000);
			vTaskDelay(100 / portTICK_PERIOD_MS);;
			noTone(buzzer);
			vTaskDelay(25 / portTICK_PERIOD_MS);
			tone(buzzer, 3000);
			vTaskDelay(100 / portTICK_PERIOD_MS);
			noTone(buzzer);
			vTaskDelay(25 / portTICK_PERIOD_MS);
			tone(buzzer, 3500);
			vTaskDelay(100 / portTICK_PERIOD_MS);
			noTone(buzzer);
			vTaskDelay(150 / portTICK_PERIOD_MS);
			tone(buzzer, 3000);
			vTaskDelay(100 / portTICK_PERIOD_MS);
			noTone(buzzer);
			vTaskDelay(400 / portTICK_PERIOD_MS);
			tone(buzzer, 3900);
			vTaskDelay(100 / portTICK_PERIOD_MS);
			noTone(buzzer);
			vTaskDelay(150 / portTICK_PERIOD_MS);
			tone(buzzer, 4000);
			vTaskDelay(100 / portTICK_PERIOD_MS);
			noTone(buzzer);
			vTaskDelay(150 / portTICK_PERIOD_MS);

			buzzerPositive = false;
			countDown = 15;
		}

		vTaskDelay(150 / portTICK_PERIOD_MS);
	}
}

void Task_Blink_Led(void* param) {
	(void)param;

	while (1) {
		// blink led pir
		if (blinkLedActive == false) {
			blinkLed = false;
			digitalWrite(ledBlinkRed, LOW);
		}
		else {
			if (blinkLed == false) {
				blinkLed = true;
				digitalWrite(ledBlinkRed, HIGH);
				if (countDown > 0) {
					countDown--;
				}
				else {
					buzzerNegative = true;
				}
			}
			else {
				blinkLed = false;
				digitalWrite(ledBlinkRed, LOW);
			}
		}

		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

void Task_Pir(void* param) {
	(void)param;

	while (1) {
		if (digitalRead(pirA) == HIGH || digitalRead(pirB) == HIGH) {
			if (onOff == false) blinkLedActive = true;

			pirSensorActive = true;
		}
		else pirSensorActive = false;

		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

void Task_Keyboard(void* param) {
	(void)param;

	int i = 0;

	while (1) {
		key = my_key_pad.getKey();

		if (key != NO_KEY) {
			Serial.println(key);
		}
		if (key == '#') {
			alarmStatus = -1;
			do {
				key = my_key_pad.getKey();
				if (key != NO_KEY) {
					if (key != '#') {
						codigo += key;
						i++;
						Serial.print("*");
					}
				}
			} while (key != '#');
		}

		if (codigo != "") {
			Serial.println("");
			Serial.println(codigo);

			if (codigo == autenticationAdmin) {
				Serial.println("Correto!");
				blinkLedActive = false;
				buzzerPositive = true;
				buzzerNegative = false;
				buzzerStatus = false;

				if (onOff == true) {
					onOff = false;
					alarmStatus = 0;
					Serial.println("Alarm deactivated!");
				}
				else {
					onOff = true;
					alarmStatus = 1;
					Serial.println("Alarm activated!");
				}
			}
			else {
				Serial.println("Incorreto!");
			}

			codigo = "";
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
		key = NO_KEY;
	}
}