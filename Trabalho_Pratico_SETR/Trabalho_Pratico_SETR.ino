#include <Arduino_FreeRTOS.h>
#include <Keypad.h>

// task
void Task_Led(void* param);
void Task_Pir(void* param);
void Task_Keyboard(void* param);

// handle
TaskHandle_t Task_Led_Handle;
TaskHandle_t Task_Pir_Handle;
TaskHandle_t Task_Keyboard_Handle;

// pin
#define ledPir 11
#define ledMag 9
#define ledWat 10
#define ledOn 12
#define ledOff 13
#define pir1 2
#define pir2 3

// variable
bool pirSensorActive = false;
bool alarmActive = false;
char key = NO_KEY;

// keyboard
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

	pinMode(ledOn, OUTPUT);
	pinMode(ledOff, OUTPUT);
	pinMode(ledPir, OUTPUT);
	pinMode(pir1, INPUT);
	pinMode(pir2, INPUT);

	// task create
	xTaskCreate(Task_Led, "TASK_LED", 128, NULL, 1, &Task_Led_Handle);
	xTaskCreate(Task_Pir, "TASK_PIR", 128, NULL, 1, &Task_Pir_Handle);
	xTaskCreate(Task_Keyboard, "TASK_KEYBOARD", 1024, NULL, 1, &Task_Keyboard_Handle);
}

void loop() {}

void Task_Led(void* param) {
	(void)param;

	while (1) {
		if (pirSensorActive == true) {
			digitalWrite(ledOn, HIGH);
			digitalWrite(ledOff, LOW);
		}
		else {
			digitalWrite(ledOn, LOW);
			digitalWrite(ledOff, HIGH);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
	}
}

void Task_Pir(void* param) {
	(void)param;

	while (1) {
		if (digitalRead(pir1) == HIGH || digitalRead(pir2) == HIGH) {
			digitalWrite(ledPir, HIGH);
		}
		else {
			digitalWrite(ledPir, LOW);
		}
		vTaskDelay(10 / portTICK_PERIOD_MS);
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
				if (alarmActive == true) {
					alarmActive = false;
					Serial.println("Alarm deactivated!");
					pirSensorActive = false;
				}
				else {
					alarmActive = true;
					Serial.println("Alarm activated!");
					pirSensorActive = true;
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