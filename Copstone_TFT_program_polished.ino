#include <UTFT.h>
#include <URTouch.h>
#include <SchedBase.h>
#include <SchedTask.h>

/********************* Functions Pre-Declare ************************************/
// Grow Light
void turnOnGL8H16H();        
void turnOffGL8H16H();        
void turnOnGL6H18H();
void turnOffGL6H18H();
void turnOnGL10H14H();
void turnOffGL10H14H();
void turnOnGL12H12H();
void turnOffGL12H12H();
// Water Pump
void turnOnWP2();          
void turnOffWP2();        
void turnOnWP3();          
void turnOffWP3();        
void turnOnWP4();          
void turnOffWP4();        
void turnOnWP8();        
void turnOffWP8();
     
/********************* Pin Assignment ************************************/
// water level
const int waterLevelPin = A1;   //input of water level sensor
const int lowWaterPin = 13;     //output for low water level
// water quality
const int waterQualityPin = A3; //input of water quality sensor
const int qualityAlarmPin = 7;  //output for water quality alarm
// air temperature
const int airTempPin = A2;      //input of air temp. sensor
const int coolingFanPin = 11;   //output for cooling fan
// outputs on a schedule base
const int growLightPin = 8;     //output for grow lights
const int waterPumpPin = 9;     //output for water pump
// other
const int LEDLights = 12;      //output for LED Lights

/********************* Global Vars ************************************/
int SchedBase;        // schedule base use with dispatcher
// time units constants
const unsigned long SECOND = 1000UL;
const unsigned long MINUTE = 60UL*SECOND;
const unsigned long HOUR = 3600UL*SECOND;
// Grow Light Scheduler variables
int growLightSchedule = 1;
bool GrowLightOn1, GrowLightOn2, GrowLightOn3, GrowLightOn4 = false;
bool initGrowLight1, initGrowLight2, initGrowLight3, initGrowLight4 = false;
// air variables
int airTemp;      // analog value of Air Temp Sensor
int airTempN;     // normalized Air Temperature value
// water Pump variables
int waterPumpSchedule = 1;
bool waterPumpOn1,waterPumpOn2,waterPumpOn3,waterPumpOn4 = false;
bool initWaterPump1,initWaterPump2,initWaterPump3,initWaterPump4 = false;
int waterQuality; // analog value of Water Quality Sensor
int waterQualityN;// normalized Water Quality value
int waterLevel;   // analog value of Water Level Sensor
int waterLevelN;  // normalized Water Level value

/********************* Task Scheduler Calls ************************************/
/*Below is each of the functions we use on a schedule base.
 This is using the #include <SchedBase.h> and
#include <SchedTask.h> libraries.
 */
// Grow Light schedules
SchedTask TurnOnGL8H16H (NEVER, ONESHOT, turnOnGL8H16H);      
SchedTask TurnOffGL8H16H (NEVER, ONESHOT, turnOffGL8H16H);  
SchedTask TurnOnGL6H18H (NEVER, ONESHOT, turnOnGL6H18H);
SchedTask TurnOffGL6H18H (NEVER, ONESHOT, turnOffGL6H18H);
SchedTask TurnOnGL10H14H (NEVER, ONESHOT, turnOnGL10H14H);
SchedTask TurnOffGL10H14H (NEVER, ONESHOT, turnOffGL10H14H);
SchedTask TurnOnGL12H12H (NEVER, ONESHOT, turnOnGL12H12H);
SchedTask TurnOffGL12H12H (NEVER, ONESHOT, turnOffGL12H12H);
// Water Pump Schedules
SchedTask TurnOnWP2 (NEVER, ONESHOT, turnOnWP2);    
SchedTask TurnOffWP2 (NEVER, ONESHOT, turnOffWP2);    
SchedTask TurnOnWP3 (NEVER, ONESHOT, turnOnWP3);      
SchedTask TurnOffWP3 (NEVER, ONESHOT, turnOffWP3);    
SchedTask TurnOnWP4 (NEVER, ONESHOT, turnOnWP4);    
SchedTask TurnOffWP4 (NEVER, ONESHOT, turnOffWP4);    
SchedTask TurnOnWP8 (NEVER, ONESHOT, turnOnWP8);    
SchedTask TurnOffWP8 (NEVER, ONESHOT, turnOffWP8);  

