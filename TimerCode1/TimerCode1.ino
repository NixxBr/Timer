//====================================================================
//  TIMER ARDUINO BASED 
//====================================================================
// To Do :
// Status	|  Num	| Description
// Done		|  1.	| Save Shots to Memory  = Done
//			|		|    first attempt - array of longs (each is the elapsed in milliseconds) 
//			|		|
// Done		|  2.	| Go button = review  = stop timer = mode = review = Done
//			|		|    = show each shot & split & elapsed from 1 to n & then loop
//			|		|
// Done		|  3.	| HOW DO WE RESTART TIMER to get next shooters times? 
//			|		|	   - hold go button for x seconds? 
//			|		|      - how do we do this?  - can show bar graph on display as we get near reset time.
//			|		|	-or do we use another button ?  -  for reset ? like this better for now. = Done
//			|		|	(in future versions - buttons can be swapped for lefties)
//			|		|	for dev puposes for now - in in review & click on shot button = reset button.
//			|		|
//                      |      4.	|	Use Pin13 (LED) for now as indicator of Buzzer	
//			|		|
//			|	5.	|	Setup Screens & Configuration values => EEPROM 
//			|		|		- Buzzer Volume ?  (voltage output = PWM?)
//			|		|		- Sensitivity
//			|		|		- backlight duration / off 
//			|		|		- Rot Encoder or two button nav ?
//			|		|		- both buttons for setup?
//			|		|
//			|	6.	|	Listen for shots - 
//			|		|		- microphone Circuit
//			|		|		- sensitivity
//			|		|
// ================================================================================================
 
#include <math.h>
#include <LiquidCrystal.h>
//===========Timer State ==========================
#define TIMERSTATE_IDLE 0;
#define TIMERSTATE_RUNNING 1;
#define TIMERSTATE_REVIEW 2;

// Ken Setup  =================================================================
//LiquidCrystal lcd(6, 7, 8, 9, 10, 11);
//Rob Setup   =================================================================
LiquidCrystal lcd(A8, A9, A10, A11, A12, A13 );
//==================================================
int shotCntr = 0;
int ReviewShot = 0;
int ledpin = 13;
//=============Shot Btn ==================
int ShotPin = A0;
int oldShotBtnVal = 0;
int shotBtnVal = 0;
// =========== start btn ==============
int goBtnPin = A1;
int oldGoBtnVal = 0;
int goBtnVal = 0;

int TimerState = TIMERSTATE_IDLE;

long startTime = 0;
long elapsed = 0;
long oldElapsed = 0;
long diff = 0;
float divStuff = 0;
float divStuff2 = 0;
float displayTime = 0;
int rem = 0;
int mins = 0;
int secs = 0;

// shot memory
long shots[999];

void setup()
{
	lcd.begin(20, 4);
	Serial.begin(115200);
	/* add setup code here */

	pinMode(ledpin, OUTPUT);
	//cntr = 0;
	pinMode(ShotPin, INPUT);
	//flash led 4 times -
	delay(2000);
	for (int i = 0; i < 4; i++)
	{
		delay(250);
		digitalWrite(ledpin, HIGH);
		delay(250);
		digitalWrite(ledpin, LOW);
	}
	// get initial value of Pin
	oldShotBtnVal = digitalRead(ShotPin);
	oldGoBtnVal = digitalRead(goBtnPin);
	
	lcd.print("Timer v0.2");
	lcd.setCursor(0, 1);
	lcd.print("release go btn");

}

void TimerStart()
{
	// this is when buzzer sounds and elapsed is set to 0;
	// also screen updated to show running Timer;
	
	startTime = millis(); 
	elapsed = 0;
	shotCntr = 0;
	// set up LCD
	lcd.clear();
	lcd.print("Running");
	lcd.setCursor(0, 1);
	lcd.print("Shot # : 0");
	lcd.setCursor(0, 2);
	lcd.print("Split :");
	lcd.setCursor(0, 3);
	lcd.print("Elapsed :");
	TimerState = TIMERSTATE_RUNNING;

	// set first shot to 0 (just in case)
	shots[0] = 0;
}

