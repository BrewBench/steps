// Greg Kallfa http://www.visionquestbrewing.com

#define SECOND 1000L

#define BV1Pin 10      //Keg In Hot Water (from water heater)
#define BV2Pin 13      //Solenoid Keg In CO2
#define BV3Pin 4      //Keg Out to SAN
#define BV4Pin 3      //Keg Out to PBW
#define BV5Pin 5       //PBW Tank Drain
#define BV6Pin 6       //SAN Tank Drain
#define BV7Pin 7       //Drain
#define BV8Pin 8       //Pre Keg Inline Valve
#define BV9Pin 2       //Fill SAN
#define BV10Pin 14      //Fill PBW
#define BV11Pin 9      //open air valve (to drain)
#define PumpPin 11
#define ElementPin 12

unsigned long previousMillis = 0;   //will store the last time
unsigned long currentMillis;       //stores the current time
const int PBWTempRef = 140;      //this is equal to approx temp in Celcius /2 (or exactly C=sensorValue * .48828125)
//unsigned long previousButtonMillis = 0;   //stores the cancel button's delay start time. previousButtonMillis = millis(); (delete after program is functional)

long lastDebouncePBWTime = 0;    //debounce for PBW tank level (avoiding dry fire)
long lastDebounceSANTime = 0;    //debounce for SAN tank level
long debouncePBWDelay = 1000;     //used for both PBW and SAN debouncing
int volumeState;
int LevelPBWPin;
int LevelSANPin;
int sensorValue;
int PBWState = 0;


long lastDebounceTime = 0;     //debouncing main start button
int lastButtonState = HIGH;   //debouncing main start button
long debounceDelay = 25;
int buttonState = 1;
long cancelDelay = 3000;

int currentState = 0;
int currentProgram = 0;


