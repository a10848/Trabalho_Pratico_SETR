#include <EEPROM.h>
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

// variable
#define countDownTime 30
#define codeErrorAttempts 3
bool pirSensorActive = false;
bool magneticSensorActive = false;
bool waterSensorActive = false;
bool gasSensorActive = false;
bool onOff;
bool blinkLed = false;
bool blinkLedNegativeActive = false;
bool buzzerPositive = false;
bool buzzerNegative = false;
bool buzzerStatus = false;
bool loginAdmin = false;
bool loginUserA = false;
bool loginUserB = false;
int alarmStatus = 0;
int previousAlarmStatus = 0;
int waterLevel = 0;
int countDown = countDownTime;
int codeError = codeErrorAttempts;
int cntDigits = 0;
int screenClock = 30;

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

#pragma region criação de tasks

	// task create
	xTaskCreate(Task_Led, "TASK_LED", 256, NULL, 1, &Task_Led_Handle);
	xTaskCreate(Task_Blink_Led_Negative, "TASK_BLINK_LED_NEGATIVE", 256, NULL, 1, &Task_Blink_Led_Negative_Handle);
	xTaskCreate(Task_Pir, "TASK_PIR", 256, NULL, 1, &Task_Pir_Handle);
	xTaskCreate(Task_Magnet, "TASK_MAGNET", 256, NULL, 1, &Task_Magnet_Handle);
	xTaskCreate(Task_Gas, "TASK_GAS", 256, NULL, 1, &Task_Gas_Handle);
	xTaskCreate(Task_Water, "TASK_WATER", 256, NULL, 1, &Task_Water_Handle);
	xTaskCreate(Task_Buzzer, "TASK_BUZZER", 256, NULL, 1, &Task_Buzzer_Handle);
	xTaskCreate(Task_Alarm, "TASK_ALARM", 2048, NULL, 1, &Task_Alarm_Handle);
	xTaskCreate(Task_Led_Water, "TASK_LED_WATER", 256, NULL, 1, &Task_Led_Water_Handle);
	xTaskCreate(Task_Screen, "TASK_LED", 1024, NULL, 1, &Task_Screen_Handle);

#pragma endregion

}

#pragma endregion

void loop() {}

#pragma region task para o ecrã lcd