void displayElapsed(int thisShot)
{
	// each time we display - we loop through shots so we can keep track of:
	//    - total elapsed
	//    - this split
	// ======================================================
	long split = 0;

	// Calc split  =============================
	if (thisShot == 0)
	{
		split = shots[thisShot];
	}
	else
	{
		split = shots[thisShot] - shots[thisShot - 1];
	}
	
	// convert to sec/100 (instead of sec/1000 )  ========
	divStuff = (float)split / (float)10;
	divStuff2 = (float)shots[thisShot] / (float)10;

	// update Display ====================================
	// Shot number -------------
	lcd.setCursor(9, 1);
	lcd.print("         ");
	lcd.setCursor(9, 1);
	if (TimerState == 2) // TIMERSTATE_REVIEW
	{
		lcd.print(thisShot + 1);  //zero based
		lcd.print("/");
		lcd.print(shotCntr);
	}
	else
	{
		lcd.print(thisShot + 1);  //zero based
	}
	// Split ----------------------------
	lcd.setCursor(8, 2);
	lcd.print("        ");
	lcd.setCursor(8, 2);
	lcd.print((float)round(divStuff) / (float)100);
	// Elapsed ------------------------------
	lcd.setCursor(10, 3);
	displayTime = round((float)shots[thisShot] / (float)10) / (float)100;

	if (displayTime > 60)
	{
		// get minutes from displayTime
		rem = (int)displayTime % 60;
		// get mins 
		mins = ((int)displayTime - rem) / 60;
		lcd.print(mins);
		lcd.print(":");
		if (rem < 10) lcd.print("0");
		lcd.print(rem);
		// seconds 
		lcd.print(".");
		secs = (long)(displayTime * 100) - ((long)displayTime) * 100;
		if (secs < 10)lcd.print("0");
		lcd.print(secs);
	}
	else
	{
		lcd.print(displayTime);
	}
}


void recordShot()
{
	elapsed = millis() - startTime;
	// save to shots
	shots[shotCntr] = elapsed;
	// display Shot
	displayElapsed(shotCntr);
	shotCntr = shotCntr + 1;
}

void loop()
{
	if (TimerState == 0)  // TIMERSTATE_IDLE
	{
		goBtnVal = digitalRead(goBtnPin);
		Serial.println("=======in loop ===================");
		Serial.println("goBtnVal");
		Serial.println(goBtnVal);
		Serial.println("==========================");
		if (goBtnVal != oldGoBtnVal)
		{
			// start button is diff ans been released
			if (goBtnVal == 0)
			{
				Serial.println("Running TimerStart");
				TimerStart();
			}
			oldGoBtnVal = goBtnVal;
		}
	}
	if (TimerState == 1) // TIMERSTATE_RUNNING
	{
		shotBtnVal = digitalRead(ShotPin);
		if (shotBtnVal != oldShotBtnVal)
		{
			// this happens when we hear a PEW!
			if (shotBtnVal == 0)  // we check that this is the start of the PEW sound.
			{
				// record Bang;
				recordShot();

			}

			oldShotBtnVal = shotBtnVal;
		}

		// check for Review mode
		goBtnVal = digitalRead(goBtnPin);
		if (goBtnVal != oldGoBtnVal)
		{
			// start button is diff ans been released
			if (goBtnVal == 0)
			{
				Serial.println("Change To ReviewMode");
				TimerState = TIMERSTATE_REVIEW;
				lcd.setCursor(0, 0);
				lcd.print("Review");
				ReviewShot = 0;
				displayElapsed(ReviewShot);
			}
			oldGoBtnVal = goBtnVal;
		}



	}
	if (TimerState == 2)  // TIMERSTATE_REVIEW
	{
		goBtnVal = digitalRead(goBtnPin);
		Serial.println("=======in loop ===================");
		Serial.println("goBtnVal");
		Serial.println(goBtnVal);
		Serial.println("==========================");
		if (goBtnVal != oldGoBtnVal)
		{
			// start button is diff ans been released
			if (goBtnVal == 0)
			{
				ReviewShot++;
				if (ReviewShot >= shotCntr) { ReviewShot = 0; }
				Serial.print("Reviewing Shot : ");
				Serial.println(ReviewShot);
				displayElapsed(ReviewShot);
			}
			oldGoBtnVal = goBtnVal;
		}
		shotBtnVal = digitalRead(ShotPin);
		if (shotBtnVal != oldShotBtnVal)
		{
			// this happens when we hear a PEW!
			if (shotBtnVal == 0)  // we check that this is the start of the PEW sound.
			{
				// record Bang;
				// RESET TIMER;
				TimerState = TIMERSTATE_IDLE;
				shotCntr = 0;
				ReviewShot = 0;
				lcd.clear();
				lcd.print("Timer v0.2");
				lcd.setCursor(0, 1);
				lcd.print("release go btn");

			}

			oldShotBtnVal = shotBtnVal;
		}
	}
}
