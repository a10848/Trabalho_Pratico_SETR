#include <Arduino_FreeRTOS.h>
#include <Keypad.h>
#include <SPI.h>
#include <RFID.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <virtuabotixRTC.h>
#include <Time.h>

#pragma region tasks e handles

// task
void Task_Led(void* param);
void Task_Blink_Led_Negative(void* param);
void Task_Pir(void* param);
void Task_Magnet(void* param);
void Task_Water(void* param);
void Task_Gas(void* param);
void Task_Buzzer(void* param);
void Task_Alarm(void* param);
void Task_Led_Water(void* param);
void Task_Screen(void* param);
void Task_Led_Presence(void* param);
void Task_Presence(void* param);
void Task_Read(void* param);

// handle
TaskHandle_t Task_Led_Handle;
TaskHandle_t Task_Blink_Led_Negative_Handle;
TaskHandle_t Task_Pir_Handle;
TaskHandle_t Task_Magnet_Handle;
TaskHandle_t Task_Water_Handle;
TaskHandle_t Task_Gas_Handle;
TaskHandle_t Task_Buzzer_Handle;
TaskHandle_t Task_Alarm_Handle;
TaskHandle_t Task_Led_Water_Handle;
TaskHandle_t Task_Screen_Handle;
TaskHandle_t Task_Led_Presence_Handle;
TaskHandle_t Task_Presence_Handle;
TaskHandle_t Task_Read_Handle;

#pragma endregion

#pragma region pinos e variáveis

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
#define GasSensor 11
#define WaterSensor A0
#define sdaPin 9
#define resetPin 8
#define ce 2
#define io 3
#define clk 4
#define ledDoor 46
#define ledWindow 47
#define SIZE 10

// variable
#define countDownTime 30
#define codeErrorAttempts 3
bool pirSensorActive = false;
bool magneticSensorActive = false;
bool waterSensorActive = false;
bool gasSensorActive = false;
bool onOff = false;
bool blinkLed = false;
bool blinkLedNegativeActive = false;
bool blinkLedNegativeActive2 = false;
bool buzzerPositive = false;
bool buzzerNegative = false;
bool buzzerStatus = false;
bool loginAdmin = false;
bool loginUser = false;
bool loginSOS = false;
bool loginAdminDoor = false;
bool loginAdminWindow = false;
bool ledDoorActive = false;
bool ledWindowActive = false;
bool presence = false;
bool isItDoor = true;
int alarmStatus = 0;
int previousAlarmStatus = 0;
int waterLevel = 0;
int countDown = countDownTime;
int codeError = codeErrorAttempts;
int cntDigits = 0;
int screenClock = 30;
int PresenceClock = 60;
int consoleControl = 0;
bool waterPrevious = false;
bool firePrevious = false;
bool pirPreviousA = false;
bool pirPreviousB = false;
bool windowPreviousA = false;
bool windowPreviousB = false;
bool windowPrevious = false;
bool displayRefresh = false;
bool doorPrevious = false;
bool alarmPrevious = false;
bool buzzerError = false;
bool blinkLed2 = false;

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
String users[SIZE] = { "SOS", "Admin", "xavita", "guelhas" };
String rfids[SIZE] = { "0000000000", "8910412177140", "443023322657", "10824477172121" };
String passwords[SIZE] = { "5555", "5052", "1234", "4321" };
String rfidCode;
String user;
String password;

// screen
LiquidCrystal_I2C lcd(0x27, 20, 4);

// clock
virtuabotixRTC myRTC(clk, io, ce);

#pragma endregion

#pragma region SETUP

