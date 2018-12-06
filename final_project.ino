#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioSynthWaveform       waveform1;      //xy=110,400
AudioInputI2S            i2s1;           //xy=72,329
AudioEffectFreeverb      freeverb1;      //xy=248,221
AudioEffectMultiply      multiply1;      //xy=263,380
AudioEffectBitcrusher    bitcrusher1;    //xy=250,183
AudioMixer4              mixer1;         //xy=423,297
AudioOutputI2S           i2s5;           //xy=582,304
AudioConnection          patchCord1(i2s1, 0, bitcrusher1, 0);
AudioConnection          patchCord2(i2s1, 0, freeverb1, 0);
AudioConnection          patchCord6(waveform1, 0, multiply1,1);
AudioConnection          patchCord11(i2s1, 0, multiply1, 0);
AudioConnection          patchCord4(i2s1, 0, mixer1, 3);
AudioConnection          patchCord5(freeverb1, 0, mixer1, 1);
AudioConnection          patchCord8(multiply1, 0, mixer1, 2);
AudioConnection          patchCord7(bitcrusher1, 0, mixer1, 0);
AudioConnection          patchCord10(mixer1, 0, i2s5, 0);
AudioConnection          patchCord9(mixer1, 0, i2s5, 1);

AudioControlSGTL5000     sgtl5000_1;     //xy=268,367
// GUItool: end automatically generated code

#include <Encoder.h>
#include <Wire.h>
#include <Adafruit_Si4713.h>
#define RESETPIN 34
#define FMSTATION freqVal      // 10230 == 102.30 MHz
Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);

Encoder myEnc (31, 32);
Encoder myEnc2 (29, 30);
int newPosition = 0;
int oldPosition = 0;
int newPosition2 = 0;
int oldPosition2 = 0;
int audioShieldVal = 0;
int freqVal = 8800;
int lowestNoise = 100;
int cleanestChannel;
int control1 = 0;
int control2 = 0;



void setup() {

  Serial1.begin(9600);
  setBrightness(255);

  AudioMemory(50);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.8);
  sgtl5000_1.inputSelect(AUDIO_INPUT_LINEIN);
  waveform1.begin(WAVEFORM_TRIANGLE);
  waveform1.amplitude(1);
  mixer1.gain(0, 0); // bitcrusher
  mixer1.gain(1, 0); // freeverb
  mixer1.gain(2, 0); // multiply
  mixer1.gain(3, 0); // bypass





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
    if (radio.currNoiseLevel < lowestNoise) {
      cleanestChannel = f;
      lowestNoise = radio.currNoiseLevel;

    }

  }
  Serial.print("cleanest channel: ");
  Serial.println(cleanestChannel);
  clearDisplay();
  freqVal = cleanestChannel;
  set7seg();



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
  effectsRack();

  // displayNumber(freqVal / 10);
  //  Serial1.write(0x77);
  //  Serial1.write(0b0000100); //3rd position decimal

  //  else if (freqVal > 9990) {
  //    displayNumber(freqVal / 10);
  //    Serial1.write(0x76);  // Clear display command
  //    Serial1.write(0x77);
  //    Serial1.write(0b0000010); //2nd position decimal
  //  }

  //  delay(200);
  /**
     FM loop

  */
  // delay (2000);
  // radio.readASQ();
  //  Serial.print("\tCurr ASQ: 0x");
  //  Serial.println(radio.currASQ, HEX);
  //  Serial.print("\tCurr InLevel:");
  //  Serial.println(radio.currInLevel);
  //  toggle GPO1 and GPO2
  //  radio.setGPIO(_BV(1));
  //  delay(500);
  //  radio.setGPIO(_BV(2));
  //  delay(500);
  /**
     Rotary Encoder Loop
  */
  encoderFunc();
  audioShieldEnc();
}



void encoderFunc() {

  newPosition = myEnc.read();

  if (newPosition >= oldPosition + 4) {
    oldPosition = newPosition;
    //freqVal = oldPosition / 4 * (10) + 8750;
    freqVal = freqVal + 10;
    if (freqVal > 10800) {
      freqVal = 8750;
    }
    Serial.println("forward");
    Serial.println(freqVal);
    setFMmodule();
    set7seg();
  } else if (newPosition <= oldPosition - 4) {
    oldPosition = newPosition;
    //freqVal = oldPosition / 4 * (10) + 8750;
    freqVal = freqVal - 10;
    if (freqVal < 8750) {
      freqVal = 10800;
    }
    Serial.println("backwards");
    Serial.println(freqVal);

    setFMmodule();
    set7seg();
  }
}

void audioShieldEnc() {
  newPosition2 = myEnc2.read();

  if (newPosition2 >= oldPosition2 + 4) {
    oldPosition2 = newPosition2;
    audioShieldVal = audioShieldVal + 1;
    if (audioShieldVal > 3) {
      audioShieldVal = 0;
    }
    Serial.println("forward");
    Serial.println(audioShieldVal);

  } else if (newPosition2 <= oldPosition2 - 4) {
    oldPosition2 = newPosition2;
    audioShieldVal = audioShieldVal - 1;
    if (audioShieldVal < 0) {
      audioShieldVal = 3;
    }
    Serial.println("backwards");
    Serial.println(audioShieldVal);

  }
}


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

void set7seg () {
  displayNumber(freqVal / 10);
  Serial1.write(0x77);
  Serial1.write(0b0000100); //3rd position decimal
}

void effectsRack () {

  if (audioShieldVal == 0) { //Bypass
    control1 = map(analogRead(A16), 0, 1023, 0, 50);
    float control1grad = control1 / 50.0;
    mixer1.gain(3, 1);
    mixer1.gain(0, 0); // bitcrusher
    mixer1.gain(1, 0); // freeverb
    mixer1.gain(2, 0); // chorus
  }
  if (audioShieldVal == 1) { // multiply
    control1 = map(analogRead(A16), 0, 1023, 20, 20000);
    control2 = map(analogRead(A17), 0, 1023, 0, 50);
    float control2grad = control2 / 50.0;
    mixer1.gain(2, 1);
    mixer1.gain(0, 0); // bitcrusher
    mixer1.gain(1, 0); // freeverb
    mixer1.gain(3, 0.3); // chorus
    waveform1.frequency(control1);
    waveform1.amplitude(control2grad);

  }
  if (audioShieldVal == 2) { // freeverb
    control1 = map(analogRead(A16), 0, 1023, 0, 50);
    control2 = map(analogRead(A17), 0, 1023, 0, 50);
    float control1grad = control1 / 50.0;
    float control2grad = control2 / 50.0;
    mixer1.gain(1, 1);
    mixer1.gain(0, 0); // bitcrusher
    mixer1.gain(2, 0); // freeverb
    mixer1.gain(3, 0); // chorus
    freeverb1.roomsize(control1grad);
    freeverb1.damping(control2grad);


  }
  if (audioShieldVal == 3) { // bitcrusher
    control1 = map(analogRead(A16), 0, 1023, 1, 16);
    control2 = map(analogRead(A17), 0, 1023, 1, 44100);
    mixer1.gain(0, 1);
    mixer1.gain(3, 0); // bitcrusher
    mixer1.gain(1, 0); // freeverb
    mixer1.gain(2, 0); // chorus
    bitcrusher1.bits(control1);
    bitcrusher1.sampleRate(control2);

  }
}