void setup() {

  Serial.begin(9600);
 pinMode(A0, INPUT_PULLUP);      //Purple Switch 1 - Regular Sanke Keg Cycle
 pinMode(A1, INPUT_PULLUP);      //Purple Switch 2 - Clean PBW Tank
 pinMode(A2, INPUT_PULLUP);      //Purple Switch 3 - Clean SANI Tank
 pinMode(A3, INPUT_PULLUP);      //Purple Switch 4 - Clean BOTH Tanks
 pinMode(A4, INPUT_PULLUP);      //PBW level
 pinMode(A5, INPUT_PULLUP);      //SAN level
 pinMode(A6, INPUT_PULLUP);      //CO2 level
 pinMode(A7, INPUT);             //LM35 temperature sensor
 pinMode(A8, OUTPUT);            //+5V for LM35
 pinMode(A9, OUTPUT);            //GND for LM35
 pinMode(A10, OUTPUT);           //GND for finished cycle LED
 pinMode(A11, OUTPUT);           //+5V for LED
 pinMode(A12, OUTPUT);           //GND for momentary switch
 pinMode(A13, INPUT_PULLUP);     //momentary switch
 pinMode(A14, INPUT_PULLUP);     //Purple Switch 5 - New Cycle
 pinMode(A15, INPUT_PULLUP);     //Purple Switch 6 - New Cycle

 digitalWrite(A10, LOW);        //GND for LED
 digitalWrite(A11, HIGH);      //5V for LED (ON)

 digitalWrite(A8, HIGH);       //5V for temp sensor
 digitalWrite(A9, LOW);        //GND for temp sensor

 pinMode(14, OUTPUT);
 pinMode(2, OUTPUT);
 pinMode(3, OUTPUT);
 pinMode(4, OUTPUT);
 pinMode(5, OUTPUT);
 pinMode(6, OUTPUT);
 pinMode(7, OUTPUT);
 pinMode(8, OUTPUT);
 pinMode(9, OUTPUT);
 pinMode(10, OUTPUT);
 pinMode(11, OUTPUT);
 pinMode(12, OUTPUT);
 pinMode(13, OUTPUT);

 digitalWrite(BV1Pin, LOW);
 digitalWrite(BV2Pin, LOW);
 digitalWrite(BV3Pin, LOW);
 digitalWrite(BV4Pin, LOW);
 digitalWrite(BV5Pin, LOW);
 digitalWrite(BV6Pin, LOW);
 digitalWrite(BV7Pin, LOW);
 digitalWrite(BV8Pin, LOW);
 digitalWrite(BV9Pin, LOW);
 digitalWrite(BV10Pin, LOW);
 digitalWrite(BV11Pin, LOW);
 digitalWrite(PumpPin, LOW);
 digitalWrite(ElementPin, LOW);
}

 void CleanPBW ()  {
   switch (currentState) {

 case 0:
   digitalWrite(A10, LOW);          //set A10 as GND for LED
   digitalWrite(A11, LOW);         //set A11 as 5V for LED
   digitalWrite(BV5Pin, HIGH);       //PBW drain open
   digitalWrite(BV11Pin, HIGH);       //main drain open
   if (currentMillis - previousMillis >= 340*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 1:
   digitalWrite(BV10Pin, HIGH);     //open PBW fill valve
   if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis();  }
   break;
 case 2:
   digitalWrite(BV10Pin, LOW);       //close PBW fill valve
   if (currentMillis - previousMillis >= 70*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 3:
   digitalWrite(BV10Pin, HIGH);      //open PBW fill valve
   if (currentMillis - previousMillis >= 6*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 4:
   digitalWrite(BV10Pin, LOW);
   if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 5:
   digitalWrite(BV10Pin, HIGH);
   if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 6:
   digitalWrite(BV10Pin, LOW);
   if (currentMillis - previousMillis >= 30*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 7:
   digitalWrite(BV10Pin, HIGH);
   if (currentMillis - previousMillis >= 3*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 8:
   digitalWrite(BV10Pin, LOW);
   if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 9:
   digitalWrite(BV5Pin, LOW);       //PBW drain close
   digitalWrite(BV11Pin, LOW);       //Main drain close
   digitalWrite(A10, LOW);          //set A10 as GND for LED
   digitalWrite(A11, HIGH);         //set A11 as 5V for LED
  currentProgram = 0;
  currentState = 0;
  break;
   }
 }

 void CleanSAN () {
   switch (currentState) {
   case 0:
   digitalWrite(A10, LOW);          //set A10 as GND for LED
   digitalWrite(A11, LOW);         //set A11 as 5V for LED
   digitalWrite(BV6Pin, HIGH);     //open SAN drain
   digitalWrite(BV11Pin, HIGH);     //open main drain
   if (currentMillis - previousMillis >= 340*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 1:
   digitalWrite(BV9Pin, HIGH);     //open SAN fill
   if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 2:
   digitalWrite(BV9Pin, LOW);     //close SAN fill
   if (currentMillis - previousMillis >= 70*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 3:
   digitalWrite(BV9Pin, HIGH);
   if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 4:
   digitalWrite(BV9Pin, LOW);
   if (currentMillis - previousMillis >= 60*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 5:
   digitalWrite(BV9Pin, HIGH);
   if (currentMillis - previousMillis >= 5*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 6:
   digitalWrite(BV9Pin, LOW);
   if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 7:
   digitalWrite(BV6Pin, LOW);     //close SAN drain
   digitalWrite(BV11Pin, LOW);     //close main drain
   digitalWrite(A10, LOW);          //set A10 as GND for LED
   digitalWrite(A11, HIGH);         //set A11 as 5V for LED
   currentState = 0;
   currentProgram = 0;
   break;
   }
 }

 void CleanBOTH ()  {
   switch (currentState) {

 case 0:
   digitalWrite(A10, LOW);          //set A10 as GND for LED
   digitalWrite(A11, LOW);         //set A11 as 5V for LED
   digitalWrite(BV5Pin, HIGH);       //PBW drain open
   digitalWrite(BV11Pin, HIGH);       //main drain open
   if (currentMillis - previousMillis >= 340*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 1:
   digitalWrite(BV10Pin, HIGH);     //open PBW fill valve
   if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis();  }
   break;
 case 2:
   digitalWrite(BV10Pin, LOW);       //close PBW fill valve
   if (currentMillis - previousMillis >= 70*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 3:
   digitalWrite(BV10Pin, HIGH);      //open PBW fill valve
   if (currentMillis - previousMillis >= 6*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 4:
   digitalWrite(BV10Pin, LOW);
   if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 5:
   digitalWrite(BV10Pin, HIGH);
   if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 6:
   digitalWrite(BV10Pin, LOW);
   if (currentMillis - previousMillis >= 30*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 7:
   digitalWrite(BV10Pin, HIGH);
   if (currentMillis - previousMillis >= 3*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 8:
   digitalWrite(BV10Pin, LOW);
   if (currentMillis - previousMillis >= 25*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 9:
   digitalWrite(BV5Pin, LOW);       //PBW drain close
   digitalWrite(BV6Pin, HIGH);     //open SAN drain
   if (currentMillis - previousMillis >= 340*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 10:
   digitalWrite(BV9Pin, HIGH);     //open SAN fill
   if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 11:
   digitalWrite(BV9Pin, LOW);     //close SAN fill
   if (currentMillis - previousMillis >= 70*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 12:
   digitalWrite(BV9Pin, HIGH);
   if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 13:
   digitalWrite(BV9Pin, LOW);
   if (currentMillis - previousMillis >= 60*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 14:
   digitalWrite(BV9Pin, HIGH);
   if (currentMillis - previousMillis >= 5*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 15:
   digitalWrite(BV9Pin, LOW);
   if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 16:
   digitalWrite(BV6Pin, LOW);     //close SAN drain
   digitalWrite(BV11Pin, LOW);     //close main drain
   digitalWrite(A10, LOW);          //set A10 as GND for LED
   digitalWrite(A11, HIGH);         //set A11 as 5V for LED
   currentState = 0;
   currentProgram = 0;
   break;
   }
 }

 void CleanSanke15 () {

switch (currentState)  {
 case 0:
     {
     digitalWrite(A10, LOW);              //GND for LED
     digitalWrite(A11, LOW);              //5V for LED
   LevelPBWPin = digitalRead(A4);    //check the PBW level sensor
      if (LevelPBWPin == LOW) {digitalWrite(BV10Pin, HIGH); lastDebouncePBWTime = millis(); }  //reset the debouncing timer and keep PBW fill valve open
      if (LevelPBWPin == HIGH) {
         if ((millis() - lastDebouncePBWTime) > debouncePBWDelay) {digitalWrite(BV10Pin, LOW); lastDebouncePBWTime = 0; currentState++; break; } } }
      break;
 case 1:
      {
        sensorValue = analogRead(A7);                    //take a sensor reading
           //float voltage = sensorValue *  (5000.0/ 1024.0);   //convert sensor reading to mV
           //float temperature = voltage/10.0;                  //convert the mV to celsius
      if(sensorValue < PBWTempRef) {digitalWrite(ElementPin, HIGH); currentState++; }
      }
      break;
 case 2:
    digitalWrite(BV7Pin, HIGH);     //Post Keg drain valve open
    if (currentMillis - previousMillis >= 50*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 3:
    digitalWrite(BV11Pin, HIGH);   digitalWrite(BV8Pin, HIGH); //Pre Keg drain valve open + Pre Keg Inline Valve Open
     if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 4:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 5:
    digitalWrite(BV1Pin, HIGH);     //hot water rinse on
     if (currentMillis - previousMillis >= 12*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 6:                                                               //-----------------------------1st rinse
    digitalWrite(BV1Pin, LOW);      //hot water rinse off
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 7:
    digitalWrite(BV2Pin, HIGH);      //CO2 on
     if (currentMillis - previousMillis >= 15*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 8:
    digitalWrite(BV2Pin, LOW);       //CO2 off
     if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 9:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
     if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 10:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 11:
    digitalWrite(BV1Pin, HIGH);     //hot water rinse on
     if (currentMillis - previousMillis >= 8*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 12:                                                           //-------------------------------2nd rinse
    digitalWrite(BV1Pin, LOW);      //hot water rinse off
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 13:
    digitalWrite(BV2Pin, HIGH);      //CO2 on
     if (currentMillis - previousMillis >= 12*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 14:
    digitalWrite(BV2Pin, LOW);       //CO2 off
     if (currentMillis - previousMillis >= 30*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 15:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
     if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 16:

    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 17:
    digitalWrite(BV1Pin, HIGH);     //hot water rinse on
     if (currentMillis - previousMillis >= 8*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 18:               //-------------------------------3rd rinse
    digitalWrite(BV1Pin, LOW);       //hot water rinse off
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 19:
    digitalWrite(BV2Pin, HIGH);       //CO2 on
     if (currentMillis - previousMillis >= 12*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 20:
    digitalWrite(BV2Pin, LOW);       //CO2 off
     if (currentMillis - previousMillis >= 30*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 21:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
     if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 22:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
    digitalWrite(BV7Pin, LOW);      //Post Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 23:
    {
    sensorValue = analogRead(A7);
      if (sensorValue < PBWTempRef) {digitalWrite(ElementPin, HIGH);  }
      if(sensorValue > PBWTempRef) {currentState++; digitalWrite(ElementPin, LOW); }
    }
   break;
 case 24:
    digitalWrite(BV5Pin, HIGH);      //open PBW drain
    digitalWrite(BV4Pin, HIGH);      //open PBW return valve
    if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 25:
    digitalWrite(PumpPin, HIGH);    //turn on pump
    if (currentMillis - previousMillis >= 60*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 26:
    digitalWrite(PumpPin, LOW);     //turn off pump
    digitalWrite(BV5Pin, LOW);      //close PBW drain
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 27:
    digitalWrite(BV2Pin, HIGH);     //CO2 on
    if (currentMillis - previousMillis >= 23*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 28:
    digitalWrite(BV2Pin, LOW);      //CO2 off
    if (currentMillis - previousMillis >= 110*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 29:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 30:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
    digitalWrite(BV4Pin, LOW);      //Keg return to PBW close
    digitalWrite(BV7Pin, HIGH);     //Post Keg drain valve open
    if (currentMillis - previousMillis >= 3*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 31:
    digitalWrite(BV1Pin, HIGH);     //-----------------------------------------1st hot water rinse after PBW
    if (currentMillis - previousMillis >= 8*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 32:
    digitalWrite(BV1Pin, LOW);      //hot water off
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 33:
    digitalWrite(BV2Pin, HIGH);     //CO2 on
    if (currentMillis - previousMillis >= 14*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 34:
    digitalWrite(BV2Pin, LOW);      //CO2 off
    if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 35:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 36:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
    if (currentMillis - previousMillis >= 3*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 37:
    digitalWrite(BV1Pin, HIGH);     //---------------------------------------2nd hot water rinse after PBW
    if (currentMillis - previousMillis >= 7*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 38:
    digitalWrite(BV1Pin, LOW);      //hot water off
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 39:
    digitalWrite(BV2Pin, HIGH);     //CO2 on
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 40:
    digitalWrite(BV2Pin, LOW);      //CO2 off
    if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 41:
    digitalWrite(BV11Pin, HIGH);   //Pre Keg drain valve open
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 42:
    {
    digitalWrite(BV11Pin, LOW);    //Pre Keg drain valve close
    digitalWrite(BV7Pin, LOW);     //Post Keg drain close
    //----------------------------------------------------------------------------------------------------------SAN
    LevelSANPin = digitalRead(A5);
       if (LevelSANPin == LOW) {digitalWrite(BV9Pin, HIGH); lastDebounceSANTime = millis(); }  //reset the debouncing timer and keep PBW fill valve open
       if (LevelSANPin == HIGH) {   // currentMillis = millis();        //take current time
       if ((millis() - lastDebounceSANTime) > debouncePBWDelay) {digitalWrite(BV9Pin, LOW); lastDebounceSANTime = 0; currentState++; break; } } }
      break;
   case 43:
    digitalWrite(BV9Pin, LOW);        //close SAN fill valve
    digitalWrite(BV6Pin, HIGH);       //open SAN tank drain
    digitalWrite(BV3Pin, HIGH);       //open keg out to SAN
    if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 44:                 //20 seconds for BV opening and pump priming
    digitalWrite(PumpPin, HIGH);      //turn on pump
    if (currentMillis - previousMillis >= 15*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 45:                 //to fill keg with sani, adjust if this is found to be faster
    digitalWrite(PumpPin, LOW);       //turn off pump
    digitalWrite(BV6Pin, LOW);        //close SAN drain
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 46:
    digitalWrite(BV2Pin, HIGH);       //open CO2
     if (currentMillis - previousMillis >= 22*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 47:                //let CO2 push out SAN
     digitalWrite(BV2Pin, LOW);       //close CO2
     if (currentMillis - previousMillis >= 90*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 48:                //let excess CO2 push out SAN
     digitalWrite(BV3Pin, LOW);       //close keg out to SAN
     digitalWrite(BV8Pin, LOW);       //close Pre Keg Inline Valve
     digitalWrite(A10, LOW);          //set A10 as GND for LED
     digitalWrite(A11, HIGH);         //set A11 as 5V for LED
     currentState = 0;
     currentProgram = 0;
     break;
  } }

void CleanHot() {        //have not changed this from reg cycle yet

switch (currentState)  {
 case 0:
     {
     digitalWrite(A10, LOW);              //GND for LED
     digitalWrite(A11, LOW);              //5V for LED
   LevelPBWPin = digitalRead(A4);    //check the PBW level sensor
      if (LevelPBWPin == LOW) {digitalWrite(BV10Pin, HIGH); lastDebouncePBWTime = millis(); }  //reset the debouncing timer and keep PBW fill valve open
      if (LevelPBWPin == HIGH) {
         if ((millis() - lastDebouncePBWTime) > debouncePBWDelay) {digitalWrite(BV10Pin, LOW); lastDebouncePBWTime = 0; currentState++; break; } } }
      break;
 case 1:
      {
        sensorValue = analogRead(A7);                    //take a sensor reading
           //float voltage = sensorValue *  (5000.0/ 1024.0);   //convert sensor reading to mV
           //float temperature = voltage/10.0;                  //convert the mV to celsius
      if(sensorValue < PBWTempRef) {digitalWrite(ElementPin, HIGH); currentState++; }
      }
      break;
 case 2:
    digitalWrite(BV7Pin, HIGH);     //Post Keg drain valve open
    if (currentMillis - previousMillis >= 50*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 3:
    digitalWrite(BV11Pin, HIGH);   digitalWrite(BV8Pin, HIGH); //Pre Keg drain valve open + Pre Keg Inline Valve Open
     if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 4:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 5:
    digitalWrite(BV1Pin, HIGH);     //hot water rinse on
     if (currentMillis - previousMillis >= 12*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 6:                                                               //-----------------------------1st rinse
    digitalWrite(BV1Pin, LOW);      //hot water rinse off
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 7:
    digitalWrite(BV2Pin, HIGH);      //CO2 on
     if (currentMillis - previousMillis >= 15*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 8:
    digitalWrite(BV2Pin, LOW);       //CO2 off
     if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 9:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
     if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 10:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 11:
    digitalWrite(BV1Pin, HIGH);     //hot water rinse on
     if (currentMillis - previousMillis >= 8*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 12:                                                           //-------------------------------2nd rinse
    digitalWrite(BV1Pin, LOW);      //hot water rinse off
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 13:
    digitalWrite(BV2Pin, HIGH);      //CO2 on
     if (currentMillis - previousMillis >= 12*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 14:
    digitalWrite(BV2Pin, LOW);       //CO2 off
     if (currentMillis - previousMillis >= 30*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 15:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
     if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 16:

    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 17:
    digitalWrite(BV1Pin, HIGH);     //hot water rinse on
     if (currentMillis - previousMillis >= 8*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 18:               //-------------------------------3rd rinse
    digitalWrite(BV1Pin, LOW);       //hot water rinse off
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 19:
    digitalWrite(BV2Pin, HIGH);       //CO2 on
     if (currentMillis - previousMillis >= 12*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 20:
    digitalWrite(BV2Pin, LOW);       //CO2 off
     if (currentMillis - previousMillis >= 30*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 21:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
     if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 22:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
    digitalWrite(BV7Pin, LOW);      //Post Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 23:
    {
    sensorValue = analogRead(A7);
      if (sensorValue < PBWTempRef) {digitalWrite(ElementPin, HIGH);  }
      if(sensorValue > PBWTempRef) {currentState++; digitalWrite(ElementPin, LOW); }
    }
   break;
 case 24:
    digitalWrite(BV5Pin, HIGH);      //open PBW drain
    digitalWrite(BV4Pin, HIGH);      //open PBW return valve
    if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 25:
    digitalWrite(PumpPin, HIGH);    //turn on pump
    if (currentMillis - previousMillis >= 60*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 26:
    digitalWrite(PumpPin, LOW);     //turn off pump
    digitalWrite(BV5Pin, LOW);      //close PBW drain
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 27:
    digitalWrite(BV2Pin, HIGH);     //CO2 on
    if (currentMillis - previousMillis >= 23*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 28:
    digitalWrite(BV2Pin, LOW);      //CO2 off
    if (currentMillis - previousMillis >= 110*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 29:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 30:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
    digitalWrite(BV4Pin, LOW);      //Keg return to PBW close
    digitalWrite(BV7Pin, HIGH);     //Post Keg drain valve open
    if (currentMillis - previousMillis >= 3*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 31:
    digitalWrite(BV1Pin, HIGH);     //-----------------------------------------1st hot water rinse after PBW
    if (currentMillis - previousMillis >= 8*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 32:
    digitalWrite(BV1Pin, LOW);      //hot water off
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 33:
    digitalWrite(BV2Pin, HIGH);     //CO2 on
    if (currentMillis - previousMillis >= 14*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 34:
    digitalWrite(BV2Pin, LOW);      //CO2 off
    if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 35:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 36:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
    if (currentMillis - previousMillis >= 3*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 37:
    digitalWrite(BV1Pin, HIGH);     //---------------------------------------2nd hot water rinse after PBW
    if (currentMillis - previousMillis >= 7*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 38:
    digitalWrite(BV1Pin, LOW);      //hot water off
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 39:
    digitalWrite(BV2Pin, HIGH);     //CO2 on
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 40:
    digitalWrite(BV2Pin, LOW);      //CO2 off
    if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 41:
    digitalWrite(BV11Pin, HIGH);   //Pre Keg drain valve open
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 42:
    {
    digitalWrite(BV11Pin, LOW);    //Pre Keg drain valve close
    digitalWrite(BV7Pin, LOW);     //Post Keg drain close
    //----------------------------------------------------------------------------------------------------------SAN
    LevelSANPin = digitalRead(A5);
       if (LevelSANPin == LOW) {digitalWrite(BV9Pin, HIGH); lastDebounceSANTime = millis(); }  //reset the debouncing timer and keep PBW fill valve open
       if (LevelSANPin == HIGH) {   // currentMillis = millis();        //take current time
       if ((millis() - lastDebounceSANTime) > debouncePBWDelay) {digitalWrite(BV9Pin, LOW); lastDebounceSANTime = 0; currentState++; break; } } }
      break;
   case 43:
    digitalWrite(BV9Pin, LOW);        //close SAN fill valve
    digitalWrite(BV6Pin, HIGH);       //open SAN tank drain
    digitalWrite(BV3Pin, HIGH);       //open keg out to SAN
    if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 44:                 //20 seconds for BV opening and pump priming
    digitalWrite(PumpPin, HIGH);      //turn on pump
    if (currentMillis - previousMillis >= 15*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 45:                 //to fill keg with sani, adjust if this is found to be faster
    digitalWrite(PumpPin, LOW);       //turn off pump
    digitalWrite(BV6Pin, LOW);        //close SAN drain
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 46:
    digitalWrite(BV2Pin, HIGH);       //open CO2
     if (currentMillis - previousMillis >= 22*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 47:                //let CO2 push out SAN
     digitalWrite(BV2Pin, LOW);       //close CO2
     if (currentMillis - previousMillis >= 90*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 48:                //let excess CO2 push out SAN
     digitalWrite(BV3Pin, LOW);       //close keg out to SAN
     digitalWrite(BV8Pin, LOW);       //close Pre Keg Inline Valve
     digitalWrite(A10, LOW);          //set A10 as GND for LED
     digitalWrite(A11, HIGH);         //set A11 as 5V for LED
     currentState = 0;
     currentProgram = 0;
     break;
  } }

void CleanNasty() {      //have not changed this from regular cycle yet

switch (currentState)  {
 case 0:
     {
     digitalWrite(A10, LOW);              //GND for LED
     digitalWrite(A11, LOW);              //5V for LED
   LevelPBWPin = digitalRead(A4);    //check the PBW level sensor
      if (LevelPBWPin == LOW) {digitalWrite(BV10Pin, HIGH); lastDebouncePBWTime = millis(); }  //reset the debouncing timer and keep PBW fill valve open
      if (LevelPBWPin == HIGH) {
         if ((millis() - lastDebouncePBWTime) > debouncePBWDelay) {digitalWrite(BV10Pin, LOW); lastDebouncePBWTime = 0; currentState++; break; } } }
      break;
 case 1:
      {
        sensorValue = analogRead(A7);                    //take a sensor reading
           //float voltage = sensorValue *  (5000.0/ 1024.0);   //convert sensor reading to mV
           //float temperature = voltage/10.0;                  //convert the mV to celsius
      if(sensorValue < PBWTempRef) {digitalWrite(ElementPin, HIGH); currentState++; }
      }
      break;
 case 2:
    digitalWrite(BV7Pin, HIGH);     //Post Keg drain valve open
    if (currentMillis - previousMillis >= 50*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 3:
    digitalWrite(BV11Pin, HIGH);   digitalWrite(BV8Pin, HIGH); //Pre Keg drain valve open + Pre Keg Inline Valve Open
     if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 4:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 5:
    digitalWrite(BV1Pin, HIGH);     //hot water rinse on
     if (currentMillis - previousMillis >= 12*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 6:                                                               //-----------------------------1st rinse
    digitalWrite(BV1Pin, LOW);      //hot water rinse off
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 7:
    digitalWrite(BV2Pin, HIGH);      //CO2 on
     if (currentMillis - previousMillis >= 15*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 8:
    digitalWrite(BV2Pin, LOW);       //CO2 off
     if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 9:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
     if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 10:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 11:
    digitalWrite(BV1Pin, HIGH);     //hot water rinse on
     if (currentMillis - previousMillis >= 8*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 12:                                                           //-------------------------------2nd rinse
    digitalWrite(BV1Pin, LOW);      //hot water rinse off
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 13:
    digitalWrite(BV2Pin, HIGH);      //CO2 on
     if (currentMillis - previousMillis >= 12*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 14:
    digitalWrite(BV2Pin, LOW);       //CO2 off
     if (currentMillis - previousMillis >= 30*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 15:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
     if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 16:

    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 17:
    digitalWrite(BV1Pin, HIGH);     //hot water rinse on
     if (currentMillis - previousMillis >= 8*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 18:               //-------------------------------3rd rinse
    digitalWrite(BV1Pin, LOW);       //hot water rinse off
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 19:
    digitalWrite(BV2Pin, HIGH);       //CO2 on
     if (currentMillis - previousMillis >= 12*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 20:
    digitalWrite(BV2Pin, LOW);       //CO2 off
     if (currentMillis - previousMillis >= 30*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 21:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
     if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 22:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
    digitalWrite(BV7Pin, LOW);      //Post Keg drain valve close
     if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 23:
    {
    sensorValue = analogRead(A7);
      if (sensorValue < PBWTempRef) {digitalWrite(ElementPin, HIGH);  }
      if(sensorValue > PBWTempRef) {currentState++; digitalWrite(ElementPin, LOW); }
    }
   break;
 case 24:
    digitalWrite(BV5Pin, HIGH);      //open PBW drain
    digitalWrite(BV4Pin, HIGH);      //open PBW return valve
    if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 25:
    digitalWrite(PumpPin, HIGH);    //turn on pump
    if (currentMillis - previousMillis >= 60*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 26:
    digitalWrite(PumpPin, LOW);     //turn off pump
    digitalWrite(BV5Pin, LOW);      //close PBW drain
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 27:
    digitalWrite(BV2Pin, HIGH);     //CO2 on
    if (currentMillis - previousMillis >= 23*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 28:
    digitalWrite(BV2Pin, LOW);      //CO2 off
    if (currentMillis - previousMillis >= 110*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 29:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 30:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
    digitalWrite(BV4Pin, LOW);      //Keg return to PBW close
    digitalWrite(BV7Pin, HIGH);     //Post Keg drain valve open
    if (currentMillis - previousMillis >= 3*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 31:
    digitalWrite(BV1Pin, HIGH);     //-----------------------------------------1st hot water rinse after PBW
    if (currentMillis - previousMillis >= 8*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 32:
    digitalWrite(BV1Pin, LOW);      //hot water off
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 33:
    digitalWrite(BV2Pin, HIGH);     //CO2 on
    if (currentMillis - previousMillis >= 14*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 34:
    digitalWrite(BV2Pin, LOW);      //CO2 off
    if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 35:
    digitalWrite(BV11Pin, HIGH);    //Pre Keg drain valve open
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 36:
    digitalWrite(BV11Pin, LOW);     //Pre Keg drain valve close
    if (currentMillis - previousMillis >= 3*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 37:
    digitalWrite(BV1Pin, HIGH);     //---------------------------------------2nd hot water rinse after PBW
    if (currentMillis - previousMillis >= 7*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 38:
    digitalWrite(BV1Pin, LOW);      //hot water off
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 39:
    digitalWrite(BV2Pin, HIGH);     //CO2 on
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 40:
    digitalWrite(BV2Pin, LOW);      //CO2 off
    if (currentMillis - previousMillis >= 40*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 41:
    digitalWrite(BV11Pin, HIGH);   //Pre Keg drain valve open
    if (currentMillis - previousMillis >= 10*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 42:
    {
    digitalWrite(BV11Pin, LOW);    //Pre Keg drain valve close
    digitalWrite(BV7Pin, LOW);     //Post Keg drain close
    //----------------------------------------------------------------------------------------------------------SAN
    LevelSANPin = digitalRead(A5);
       if (LevelSANPin == LOW) {digitalWrite(BV9Pin, HIGH); lastDebounceSANTime = millis(); }  //reset the debouncing timer and keep PBW fill valve open
       if (LevelSANPin == HIGH) {   // currentMillis = millis();        //take current time
       if ((millis() - lastDebounceSANTime) > debouncePBWDelay) {digitalWrite(BV9Pin, LOW); lastDebounceSANTime = 0; currentState++; break; } } }
      break;
   case 43:
    digitalWrite(BV9Pin, LOW);        //close SAN fill valve
    digitalWrite(BV6Pin, HIGH);       //open SAN tank drain
    digitalWrite(BV3Pin, HIGH);       //open keg out to SAN
    if (currentMillis - previousMillis >= 20*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 44:                 //20 seconds for BV opening and pump priming
    digitalWrite(PumpPin, HIGH);      //turn on pump
    if (currentMillis - previousMillis >= 15*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 45:                 //to fill keg with sani, adjust if this is found to be faster
    digitalWrite(PumpPin, LOW);       //turn off pump
    digitalWrite(BV6Pin, LOW);        //close SAN drain
    if (currentMillis - previousMillis >= 4*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 46:
    digitalWrite(BV2Pin, HIGH);       //open CO2
     if (currentMillis - previousMillis >= 22*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 47:                //let CO2 push out SAN
     digitalWrite(BV2Pin, LOW);       //close CO2
     if (currentMillis - previousMillis >= 90*SECOND) {currentState++; previousMillis = millis(); }
   break;
 case 48:                //let excess CO2 push out SAN
     digitalWrite(BV3Pin, LOW);       //close keg out to SAN
     digitalWrite(BV8Pin, LOW);       //close Pre Keg Inline Valve
     digitalWrite(A10, LOW);          //set A10 as GND for LED
     digitalWrite(A11, HIGH);         //set A11 as 5V for LED
     currentState = 0;
     currentProgram = 0;
     break;
  } }


 void loop()
{
  currentMillis = millis();
  int MomentarySwitch = digitalRead(A13);            //Read the switch

 if (currentProgram == 0)  {
       if (MomentarySwitch != lastButtonState)        //If the switch changed, due to noise or pressing - lastButtonState starts HIGH
              {lastDebounceTime = millis(); }         //reset the debouncing timer to current millis

  if ((millis() - lastDebounceTime) > debounceDelay) {
         if (MomentarySwitch != buttonState)  {buttonState = MomentarySwitch;}  }     //set buttonState to MomentarySwitch(LOW)

  if (buttonState == 0) {
                 //if buttonState becomes LOW and washer is not in active program, read the purple switch orientation:
       int CleanSanke = digitalRead(A0);
       int CleanPBWTank = digitalRead(A1);
       int CleanSANTank = digitalRead(A2);
       int CleanBothTanks = digitalRead(A3);
       int HotPBWCycle = digitalRead(A14);
       int NastyCycle = digitalRead(A15);
       buttonState = 1;
       if (CleanSanke == 0)      {currentState = 0; currentProgram = 1; currentMillis = millis(); previousMillis = millis();}
       if (CleanPBWTank == 0)    {currentState = 0; currentProgram = 2; currentMillis = millis(); previousMillis = millis();}
       if (CleanSANTank == 0)    {currentState = 0; currentProgram = 3; currentMillis = millis(); previousMillis = millis();}
       if (CleanBothTanks == 0)  {currentState = 0; currentProgram = 4; currentMillis = millis(); previousMillis = millis();}
       if (HotPBWCycle == 0)     {currentState = 0; currentProgram = 5; currentMillis = millis(); previousMillis = millis();}
       if (NastyCycle == 0)      {currentState = 0; currentProgram = 5; currentMillis = millis(); previousMillis = millis();}
       }
       lastButtonState = MomentarySwitch;
       }


 if (currentProgram == 1 || currentProgram == 2 || currentProgram == 3 || currentProgram == 4 || currentProgram == 5 || currentProgram == 6) {
     if (MomentarySwitch != lastButtonState)        //If the switch changed, due to noise or pressing - lastButtonState starts HIGH
           {lastDebounceTime = millis(); }         //reset the debouncing timer to current millis

  if ((millis() - lastDebounceTime) > cancelDelay) {
          if (MomentarySwitch != buttonState)   {buttonState = 2; } }

  if(buttonState == 2)
               {digitalWrite(BV1Pin, LOW);
                digitalWrite(BV2Pin, LOW);
                digitalWrite(BV3Pin, LOW);
                digitalWrite(BV4Pin, LOW);
                digitalWrite(BV5Pin, LOW);
                digitalWrite(BV6Pin, LOW);
                digitalWrite(BV7Pin, LOW);
                digitalWrite(BV8Pin, LOW);
                digitalWrite(BV9Pin, LOW);
                digitalWrite(BV10Pin, LOW);
                digitalWrite(BV11Pin, LOW);
                digitalWrite(PumpPin, LOW);
                digitalWrite(ElementPin, LOW);
                digitalWrite(A10, LOW);          //set A10 as GND for LED
                digitalWrite(A11, HIGH);         //set A11 as 5V for LED
                currentProgram = 0;
                currentState = 0;
                buttonState = 1;
                delay(3*SECOND);    }

                lastButtonState = MomentarySwitch;     }

 if (currentProgram == 1) {
   if (currentState == 2 ||
       currentState == 3 ||
       currentState == 4 ||
       currentState == 5 ||
       currentState == 6 ||
       currentState == 7 ||
       currentState == 8 ||
       currentState == 9 ||
       currentState == 10 ||
       currentState == 11 ||
       currentState == 12 ||
       currentState == 13 ||
       currentState == 14 ||
       currentState == 15 ||
       currentState == 16 ||
       currentState == 17 ||
       currentState == 18 ||
       currentState == 19 ||
       currentState == 21 ||
       currentState == 22) {
        LevelPBWPin = digitalRead(A4);          //check the PBW level sensor
          if (LevelPBWPin == HIGH) {lastDebouncePBWTime = millis(); }    //if in current program 1 and current state 2-22 keep refreshing the debounce time as current
          if (LevelPBWPin == LOW) {
           if (PBWState == 0) {lastDebouncePBWTime = millis(); PBWState = 1; }   //1st time level reading is LOW PBWstate is zero and we make lastDebouncePBWTime current
           if (PBWState == 1) {                                                  //2nd time thru it has current value
           if ((millis() - lastDebouncePBWTime) > debouncePBWDelay) {digitalWrite(ElementPin, LOW); lastDebouncePBWTime = 0; PBWState = 0; }  }  }  }  }


 if (currentProgram == 5) { ElementLevel = digitalRead(A6);          //check the element level sensor
       if (ElementLevel == HIGH) {lastDebouncePBWTime = millis(); }    //if sensor detects liquid, keep refreshing the debounce time as current
       if (ElementLevel == LOW) {
            if (ElementState == 0) {lastDebouncePBWTime = millis(); ElementState = 1; }   //1st time level reading is LOW PBWstate is zero and we make lastDebouncePBWTime current
            if (ElementState == 1) {                                                     //2nd time thru it has current value
            if ((millis() - lastDebouncePBWTime) > debouncePBWDelay) {digitalWrite(ElementPin, LOW); lastDebouncePBWTime = 0; ElementState = 0; currentProgram = 0; }  }  }

       sensorValue = analogRead(A7);                    //take a sensor reading
       if(sensorValue < PBWTempRef) {digitalWrite(ElementPin, HIGH); }

            }




           lastButtonState = MomentarySwitch;

//Serial.print(" Prog:"); Serial.print(currentProgram);
//Serial.print(", State:"); Serial.print(currentState);
//Serial.print(", Button:"); Serial.print(buttonState);
//Serial.print(", DigReadA1:"); Serial.print(CleanPBWTank);
//Serial.println("\t");
      // delay(100);

    if (currentProgram == 0) {
     digitalWrite(BV1Pin, LOW);
     digitalWrite(BV2Pin, LOW);
     digitalWrite(BV3Pin, LOW);
     digitalWrite(BV4Pin, LOW);
     digitalWrite(BV5Pin, LOW);
     digitalWrite(BV6Pin, LOW);
     digitalWrite(BV7Pin, LOW);
     digitalWrite(BV8Pin, LOW);
     digitalWrite(BV9Pin, LOW);
     digitalWrite(BV10Pin, LOW);
     digitalWrite(BV11Pin, LOW);
     digitalWrite(PumpPin, LOW);
     digitalWrite(ElementPin, LOW); }




     if (currentProgram == 1) {CleanSanke15(); }
     if (currentProgram == 2) {CleanPBW();     }
     if (currentProgram == 3) {CleanSAN();     }
     if (currentProgram == 4) {CleanBOTH();    }
     if (currentProgram == 5) {CleanHot();     }
     if (currentProgram == 6) {CleanNasty();   }
     }






/*
 int sensorValue = analogRead(A7);
  //float voltage = sensorValue *  (5000.0/ 1024.0);      //convert sensor reading to mV
  //float temperature = voltage/10.0;                     //convert the mV to celsius
  if (sensorValue > PBWTempRef) { digitalWrite(ElementPin, LOW); }

*/
