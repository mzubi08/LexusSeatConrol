//Digital Pins
int HighLED = 11;
int MedLED = 12;
int LowLED = 13;

//12v Power Relays
int HeatBackRelay = 10;
int HeatBottomRelay = 9;
int CoolRelay = 8;

//PWM Fan control
int FanBack = 6;
int FanBottom = 0;
int UDPin = 7;

//Button LEDs
int HeatBackLED = 5;
int HeatBottomLED = 4;
int CoolLED = 3;


//Analog Pins
//Buttons:
int CoolPin = 0;
int HeatBottomPin = 2;
int HeatBackPin = 1;

//Temp sensors
int ThermistorPinBack = 3; 
int ThermistorPinBottom = 4; 

//Interrupt Pin
ButtonInterrupt = 5

//Operational Vars

//Core flags/vars
String State = "";
int PotPosition;
int Level = 0;
bool Cool = false;
bool HeatBack = false;
bool HeatBottom = false;

//Prevent multiple actions from one press
bool ButtonActive = false;

//Flags for preventing the seat from going higher than desired
bool HeatBackSafety = false;
bool HeatBottomSafety = false;

//Back Fan speed Levels
int BackFanSpeedLow = (255/3);
int BackFanSpeedMed = (255/3)*2;
int BackFanSpeedHigh = 255;

//Back Fan speed Levels
int BottomFanSpeedLow = 13;
int BottomFanSpeedMed = 18;
int BottomFanSpeedHigh = 23;

int BackFanSpeed = 0;
int BottomFanSpeed = 0;

//Empty inits for button pins
float HeatBackPinValue,HeatBottomPinValue,CoolPinValue;

//Loop delay
int msDelay = 200;
float VoltageThreshold = 4.98;

//Temperature Sensing Vars
int Vo;
int BottomDesiredTemp; 
int BackDesiredTemp; 
float R1 = 10000; 
float logR2, R2, T_Back, T_Bottom; 
float c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
float Temps_back[15];
float Temps_bottom[15];
float Voltage_Memory[20];


//Bottom Temps
int DesiredBottomTempLow = 100;
int DesiredBottomTempMedium= 125;
int DesiredBottomTempHigh = 150;
int BottomTempMargin = 5;

//Bottom Temps
int DesiredBackTempLow = 85;
int DesiredBackTempMedium= 95;
int DesiredBackTempHigh = 105;
int BackTempMargin = 5;

void setup() {
  // Start serial
  Serial.begin(9600); 

  //Set Pin Modes
  pinMode(HeatBackRelay,OUTPUT);
  pinMode(HeatBottomRelay,OUTPUT);
  pinMode(CoolRelay,OUTPUT);
  
  pinMode(CoolLED,OUTPUT);
  pinMode(MedLED,OUTPUT);
  pinMode(LowLED,OUTPUT);

  pinMode(HighLED,OUTPUT);
  pinMode(HeatBackLED,OUTPUT);
  pinMode(HeatBottomLED,OUTPUT);

  pinMode(FanBack,OUTPUT);
  pinMode(FanBottom,OUTPUT);
  pinMode(UDPin,OUTPUT);
  digitalWrite(FanBottom,HIGH);
  digitalWrite(UDPin,HIGH);
  
  
  //Add interrupt
  //On the Arduino R4, the interrupt is not working.... Worked on the R3.
  //Enable the lines below on a board where it works.
  //pinMode(ButtonInterrupt, INPUT);
  //attachInterrupt(ButtonInterrupt, InterruptButtonPoller, FALLING);

  //Ensure relays are off (yes, with this relay board HIGH is off)
  digitalWrite(HeatBottomRelay,HIGH);
  digitalWrite(HeatBackRelay,HIGH);
  digitalWrite(CoolRelay,HIGH);

  //Ensure all LEDs are off
  digitalWrite(LowLED,HIGH);
  digitalWrite(MedLED,HIGH);
  digitalWrite(HighLED,HIGH);

  digitalWrite(CoolLED,HIGH);
  digitalWrite(HeatBackLED,HIGH);
  digitalWrite(HeatBottomLED,HIGH);

  PotInit();

  //A little show
  SystemReadyFlash();
}