void setup()
{
	Serial.begin(9600);
	SPI.begin();
	rfid522.init();

	lcd.init();

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
	pinMode(WaterSensor, INPUT);
	pinMode(GasSensor, INPUT);
	pinMode(ledDoor, OUTPUT);
	pinMode(ledWindow, OUTPUT);

#pragma region criação de tasks

	// task create
	xTaskCreate(Task_Led, "TASK_LED", 256, NULL, 1, &Task_Led_Handle);
	xTaskCreate(Task_Blink_Led_Negative, "TASK_BLINK_LED_NEGATIVE", 256, NULL, 1, &Task_Blink_Led_Negative_Handle);
	xTaskCreate(Task_Pir, "TASK_PIR", 256, NULL, 1, &Task_Pir_Handle);
	xTaskCreate(Task_Magnet, "TASK_MAGNET", 256, NULL, 1, &Task_Magnet_Handle);
	xTaskCreate(Task_Gas, "TASK_GAS", 256, NULL, 1, &Task_Gas_Handle);
	xTaskCreate(Task_Water, "TASK_WATER", 256, NULL, 1, &Task_Water_Handle);
	xTaskCreate(Task_Buzzer, "TASK_BUZZER", 256, NULL, 1, &Task_Buzzer_Handle);
	xTaskCreate(Task_Alarm, "TASK_ALARM", 1024, NULL, 1, &Task_Alarm_Handle);
	xTaskCreate(Task_Led_Water, "TASK_LED_WATER", 256, NULL, 1, &Task_Led_Water_Handle);
	xTaskCreate(Task_Screen, "TASK_LED", 256, NULL, 1, &Task_Screen_Handle);
	xTaskCreate(Task_Led_Presence, "TASK_LED_PRESENCE", 256, NULL, 1, &Task_Led_Presence_Handle);
	xTaskCreate(Task_Presence, "TASK_PRESENCE", 256, NULL, 1, &Task_Presence_Handle);
	xTaskCreate(Task_Read, "TASK_READ", 1024, NULL, 1, &Task_Read_Handle);

#pragma endregion

	/* Inicialização dos valores do alarme */
	Serial.println("");
	delay(500);
	Serial.println("READY#0");
	delay(500);
	for (int i = 0; i < SIZE; i++) {
		if (users[i] != "" && users[i] != "SOS") {
			Serial.print("USER#");
			Serial.println(users[i]);
			delay(500);
		}
	}
	Serial.println("ALARM#0");
	delay(500);
	Serial.println("LED#1,0");
	delay(500);
	Serial.println("LED#2,0");
	delay(500);
	Serial.println("WATER#0");
	delay(500);
	Serial.println("FIRE#0");
	delay(500);
	Serial.println("PIR#1,0");
	delay(500);
	Serial.println("PIR#2,0");
	delay(500);
	Serial.println("DOOR#0");
	delay(500);
	Serial.println("WINDOW#0,0");
	delay(500);
	Serial.println("OUTBREAK#0");
	delay(500);
	Serial.println("READY#1");
	delay(500);

}

#pragma endregion

void loop() {}

#pragma region Task para leitura de comandos

