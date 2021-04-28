#include <Arduino_FreeRTOS.h>
#include <Keypad.h>
#include <SPI.h>
#include <RFID.h>

// task
void Task_Led(void* param);
void Task_Blink_Led(void* param);
void Task_Pir(void* param);
void Task_Magnet(void* param);
void Task_Buzzer(void* param);
void Task_Alarm(void* param);

// handle
TaskHandle_t Task_Led_Handle;
TaskHandle_t Task_Blink_Led_Handle;
TaskHandle_t Task_Pir_Handle;
TaskHandle_t Task_Magnet_Handle;
TaskHandle_t Task_Buzzer_Handle;
TaskHandle_t Task_Alarm_Handle;

// pin
#define ledBlinkRed 32
#define ledPir 33
#define ledMagnetic 34
#define ledWater 35
#define ledGas 36
#define ledExtra 37
#define ledRfidBlue 38
#define ledRfidGreen 39
#define ledRfidRed 40
#define ledOnOffBlue 41
#define ledOnOffGreen 42
#define ledOnOffRed 43
#define buzzer 31
#define pirA 13
#define pirB 12
#define MagnetA 5
#define MagnetB 6
#define MagnetC 7
#define sdaPin 9
#define resetPin 8

// variable
#define countDownTime 30
#define codeErrorAttempts 3
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
bool loginAdmin = false;
bool loginUserA = false;
bool loginUserB = false;
int alarmStatus = 0;
int countDown = countDownTime;
int codeError = codeErrorAttempts;

// rfid
RFID rfid522(sdaPin, resetPin);

// keyboard
char key = NO_KEY;
const byte rows = 5;
const byte cols = 4;
char key_map[rows][cols] = { {'A','B','#','*'},{'1','2','3','U'},{'4','5','6','D'},{'7','8','9','C'},{'L','0','R','E'} };
byte col_pins[cols] = { 29, 27, 25, 23 };
byte row_pins[rows] = { 30, 28, 26, 24, 22 };
Keypad my_key_pad = Keypad(makeKeymap(key_map), row_pins, col_pins, rows, cols);

// access codes
String authenticationCode;
String authenticationAdmin = "5052";
String authenticationUserA = "1234";
String authenticationUserB = "4321";
String rfidCode;
String rfidAdmin = "8910412177140";
String rfidUserA = "443023322657";
String rfidUserB = "10824477172121";

void setup()
{
	Serial.begin(9600);
	SPI.begin();
	rfid522.init();

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
	pinMode(MagnetA, INPUT);
	pinMode(MagnetB, INPUT);
	pinMode(MagnetC, INPUT);

	// task create
	xTaskCreate(Task_Led, "TASK_LED", 256, NULL, 1, &Task_Led_Handle);
	xTaskCreate(Task_Blink_Led, "TASK_BLINK_LED", 256, NULL, 1, &Task_Blink_Led_Handle);
	xTaskCreate(Task_Pir, "TASK_PIR", 256, NULL, 1, &Task_Pir_Handle);
	xTaskCreate(Task_Magnet, "TASK_MAGNET", 256, NULL, 1, &Task_Magnet_Handle);
	xTaskCreate(Task_Buzzer, "TASK_BUZZER", 256, NULL, 1, &Task_Buzzer_Handle);
	xTaskCreate(Task_Alarm, "TASK_ALARM", 2048, NULL, 1, &Task_Alarm_Handle);
}

void loop() {}

void Task_Led(void* param) {
	(void)param;

	while (1) {
		// led pir
		if (pirSensorActive == true) {
			digitalWrite(ledPir, HIGH);
		}
		else {
			digitalWrite(ledPir, LOW);
		}

		// led magnet
		if (magneticSensorActive == true) {
			digitalWrite(ledMagnetic, HIGH);
		}
		else {
			digitalWrite(ledMagnetic, LOW);
		}

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
			countDown = countDownTime;
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
				if (countDown > (countDownTime - (countDownTime - 1))) {
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
			if (onOff == false) {
				blinkLedActive = true;
			}
			pirSensorActive = true;
		}
		else {
			pirSensorActive = false;
		}

		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

void Task_Magnet(void* param) {
	(void)param;

	while (1) {
		if (digitalRead(MagnetA) == 1 || digitalRead(MagnetB) == 1 || digitalRead(MagnetC) == 1) {
			if (onOff == false) {
				blinkLedActive = true;
			}
			magneticSensorActive = true;
		}
		else {
			magneticSensorActive = false;
		}

		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

void Task_Alarm(void* param) {
	(void)param;

	int i = 0;

	while (1) {
		rfidCode = "";
		key = my_key_pad.getKey();

		if (rfid522.isCard())
		{
			rfid522.readCardSerial();
			Serial.println("Card detected!");
			for (int i = 0; i < 5; i++)
			{
				rfidCode += rfid522.serNum[i];
			}
			Serial.println(rfidCode);
		}

		if (key != NO_KEY) {
			Serial.println(key);
		}
		if (rfidCode == rfidAdmin || rfidCode == rfidUserA || rfidCode == rfidUserB) {
			alarmStatus = -1;
			do {
				key = my_key_pad.getKey();
				if (key != NO_KEY) {
					if (key != '#') {
						authenticationCode += key;
						i++;
						Serial.print("*");
					}
				}
			} while (key != '#');
		}

		if (authenticationCode != "") {
			Serial.println("");
			Serial.println(authenticationCode);

			if (rfidCode == rfidAdmin && authenticationCode == authenticationAdmin) {
				loginAdmin = true;
			}
			else if (rfidCode == rfidUserA && authenticationCode == authenticationUserA) {
				loginUserA = true;
			}
			else if (rfidCode == rfidUserB && authenticationCode == authenticationUserB) {
				loginUserB = true;
			}

			if (loginAdmin == true || loginUserA == true || loginUserB == true) {
				Serial.println("Authentication successful!");

				if (loginAdmin == true) {
					Serial.println("Welcome Admin...");
					loginAdmin = false;
				}
				if (loginUserA == true) {
					Serial.println("Welcome Xavita...");
					loginUserA = false;
				}
				if (loginUserB == true) {
					Serial.println("Welcome Guelhas...");
					loginUserB = false;
				}

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

				codeError = codeErrorAttempts;
			}
			else {
				Serial.println("Authentication unsuccessful!");

				if (onOff == true) alarmStatus = 1;
				else alarmStatus = 0;

				if (codeError > (codeErrorAttempts - (codeErrorAttempts - 1))) {
					codeError--;
				}
				else {
					buzzerNegative = true;
					alarmStatus = 0;
					onOff = false;
				}
			}

			authenticationCode = "";
		}
		vTaskDelay(100 / portTICK_PERIOD_MS);
		key = NO_KEY;
	}
}