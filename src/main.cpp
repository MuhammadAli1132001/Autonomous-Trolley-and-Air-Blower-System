/* Air Blower System
Design an industrial usage Mover trolley and Air bolwer system, implement two motors for trolley moving and for Air blower/exhaust.
The trolley autonomously move from and to two parking positions which is stoped by the limit switches at each end of the parking.
I have used BTS7960 for motors controlling in term of forward/reverse direction, while
pulse width modulation(PWM) is programmatically configured for variable motor speeds. A reset push buton ensures restarting of the whole process.*/

#include <Arduino.h>
#include <LiquidCrystal.h>

#define Push_Enable 11
#define Limit_switchA 2
#define Limit_switchB 3
#define RotatarySwitch 4
#define RotataryA 7
#define RotataryB 8

int MoverMotorRPM = 0;                   //Mover motor M1 RPM
int FanExhaustMotorRPM = 0;             // Fan / Exhaust motor M2 RPM
int RotataryState, alaststate, astate, bstate;
int RotataryStateLastPress = 0;

int rs = A0, e = A1, d1 = A2, d2 = A3, d3 = A4, d4 = A5;
LiquidCrystal lcd(rs, e, d4, d3, d2, d1);

int M1_R_Pwm = 5;
int M1_L_Pwm = 6;
int M2_R_Pwm = 10;
int M2_L_Pwm = 9;

// int R_IS = 8;
// int L_IS = 7;
// int R_EN = 11;
// int L_EN = 12;

bool initialStart = false;
bool ReturnAfterExhaust = false;
bool ExhaustStart = false;
uint8_t state = 0;
uint8_t i = 0;

void Started(void);
void RotataryConfiguration(void);
void ShowRpmOnLcd(void);
void Move_forward(void);
void Move_reverse(void);
void Stop();
void DoExhaust(void);
void SwitchA_control();
void SwitchB_control();

void setup()
{

  pinMode(Push_Enable, INPUT);
  pinMode(Limit_switchA, INPUT);
  pinMode(Limit_switchB, INPUT);
  pinMode(RotatarySwitch, INPUT_PULLUP);
  pinMode(RotataryA, INPUT);
  pinMode(RotataryB, INPUT);

  Serial.begin(115200);
  lcd.begin(16,2);
  lcd.setCursor(0,0);
  lcd.print("Assalamolaikom");

  pinMode(M1_R_Pwm, OUTPUT);
  pinMode(M1_L_Pwm, OUTPUT);
  pinMode(M2_R_Pwm, OUTPUT);
  pinMode(M2_L_Pwm, OUTPUT);

  alaststate = digitalRead(RotataryA);
  // pinMode(R_IS, OUTPUT);
  // pinMode(L_IS, OUTPUT);
  // pinMode(R_EN, OUTPUT);
  // pinMode(L_EN, OUTPUT);

  // digitalWrite(R_EN, HIGH);
  // digitalWrite(L_EN, HIGH);
  // digitalWrite(R_IS, LOW);
  // digitalWrite(L_IS, LOW);

  attachInterrupt(digitalPinToInterrupt(Limit_switchA), SwitchA_control, RISING); // switch A ext interrupt
  attachInterrupt(digitalPinToInterrupt(Limit_switchB), SwitchB_control, RISING); // switch B ext interrupt

  delay(1000);
  lcd.clear();

}

void loop()
{
  if ((!digitalRead(RotatarySwitch) && millis() - RotataryStateLastPress > 0))
  {
    Serial.println("Rotatory switch pressed");
    RotataryState ++;
    if (RotataryState > 2)
    {
      RotataryState = 0;
      Serial.println("Not changing state");
      lcd.setCursor(12,1);
      lcd.print("  ");
      lcd.setCursor(12,0);
      lcd.print("  ");
    }
    RotataryStateLastPress = millis();
    delay(1);
  }
  delay(5);

  RotataryConfiguration();
  // switch (state)
  // {
  // case 1:
  //   Move_forward();
  //   break;

  // case 2:
  //   Move_reverse();
  //   break;

  // default:
  //   Stop();
  //   break;
  // }

  // Started();
  // Serial.print("In state ");
  // Serial.println(state);
  ShowRpmOnLcd();
}


void Started()
{
  if (!digitalRead(Push_Enable) && state == 0)
  {
    MoverMotorRPM = 120;
    FanExhaustMotorRPM = 200;
    Serial.println("Enabled button is Pressed");
    state = 1;
    digitalWrite(Push_Enable, HIGH);
    delay(20);
  }
}

