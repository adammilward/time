//#define DEBUG

#include <Arduino.h>
#include <stdlib.h>
#include "Controller.h"
#include "Gbl.h"
#include "Utilities/Light.h"
#include "LightCtr.h"

Controller masterCtr;      // handles the remotes
Stream *Gbl::strPtr = &Serial;

Light Red(Gbl::RED_PIN, 0);
Light Green(Gbl::GREEN_PIN, 1);
Light Blue(Gbl::BLUE_PIN, 2);
Light* LightCtr::red = &Red;
Light* LightCtr::green = &Green;
Light* LightCtr::blue = &Blue;

#define MEGA
//#define BTUNO
#ifdef BTUNO
	#include "SoftwareSerial.h"
	SoftwareSerial BT = SoftwareSerial(12, 13); // RX,TX
#endif

#define TME
#ifdef TME
#include <DS3231.h>
#include "TimeCtr.h"
DS3231  Clock(SDA, SCL);
DS3231* TimeCtr::clock = &Clock;
#endif

#define IR
#ifdef IR
#include "IRremote.h"
IRrecv irrecv(Gbl::IREC_PIN);      // from the ir decode library
decode_results Results; // from the ir decode library
#endif

void setup()
{
	// Setup Serial connection
	Serial.begin(115200);
#ifdef MEGA
	Serial3.begin(115200);
#endif
#ifdef BTUNO
	BT.begin(115200); // RX,TX
#endif
#ifdef TME
	Clock.begin();
#endif
	Gbl::strPtr->println(F("we are programmed to receive"));
	Gbl::strPtr = &Serial3;
	Gbl::strPtr->println(F("we are programmed to receive"));
	//Gbl::strPtr->println(F("************ RESET *************"));
	LightCtr::setFadeOffQuick(1);
#ifdef IR
	pinMode(Gbl::IREC_PIN, INPUT);
#endif

}


void loop()
{
	// for anything running on a timer
	masterCtr.timer(millis());

	// run Serial commands
	while (Serial.available()) {
		Gbl::strPtr = &Serial;
		masterCtr.serialReceive();
	}
#ifdef MEGA
	while (Serial3.available()) {
        Gbl::strPtr = &Serial3;
        masterCtr.serialReceive();
    }
#endif

#ifdef BTUNO
	while (BT.available()) {
#ifdef DEBUG
		Gbl::strPtr->println(F("BT.available"));
		Gbl::freeRam();
#endif
        Gbl::strPtr = &BT;
        masterCtr.serialReceive();
        Gbl::freeRam();
    }
#endif

#ifdef IR
    if (irrecv.decode(&Results)) {
		masterCtr.irReceive(Results.value);
		irrecv.resume(); // Receive the next value
	}
#endif
}
