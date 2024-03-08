#include <Arduino.h>
#define led 1

// put function declarations here:
#define Limit_switchA  8
#define Limit_switchB 12

int M1_R_Pwm = 10;
int M1_L_Pwm = 9;
int R_IS = 8;
int L_IS = 7;
int R_EN = 6;
int L_EN = 5;

bool initialStart = false;
 uint8_t state = 0;
 uint8_t i = 0;

void Move_forward(void);
void Move_reverse(void);
void Stop();

void setup()
{
  // put your setup code here, to run once:
  pinMode(Limit_switchA, INPUT);
  pinMode(led, OUTPUT);
  Serial.begin(115200);

  pinMode(M1_R_Pwm, OUTPUT);
  pinMode(M1_L_Pwm, OUTPUT);
  pinMode(R_IS, OUTPUT);
  pinMode(L_IS, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);
  digitalWrite(R_IS, LOW);
  digitalWrite(L_IS, LOW);

  delay(100);
}

void loop()
{
  // put your main code here, to run repeatedly:

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


  Serial.println(state);
  delay(100);
}

void Stop()
{

  Serial.println("the trolley is currently stoped");
  analogWrite(M1_L_Pwm, 0);
  analogWrite(M1_R_Pwm, 0);
  delay(5000);
  state = 1;

}

void Move_forward()
{
  if (!initialStart)
  {
    for (int i = 0; i <= 255; i++)
    {
      analogWrite(M1_R_Pwm, i);
      analogWrite(M1_L_Pwm, 0);
      delay(30);
    }
    initialStart = true;
  }

  else
  {
    analogWrite(M1_R_Pwm, 150);

    if (digitalRead(Limit_switchA))
    {
      Serial.println("limit switch A Pressed");
      digitalWrite(led, analogRead(~led));

      analogWrite(M1_R_Pwm, 0);
      analogWrite(M1_R_Pwm, 0);
      delay(5000);
      initialStart = false;
      state = 2;
    }
  }
}

void Move_reverse()
{
  if (!initialStart)
  {
    for (int i = 0; i <= 255; i++)
    {
      analogWrite(M1_R_Pwm, 0);
      analogWrite(M1_L_Pwm, i);
      delay(30);
    }
    initialStart = true;
  }

  else
  {
    analogWrite(M1_L_Pwm, 150);

    if (digitalRead(Limit_switchB))
    {
      Serial.println("limit switch B Pressed");
      digitalWrite(led, analogRead(~led));

      analogWrite(M1_R_Pwm, 0);
      analogWrite(M1_R_Pwm, 0);
      delay(1000);
      initialStart = false;
      state = 0;
 
    }
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