/********************* Graphical Interface ************************************/
// Creating Objects
UTFT myGLCD(ILI9341_16,38,39,40,41);
URTouch  myTouch( 6, 5, 4, 3, 2);
// Defining Variables
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
extern uint8_t SevenSegNumFont[];
float levelVolts;
float tempVolts;
float qualityVolts;
int x, y;
char currentPage;
//Functions for graphics interface
void drawHomeScreen();
void drawGrowLight();
void drawWaterPump();
void drawFrame(int x1, int y1, int x2, int y2);
void drawWaterPump();

/********************* Setup ************************************/
void setup() {
  // Graphical Interface Setup
  myGLCD.InitLCD();
  myGLCD.clrScr();
  myTouch.InitTouch();
  myTouch.setPrecision(PREC_MEDIUM);
  drawHomeScreen();  // Draws the Home Screen
  currentPage = '0'; // Indicates that we are at Home Screen
  // pin modes
  pinMode(lowWaterPin, OUTPUT);
  pinMode(coolingFanPin, OUTPUT);
  pinMode(qualityAlarmPin, OUTPUT);
  pinMode(growLightPin, OUTPUT);
  pinMode(waterPumpPin, OUTPUT);
  pinMode(LEDLights, OUTPUT);
  // serial communication enabled
    Serial.begin(9600);
}

