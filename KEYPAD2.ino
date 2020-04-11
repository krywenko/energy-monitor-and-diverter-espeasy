
const int CT1 = 1; 
const int CT2 = 1;                                                      // Set to 0 to disable 
const int CT3 = 1;
const int CT4 = 1;
#include "EmonLib.h"
EnergyMonitor ct1,ct2,ct3, ct4; 
  
typedef struct { int power1, power2, power3, power4, Vrms;} PayloadTX;     
PayloadTX emontx;  


//Sample using LiquidCrystal library
#include <LiquidCrystal.h>
#include <EEPROM.h>
int address = 0;
int address1 = 2;

// select the pins used on the LCD panel
LiquidCrystal lcd(8, 9, 4, 5, 6, 7); /////uno



// define some values used by the panel and buttons
int lcd_key     = 0;
int adc_key_in  = 0;
int pulse = 0;
int count = 0;
int KW =0;
boolean settled = false;
boolean time_out = false;
#define FILTERSETTLETIME 5000 // 
#define timeout 30000 // 
#define btnRIGHT  0
#define btnUP     1
#define btnDOWN   2
#define btnLEFT   3
#define btnSELECT 4
#define btnNONE   5
#define PWM 3

int Sp = 2; //  if using one CT  on N.A spit phase 240V set to 2  otherwise set to 1
int TYPE = 1;
int element = 4000;

int Max_element_size;  // determined by DIVs * 255
float Bubble_Diff;
int DIVs;

 //#### Non - adjustable 
float grid = 0;     //grid   usage
float LINE = 0;

float stepa4 = 0;   
float stepb4 = 1;
float stepc4 = 1;
float prestep4 =0; 
int stepbu4;
int stat4 ;
float curelem4 =1;
int curgrid4 = 0;
int sV;

// read the buttons
int read_LCD_buttons()
{
 adc_key_in = analogRead(5);      // read the value from the sensor
 // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
 // we add approx 50 to those values and check to see if we are close
 if (adc_key_in > 1000) return btnNONE; // We make this the 1st option for speed reasons since it will be the most likely result
 // For V1.1 us this threshold
 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 220)  return btnUP;
 if (adc_key_in < 400)  return btnDOWN;
 if (adc_key_in < 620)  return btnLEFT;
 if (adc_key_in < 950)  return btnSELECT;

 // For V1.0 comment the other threshold and use the one below:
/*
 if (adc_key_in < 50)   return btnRIGHT;
 if (adc_key_in < 195)  return btnUP;
 if (adc_key_in < 380)  return btnDOWN;
 if (adc_key_in < 555)  return btnLEFT;
 if (adc_key_in < 790)  return btnSELECT;
*/


 return btnNONE;  // when all others fail, return this...
}

void setup()
{
  Bubble_Diff = element*3; 
  DIVs= ((element/255)+1);
  
  TCCR2B = TCCR2B & B11111000 | B00000111; // for PWM frequency of 30.64 Hz 3 11
  analogWrite(PWM, 0);
  // analogWriteFreq(220);
  Serial.begin(115200);
 lcd.begin(16, 2);              // start the library
 lcd.setCursor(0,0);
 lcd.print("Adjust SSR"); // print a simple message
 Max_element_size = DIVs*255;

 element = EEPROM.read(address1);
 element = element*DIVs;
 pulse = EEPROM.read(address);
 
 

  if (CT1) ct1.current(1, 60.606);                                     // Setup emonTX CT channel (ADC input, calibration)
  if (CT2) ct2.current(2, 60.606);                                     // Calibration factor = CT ratio / burden resistance
  if (CT3) ct3.current(3, 60.606);                                     // emonTx Shield Calibration factor = (100A / 0.05A) / 33 Ohms
  if (CT4) ct4.current(4, 60.606); 

  if (CT1) ct1.voltage(0, 146.54, 1.7);                                // ct.voltageTX(ADC input, calibration, phase_shift) - make sure to select correct calibration for AC-AC adapter  http://openenergymonitor.org/emon/modules/emontx/firmware/calibration. Default set for Ideal Power adapter                                         
  if (CT2) ct2.voltage(0, 146.54, 1.7);                                // 268.97 for the UK adapter, 260 for the Euro and 130 for the US.
  if (CT3) ct3.voltage(0, 146.54, 1.7);
  if (CT4) ct4.voltage(0, 146.54, 1.7);

 
}

