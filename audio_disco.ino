#include <arduinoFFT.h>
#define SAMPLES 64
#define power 2
#define rled 5
#define gled 9
#define bled 11
double vReal[SAMPLES];
double vImag[SAMPLES];
int glow;
arduinoFFT FFT = arduinoFFT();

void setup() {
  ADCSRA = 0b11100101;    // set ADC to free running mode and set pre-scalar to 32 (0xe5)
  ADMUX = 0b00000000;     // use pin A0 and external voltage reference
  pinMode(power, OUTPUT);  
  pinMode(rled,OUTPUT);
  pinMode(gled,OUTPUT);
  pinMode(bled,OUTPUT);
  digitalWrite(power, HIGH);
  Serial.begin(115200);
}
void loop() {
  
  // Sampling    
  for (int i = 0; i < SAMPLES; i++)
  {
    while (!(ADCSRA & 0x10));
    ADCSRA = 0b11110101 ;
    int value = ADC - 512 ;
    vReal[i] = value / 8;
    vImag[i] = 0;
  }

  //FFT
  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);

  //LED control
  for (int i = 0; i < SAMPLES/2; i++){
    vReal[i] = constrain(vReal[i], 0, 40);
    vReal[i] = map(vReal[i], 0, 40, 0, 255);
    glow = vReal[i];
    Serial.print(vReal[i]);
    if(glow<64){  
      analogWrite(bled,glow*4); 
      delay(10); 
      analogWrite(bled,0);
    }
    else if(glow>=64 && glow<128){  
      analogWrite(bled,glow*2); 
      analogWrite(gled,255-glow); 
      delay(10); 
      analogWrite(bled,0); 
      analogWrite(gled,0);
    }
    else if(glow>=128 && glow<192){
      analogWrite(rled,glow*5/4); 
      analogWrite(bled,350-glow); 
      delay(10); 
      analogWrite(bled,0); 
      analogWrite(rled,0);
    }
    else{
      analogWrite(rled,glow); 
      delay(10); 
      analogWrite(rled,0);
    }
  }
}