void InterruptButtonPoller(){
  //Serial.println("INT");
  float maxVal = 0;
  float averageVal = 0.00;

  HeatBackPinValue = analogRead(HeatBackPin) * (5.0 / 1023.0);
  HeatBottomPinValue = analogRead(HeatBottomPin) * (5.0 / 1023.0);
  CoolPinValue = analogRead(CoolPin) * (5.0 / 1023.0);
  
  float buttonVals[] = {HeatBackPinValue,HeatBottomPinValue,CoolPinValue};
  int blength = sizeof(buttonVals)/sizeof(buttonVals[0]);

  for (int i = 0; i < (blength); i++) {
    if (maxVal < buttonVals[i]){maxVal = buttonVals[i];};
  }

  Serial.println(maxVal);
  Serial.println(ButtonActive);
  averageVal = AverageVoltage(HeatBackPinValue, HeatBottomPinValue, CoolPinValue);
  Serial.println(averageVal);
  
  if (!ButtonActive && maxVal > 3.5){
    if (HeatBackPinValue == maxVal){
      ButtonActive = true;
      SetMode("HeatBack");
    } else if (HeatBottomPinValue == maxVal){
      ButtonActive = true;
      SetMode("HeatBottom");
    } else if (CoolPinValue == (maxVal)){
      ButtonActive = true;
      SetMode("Cool");
    } 
  } else if (ButtonActive && maxVal < 3.5){
    ButtonActive = false;
  }
}

void SystemReadyFlash(){
  //Flash LEDs to indicate system readines
  digitalWrite(LowLED,LOW);
  delay(333);
  digitalWrite(MedLED,LOW);
  delay(333);
  digitalWrite(HighLED,LOW);
  delay(333);
  Off();

  digitalWrite(CoolLED,LOW);
  delay(333);
  digitalWrite(HeatBackLED,LOW);
  delay(333);
  digitalWrite(HeatBottomLED,LOW);
  delay(333);
  Off();
}

void SetMode(String NewMode){
  Level = Level - 1;  
  if (NewMode!= "Off" && (Level<0 || State != NewMode)){Level=3;};

  Serial.println(NewMode);
  Serial.print(Level);

  switch (Level){
    case 0:
      NewMode = "Off";
      BackFanSpeed = 0;
      BottomDesiredTemp = 0;
      BackDesiredTemp = 0;
      break;
    case 1:
      BottomDesiredTemp = DesiredBottomTempLow;
      BackDesiredTemp = DesiredBackTempLow;
      BackFanSpeed = BackFanSpeedLow;
      BottomFanSpeed = BottomFanSpeedLow;
      digitalWrite(LowLED,LOW);
      digitalWrite(MedLED,HIGH);
      digitalWrite(HighLED,HIGH);
	    break;
    case 2:
      BottomDesiredTemp = DesiredBottomTempMedium;
      BackDesiredTemp = DesiredBackTempMedium;
      BackFanSpeed = BackFanSpeedMed;
      BottomFanSpeed = BottomFanSpeedMed;
      digitalWrite(LowLED,LOW);
      digitalWrite(MedLED,LOW);
      digitalWrite(HighLED,HIGH);
      break;
    case 3:
      BottomDesiredTemp = DesiredBottomTempHigh;
      BackDesiredTemp = DesiredBackTempHigh;
      BackFanSpeed = BackFanSpeedHigh;
      BottomFanSpeed = BottomFanSpeedHigh;
      digitalWrite(LowLED,LOW);
      digitalWrite(MedLED,LOW);
      digitalWrite(HighLED,LOW);
      break;
  };

  if (NewMode == "Cool"){
    HeatBack = false;
    HeatBottom = false;
    digitalWrite(HeatBottomRelay,HIGH);
    digitalWrite(HeatBottomLED,HIGH);
    
    digitalWrite(HeatBackRelay,HIGH);
    digitalWrite(HeatBackLED,HIGH);

    //Cool Actions

    digitalWrite(CoolLED,LOW);
    digitalWrite(CoolRelay,LOW);

    analogWrite(FanBack,255-BackFanSpeed);
    MovePot(BottomFanSpeed);
    

  } else if (NewMode == "HeatBack"){
    //Make sure cooling and bottom heating is disabled
    digitalWrite(CoolLED,HIGH);
    digitalWrite(CoolRelay,HIGH);
    analogWrite(FanBack,0);
    MovePot(0);

    HeatBottom = false;
    digitalWrite(HeatBottomLED,HIGH);
    digitalWrite(HeatBottomRelay,HIGH);

    //Heat Actions
    HeatBack = true;
    digitalWrite(HeatBackRelay,LOW);
    digitalWrite(HeatBackLED,LOW);

  } else if (NewMode == "HeatBottom"){
    Serial.println(NewMode);
    //Make sure cooling is disabled
    digitalWrite(CoolLED,HIGH);
    digitalWrite(CoolRelay,HIGH);
    analogWrite(FanBack,0);
    MovePot(0);

    //Heat Actions
    HeatBack = true;
    HeatBottom = true;
    digitalWrite(HeatBottomLED,LOW);
    digitalWrite(HeatBottomRelay,LOW);
    digitalWrite(HeatBackLED,LOW);
    digitalWrite(HeatBackRelay,LOW);

  } else if (NewMode == "Off"){
    Off();
  };
  
  State = NewMode;
}