/********************* loop ************************************/
void loop() {
  // Process
  /* This dispatcher is needed for the SchedBase.h file. This
  are bing called furthing down in other functions.*/
  SchedBase::dispatcher();
  // Process functions  
  // Air Management                
  manageAir();
  // Water Management
  manageWater();
  // Turn LED light on
  turnOnLEDLights();
 
  //  Graphical Interface
  // Home Screen
  if (currentPage == '0') {
    if (myTouch.dataAvailable()) {
      myTouch.read();
      // x coordinate where the screen has been pressed
      x = myTouch.getX();
      // y coordinates where the screen has been pressed
      y = myTouch.getY();
      // If we press the Grow Light Button
      if ((x >= 35) && (x <= 120) && (y >= 105) && (y <= 160)) {
         // Highlighs the buttons when it's pressed
        drawFrame(35, 105, 120, 160);
        // Indicates that we are the first example
        currentPage = '1';
        /*  Clears the screen
         *  It is called only once, because in the next iteration
         *  of the loop, this above if statement will be false so
         *  this funtion won't be called. This function will draw
        the graphics of the first example.*/
        myGLCD.clrScr();
        drawGrowLight();
      }
      // If we press the WaterPump Button
      if ((x >= 35) && (x <= 120) && (y >= 165) && (y <= 220)) {
        drawFrame(35, 165, 120, 220);
        currentPage = '2';
        myGLCD.clrScr();
        drawWaterPump();
      }  
    }
  }
 
  // Grow Light screen options
  if (currentPage == '1') {    
     if (myTouch.dataAvailable()) {
      myTouch.read();
      // x coordinate where the screen has been pressed
      x = myTouch.getX();
      // y coordinates where the screen has been pressed
      y = myTouch.getY();
      // Gets distance from the sensor and this function is repeatedly
      // called while we are at the first example in order to print
      // the lasest results from the distance sensor
      drawGrowLight();
     
      // If we press 8/16
      if ((x >= 40) && (x <= 130) &&(y >= 135) && (y <= 163)) {
        drawFrame (40, 135, 130, 163);
        growLightSchedule = 1;
        Serial.print("growLightSchedule = '1'\n");
      }
     
      // If we press 6/18
      if ((x >= 40) && (x <= 130) &&(y >= 173) && (y <= 201)) {
        drawFrame (40, 173, 130, 201);
        growLightSchedule = 2;  
        Serial.print("growLightSchedule = '2'\n");
      }
     
      // If we press 10/14      
      if ((x >= 170) && (x <= 260) &&(y >= 135) && (y <= 163)) {
        drawFrame (170, 135, 260, 163);
        growLightSchedule = 3;
        Serial.print("growLightSchedule = '3'\n");
      }
     
      // If we press 12/12
      if ((x >= 170) && (x <= 260) &&(y >= 173) && (y <= 201)) {
        drawFrame (170, 173, 260, 201);
        growLightSchedule = 4;
        Serial.print("growLightSchedule = '4'\n");
      }
     
      // If we press the Back Button
      if ((x >= 10) && (x <= 60) &&(y >= 10) && (y <= 36)) {
        drawFrame(10, 10, 60, 36);
        currentPage = '0'; // Indicates we are at home screen
        myGLCD.clrScr();
        drawHomeScreen(); // Draws the home screen
      }
    }
  }  
  // Grow Light Scheduler
  // Grow Light Schedule 1: 8 Hours ON / 16 Hours OFF
  if (growLightSchedule == 1){
    // reset all other Grow Light Schedulers
    TurnOnGL6H18H.setNext(NEVER);
    TurnOffGL6H18H.setNext(NEVER);
    TurnOnGL10H14H.setNext(NEVER);
    TurnOffGL10H14H.setNext(NEVER);
    TurnOnGL12H12H.setNext(NEVER);
    TurnOffGL12H12H.setNext(NEVER);
    initGrowLight2 = false;
    initGrowLight3 = false;
    initGrowLight4 = false;
    GrowLightOn2 = false;
    GrowLightOn3 = false;
    GrowLightOn4 = false;
    // run this only once, then run only the else loop
    if (GrowLightOn1 == false && initGrowLight1 == false){
      TurnOnGL8H16H.setNext(NOW);
      initGrowLight1 = true;
      //Serial.print("TurnOnGL8H16H\n");
    }
    else if (GrowLightOn1 == true && initGrowLight1 == true){
      TurnOnGL8H16H.setNext(5*SECOND);
    }
  }
  // Grow Light Schedule 2: 6 Hours ON / 18 Hours OFF    
  if (growLightSchedule == 2){
    // reset all other Grow Light Schedulers
    TurnOnGL8H16H.setNext(NEVER);
    TurnOffGL8H16H.setNext(NEVER);
    TurnOnGL10H14H.setNext(NEVER);
    TurnOffGL10H14H.setNext(NEVER);
    TurnOnGL12H12H.setNext(NEVER);
    TurnOffGL12H12H.setNext(NEVER);
    initGrowLight1 = false;
    initGrowLight3 = false;
    initGrowLight4 = false;
    GrowLightOn1 = false;
    GrowLightOn3 = false;
    GrowLightOn4 = false;
    // run this only once, then run only the else loop
    if (GrowLightOn2 == false && initGrowLight2 == false){
      TurnOnGL6H18H.setNext(NOW);
      initGrowLight2 = true;
      //Serial.print("TurnOnGL6H18H\n");
    }
    else if  (GrowLightOn2 == true && initGrowLight2 == true){
      TurnOnGL6H18H.setNext(2*SECOND);
    }
  }
  // Grow Light Schedule 3: 10 Hours ON / 14 Hours OFF          
  if (growLightSchedule == 3){
    // reset all other Grow Light Schedulers
    TurnOnGL8H16H.setNext(NEVER);
    TurnOffGL8H16H.setNext(NEVER);
    TurnOnGL6H18H.setNext(NEVER);
    TurnOffGL6H18H.setNext(NEVER);
    TurnOnGL12H12H.setNext(NEVER);
    TurnOffGL12H12H.setNext(NEVER);
    initGrowLight1 = false;
    initGrowLight2 = false;
    initGrowLight4 = false;
    GrowLightOn1 = false;
    GrowLightOn2 = false;
    GrowLightOn4 = false;
    // run this only once, then run only the else loop
    if (GrowLightOn3 == false && initGrowLight3 == false){
      TurnOnGL10H14H.setNext(NOW);
      initGrowLight3 = true;
      //Serial.print("TurnOnGL10H14H\n");
    }
    else if  (GrowLightOn3 == true && initGrowLight3 == true){
      TurnOnGL10H14H.setNext(5*SECOND);
    }
  }
  // Initiate schedule for GROW LIGHT 12H12H                                
  if (growLightSchedule == 4){
    // reset all other Grow Light Schedulers
    TurnOnGL8H16H.setNext(NEVER);
    TurnOffGL8H16H.setNext(NEVER);
    TurnOnGL6H18H.setNext(NEVER);
    TurnOffGL6H18H.setNext(NEVER);
    TurnOnGL10H14H.setNext(NEVER);
    TurnOffGL10H14H.setNext(NEVER);
    initGrowLight1 = false;
    initGrowLight2 = false;
    initGrowLight3 = false;
    GrowLightOn1 = false;
    GrowLightOn2 = false;
    GrowLightOn3 = false;
    // run this only once, then run only the else loop
    if (GrowLightOn4 == false && initGrowLight4 == false){
      TurnOnGL12H12H.setNext(NOW);
      initGrowLight4 = true;
      //Serial.print("TurnOnGL12H12H\n");
    }
    else if (GrowLightOn4 == true && initGrowLight4 == true){
      TurnOnGL12H12H.setNext(5*SECOND);
    }  
  }

  // Water PUMP screen options
  if (currentPage == '2') {    
    if (myTouch.dataAvailable()) {
      myTouch.read();
      // x coordinate where the screen has been pressed
      x = myTouch.getX();
      // y coordinates where the screen has been pressed
      y = myTouch.getY();
      // Gets distance from the sensor and this function is repeatedly
      // called while we are at the first example in order to print
      // the lasest results from the distance sensor
      drawWaterPump();
       
      // If we press 2
      if ((x >= 40) && (x <= 130) &&(y >= 135) && (y <= 163)) {
        drawFrame (40, 135, 130, 163);
        waterPumpSchedule = 1;
        Serial.print("waterPumpSchedule = '2'\n");
      }
      // If we press 3
      else if ((x >= 40) && (x <= 130) &&(y >= 173) && (y <= 201)) {
        drawFrame (40, 173, 130, 201);
        waterPumpSchedule = 2;  
         Serial.print("waterPumpSchedule = '3'\n");
      }      
      // If we press 4      
      else if ((x >= 170) && (x <= 260) &&(y >= 135) && (y <= 163)) {
        drawFrame (170, 135, 260, 163);
        waterPumpSchedule = 3;
         Serial.print("waterPumpSchedule = '4'\n");
      }
      // If we press 8
      else if ((x >= 170) && (x <= 260) &&(y >= 173) && (y <= 201)) {
        drawFrame (170, 173, 260, 201);
        waterPumpSchedule = 4;
         Serial.print("waterPumpSchedule = '8'\n");
      }  
      // If we press the Back Button
      if ((x >= 10) && (x <= 60) &&(y >= 10) && (y <= 36)) {
        drawFrame(10, 10, 60, 36);
        //Serial.println("THIS")
        currentPage = '0'; // Indicates we are at home screen
        myGLCD.clrScr();
        drawHomeScreen(); // Draws the home screen
      }
    }
  }
  // Water Pump Schedule 1: 2 Cycles
  if (waterPumpSchedule == 1){
    // reset all other Grow Light Schedulers
    TurnOnWP3.setNext (NEVER);      
    TurnOffWP3.setNext (NEVER);    
    TurnOnWP4.setNext (NEVER);    
    TurnOffWP4.setNext (NEVER);    
    TurnOnWP8.setNext (NEVER);    
    TurnOffWP8.setNext (NEVER);
    waterPumpOn2 = false;
    waterPumpOn3 = false;
    waterPumpOn4 = false;
    initWaterPump2 = false;
    initWaterPump3 = false;
    initWaterPump4 = false;
   
    // run this only once, then run only the else loop
    if (waterPumpOn1 == false && initWaterPump1 == false){
      TurnOnWP2.setNext(NOW);
      initWaterPump1 = true;
      Serial.print("TurnOnWP2\n");
         }
    else if (waterPumpOn1 == true && initWaterPump1 == true){
      TurnOnWP2.setNext(1*SECOND);
    }
  }
  // Grow Light Schedule 2: 6 Hours ON / 18 Hours OFF    
  if (waterPumpSchedule == 2){
    // reset all other Grow Light Schedulers
    TurnOnWP2.setNext (NEVER);      
    TurnOffWP2.setNext (NEVER);    
    TurnOnWP4.setNext (NEVER);    
    TurnOffWP4.setNext (NEVER);    
    TurnOnWP8.setNext (NEVER);    
    TurnOffWP8.setNext (NEVER);
    waterPumpOn1 = false;
    waterPumpOn3 = false;
    waterPumpOn4 = false;
    initWaterPump1 = false;
    initWaterPump3 = false;
    initWaterPump4 = false;
    // run this only once, then run only the else loop
    if (waterPumpOn2 == false && initWaterPump2 == false){
      TurnOnWP3.setNext(NOW);
      initWaterPump2 = true;
      //Serial.print("TurnOnWP3\n");
    }
    else if (waterPumpOn2 == true && initWaterPump2 == true){
      TurnOnWP3.setNext(1*SECOND);
    }
  }
  // Grow Light Schedule 3: 10 Hours ON / 14 Hours OFF          
  if (waterPumpSchedule == 3){
    // reset all other Grow Light Schedulers
    TurnOnWP2.setNext (NEVER);      
    TurnOffWP2.setNext (NEVER);    
    TurnOnWP3.setNext (NEVER);    
    TurnOffWP3.setNext (NEVER);    
    TurnOnWP8.setNext (NEVER);    
    TurnOffWP8.setNext (NEVER);
    waterPumpOn1 = false;
    waterPumpOn2 = false;
    waterPumpOn4 = false;
    initWaterPump1 = false;
    initWaterPump2 = false;
    initWaterPump4 = false;
    // run this only once, then run only the else loop
    if (waterPumpOn3 == false && initWaterPump3 == false){
      TurnOnWP4.setNext(NOW);
      initWaterPump3 = true;
      //Serial.print("TurnOnWP4\n");
    }
    else if (waterPumpOn3 == true && initWaterPump3 == true){
      TurnOnWP4.setNext(1*SECOND);
    }
  }
  // Initiate schedule for GROW LIGHT 12H12H                                
  if (waterPumpSchedule == 4){
    // reset all other Grow Light Schedulers
    TurnOnWP2.setNext (NEVER);    
    TurnOffWP2.setNext (NEVER);
    TurnOnWP3.setNext (NEVER);      
    TurnOffWP3.setNext (NEVER);    
    TurnOnWP4.setNext (NEVER);    
    TurnOffWP4.setNext (NEVER);    
   
    waterPumpOn1 = false;
    waterPumpOn2 = false;
    waterPumpOn3 = false;
    initWaterPump1 = false;
    initWaterPump2 = false;
    initWaterPump3 = false;
    // run this only once, then run only the else loop
    if (waterPumpOn4 == false && initWaterPump4 == false){
      TurnOnWP8.setNext(NOW);
      initWaterPump4 = true;
      //Serial.print("TurnOnWP8\n");
    }
    else if (waterPumpOn1 == true && initWaterPump1 == true){
      TurnOnWP8.setNext(1*SECOND);
    }
  }
}