void loop()
{

  if (CT1) {
    ct1.calcVI(20,2000);                                                  // Calculate all. No.of crossings, time-out 
    emontx.power1 = ct1.realPower;
 //  Serial.print(emontx.power1);  
    Serial.print("TaskValueSet,1,1,"); Serial.println(emontx.power1*Sp); 
    LINE = emontx.power1;
    count = count + 1;
    //Serial.println(count);
     KW = (KW +  emontx.power1) ;                                   
  }

 // emontx.Vrms = ct1.Vrms*100;                                            // AC Mains rms voltage 

  if (CT2) {
    ct2.calcVI(20,2000);                                                  // Calculate all. No.of crossings, time-out 
    emontx.power2 = ct2.realPower;
   // Serial.print(" "); Serial.print(emontx.power2);
     Serial.print("TaskValueSet,1,2,"); Serial.println(emontx.power2);
  } 

  if (CT3) {
    ct3.calcVI(20,2000);                                                  // Calculate all. No.of crossings, time-out 
    emontx.power3 = ct3.realPower;
  //  Serial.print(" "); Serial.print(emontx.power3);
     Serial.print("TaskValueSet,1,3,"); Serial.println(emontx.power3);
  } 

   if (CT4) {
     ct4.calcVI(20,2000);                                                  // Calculate all. No.of crossings, time-out 
    emontx.power4 = ct4.realPower;
  //  Serial.print(" "); Serial.print(emontx.power4);
     Serial.print("TaskValueSet,1,4,"); Serial.println(emontx.power4);
  } 

 // Serial.print(" "); Serial.print(ct1.Vrms);
   Serial.print("TaskValueSet,2,1,"); Serial.println(ct1.Vrms);

//################ LCD ###################
   
 if (TYPE == 1) { 
 lcd.setCursor(11,0);            // move cursor to second line "1" and 9 spaces over
 //lcd.print(millis()/1000);      // display seconds elapsed since power-up
 int Display = (pulse/2.55);
 lcd.print(Display);lcd.print("%  ");
 }
 if ( TYPE == 0 ){
   lcd.setCursor(11,0);            
 //lcd.print(millis()/1000);      /
 
 int Display = (curgrid4/2.55);
 lcd.print(Display);lcd.print("%  ");
 }

 
 lcd.setCursor(0,1);            // move to the begining of the second line
 lcd_key = read_LCD_buttons();  // read the buttons

 //######################## Start if bubble Search ###########################

grid = element - LINE;
//Serial.println(grid);
if (CT1){

if (grid !=0) { 
  //curgrid = 0;
  stepc4 = (grid / Bubble_Diff); 
  prestep4 = (stepb4);

stepb4 = (prestep4 + stepc4);
  if (stepb4 > 1) {
    stepb4 =1;
  }
  if (stepb4 < 0) {
    stepb4 = 0;
  }
  curelem4 = (0  + stepb4);
  curgrid4 = ( 255 * curelem4  );
  //curgrid4 =(255-curgrid4);  //inverts the value of curgrid if need be 
}

}

 switch (lcd_key)               // depending on which button was pushed, we perform an action
 {
   case btnRIGHT:
     {
      TYPE = 0;
     
     element = element + DIVs;
     lcd.print(element*Sp);lcd.print(" w   ");
     break;
     }
   case btnLEFT:
     {
      TYPE = 0;
     
     element = element - DIVs;
     lcd.print(element*Sp);lcd.print(" w   ");
     break;
     }
   case btnUP:
     {
      TYPE = 1;
     lcd.print("UP    ");
     pulse = pulse + 1;
     //delay(100);
     
     break;
     }
   case btnDOWN:
     {
      TYPE = 1;
     lcd.print("DOWN  ");
     pulse = pulse - 1;
     //delay(100);
     break;
     }
   case btnSELECT:
     {   

       int EEP = element/DIVs; 
       //Serial.print("######################"); Serial.println(EEP);
      EEPROM.update(address1, EEP );
      address1 = address1 + 1;
        if (address1 == EEPROM.length()) {
           address1 = 1;
        }

       EEPROM.update(address, pulse);
      address = address + 1;
        if (address == EEPROM.length()) {
           address = 0;
        }           
   
           
       lcd.print("SAVED  ");
       delay(1000);
       TYPE=1;
     break;
     }
     case btnNONE:
     {
      lcd.print(element*Sp);lcd.print(" w->");
     //
     break;
     }
 }
   int DIV = DIVs -1;
  if ( element < DIV) { 
  element =0;
 }
 if (element > Max_element_size ) {
  element  = Max_element_size;
  }
 
 if ( pulse < 1) { 
  pulse =0;
 }
 if (pulse > 255 ) {
  pulse = 255;
  }
  /*
if ( TYPE ==0 ){  
if (!time_out && millis() > timeout) time_out = true;
 if (time_out) {
  TYPE=1;
 }
} */
 if (!settled && millis() > FILTERSETTLETIME) settled = true;

  if (settled)                                                            
  { 
    /*
  boolean st=false;                                  //an indicator to exit the while loop
  unsigned long start = millis();                    //millis()-start makes sure it doesnt get stuck in the loop if there is an error.
  while(st==false)                                   //the while loop...
  {
    sV = analogRead(5);                              //using the voltage waveform
    if ((sV < (1024*0.55)) && (sV > (1024*0.45))) st=true;  //check its within range
    if ((millis()-start)>2000) st = true;
  }
*/
    if ( TYPE ==1){                                                    
       analogWrite(PWM, pulse); 
       
      }
    
    if ( TYPE ==0){ 
      analogWrite(PWM, curgrid4); 
        lcd.setCursor(11,0);            
         int Display = (curgrid4/2.55);
         lcd.print(Display);lcd.print("%  ");  
         pulse = curgrid4; 
      }   
      
      lcd.setCursor(9,1);
      if ( count >= 4){
      lcd.print((KW/count)*Sp);lcd.print(" W    ");
      count=0;
      KW=0;
      }                                             
  }

//Serial.println(adc_key_in);
}
