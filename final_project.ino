
#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_Si4713.h>
#define RESETPIN 34
#define FMSTATION freqVal      // 10230 == 102.30 MHz
Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

Encoder myEnc (31, 32);
int newPosition = 0;
int oldPosition = 0;
int freqVal = 0;



void setup() {

  Serial1.begin(9600);
  setBrightness(255);



  /**
     FM transmitter setup
  */
  Serial.begin(9600);
  Serial.println("Adafruit Radio - Si4713 Test");

  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("Couldn't find radio?");
    while (1);
  }

  // Uncomment to scan power of entire range from 87.5 to 108.0 MHz

  for (uint16_t f  = 8750; f < 10800; f += 10) {
    radio.readTuneMeasure(f);
    Serial.print("Measuring "); Serial.print(f); Serial.print("...");
    radio.readTuneStatus();
    Serial.println(radio.currNoiseLevel);
  }


  Serial.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max

  Serial.print("\nTuning into ");
  Serial.print(FMSTATION / 100);
  Serial.print('.');
  Serial.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION); // 102.3 mhz

  // This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();
  Serial.print("\tCurr freq: ");
  Serial.println(radio.currFreq);
  Serial.print("\tCurr freqdBuV:");
  Serial.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:");
  Serial.println(radio.currAntCap);

  // begin the RDS/RDBS transmission
  radio.beginRDS();
  radio.setRDSstation("AdaRadio");
  radio.setRDSbuffer( "Adafruit g0th Radio!");

  Serial.println("RDS on!");

  radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
  /**
     Rotary Encoder Stuff
  */
  Serial.println("Basic Encoder Test:");

}



void loop() {
  /**
     7 Seg
  */

  displayNumber(freqVal / 10);
  Serial1.write(0x77);
  Serial1.write(0b0000100); //3rd position decimal

  //  else if (freqVal > 9990) {
  //    displayNumber(freqVal / 10);
  //    Serial1.write(0x76);  // Clear display command
  //    Serial1.write(0x77);
  //    Serial1.write(0b0000010); //2nd position decimal
  //  }

  delay(500);
  /**
     FM loop

  */
  delay (2000);
  radio.readASQ();
  Serial.print("\tCurr ASQ: 0x");
  Serial.println(radio.currASQ, HEX);
  Serial.print("\tCurr InLevel:");
  Serial.println(radio.currInLevel);
  // toggle GPO1 and GPO2
  radio.setGPIO(_BV(1));
  delay(500);
  radio.setGPIO(_BV(2));
  delay(500);
  /**
     Rotary Encoder Loop
  */
  encoderFunc();
}



void encoderFunc() {

  newPosition = myEnc.read();

  if (newPosition >= oldPosition + 4) {
    oldPosition = newPosition;
    freqVal = oldPosition / 4 * (10);
    Serial.println("forward");
    Serial.println(freqVal);
    setFMmodule();
  } else if (newPosition <= oldPosition - 4) {
    oldPosition = newPosition;
    freqVal = oldPosition / 4 * (10);
    Serial.println("backwards");
    Serial.println(freqVal);
    setFMmodule();

  }
}
//  if (freqVal > 10800) {
//    freqVal = 8750;
//    setFMmodule();
//  } else if (freqVal < 8750) {
//    freqVal = 10800;
//    setFMmodule();
//  }
//  } else if (freqVal == 0) {
//    freqVal = 8750;
//    setFMmodule();
//  }




void setFMmodule () {
  Serial.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max

  Serial.print("\nTuning into ");
  Serial.print(FMSTATION / 100);
  Serial.print('.');
  Serial.println(FMSTATION % 100);
  radio.tuneFM(FMSTATION); // 102.3 mhz
}

void displayNumber(int number) {
  char displayFormattedNumber[4];
  sprintf(displayFormattedNumber, "%4d", number);
  clearDisplay();
  Serial1.print(displayFormattedNumber);
}

void clearDisplay()
{
  Serial1.write(0x76);  // Clear display command
}
void setBrightness(int number) {
  Serial1.write(0x7A);
  Serial1.write(number);
}