/********************* Functions ************************************/
/*  Grow Light Schedule:
 *  1 = Ligh ON for 8 hours / 24 hours (default)
 *  2 = Ligh ON for 6 hours / 24 hours
 *  3 = Ligh ON for 10 hours / 24 hours
 *  4 = Ligh ON for 12 hours / 24 hours
 */
// Grow Light Schedule 1: 8/16
// Turn On for 8 hours
void turnOnGL8H16H() {
  // Turn on the Grow Light
  digitalWrite(growLightPin, HIGH);                    
  Serial.print("Grow Light is turned ON 8H16H!\n");
  GrowLightOn1 = true;
  // schedule turnOffGL8H16H after DURATION ( sec)
  TurnOffGL8H16H.setNext(5*SECOND);                    
}
// turn Off for 16 Hours
void turnOffGL8H16H() {
  digitalWrite(growLightPin, LOW);                
  Serial.print("Grow Light is turned OFF 8H16H!\n");
  GrowLightOn1 = false;
  TurnOnGL8H16H.setNext(5*SECOND);                
}
// Grow Light Schedule 2: 6/18
// Turn On for 6 hours
void turnOnGL6H18H() {
  digitalWrite(growLightPin, HIGH);                  
  Serial.print("Grow Light is turned ON 6H18H!\n");
  GrowLightOn2 = true;
  TurnOffGL6H18H.setNext(5*SECOND);                    
}
// Turn Off for 18 Hours
void turnOffGL6H18H() {
  digitalWrite(growLightPin, LOW);                
  Serial.print("Grow Light is turned OFF 6H18H!\n");
  GrowLightOn2 = false;
  TurnOnGL6H18H.setNext(5*SECOND);          
}
// Grow Light Schedule 3: 10/14
// Turn On for 10 hours
void turnOnGL10H14H() {
  digitalWrite(growLightPin, HIGH);                  
  Serial.print("Grow Light is turned ON 10H14H!\n");
  GrowLightOn3 = true;
  TurnOffGL10H14H.setNext(5*SECOND);                
}
// Turn Off 14 Hours
void turnOffGL10H14H() {
  digitalWrite(growLightPin, LOW);              
  Serial.print("Grow Light is turned OFF 10H14H!\n");
  GrowLightOn3 = false;
  TurnOnGL10H14H.setNext(5*SECOND);              
}
// Grow Light Schedule 4: 12/12
// Turn On for 12 hours
void turnOnGL12H12H() {
  digitalWrite(growLightPin, HIGH);                  
  Serial.print("Grow Light is turned ON 12H12H!\n");
  GrowLightOn4 = true;
  TurnOffGL12H12H.setNext(5*SECOND);                  
}
//Turn Off 12 Hours
void turnOffGL12H12H() {
  digitalWrite(growLightPin, LOW);              
  Serial.print("Grow Light is turned OFF 12H12H!\n");
  GrowLightOn4 = false;
  TurnOnGL12H12H.setNext(2*SECOND);                
}