void Off(){
  //Serial.println("OFF");
  HeatBack = false;
  HeatBottom = false;
  Level=0;
  State="";

  analogWrite(FanBack,255);
  MovePot(0);

  digitalWrite(CoolRelay,HIGH);
  digitalWrite(HeatBottomRelay,HIGH);
  digitalWrite(HeatBackRelay,HIGH);

  digitalWrite(CoolLED,HIGH);
  digitalWrite(HeatBottomLED,HIGH);
  digitalWrite(HeatBackLED,HIGH);

  digitalWrite(LowLED,HIGH);
  digitalWrite(MedLED,HIGH);
  digitalWrite(HighLED,HIGH);
}

float AverageVoltage(float v1, float v2, float v3){
  float total = 0.00;
  float avg = 0.00;
  float voltage = 0.00;
  int length = 0;

  voltage = (v1+v2+v3)/3;
  
  length = sizeof(Voltage_Memory)/sizeof(Voltage_Memory[0]);

  for (int i = 0; i < (length-1); i++) {
    Voltage_Memory[i] = Voltage_Memory[i+1];
  }
  
  Voltage_Memory[length-1] = voltage;

  for (float voltEntry : Voltage_Memory){
    total = total+voltEntry;
  }

  avg = total/length;
  
  return avg;
}

float AverageTemp(float temp, String position){
  float total = 0.00;
  float avg = 0.00;
  int length = 0;
  
  if (position == "back"){
    length = sizeof(Temps_back)/sizeof(Temps_back[0]);

    for (int i = 0; i < (length-1); i++) {
      Temps_back[i] = Temps_back[i+1];
    }
    Temps_back[length-1] = temp;

    for (float tempEntry : Temps_back){
      total = total+tempEntry;
    }

    avg = total/length;
  } else {
    length = sizeof(Temps_bottom)/sizeof(Temps_bottom[0]);

    for (int i = 0; i < (length-1); i++) {
      Temps_bottom[i] = Temps_bottom[i+1];
    }
    Temps_bottom[length-1] = temp;

    for (float tempEntry : Temps_bottom){
      total = total+tempEntry;
    }
    
    avg = total/length;
  }
  
  return avg;
}

