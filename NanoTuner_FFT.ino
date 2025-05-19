#include "arduinoFFT.h" 
#include <Wire.h>
#include <Tiny4kOLED.h>

/*
These values can be changed in order to evaluate the functions
*/
// OLED Display Config

#define CHANNEL A3 // Choose whatever analog pin you'd like.
const uint16_t samples = 128; //This value MUST ALWAYS be a power of 2.
const double samplingFrequency = 1000; //Hz, must be less than 10000 due to ADC. 
//Choose what frequency would be most helpful, using the nyquist theorem.
unsigned int sampling_period_us; //Variable which helps us create a loop later, and also helps us define Period, as this is what Nano runs on.
unsigned long microseconds; // Variable that helps with loop above

/*
These are the input and output vectors
Input vectors receive computed results from FFT
*/
double vReal[samples];//Array for real values of our signal. It captures 128 pieces of info.
double vImag[samples];// Same as above, but for non-real #s
double peak = 0 ; // We'll use this variable later to find the most dominant freq in our FFT, using FFT function.
bool detectFlag = 0 ; // Variable we'll use later to w/ FFT Function.

const int READ_PIN = 2 ;
bool readFlag = 0 ;

/* Create FFT object with weighing factor storage */
ArduinoFFT<double> FFT = ArduinoFFT<double>(vReal, vImag, samples, samplingFrequency, true);

void setup()
{
  Serial.begin(115200); // High baud rate, necessary for many samples.
  Serial.println("Ready"); 

  //Initialize OLED
  oled.begin();
  oled.clear();
  oled.on();
  oled.switchRenderFrame();

  sampling_period_us = round(1000000*(1.0/samplingFrequency));// Defines our period

  pinMode(READ_PIN, INPUT_PULLUP) ; // Takes READ_PIN (D2) and pulls it up to 5V.
}

void loop()
{
  /*SAMPLING*/
  microseconds = micros(); // Starts off each loop by equalizing "microseconds" to micros() (this function is continuously increasing from start to end)
  for(int i=0; i<samples; i++) // From 0-127 samples, this loop keeps on going, then resets
  {
      vReal[i] = analogRead(CHANNEL) ;// You read Analog and store it in the Array Real, until you reace 128 Pieces of info.
      vImag[i] = 0;
      while(micros() - microseconds < sampling_period_us){ // This is an empty loop, but it ensures that the loop above will continue untill 128 pieces of info are stored. Essentially, It is a timing mechanism, more percise than "delay".
        //empty loop
      }
      microseconds += sampling_period_us;// This resets the loop, allowing for more to take place after. Remeber that micros() is constantly increasing from start of program.
  }
  //Bellow are all simply functions that I need to actually run the FFT.
  FFT.dcRemoval() ; //Removes the DC offest of my mic
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward);	//Weigh data
  FFT.compute(FFTDirection::Forward); // Compute FFT
  FFT.complexToMagnitude(); //Compute magnitudes

  //Look to make sure actual sound is detected
  detectFlag = 0 ; //Bool value false for every loop
  for (int i=0; i<samples; i++){// Starts at index 0, and samples until 127, and increases i by one each time
    if (vReal[i]>50){// if one # in the real array is greater than 200, where 200 is 200/1023 which is about 1 Volt, then you are hearing sound.
      detectFlag = 1; // You turn detect flag into true, and now you can apply FFT
    }
  }
if (detectFlag) { // If sound is detected
    peak = FFT.majorPeak(); // Find the dominant frequency
    checkTuning(peak); // Pass 'peak' as an argument
   //printPeak();
} 
else { 
    Serial.println("No sound heard");
}
//Uncomment the following if you'd like to include a button on the board to display "raw data" (see function below)
//if (!digitalRead(READ_PIN) && !readFlag) {
   // readFlag = 1;
   //printRawData();
//} 
//else if (digitalRead(READ_PIN) && readFlag) {
    //readFlag = 0;
//}

// Above code is the code that prints out one output on the serial monitor when the button is pressed, which can help with debugging.
//Logic: Digitalread is high initially, and if you press the button, it goes low.
//readFlag is also false initially, so on a button press it prints raw data, and switches readFlag to True so that it only prints it out once.
// Once you release the button, digitalread is now true (high) and so is readFlag. Now it resets readFlag to false (0) so that you can do this whole cycle again.

}
//OLED Display 
//Below are many if statments that define the ranges of the Low and High E, the B, G, D, and A strings.
 void checkTuning(float peak) {
    oled.clear();
    oled.setFont(FONT8X16);
    float range = peak;
// High E String
    if (290 <= range && range <= 335) {
        oled.setCursor(32, 0);
        oled.print(F("High E"));
        oled.setCursor(32,2);
        if (325 <= peak && peak <= 335) {
            oled.print(F("String tuned"));
        } else if (peak > 335) {
          oled.setCursor(25,2);
          oled.print(F("Tune down"));
        } else {
          oled.setCursor(30,2);
          oled.print(F("Tune up"));
        }
    }
    // B String
    else if (215 <= range && range <= 280) {
        oled.setCursor(32, 0);
        oled.print(F("B String"));
        oled.setCursor(32, 2);
        if (240 <= peak && peak <= 250) {
            oled.print(F("String tuned"));
        } else if (peak > 250) {
            oled.print(F("Tune down"));
        } else {
            oled.print(F("Tune up"));
        }
    }

    // G String
    else if (172 <= range && range <= 215) {
        oled.setCursor(32, 0);
        oled.print(F("G String"));
        oled.setCursor(32, 2);
        if (190 <= peak && peak <= 200) {
            oled.print(F("String tuned"));
        } else if (peak > 200) {
            oled.print(F("Tune down"));
        } else {
            oled.print(F("Tune up"));
        }
    }

    // D String
    else if (128 <= range && range <= 172) {
        oled.setCursor(32, 0);
        oled.print(F("D String"));
        oled.setCursor(32, 2);
        if (140 <= peak && peak <= 150) {
            oled.print(F("String tuned"));
        } else if (peak > 150) {
            oled.print(F("Tune down"));
        } else {
            oled.print(F("Tune up"));
        }
    }

    // A String
    else if (96 <= range && range <= 128) {
        oled.setCursor(32, 0);
        oled.print(F("A String"));
        oled.setCursor(32, 2);
        if (105 <= peak && peak <= 115) {
            oled.print(F("String tuned"));
        } else if (peak > 115) {
            oled.print(F("Tune down"));
        } else {
            oled.print(F("Tune up"));
        }
    }

    // Low E String
    else if (50 <= range && range <= 96) {
        oled.setCursor(32, 0);
        oled.print(F("Low E"));
        oled.setCursor(32, 2);
        if (77 <= peak && peak <= 87) {
            oled.print(F("String tuned"));
        } else if (peak > 87) {
            oled.print(F("Tune down"));
        } else {
            oled.print(F("Tune up"));
        }
    } 

    // No valid frequency detected
    else {
        oled.setCursor(20, 0);
        oled.setFont(FONT6X8);
        oled.print(F("Pluck a String"));
    }
    oled.switchFrame();
}
//Below is function that you can use for debugging. 
void printRawData(){
  for (int i=0; i<samples; i++){
   // Serial.print((samplingFrequency/2) /samples * i) ;
    Serial.print("\t") ;
    Serial.println(vReal[i]) ;
  }
}
void printPeak(){
  for (int i=0; i<samples; i++){
    Serial.println(peak);
  }

}