// Water Pump Schedule 1: 2 cycles
// turn the Water Pump on
void turnOnWP2() {
  digitalWrite(waterPumpPin, HIGH);                  
  TurnOffWP2.setNext(1000);
   Serial.print("WP 2 cycle on\n");            
}
// turn the Water Pump off
void turnOffWP2() {
  digitalWrite(waterPumpPin, LOW);                
  TurnOnWP2.setNext(1000);  
  Serial.print("im here NOW");  
   Serial.print("WP 2 cycle off\n");              
}
// Water Pump Schedule 1: 3 cycles
// turn the Water Pump on
void turnOnWP3() {
  digitalWrite(waterPumpPin, HIGH);                  
  TurnOffWP3.setNext(2000);  
   Serial.print("WP 3 cycle on\n");                  
}
// turn the Water Pump off
void turnOffWP3() {
  digitalWrite(waterPumpPin, LOW);                
  TurnOnWP3.setNext(2000);
   Serial.print("WP 2 cycle off\n");                
}
// Water Pump Schedule 1: 4 cycles
// turn the Water Pump on
void turnOnWP4() {
  digitalWrite(waterPumpPin, HIGH);                  
  TurnOffWP4.setNext(3000);  
   Serial.print("WP 4 cycle on\n");                    
}
// turn the Water Pump off
void turnOffWP4() {
  digitalWrite(waterPumpPin, LOW);                
  TurnOnWP4.setNext(3000);
   Serial.print("WP 4 cycle off\n");                  
}
// Water Pump Schedule 1: 8 cycles
// turn the Water Pump on
void turnOnWP8() {
  digitalWrite(waterPumpPin, HIGH);                  
  TurnOffWP8.setNext(4000);  
   Serial.print("WP 8 cycle on\n");                    
}
// turn the Water Pump off
void turnOffWP8() {
  digitalWrite(waterPumpPin, LOW);              
  TurnOnWP8.setNext(4000);
   Serial.print("WP 8 cycle off\n");              
}
// manage Air function
void manageAir(){
  // analog read of air temp sensor
  airTemp = analogRead(airTempPin);  
  // normalize
 
  // warnings

  // control
  if (airTemp > 400){
    digitalWrite(coolingFanPin, HIGH);
  } else {
    digitalWrite(coolingFanPin, LOW);  
  }  
    //Serial.println(airTemp);
}