void loop() { 
  //If the interrupt on your board works, comment out the line below.
  InterruptButtonPoller();

  HeatBackPinValue = analogRead(HeatBackPin) * (5.0 / 1023.0);
  HeatBottomPinValue = analogRead(HeatBottomPin) * (5.0 / 1023.0);
  CoolPinValue = analogRead(CoolPin) * (5.0 / 1023.0);

  Serial.print("Back: ");
  Serial.println(HeatBackPinValue);
  Serial.print("Bottom: ");
  Serial.println(HeatBottomPinValue);
  Serial.print("Cool: ");
  Serial.println(CoolPinValue);  
  // Serial.print("IntPin: ");
  // Serial.println(ButtonInterruptValue);
  Serial.println("------------------------");

  //For controlling/testing via serial
  if (Serial.available()>2){
    String command = Serial.readString();
    //Serial.print(command);
    if (command=="back"){SetMode("HeatBack");}
    else if (command=="bottom"){SetMode("HeatBottom");}
    else if (command=="cool"){SetMode("Cool");}
    else if (command=="off"){SetMode("Off");};
  }


  if (HeatBack){
    Vo = analogRead(ThermistorPinBack);  
    T_Back = CalcTemp(Vo);

    float TempAvg = AverageTemp(T_Back,"back");
    Serial.print("BACK TEMP: ");
    Serial.print(TempAvg);
    Serial.println(" F");
    Serial.print("BACK Vo: ");
    Serial.print(Vo);
    Serial.println("v");
    Serial.println("---");

    if (!HeatBackSafety && (TempAvg > (BackDesiredTemp + BackTempMargin)) ){
      digitalWrite(HeatBackRelay,HIGH);
      //Serial.print("BK TEMP RCHD --- ");
      HeatBackSafety = true;
    } else if (HeatBackSafety && (TempAvg < (BackDesiredTemp - BackTempMargin)) ){ 
      //Serial.print("BK ON --- ");
      digitalWrite(HeatBackRelay,LOW);
      HeatBackSafety = false;
    }
  }

  if (HeatBottom){     
      Vo = analogRead(ThermistorPinBottom);
      T_Bottom = CalcTemp(Vo);

      float TempAvg = AverageTemp(T_Bottom,"bottom");
      Serial.print("BOTTOM TEMP: ");
      Serial.print(TempAvg);
      Serial.println(" F");
      Serial.print("BOTTOM Vo: ");
      Serial.print(Vo);
      Serial.println("v");
      Serial.println("---");

      if (!HeatBottomSafety && (TempAvg > (BottomDesiredTemp + BottomTempMargin)) ){
        digitalWrite(HeatBottomRelay,HIGH);
        HeatBottomSafety = true;
      } else if (HeatBottomSafety && (TempAvg < (BottomDesiredTemp - BottomTempMargin)) ){ 
        digitalWrite(HeatBottomRelay,LOW);
        HeatBottomSafety = false;
      }
  }

  delay(msDelay);
}

float CalcTemp(int Vo){
  float Temp;

  R2 = R1 * (1023.0 / (float)Vo - 1.0);
  logR2 = log(R2);
  Temp = (1.0 / (c1 + c2*logR2 + c3*logR2*logR2*logR2));
  Temp = Temp - 273.15;
  Temp = (Temp * 9.0)/ 5.0 + 32.0; 

  return Temp;
}

void PotInit(){
  int l;
  for (l=100;l>0;l--){
    PotDown();
    delay(5);
  }

  PotPosition = 0;
}

void PotUp(){
  digitalWrite(UDPin,HIGH);
  Increment();
  PotPosition++;
}

void PotDown(){
  digitalWrite(UDPin,LOW);
  Increment();
  PotPosition--;
}

void Increment() {
  digitalWrite(FanBottom,LOW);  
  delay(5);
  digitalWrite(FanBottom,HIGH);
  delay(5);
}

void MovePot(int newPos){
  if (newPos == 0){
    int l;
      for (l=100;l>0;l--){
        PotDown();
        delay(5);
      }
    PotPosition = 0;
  } else if (newPos > PotPosition){
      int mv = newPos - PotPosition;
      int l;
      Serial.println();
      Serial.print(PotPosition);
      Serial.print(" > UP > ");
      Serial.print(newPos);
      Serial.print(" - Steps Up: ");
      Serial.println(mv);
      Serial.println();
      for (l=mv;l>0;l--){
        PotUp();
        }
  } else if (newPos < PotPosition){
    int mv = PotPosition - newPos;
    int l;
    Serial.println();
    Serial.print(PotPosition);
    Serial.print(" > DWN > ");
    Serial.print(newPos);
    Serial.print(" - Steps Down: ");
    Serial.println(mv);
    Serial.println();
    for (l=mv;l>0;l--){
      PotDown();
      }
    }
  PotPosition = newPos;
}