void RotataryConfiguration()
{

  switch (RotataryState)
  {
  case 1:
    lcd.setCursor(12,1);
    lcd.print("  ");
    lcd.setCursor(12,0);
    lcd.print("<-");
    astate = digitalRead(RotataryA);
    if (astate != alaststate)
    {
      if (digitalRead(RotataryB) != astate)
      {
        MoverMotorRPM = MoverMotorRPM + 1;
      }
      else
      {
        MoverMotorRPM = MoverMotorRPM - 1;
      }
      Serial.print("Mover Motor RPM: ");
      Serial.print(MoverMotorRPM);
      Serial.println();
    }
    alaststate = astate;
    break;

  case 2:
    lcd.setCursor(12,0);
    lcd.print("  ");
    lcd.setCursor(12,1);
    lcd.print("<-");
    astate = digitalRead(RotataryA);
    if (astate != alaststate)
    {
      if (digitalRead(RotataryB) != astate)
      {
        FanExhaustMotorRPM = FanExhaustMotorRPM + 1;
      }
      else
      {
        FanExhaustMotorRPM = FanExhaustMotorRPM - 1;
      }
      Serial.print("Fan/Exhaust Motor RPM: ");
      Serial.print(MoverMotorRPM);
      Serial.println();
    }
    alaststate = astate;
  
  default:
    break;
  }

}
void ShowRpmOnLcd()
{
  lcd.setCursor(0,0);
  lcd.print("M1 RPM: ");
  lcd.print(MoverMotorRPM);
  lcd.setCursor(0,1);
  lcd.print("M2 RPM: ");
  lcd.print(FanExhaustMotorRPM);
}
void SwitchA_control() // ISR for limit A pressed
{

  Serial.println("limit switch A Pressed");

  analogWrite(M1_R_Pwm, 0);
  analogWrite(M1_L_Pwm, 0);
  analogWrite(M2_R_Pwm, 0);
  analogWrite(M2_L_Pwm, 0);
  initialStart = false;
  state = 0;
}

void SwitchB_control() // ISR for limit B pressing
{
  Serial.println("limit switch B Pressed");

  analogWrite(M1_R_Pwm, 0);
  analogWrite(M1_L_Pwm, 0);
  analogWrite(M2_R_Pwm, 0);
  analogWrite(M2_L_Pwm, 0);
  delay(100);

  ExhaustStart = true;
  ReturnAfterExhaust = true;
  initialStart = false;
  state = 2;
}

void Stop() // default and after reverse direction state
{

  Serial.println("the trolley is currently stoped");
  analogWrite(M1_L_Pwm, 0);
  analogWrite(M1_R_Pwm, 0);
  analogWrite(M2_L_Pwm, 0);
  analogWrite(M2_R_Pwm, 0);
  ReturnAfterExhaust = false;
  delay(500);
  // state = 1;
}

void DoExhaust()
{
  int delays = 0;

  while (delays != 1000) // moving M2 with full speed in opposite direction for Exhaust, upto 10 seconds
  {
    analogWrite(M2_L_Pwm, 0);
    analogWrite(M2_R_Pwm, FanExhaustMotorRPM);
    delay(10);
    delays = delays + 1;
    Serial.println(delays);
  }
  ReturnAfterExhaust = false;
}

void Move_forward()                             // moving the trolley forward with air blower as well
{

  if (!initialStart)                           //  smooth startup
  {
    for (int i = 0; i <= MoverMotorRPM; i++)
    {
      analogWrite(M1_R_Pwm, 0);
      analogWrite(M1_L_Pwm, i);
      analogWrite(M2_R_Pwm, 0);
      analogWrite(M2_L_Pwm, i);
      delay(20);
    }
    initialStart = true;
  }

  else
  {
    analogWrite(M2_L_Pwm, MoverMotorRPM);           //constant speed
    analogWrite(M1_L_Pwm, MoverMotorRPM);
  }
}


void Move_reverse()                               // moving the trolley in reverse direction with air blower
{
  if (ReturnAfterExhaust)                        // firstly checking to run exhaust 
  {
    delay(5000);
    Serial.println("exhaust fan ruining");
    DoExhaust();
    Serial.println("Stopping after exhaust upto 5 second");
    // analogWrite(M2_R_Pwm, 100);
    // analogWrite(M2_L_Pwm, 0);
    // delay(1000);
    analogWrite(M2_R_Pwm, 50);                    // gradually stoping
    analogWrite(M2_L_Pwm, 0);
    delay(1000);
    analogWrite(M2_R_Pwm, 20);
    analogWrite(M2_L_Pwm, 0);
    delay(1000);
    analogWrite(M2_R_Pwm, 10);
    analogWrite(M2_L_Pwm, 0);
    delay(1000);
    analogWrite(M2_R_Pwm, 0);
    analogWrite(M2_L_Pwm, 0);
    delay(5000);
    ReturnAfterExhaust = false;
    initialStart = false;
    state = 2;
  }

  else                                       //and then reverse direction toward parking
  {

    if (!initialStart)
    {
      for (int i = 0; i <= MoverMotorRPM; i++)
      {
        analogWrite(M1_R_Pwm, i);
        analogWrite(M1_L_Pwm, 0);
        analogWrite(M2_R_Pwm, i);
        analogWrite(M2_L_Pwm, 0);
        delay(20);
      }
      initialStart = true;
    }

    else
    {
      analogWrite(M1_R_Pwm, MoverMotorRPM);
      analogWrite(M2_R_Pwm, MoverMotorRPM);
    }
  }
}
