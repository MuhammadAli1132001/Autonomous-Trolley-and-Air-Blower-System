#include <Arduino.h>
#define led 1

// put function declarations here:
#define Limit_switchA  2
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

void Move_forward(void);
void Move_reverse(void);
void Stop();
void M1_control();
void M2_control();

void setup()
{

  pinMode(Limit_switchA, INPUT);
  pinMode(led, OUTPUT);
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

  attachInterrupt(digitalPinToInterrupt(Limit_switchA), M1_control, RISING);
  attachInterrupt(digitalPinToInterrupt(Limit_switchB), M2_control, RISING);

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


  Serial.println(state);
  delay(10);
}

void M1_control()
{
    
    Serial.println("limit switch A Pressed");
    digitalWrite(led, analogRead(~led));

    analogWrite(M1_R_Pwm, 0);
    analogWrite(M1_L_Pwm, 0);
    analogWrite(M2_R_Pwm, 0);
    analogWrite(M2_L_Pwm, 0);
    delay(2000);
    initialStart = false;
    state = 2;
    
}


void M2_control()
{
    int delays = 0;

    Serial.println("limit switch B Pressed");
    digitalWrite(led, analogRead(~led));

    analogWrite(M1_R_Pwm, 0);
    analogWrite(M1_L_Pwm, 0);
    analogWrite(M2_R_Pwm, 0);
    analogWrite(M2_L_Pwm, 0);
    delay(1000);

    Serial.println("exhaust fan ruining");

    while (delays != 10000)
    {    
    analogWrite(M2_L_Pwm, 0);
    analogWrite(M2_R_Pwm, 255);
    delay(20);
    delays = delays + 1;
    Serial.println(delays);

    }

    initialStart = false;
    state = 0;
 
    
}
void Stop()
{

  Serial.println("the trolley is currently stoped");
  analogWrite(M1_L_Pwm, 0);
  analogWrite(M1_R_Pwm, 0);
  analogWrite(M2_L_Pwm, 0);
  analogWrite(M2_R_Pwm, 0);

  delay(10000);
  state = 1;

}

void Move_forward()
{
  if (!initialStart)
  {
    for (int i = 0; i <= 255; i++)
    {      
      analogWrite(M2_R_Pwm, i); 
      analogWrite(M1_L_Pwm, 0); 
      analogWrite(M1_R_Pwm, i);
      analogWrite(M2_L_Pwm, 0);
      delay(20);
    }
    initialStart = true;
  }

  else
  {
    analogWrite(M2_R_Pwm, 150);
    analogWrite(M1_R_Pwm, 150);

  }
}

void Move_reverse()
{
  if (!initialStart)
  {
    for (int i = 0; i <= 255; i++)
    {      
      analogWrite(M2_L_Pwm, i);
      analogWrite(M1_R_Pwm, 0);
      analogWrite(M1_L_Pwm, i);
      analogWrite(M2_R_Pwm, 0);
      delay(30);
    }
    initialStart = true;
  }

  else
  {
    analogWrite(M1_L_Pwm, 150);
    analogWrite(M2_L_Pwm, 150);

    // if (digitalRead(Limit_switchB))
    // {
    //   Serial.println("limit switch B Pressed");
    //   digitalWrite(led, analogRead(~led));

    //   analogWrite(M1_R_Pwm, 0);
    //   analogWrite(M1_R_Pwm, 0);
    //   delay(1000);
    //   initialStart = false;
    //   state = 0;
 
    // }
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