// manage water: quality, level and cycle
void manageWater(){
  // analog reads: quality and level
  waterQuality = analogRead(waterQualityPin);
  waterLevel = analogRead(waterLevelPin);
 
  // normalize
 
  // warnings
  if ((waterQuality > 500) && (waterQuality < 1000)){
    digitalWrite(qualityAlarmPin, HIGH);
  } else {
    digitalWrite(qualityAlarmPin, LOW);
  }
  if (waterLevel < 800){
    digitalWrite(lowWaterPin, HIGH);
  } else {
    digitalWrite(lowWaterPin, LOW);
  }
  // control
 
  //Serial.println(waterLevel);
}

void turnOnLEDLights(){
  digitalWrite(LEDLights,HIGH);
}
void drawHomeScreen() {
  // Title - Hydra Grow
  // Sets the background color of the area where the text will be printed to
  myGLCD.setBackColor(0,0,0);
  // Sets color to white
  myGLCD.setColor(255, 255, 255);
  // Sets font to big
  myGLCD.setFont(BigFont);
  // Prints the string on the screen
  myGLCD.print((char*)(char*)"HYDRA GROW", CENTER, 10);
   // Sets color to green
  myGLCD.setColor(16, 167, 103);
  // Draws the green line
  myGLCD.drawLine(150,55,150,230);
  // Draws the green line
  myGLCD.drawLine(0,32,319,32);
   // Sets color to white
  myGLCD.setColor(255, 255, 255);
  // Sets the font to small
  myGLCD.setFont(SmallFont);
 
  // Prints the Process functions
  myGLCD.print((char*)(char*)"Capstone Project", CENTER, 41);
  myGLCD.print((char*)(char*)"Temperature = ", 165, 114);
  myGLCD.print((char*)(char*)"Water Quality = ", 165, 135);
  myGLCD.print((char*)(char*)"Water Level = ", 165, 155);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)(char*)"Select", 20, 55);
  myGLCD.print((char*)(char*)"Function", 17, 73);
  myGLCD.print((char*)(char*)"INFO", 205, 65);
 
  // Button - Grow Light
  // Sets green color
  myGLCD.setColor(16, 167, 103);
  // Draws filled rounded rectangle
  myGLCD.fillRoundRect (35, 105, 120, 160);
   // Sets color to white
  myGLCD.setColor(255, 255, 255);
  // Draws rounded rectangle without a fill, so the overall appearance of the button looks like it has a frame
  myGLCD.drawRoundRect (35, 105, 120, 160);
  // Sets the font to big
  myGLCD.setFont(BigFont);
  // Sets the background color of the area where the text will be printed to green, same as the button
  myGLCD.setBackColor(16, 167, 103);
   // Prints the string
  myGLCD.print((char*)"Grow", 45, 114);
  myGLCD.print((char*)"Light", 40, 133);
 
  // Button - Water Pump
  myGLCD.setColor(16, 167, 103);
  myGLCD.fillRoundRect (35, 165, 120, 220);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (35, 165, 120, 220);
  myGLCD.setFont(BigFont); // Sets the font to big
  myGLCD.setBackColor(16, 167, 103);
  myGLCD.print((char*)"Water", 40, 175);
  myGLCD.print((char*)"Pump", 45, 195);

  //Print the Temperature
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(tempVolts,274, 114, 3,'0');
  myGLCD.setFont(SmallFont);
 
  //Print the Water Quality
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(qualityVolts,288, 135, 3,'0');
  myGLCD.setFont(SmallFont);
 
  //Print the Water Quality
  myGLCD.setFont(SmallFont);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.printNumI(levelVolts,274, 155, 3,'0');
  myGLCD.setFont(SmallFont);
}

