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
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
//===========Timer State ==========================
#define TIMERSTATE_IDLE 0;
#define TIMERSTATE_RUNNING 1;
#define TIMERSTATE_REVIEW 2;

// Software SPI (slower updates, more flexible pin options):
// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);
//==================================================
int shotCntr = 0;
int ReviewShot = 0;
int ledpin = 13;
//=============Shot Btn ==================
int shotPin = 12;
int oldShotBtnVal = 0;
int shotBtnVal = 0;
// =========== start btn ==============
int goBtnPin = 11;
int oldGoBtnVal = 0;
int goBtnVal = 0;
// =========== reset btn ==============
int resetBtnPin = 10;
int oldResetBtnVal = 0;
int resetBtnVal = 0;



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
long shots[99];

void setup()
{
        display.begin();
        display.setContrast(50);
        display.clearDisplay();
        display.setTextSize(1);
        display.setTextColor(BLACK); 
        
  	//lcd.begin(20, 4);
	Serial.begin(115200);
	/* add setup code here */

	pinMode(ledpin, OUTPUT);
	//cntr = 0;
	pinMode(shotPin, INPUT);
	pinMode(resetBtnPin, INPUT);
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
	oldShotBtnVal = digitalRead(shotPin);
	oldGoBtnVal = digitalRead(goBtnPin);
	oldResetBtnVal = digitalRead(resetBtnPin);

	display.println("Timer v0.3");
	
	display.print("release go btn");
        display.display();
}

void TimerStart()
{
	// this is when buzzer sounds and elapsed is set to 0;
	// also screen updated to show running Timer;
	
	startTime = millis(); 
	elapsed = 0;
	shotCntr = 0;
	// set up LCD
        display.setTextSize(1);
	display.clearDisplay();
	display.print("# Run    Split");
        display.println("");
        //display.println("");
        display.println("ELAPSED ");
        display.display();
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
	display.clearDisplay();
	display.print("# Run    Split");

	if (TimerState == 2) // TIMERSTATE_REVIEW
	{
		display.print(thisShot + 1);  //zero based
		display.print("/");
		display.print(shotCntr);
	}
	else
	{
		display.print(thisShot + 1);  //zero based
	}

	// Split ----------------------------
        display.setCursor(48,9);
	display.println((float)round(divStuff) / (float)100);

        display.println("ELAPSED ");
	// Elapsed ------------------------------
	display.setCursor(0, 33);
        display.setTextSize(2);
	displayTime = round((float)shots[thisShot] / (float)10) / (float)100;

	if (displayTime > 60)
	{
		// get minutes from displayTime
		rem = (int)displayTime % 60;
		// get mins 
		mins = ((int)displayTime - rem) / 60;
		display.print(mins);
		display.print(":");
		if (rem < 10) display.print("0");
		display.print(rem);
		// seconds 
		display.print(".");
		secs = (long)(displayTime * 100) - ((long)displayTime) * 100;
		if (secs < 10)display.print("0");
		display.print(secs);
	}
	else
	{
		display.print(displayTime);
	}
        display.setTextSize(1);
        display.display();
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
		//Serial.println("=======in loop ===================");
		//Serial.println("goBtnVal");
		//Serial.println(goBtnVal);
		//Serial.println("==========================");
		if (goBtnVal != oldGoBtnVal)
		{
			// start button is diff ans been released
			if (goBtnVal == 0)
			{
				//Serial.println("Running TimerStart");
				TimerStart();
			}
			oldGoBtnVal = goBtnVal;
		}
	}
	if (TimerState == 1) // TIMERSTATE_RUNNING
	{
		shotBtnVal = digitalRead(shotPin);
		if (shotBtnVal != oldShotBtnVal)
		{
			// this happens when we hear a PEW!
			if (shotBtnVal == 1)  // we check that this is the start of the PEW sound.
			{
				// record Bang;
				recordShot();
                                delay(10);

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
				//Serial.println("Change To ReviewMode");
				TimerState = TIMERSTATE_REVIEW;
				display.setCursor(0, 0);
				display.setTextSize(1);
                                display.print("Review");
                                display.display();
				ReviewShot = 0;
				displayElapsed(ReviewShot);
			}
			oldGoBtnVal = goBtnVal;
		}



	}
	if (TimerState == 2)  // TIMERSTATE_REVIEW
	{
		goBtnVal = digitalRead(goBtnPin);
		//Serial.println("=======in loop ===================");
		//Serial.println("goBtnVal");
		//Serial.println(goBtnVal);
		//Serial.println("==========================");
		if (goBtnVal != oldGoBtnVal)
		{
			// start button is diff ans been released
			if (goBtnVal == 0)
			{
				ReviewShot++;
				if (ReviewShot >= shotCntr) { ReviewShot = 0; }
				//Serial.print("Reviewing Shot : ");
				//Serial.println(ReviewShot);
				displayElapsed(ReviewShot);
			}
			oldGoBtnVal = goBtnVal;
		}
		resetBtnVal = digitalRead(resetBtnPin);
		if (resetBtnVal != oldResetBtnVal)
		{
			// this happens when we hear a PEW!
			if (resetBtnVal == 0)  // we check that this is the start of the PEW sound.
			{
				// record Bang;
				// RESET TIMER;
				TimerState = TIMERSTATE_IDLE;
				shotCntr = 0;
				ReviewShot = 0;
				display.clearDisplay();
                                display.setTextSize(1);
				display.println("Timer v0.2");
	
	                        display.print("release go btn");
                                display.display();
			}

			oldResetBtnVal = resetBtnVal;
		}
	}
}
