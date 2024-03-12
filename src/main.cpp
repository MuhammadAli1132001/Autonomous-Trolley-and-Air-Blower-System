/* Air Blower System
Design an industrial usage Mover trolley and Air bolwer system, implement two motors for trolley moving and for Air blower/exhaust.
The trolley autonomously move from and to two parking positions which is stoped by the limit switches at each end of the parking.
I have used BTS7960 for motors controlling in term of forward/reverse direction, while
pulse width modulation(PWM) is programmatically configured for variable motor speeds. A reset push buton ensures restarting of the whole process.*/

#include <Arduino.h>

#define Push_Enable 11
#define Limit_switchA 2
#define Limit_switchB 3

int M1_R_Pwm = 5;
int M1_L_Pwm = 6;
int M2_R_Pwm = 10;
int M2_L_Pwm = 9;

int R_IS = 8;
int L_IS = 7;
int R_EN = 11;
int L_EN = 12;

bool initialStart = false;
uint8_t state = 0;
uint8_t i = 0;

void Started(void);
void Move_forward(void);
void Move_reverse(void);
void Stop();
void SwitchA_control();
void SwitchB_control();

void setup()
{

  pinMode(Push_Enable, INPUT);
  pinMode(Limit_switchA, INPUT);
  pinMode(Limit_switchB, INPUT);

  Serial.begin(115200);

  pinMode(M1_R_Pwm, OUTPUT);
  pinMode(M1_L_Pwm, OUTPUT);
  pinMode(M2_R_Pwm, OUTPUT);
  pinMode(M2_L_Pwm, OUTPUT);

  pinMode(R_IS, OUTPUT);
  pinMode(L_IS, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);
  digitalWrite(R_IS, LOW);
  digitalWrite(L_IS, LOW);

  attachInterrupt(digitalPinToInterrupt(Limit_switchA), SwitchA_control, RISING); // switch A ext interrupt
  attachInterrupt(digitalPinToInterrupt(Limit_switchB), SwitchB_control, RISING); // switch B ext interrupt

  delay(100);
}

void loop()
{

  switch (state)
  {
  case 1:
    Move_forward();
    break;

  case 2:
    Move_reverse();
    break;

  default:
    Stop();
    break;
  }

  Started();
  Serial.print("state ");
  Serial.println(state);
  delay(10);
}

void Started()
{
  if (!digitalRead(Push_Enable))
  {
    Serial.println("Enabled is Pressed");
    state = 1;
    digitalWrite(Push_Enable, HIGH);
    delay(20);
  }
}

void SwitchA_control()                               // ISR for limit A pressed
{

  Serial.println("limit switch A Pressed");

  analogWrite(M1_R_Pwm, 0);
  analogWrite(M1_L_Pwm, 0);
  analogWrite(M2_R_Pwm, 0);
  analogWrite(M2_L_Pwm, 0);
  initialStart = false;
  state = 0;
}

void SwitchB_control()                                 // ISR for limit B pressing
{
  int delays = 0;

  Serial.println("limit switch B Pressed");

  analogWrite(M1_R_Pwm, 0);
  analogWrite(M1_L_Pwm, 0);
  analogWrite(M2_R_Pwm, 0);
  analogWrite(M2_L_Pwm, 0);
  delay(1000);

  Serial.println("exhaust fan ruining");
  delay(300);

  while (delays <= 5000)                               // moving M2 with full speed in opposite direction for Exhaust, upto 10 seconds
  {
    analogWrite(M2_L_Pwm, 0);
    analogWrite(M2_R_Pwm, 255);
    delay(20);
    delays = delays + 1;
    Serial.println(delays);
  }
  delay(5000);
  initialStart = false;
  state = 2;
}

void Stop()                                              // default and after reverse direction state
{

  Serial.println("the trolley is currently stoped");
  analogWrite(M1_L_Pwm, 0);
  analogWrite(M1_R_Pwm, 0);
  analogWrite(M2_L_Pwm, 0);
  analogWrite(M2_R_Pwm, 0);

  delay(500);
  // state = 1;
}

void Move_forward()                                    // moving the trolley forward with air blower as well
{
  if (!initialStart)
  {
    for (int i = 0; i <= 150; i++)
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
    analogWrite(M2_L_Pwm, 125);
    analogWrite(M1_L_Pwm, 125);
  }
}

void Move_reverse()                              // moving the trolley in reverse direction with air blower
{
  if (!initialStart)

  {
    for (int i = 0; i <= 125; i++)
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
    analogWrite(M1_R_Pwm, 125);
    analogWrite(M2_R_Pwm, 125);
  }
}

// while (digitalRead(switch_button))
// {
//   analogWrite(M1_R_Pwm, i);
//   analogWrite(M1_L_Pwm, 0);
//   delay(50);

//   i = i + 1;

//   if (i >= 255)
//   {
//     i = 0;
//   }