void Task_Screen(void* param) {
	(void)param;

	while (1) {
		// screen
		lcd.backlight();

		if (screenClock >= 0) {
			lcd.setCursor(0, 0);
			lcd.print("Alarme SETR");

			if (alarmStatus == -1) {
				lcd.setCursor(0, 1);
				lcd.print("Codigo:");
			}
			else if (alarmStatus == 1) {
				lcd.setCursor(0, 1);
				lcd.print("Desarmado");
			}
			else if (alarmStatus == 0) {
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
		}
		else {
			digitalWrite(ledGas, LOW);
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

		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para o led do sensor de agua

void Task_Led_Water(void* param) {
	(void)param;

	while (1) {

		// led water, liga quando o sensor é ativado
		if (waterSensorActive == true) {
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
		}
		else {
			digitalWrite(ledWater, LOW);
		}
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

		vTaskDelay(500 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para o sensor de movimento pir

void Task_Pir(void* param) {
	(void)param;

	while (1) {

		// ve o valor obtido por cada sensor de movimento pir
		if (digitalRead(pirA) == HIGH || digitalRead(pirB) == HIGH) {
			if (onOff == false) {
				blinkLedNegativeActive = true;
			}
			pirSensorActive = true;
		}
		else {
			pirSensorActive = false;
		}

		vTaskDelay(50 / portTICK_PERIOD_MS);
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
				blinkLedNegativeActive = true;
			}
			gasSensorActive = true;
		}
		else {
			gasSensorActive = false;
		}

		vTaskDelay(50 / portTICK_PERIOD_MS);
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

		vTaskDelay(50 / portTICK_PERIOD_MS);
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
				blinkLedNegativeActive = true;
			}
			waterLevel = 1;
			waterSensorActive = true;
		}
		else if (analogRead(WaterSensor) >= 500 && analogRead(WaterSensor) < 600) {
			if (onOff == false) {
				blinkLedNegativeActive = true;
			}
			waterLevel = 2;
			waterSensorActive = true;
		}
		else if (analogRead(WaterSensor) >= 600) {
			if (onOff == false) {
				blinkLedNegativeActive = true;
			}
			waterLevel = 3;
			waterSensorActive = true;
		}
		else {
			waterSensorActive = false;
			waterLevel = 0;
		}

		vTaskDelay(50 / portTICK_PERIOD_MS);
	}
}

#pragma endregion

#pragma region Task para verificar a autenticação do alarme

void Task_Alarm(void* param) {
	(void)param;

	int i = 0;

	while (1) {
		rfidCode = "";
		key = my_key_pad.getKey(); // le guarda tag rfid

		// mostra o codigo tag no serial monitor 
		if (rfid522.isCard())
		{
			rfid522.readCardSerial();
			Serial.println("Card detected!");
			for (int i = 0; i < 5; i++)
			{
				rfidCode += rfid522.serNum[i];
			}
			Serial.println(rfidCode);
			screenClock = 30;
		}

		// escreve no serial monitor a tecla que é primida
		if (key != NO_KEY) {
			screenClock = 30;
			Serial.println(key);
		}

		// guarda o codigo introduzido para verificar se a autehentication pode ser validada
		if (rfidCode == rfidAdmin || rfidCode == rfidUserA || rfidCode == rfidUserB) {
			alarmStatus = -1; // estado de introdução do codigo
			lcd.clear();
			previousAlarmStatus = alarmStatus;
			do {
				key = my_key_pad.getKey();
				if (key != NO_KEY) {
					if (key != '#') {
						authenticationCode += key;
						i++;
						Serial.print("*");
						cntDigits++;
						lcd.setCursor(cntDigits + 8, 1);
						lcd.print("*");

						if (screenClock <= 0) {
							break;
						}
					}
				}
			} while (key != '#');
		}

		if (authenticationCode != "") {
			Serial.println("");
			Serial.println(authenticationCode);

			// verifica se o codigo se verifica com a tag rfid dependendo do utilizador
			if (rfidCode == rfidAdmin && authenticationCode == authenticationAdmin) {
				loginAdmin = true;
			}
			else if (rfidCode == rfidUserA && authenticationCode == authenticationUserA) {
				loginUserA = true;
			}
			else if (rfidCode == rfidUserB && authenticationCode == authenticationUserB) {
				loginUserB = true;
			}
			else if (authenticationCode == authenticationAdmin) {
				loginAdmin = true;
			}

			// mostra mensagem dependendo do utilizador
			if (loginAdmin == true || loginUserA == true || loginUserB == true) {
				Serial.println("Authentication successful!");

				// admin
				if (loginAdmin == true) {
					Serial.println("Welcome Admin...");
					loginAdmin = false;
				}

				// user xavita
				if (loginUserA == true) {
					Serial.println("Welcome Xavita...");
					loginUserA = false;
				}

				// user guelhas
				if (loginUserB == true) {
					Serial.println("Welcome Guelhas...");
					loginUserB = false;
				}

				// desativa alarme
				blinkLedNegativeActive = false;
				buzzerPositive = true;
				buzzerNegative = false;
				buzzerStatus = false;
				cntDigits = 0;

				// altera o estado da viariavel onOff para verificar se o estadoanterior era armado ou desarmado.
				if (onOff == true) {
					onOff = false;
					alarmStatus = 0;
					Serial.println("Alarm deactivated!");
					lcd.clear();
				}
				else {
					onOff = true;
					alarmStatus = 1;
					Serial.println("Alarm activated!");
					lcd.clear();
				}

				codeError = codeErrorAttempts;
			}

			// authentication fail
			else {
				Serial.println("Authentication unsuccessful!");

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
		}

		vTaskDelay(100 / portTICK_PERIOD_MS); // delay
		key = NO_KEY; // sem teclas primidas
	}
}

#pragma endregion