// Highlights the button when pressed
void drawFrame(int x1, int y1, int x2, int y2) {
  myGLCD.setColor(255, 0, 0);
  myGLCD.drawRoundRect (x1, y1, x2, y2);
  while (myTouch.dataAvailable())
    myTouch.read();
    myGLCD.setColor(255, 255, 255);
    myGLCD.drawRoundRect (x1, y1, x2, y2);
}

//Grow Light screen
void drawGrowLight() {
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRoundRect (10, 10, 60, 36);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 10, 60, 36);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print((char*)"<-", 18, 15);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SmallFont);
  myGLCD.print((char*)"Back to Main Menu", 70, 18);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"GROW LIGHT", CENTER, 60);
  myGLCD.print((char*)"SCHEDULE", CENTER, 75);
  myGLCD.setColor(225, 225, 0);    
  myGLCD.drawLine(0,100,319,100);  
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(SmallFont);
  myGLCD.print((char*)"Select Unit Timing*", CENTER, 114);
 
  //Botton 1
  myGLCD.setColor(225, 225, 0);
  myGLCD.fillRoundRect (40, 135, 130, 163);
  myGLCD.setColor(225, 255, 255);
  myGLCD.drawRoundRect (40, 135, 130, 163);
  myGLCD.setBackColor(225, 225, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"8/16", 55, 142);
 
  //Botton 2
  myGLCD.setColor(225, 225, 0);
  myGLCD.fillRoundRect (40, 173, 130, 201);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (40, 173, 130, 201);
  myGLCD.setBackColor(225, 225, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"6/18", 55, 180);
 
   //Botton 3
  myGLCD.setColor(225, 225, 0);
  myGLCD.fillRoundRect (170, 135, 260, 163);
  myGLCD.setColor(225, 255, 255);
  myGLCD.drawRoundRect (170, 135, 260, 163);
  myGLCD.setBackColor(225, 225, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"10/14", 179, 142);
 
  //Botton 4
  myGLCD.setColor(225, 225, 0);
  myGLCD.fillRoundRect (170, 173, 260, 201);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (170, 173, 260, 201);
  myGLCD.setBackColor(225, 225, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"12/12", 179, 180);
  myGLCD.setBackColor(0, 0, 0);
 
  //Bottom of the screen text
  myGLCD.setFont(SmallFont);
  myGLCD.print((char*)"*Time is represented in hours.", CENTER, 220);
}