void Task_Read(void* param) {
	(void)param;

	while (1) {

		if (Serial.available() > 0) {
			String readComand = Serial.readStringUntil('\n');
			bool validComand = readComand.startsWith("CMD#");

			if (!validComand) {
				Serial.println("ERRO#Comando inválido.");
				return;
			}

			String option;
			String value;
			int intValue;
			int intOption;

			String comand = readComand.substring(4);
			int index = comand.indexOf(',');

			if (index != -1) {
				option = comand.substring(0, index);
				value = comand.substring(index + 1);
			}

			intOption = option.toInt();

			switch (intOption) {

			case 1:

				intValue = value.toInt();

				if (intValue == 1) {
					ledDoorActive = true;
					consoleControl = 1;
				}
				else if (intValue == 0) {
					ledDoorActive = false;
					consoleControl = 2;
				}

				break;

			case 2:

				intValue = value.toInt();

				if (intValue == 1) {
					ledWindowActive = true;
					consoleControl = 3;
				}
				else if (intValue == 0) {
					ledWindowActive = false;
					consoleControl = 4;
				}

				break;

			case 5:

				user = value;

				break;

			case 6:

				password = value;

				break;

			default:
				Serial.println("ERRO#Comando inválido.");
				break;
			}
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region task para o ecrã lcd

void Task_Screen(void* param) {
	(void)param;

	while (1) {
		// screen
		lcd.backlight();
		if (displayRefresh != true) {
			if (screenClock >= 0) {
				lcd.setCursor(0, 0);
				lcd.print("Alarme SETR");

				if (alarmStatus == -1) {
					lcd.setCursor(0, 1);
					lcd.print("Codigo:");
				}
				else if (alarmStatus == 1) {
					lcd.clear();
					lcd.setCursor(0, 0);
					lcd.print("Alarme SETR");
					lcd.setCursor(0, 1);
					lcd.print("Desarmado");
				}
				else if (alarmStatus == 0) {
					lcd.clear();
					lcd.setCursor(0, 0);
					lcd.print("Alarme SETR");
					lcd.setCursor(0, 1);
					lcd.print("Armado");
				}
				screenClock--;
			}
			else {
				lcd.clear();
				// relogio
				if (alarmStatus == -1) {

					alarmStatus = previousAlarmStatus;

					myRTC.updateTime();

					lcd.setCursor(0, 0);
					lcd.print((String)myRTC.hours + ":" + (String)myRTC.minutes + ":" + (String)myRTC.seconds);
					lcd.setCursor(0, 1);
					lcd.print((String)myRTC.dayofmonth + "/" + (String)myRTC.month + "/" + (String)myRTC.year);
				}
				else {
					myRTC.updateTime();

					lcd.setCursor(0, 0);
					lcd.print((String)myRTC.hours + ":" + (String)myRTC.minutes + ":" + (String)myRTC.seconds);
					lcd.setCursor(0, 1);
					lcd.print((String)myRTC.dayofmonth + "/" + (String)myRTC.month + "/" + (String)myRTC.year);
				}
			}
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para os leds de presença

void Task_Led_Presence(void* param) {
	(void)param;

	while (1) {
		// led door
		if (ledDoorActive == true) {
			if (consoleControl == 1) {
				digitalWrite(ledDoor, HIGH);
				Serial.println("LED#1,1");
				consoleControl = 0;
			}
		}
		else {
			if (consoleControl == 2) {
				digitalWrite(ledDoor, LOW);
				Serial.println("LED#1,0");
				consoleControl = 0;
			}
		}

		// led window
		if (ledWindowActive == true) {
			if (consoleControl == 3) {
				digitalWrite(ledWindow, HIGH);
				Serial.println("LED#2,1");
				consoleControl = 0;
			}
		}
		else {
			if (consoleControl == 4) {
				digitalWrite(ledWindow, LOW);
				Serial.println("LED#2,0");
				consoleControl = 0;
			}
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task simulação de presença

void Task_Presence(void* param) {
	(void)param;

	while (1) {

		// simulador de presença
		if (presence == true)
		{
			if (PresenceClock >= 0)
			{
				PresenceClock--;
			}
			else {
				if (isItDoor == true)
				{
					isItDoor = false;
					ledDoorActive = true;
					ledWindowActive = false;
				}
				else
				{
					isItDoor = true;
					ledDoorActive = false;
					ledWindowActive = true;
				}

				PresenceClock = 60;
			}

			//Serial.println("Simulador: " + (String)PresenceClock);
		}

		vTaskDelay(1000 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para os leds de variados sensores mais genericos

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

		// led gas
		if (gasSensorActive == true) {
			digitalWrite(ledGas, HIGH);

			if (firePrevious != true) {
				Serial.println("FIRE#1");
			}

			firePrevious = true;
		}

		// led magnet
		if (magneticSensorActive == true) {
			digitalWrite(ledMagnetic, HIGH);
		}
		else {
			digitalWrite(ledMagnetic, LOW);
		}

		// led water
		if (waterSensorActive == true) {
			if (WaterSensor == 1) {
				digitalWrite(ledWater, HIGH);
			}
		}
		else {
			digitalWrite(ledWater, LOW);
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

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para o led do sensor de agua

void Task_Led_Water(void* param) {
	(void)param;

	while (1) {

		// led water, liga quando o sensor é ativado
		if (waterSensorActive == true) {

			if (waterPrevious != true) {
				Serial.println("WATER#1");
			}

			if (waterLevel == 1) {
				digitalWrite(ledWater, HIGH);
				vTaskDelay(500 / portTICK_PERIOD_MS);
				digitalWrite(ledWater, LOW);
				vTaskDelay(500 / portTICK_PERIOD_MS);

			}
			else if (waterLevel == 2) {
				digitalWrite(ledWater, HIGH);
				vTaskDelay(250 / portTICK_PERIOD_MS);
				digitalWrite(ledWater, LOW);
				vTaskDelay(250 / portTICK_PERIOD_MS);
			}
			else if (waterLevel == 3) {
				digitalWrite(ledWater, HIGH);
				vTaskDelay(75 / portTICK_PERIOD_MS);
				digitalWrite(ledWater, LOW);
				vTaskDelay(75 / portTICK_PERIOD_MS);
			}

			waterPrevious = true;
		}

		vTaskDelay(100 / portTICK_PERIOD_MS); // delay
	}
}

#pragma endregion

#pragma region Task para os dois sons de alarme, positivo e negativo

void Task_Buzzer(void* param) {
	(void)param;

	while (1) {
		// buzzer
		while (buzzerNegative == true) {

			// tom do alarme quando dipara
			if (buzzerStatus == false) {
				tone(buzzer, 4000);
				vTaskDelay(250 / portTICK_PERIOD_MS);
				noTone(buzzer);
				buzzerStatus = true;
			}
			else {
				tone(buzzer, 3000);
				vTaskDelay(250 / portTICK_PERIOD_MS);
				noTone(buzzer);
				buzzerStatus = false;
			}

			if (buzzerNegative == true) {
				if (alarmPrevious != true) {
					Serial.println("OUTBREAK#1");
					alarmPrevious = true;
				}
			}
			else {
				if (alarmPrevious != false) {
					Serial.println("OUTBREAK#0");
					alarmPrevious = false;
				}
			}
		}

		// tom do alarme para confirmação de authentication e de armado e desarmado
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

		if (buzzerError == true) {
			for (int i = 0; i < 500; i++) {
				tone(buzzer, 3500 + i);
				vTaskDelay(15 / portTICK_PERIOD_MS);
			}
			tone(buzzer, 4000);
			vTaskDelay(250 / portTICK_PERIOD_MS);

			for (int i = 0; i < 500; i++) {
				tone(buzzer, 3300 + i);
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			tone(buzzer, 3800);
			vTaskDelay(250 / portTICK_PERIOD_MS);

			for (int i = 0; i < 500; i++) {
				tone(buzzer, 3100 + i);
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			tone(buzzer, 3600);
			vTaskDelay(250 / portTICK_PERIOD_MS);

			for (int i = 0; i < 500; i++) {
				tone(buzzer, 2900 + i);
				vTaskDelay(10 / portTICK_PERIOD_MS);
			}
			tone(buzzer, 3400);
			vTaskDelay(500 / portTICK_PERIOD_MS);

			noTone(buzzer);
			buzzerError = false;
		}

		vTaskDelay(150 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para o led vermelho com a contagem decrescente para o disparo do alarme

void Task_Blink_Led_Negative(void* param) {
	(void)param;

	while (1) {

		// blink led pir
		if (blinkLedNegativeActive == false) {
			blinkLed = false;
			digitalWrite(ledBlinkRed, LOW);
		}
		else {

			// faz a contagem decrescente sempre que o led liga ate chegar a 0. depois disso o alarme dispara
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

		if (blinkLedNegativeActive2 == false) {
			blinkLed2 = false;
			digitalWrite(ledBlinkRed, LOW);
		}
		else {

			if (blinkLed2 == false) {
				blinkLed2 = true;
				digitalWrite(ledBlinkRed, HIGH);
				buzzerNegative = true;
			}
			else {
				blinkLed2 = false;
				digitalWrite(ledBlinkRed, LOW);
			}
		}

		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para o sensor de movimento pir

void Task_Pir(void* param) {
	(void)param;

	while (1) {

		// ve o valor obtido por cada sensor de movimento pir
		if (digitalRead(pirA) == HIGH) {
			if (pirPreviousA != true) {
				Serial.println("PIR#1,1");

				pirPreviousA = true;
			}
		}
		else {
			if (pirPreviousA != false) {
				Serial.println("PIR#1,0");
			}

			pirPreviousA = false;
		}

		if (digitalRead(pirB) == HIGH) {
			if (pirPreviousB != true) {
				Serial.println("PIR#2,1");

				pirPreviousB = true;
			}
		}
		else {
			if (pirPreviousB != false) {
				Serial.println("PIR#2,0");
			}

			pirPreviousB = false;
		}


		if (digitalRead(pirA) == HIGH || digitalRead(pirB) == HIGH) {
			if (onOff == false) {
				blinkLedNegativeActive = true;
			}
			pirSensorActive = true;
		}
		else if (digitalRead(pirA) == HIGH && loginAdminDoor == true) {
			if (onOff == false) {
				blinkLedNegativeActive = true;
			}
			pirSensorActive = true;
		}
		else if (digitalRead(pirB) == HIGH && loginAdminWindow == true) {
			if (onOff == false) {
				blinkLedNegativeActive = true;
			}
			pirSensorActive = true;
		}
		else {
			pirSensorActive = false;
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para o sensor de gás

void Task_Gas(void* param) {
	(void)param;

	while (1) {

		// le o valor obtido no sensor de gás. é detetado se o valor for low
		if (digitalRead(GasSensor) == LOW) {
			if (onOff == false) {
				blinkLedNegativeActive2 = true;
			}
			gasSensorActive = true;
		}
		else {
			gasSensorActive = false;
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para o sensor magnético da porta e janela

void Task_Magnet(void* param) {
	(void)param;

	while (1) {

		// le o valor obtido por cada sensor magnetico
		if (digitalRead(MagnetA) == 1 || digitalRead(MagnetB) == 1 || digitalRead(MagnetC) == 1) {
			if (onOff == false) {
				blinkLedNegativeActive = true;
			}
			magneticSensorActive = true;
		}
		else {
			magneticSensorActive = false;
		}

		// janela A
		if (digitalRead(MagnetA) == 1 && digitalRead(MagnetB) == 0) {
			if (windowPreviousA != true) {
				Serial.println("WINDOW#1,0");

				windowPrevious = false;
				windowPreviousA = true;
				windowPreviousB = false;
			}
		}
		//janela B
		else if (digitalRead(MagnetB) == 1 && digitalRead(MagnetA) == 0) {
			if (windowPreviousB != true) {
				Serial.println("WINDOW#0,1");

				windowPrevious = false;
				windowPreviousA = false;
				windowPreviousB = true;
			}
		}
		//ambas janelas
		else if (digitalRead(MagnetA) == 1 && digitalRead(MagnetB) == 1) {
			if (windowPrevious != true) {
				Serial.println("WINDOW#1,1");

				windowPrevious = true;
				windowPreviousA = false;
				windowPreviousB = false;
			}
		}
		else {
			if (windowPrevious != false) {
				if (digitalRead(MagnetA) == 0 && digitalRead(MagnetB) == 0) {
					Serial.println("WINDOW#0,0");

					windowPrevious = false;
					windowPreviousA = false;
					windowPreviousB = false;
				}
				else if (digitalRead(MagnetA) == 1 && digitalRead(MagnetB) == 0) {
					Serial.println("WINDOW#1,0");

					windowPrevious = false;
					windowPreviousA = true;
					windowPreviousB = false;
				}
				else if (digitalRead(MagnetA) == 0 && digitalRead(MagnetB) == 1) {
					Serial.println("WINDOW#0,1");

					windowPrevious = false;
					windowPreviousA = false;
					windowPreviousB = true;
				}
			}
			else if (windowPreviousA != false) {
				if (digitalRead(MagnetA) == 0 && digitalRead(MagnetB) == 0) {
					Serial.println("WINDOW#0,0");

					windowPrevious = false;
					windowPreviousA = false;
					windowPreviousB = false;
				}
			}
			else if (windowPreviousB != false) {
				if (digitalRead(MagnetB) == 0 && digitalRead(MagnetA) == 0) {
					Serial.println("WINDOW#0,0");
				}

				windowPrevious = false;
				windowPreviousA = false;
				windowPreviousB = false;
			}
		}
		if (digitalRead(MagnetC) == 1) {
			if (doorPrevious != true) {
				Serial.println("DOOR#1");

				doorPrevious = true;
			}
		}
		else {
			if (doorPrevious != false) {
				if (digitalRead(MagnetC) == 0) {
					Serial.println("DOOR#0");

					doorPrevious = false;
				}
			}
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para sensor de deteção de água

void Task_Water(void* param) {
	(void)param;

	while (1) {

		// le o valor detetado pelo sensor para definir qual o nivel de agua que se encontra no presente
		if (analogRead(WaterSensor) >= 100 && analogRead(WaterSensor) < 500) {
			if (onOff == false) {
				blinkLedNegativeActive2 = true;
			}
			waterLevel = 1;
			waterSensorActive = true;
		}
		else if (analogRead(WaterSensor) >= 500 && analogRead(WaterSensor) < 600) {
			if (onOff == false) {
				blinkLedNegativeActive2 = true;
			}
			waterLevel = 2;
			waterSensorActive = true;
		}
		else if (analogRead(WaterSensor) >= 600) {
			if (onOff == false) {
				blinkLedNegativeActive2 = true;
			}
			waterLevel = 3;
			waterSensorActive = true;
		}
		else {
			waterSensorActive = false;
			waterLevel = 0;
		}

		vTaskDelay(100 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para verificar a autenticação do alarme

void Task_Alarm(void* param) {
	(void)param;

	int i = 0;
	int index = -1;

	while (1) {
		rfidCode = "";
		key = my_key_pad.getKey(); // le guarda tag rfid

		// mostra o codigo tag no serial monitor 
		if (rfid522.isCard())
		{
			rfid522.readCardSerial();
			//Serial.println("Card detected!");
			for (int i = 0; i < 5; i++)
			{
				rfidCode += rfid522.serNum[i];
			}
			Serial.print("RFID#");
			Serial.println(rfidCode);
			screenClock = 30;
		}

		// escreve no serial monitor a tecla que é primida
		if (key != NO_KEY) {
			screenClock = 30;
			Serial.print("KEY#");
			Serial.println(key);
		}

		// liga luz da porta
		if (key == 'A') {
			if (ledDoorActive == true) {
				ledDoorActive = false;
				consoleControl = 2;
			}
			else
			{
				ledDoorActive = true;
				consoleControl = 1;
			}
		}

		// liga luz da janela
		if (key == 'B') {
			if (ledWindowActive == true) {
				ledWindowActive = false;
				consoleControl = 4;
			}
			else
			{
				ledWindowActive = true;
				consoleControl = 3;
			}
		}

		// guarda o codigo introduzido para verificar se a autehentication pode ser validada
		if (RfidCheck(rfidCode)) {
			alarmStatus = -1; // estado de introdução do codigo
			lcd.clear();
			previousAlarmStatus = alarmStatus;

			do {
				key = my_key_pad.getKey();
				if (key != NO_KEY) {
					if (key != '#') {
						authenticationCode += key;
						i++;
						cntDigits++;
						lcd.setCursor(cntDigits + 8, 1);
						lcd.print("*");

						if (screenClock <= 0) {
							break;
						}
					}
				}
			} while (key != '#');
			cntDigits = 0;
		}
		else if (key == '#' && onOff == false) {
			alarmStatus = -1; // estado de introdução do codigo
			lcd.clear();
			previousAlarmStatus = alarmStatus;

			do {
				key = my_key_pad.getKey();
				if (key != NO_KEY) {
					if (key != '#') {
						authenticationCode += key;
						i++;
						cntDigits++;
						lcd.setCursor(cntDigits + 8, 1);
						lcd.print("*");

						if (screenClock <= 0) {
							break;
						}
					}
				}
			} while (key != '#');
			cntDigits = 0;
			rfidCode = "0000000000";
		}

		// verifica dados recebidos da interface
		if (user != "" && password != "") {
			index = RfidIndexUser(user);

			authenticationCode = password;
			rfidCode = rfids[index];
		}

		index = RfidIndex(rfidCode);

		if (index != -1) {

			if (authenticationCode != "") {

				// verifica se o codigo se verifica com a tag rfid dependendo do utilizador
				if (rfids[index] == rfidCode && authenticationCode == passwords[index]) {

					if (users[index] == "Admin") {
						loginAdmin = true;
						loginAdminDoor = true;
						loginAdminWindow = true;
					}
					else if (users[index] == "SOS") {
						loginSOS = true;
					}
					else {
						loginUser = true;
					}

					buzzerError = false;
				}

				// mostra mensagem dependendo do utilizador
				if (loginAdmin == true || loginUser == true || loginSOS == true) {

					// desativa alarme
					blinkLedNegativeActive = false;
					blinkLedNegativeActive2 = false;
					buzzerPositive = true;
					buzzerNegative = false;
					buzzerStatus = false;

					if (waterSensorActive == false) {
						digitalWrite(ledWater, LOW);

						if (waterPrevious != false) {
							Serial.println("WATER#0");
						}

						waterPrevious = false;
					}

					if (gasSensorActive == false) {
						digitalWrite(ledGas, LOW);

						if (firePrevious != false) {
							Serial.println("FIRE#0");
						}

						firePrevious = false;
					}

					// altera o estado da viariavel onOff para verificar se o estado anterior era armado ou desarmado.
					if (onOff == true) {
						onOff = false;
						presence = true;
						alarmStatus = 0;
						Serial.println("ALARM#0");
						lcd.clear();
					}
					else {
						onOff = true;
						presence = false;
						alarmStatus = 1;
						Serial.println("ALARM#1");
						lcd.clear();
					}

					displayRefresh = true;
					lcd.clear();

					// admin
					if (loginAdmin == true) {
						lcd.setCursor(0, 0);
						lcd.print("Administrador:");
						lcd.setCursor(0, 1);
						lcd.print(users[index]);
						vTaskDelay(2000 / portTICK_PERIOD_MS);
						loginAdmin = false;
					}

					// user xavita
					if (loginUser == true) {
						lcd.clear();
						lcd.setCursor(0, 0);
						lcd.print("Utilizador:");
						lcd.setCursor(0, 1);
						lcd.print(users[index]);
						vTaskDelay(2000 / portTICK_PERIOD_MS);
						loginUser = false;
					}

					// SOS
					if (loginSOS == true) {
						lcd.clear();
						lcd.setCursor(0, 0);
						lcd.print("Emergencia:");
						lcd.setCursor(0, 1);
						lcd.print(users[index]);
						vTaskDelay(2000 / portTICK_PERIOD_MS);
						loginSOS = false;
					}
					displayRefresh = false;

					codeError = codeErrorAttempts;
				}

				// authentication fail
				else {
					buzzerError = true;
					// status do alarme, armado ou desarmado.
					if (onOff == true)
					{
						alarmStatus = 1;
					}
					else
					{
						alarmStatus = 0;
					}

					// numero máximo de tentativas possiveis. caso contrario, o alarme dispara e fica armado.
					if (codeError > (codeErrorAttempts - (codeErrorAttempts - 1))) {
						codeError--;
					}
					else {
						buzzerNegative = true;
						alarmStatus = 0;
						onOff = false;
					}
				}

				authenticationCode = ""; // reset à variavel onde guarda a pass introduzida
				user = "";
				password = "";
				rfidCode = "";
			}
		}

		vTaskDelay(100 / portTICK_PERIOD_MS); // delay
		key = NO_KEY; // sem teclas primidas
	}
}

#pragma endregion

#pragma region Funções

bool RfidCheck(String value) {

	for (int i = 0; i < SIZE; i++) {
		if (rfids[i] != "") {
			if (value == rfids[i]) {
				return true;
			}
		}
	}
	return false;
}

int RfidIndex(String value) {

	for (int i = 0; i < SIZE; i++) {
		if (rfids[i] != "") {
			if (value == rfids[i]) {
				return i;
			}
		}
	}
	return -1;
}

int RfidIndexUser(String value) {

	for (int i = 0; i < SIZE; i++) {
		if (users[i] != "") {
			if (value == users[i]) {
				return i;
			}
		}
	}
	return -1;
}

#pragma endregion