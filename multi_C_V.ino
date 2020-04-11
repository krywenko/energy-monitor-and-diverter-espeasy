#define FILTERSETTLETIME 5000  
                                         //  Time (ms) to allow the filters to settle before sending data
//#### ADJUSTABLE #####

const int CT1 = 1; 
const int CT2 = 0;                                                      // Set to 0 to disable 
const int CT3 = 0;
const int CT4 = 0;

int ios = 1;              /// # of SSR
int SSR1 = 3;             /// pin asociated with SSR
int SSR2 = 4;
int SSR3 = 5;
int SSR4 = 6;
float element = 1000;     // element size

//###non adjusable

float grid;
int VALUE;
int C_SSR = 0;
float stepa = 0;   // 
float stepb = 1;
float stepc = 1;
float prestep = 1;
int stepbu;
float curinvt = 1; //percentage of power usage comparison over or below grid usage 
float curelem =1;
int curgrid = 0;       // current  PMW step
float stepa1 = 0;   
float stepb1 = 1;
float stepc1 = 1;
float prestep1 =0; 
int stepbu1;
float curelem1 =1;
int curgrid1 = 0;
int stat ;
int full=0;
//######### END #########

#include "EmonLibD.h"
EnergyMonitor ct1,ct2,ct3, ct4;                                              // Create  instances for each CT channel

typedef struct { int power1, power2, power3, power4, Vrms;} PayloadTX;     
PayloadTX emontx;                                                       


boolean settled = false;

void setup() 
{
  Serial.begin(115200);
    
   
  pinMode(SSR1, OUTPUT); 
  pinMode(SSR2, OUTPUT); 
  pinMode(SSR3, OUTPUT); 
  pinMode(SSR4, OUTPUT); 


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
    ct1.calcVI(20,2000,ios,stat,SSR1,SSR2,SSR3,SSR4,VALUE);                                   // # of crossings, time-out, # SSR, Current SSR, pin1 ,pin2, pin3, pin4, Value 
    emontx.power1 = ct1.realPower;
    grid=emontx.power1;
 //  Serial.print(emontx.power1);  
    Serial.print("TaskValueSet,1,1,"); Serial.println(emontx.power1);    //FOR DOMOTICZ                                    
  Serial.println(ios);
  Serial.println(stat);
Serial.println(VALUE);

Serial.print(SSR1);Serial.print(SSR2); Serial.print(SSR3);Serial.println(SSR4);



  
  }

  emontx.Vrms = ct1.Vrms*100;                                            // AC Mains rms voltage 

  if (CT2) {
    ct2.calcVI(20,2000,ios,stat,SSR1,SSR2,SSR3,SSR4,VALUE);                                     // # of crossings, time-out, # SSR, Current SSR, pin1 ,pin2, pin3, pin4, Value 
    emontx.power2 = ct2.realPower;
   // Serial.print(" "); Serial.print(emontx.power2);
     Serial.print("TaskValueSet,1,2,"); Serial.println(emontx.power2);    //FOR DOMOTICZ 
  } 

  if (CT3) {
    ct3.calcVI(20,2000,ios,stat,SSR1,SSR2,SSR3,SSR4,VALUE);                                     // # of crossings, time-out, # SSR, Current SSR, pin1 ,pin2, pin3, pin4, Value 
    emontx.power3 = ct3.realPower;
  //  Serial.print(" "); Serial.print(emontx.power3);
     Serial.print("TaskValueSet,1,3,"); Serial.println(emontx.power3);     //FOR DOMOTICZ 
  } 

   if (CT4) {
     ct4.calcVI(20,2000,ios,stat,SSR1,SSR2,SSR3,SSR4,VALUE);                                    // # of crossings, time-out, # SSR, Current SSR, pin1 ,pin2, pin3, pin4, Value 
    emontx.power4 = ct4.realPower;
  //  Serial.print(" "); Serial.print(emontx.power4);
     Serial.print("TaskValueSet,1,4,"); Serial.println(emontx.power4);      //FOR DOMOTICZ 
  } 

 // Serial.print(" "); Serial.print(ct1.Vrms);
   Serial.print("TaskValueSet,2,1,"); Serial.println(ct1.Vrms);


//######################## Start if bubble Search ###########################

//######################Cascading pulse ##################################
if (CT1){
if (grid !=0) {
 
  stepc = (grid / element); 
  prestep = (stepb);

stepb = (prestep + stepc);
  if (stepb > 0) {
    stepb =0;
  }
  if (stepb < (0-ios)) {
    stepb = (0-ios);
  }
  curelem = (0  + stepb);
stepbu=curelem;
curelem = (curelem - stepbu);
  curgrid = ( 254 * curelem  );
  curgrid =(0-curgrid);  //inverts the value of curgrid if need be 
  //VALUE=(curgrid *4);
  //int data1 = map(curgrid, 0, 255,10,40);
  //VALUE=data1;
  Serial.println(curgrid);
}

//########################## static pulse ###############################
if (grid !=0) { 

  stepc1 = (grid / element); 
  prestep1 = (stepb1);

 stepb1 = (prestep1 + stepc1);
  if (stepb1 > 1) {
    stepb1 =1;
  }
  if (stepb1 < 0) {
    stepb1 = 0;
  }
   curelem1 = (0  + stepb1);
  curgrid1 = ( 255 * curelem1  );
  //curgrid1 =(255-curgrid1);  //inverts the value of curgrid if need be 
  Serial.println(curgrid1);
  VALUE=(curgrid1*4);
}

}

int statc ;
int ivar;
int statb ;

stat = (0-stepbu);
if (curgrid==256){curgrid=0;}

if (stat > (ios-1)) {stat=(ios-1);curgrid=255;full=1;}
if (stat ==0) {ivar = 1;}
else  {ivar = 0;}

stat=stat+1;
//################### end of bubble search ######################








  // because millis() returns to zero after 50 days ! 
  if (!settled && millis() > FILTERSETTLETIME) settled = true;

  if (settled)                                                            // send data only after filters have settled
  { 
                                                       
  }
}