void drawWaterPump() {
  myGLCD.setColor(100, 155, 203);
  myGLCD.fillRoundRect (10, 10, 60, 36);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (10, 10, 60, 36);
  myGLCD.setFont(BigFont);
  myGLCD.setBackColor(100, 155, 203);
  myGLCD.print((char*)"<-", 18, 15);
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setFont(SmallFont);
  myGLCD.print((char*)"Back to Main Menu", 70, 18);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"Water Pump", CENTER, 60);
  myGLCD.print((char*)"SCHEDULE", CENTER, 75);
  myGLCD.setColor(0, 0, 255);    
  myGLCD.drawLine(0,100,319,100);  
  myGLCD.setBackColor(0, 0, 0);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(SmallFont);
  myGLCD.print((char*)"Select how many cycles per day", CENTER, 114);
 
  //Botton 1
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (40, 135, 130, 163);
  myGLCD.setColor(225, 255, 255);
  myGLCD.drawRoundRect (40, 135, 130, 163);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"2", 77, 142);
 
  //Botton 2
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (40, 173, 130, 201);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (40, 173, 130, 201);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"3", 77, 180);
 
   //Botton 3
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (170, 135, 260, 163);
  myGLCD.setColor(225, 255, 255);
  myGLCD.drawRoundRect (170, 135, 260, 163);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"4", 207, 142);
 
  //Botton 4
  myGLCD.setColor(0, 0, 255);
  myGLCD.fillRoundRect (170, 173, 260, 201);
  myGLCD.setColor(255, 255, 255);
  myGLCD.drawRoundRect (170, 173, 260, 201);
  myGLCD.setBackColor(0, 0, 255);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setFont(BigFont);
  myGLCD.print((char*)"8", 207, 180);
  myGLCD.setBackColor(0, 0, 0);
 
  //Bottom of the screen text
  myGLCD.setFont(SmallFont);
  myGLCD.print((char*)"*Water Pump runs in 15 minute cycles", CENTER, 220);
}
