
#include <LiquidCrystal.h>
#include <string.h>
#include <stdlib.h>

#define MENU_OPT_NR 4
#define MENIU 0

#define MESAJE 1
#define MESAJE_NECITITE 10
#define MESAJE_CITITE 11
#define MESAJE_STERGERE 12

#define CONTROL 2
#define CONTROL_MANUAL 20
#define CONTROL_AUTOMAT 21

#define TEMPERATURA 3
#define INUNDATII 4
#define LOCKED 999

//led
int red_light_pin = 7;
int green_light_pin = 5;
int blue_light_pin = 3;

unsigned long long lockTime = 0;

char hexstring[50];
char hexnr[2];

//LCD pin to Arduino
const int pin_RS = 8;
const int pin_EN = 9;
const int pin_d4 = 4;
const int pin_d5 = 5;
const int pin_d6 = 6;
const int pin_d7 = 7;
const int pin_BL = 10;
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

int stage = LOCKED;
int lastStage = LOCKED;
int arrow = 0;
int selected = 0;
int showMenuThisTime = 1; //Used to display the new menu only once after RIGHT or LEFT is pressed
int password[4], counter = 0;


String options[4] = {"1. Mesaje", "2. Control", "3. Temperatura", "4. Inundatii"};
String options1[3] = {"1.1 Necitite", "1.2 Citite", "1.3 Stergere"};
String options2[2] = {"2.1 Manual", "2.2 Automat"};
String options2Maual[2] = {"Setati culoarea", "prin serial"};
String options2Automat[2] = {"Mod automat", "selectat"};
String options3[2] = {"3.1 Temperatura", "curenta"};

// arrow right
byte R_arrow[8] = {B00000, B00100, B00010, B11111, B00010, B00100, B00000, B00000};
byte Lock_A[8] = {B00000, B00000, B00001, B00001, B00011, B00011, B00011, B00011};
byte Lock_B[8] = {B01110, B11111, B10001, B00000, B00000, B00000, B00000, B00000};
byte Lock_C[8] = {B00000, B00000, B10000, B10000, B11000, B11000, B11000, B11000};
byte Lock_D[8] = {B11111, B11111, B11111, B11111, B11111, B11111, B11111, B11111};
byte Lock_E[8] = {B11111, B11111, B11011, B11011, B11011, B11111, B11111, B11111};


//headers
void showMenu(String options[]);
int buttonPressed(String options[], int sizeOfMenu);
void RGB_color(long int red_light_value, int green_light_value, int blue_light_value);

/////////////////////////////////////////////////////////
//       SETUP                                         //
/////////////////////////////////////////////////////////
void setup() {
  lcd.createChar(1, R_arrow);  //create arrow char for lcd
  lcd.createChar(2, Lock_A);
  lcd.createChar(3, Lock_B);
  lcd.createChar(4, Lock_C);
  lcd.createChar(5, Lock_D);
  lcd.createChar(6, Lock_E);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  //RGB LED setup
  pinMode(red_light_pin, OUTPUT);
  pinMode(green_light_pin, OUTPUT);
  pinMode(blue_light_pin, OUTPUT);

  Serial.begin(9600);
  while (!Serial);

  analogWrite(red_light_pin, 0);
  analogWrite(green_light_pin, 0);
  analogWrite(blue_light_pin, 0);
}

/////////////////////////////////////////////////////////
//       LOOP                                          //
/////////////////////////////////////////////////////////
void loop() {
  
  if (millis() - lockTime > 50000 && stage != LOCKED)   //Lock screen if no button pressed for a while
  {
    lcd.clear();
    stage = LOCKED;
  }
  switch (stage)
  {
    default:
      {
        stage = stage / 10;
        break;
      }
    case MENIU:
      {
        if (showMenuThisTime == 1)
        {
          showMenu(options);
          showMenuThisTime = 0;
        }
        buttonPressed(options, 4);
        break;
      }
    case MESAJE:
      {
        if (showMenuThisTime == 1)
        {
          showMenu(options1);
          showMenuThisTime = 0;
        }
        buttonPressed(options1, 3);
        break;
      }
    case CONTROL:
      {
        if (showMenuThisTime == 1)
        {
          showMenu(options2);
          showMenuThisTime = 0;
        }
        buttonPressed(options2, 2);

        break;
      }
    case CONTROL_MANUAL:
      {
        if (showMenuThisTime == 1)
        {
          showMenu(options2Maual);
          showMenuThisTime = 0;
        }
        buttonPressed(options2Maual, 2);

        if (Serial.available())
        {
          String color;
          color = Serial.readString();
          if (color)
          {
            color.toCharArray(hexstring, 50);
            long int number = strtol(&hexstring[0], NULL, 16);
            RGB_color(((number >> 16) & 255), ((number >> 8) & 255), number & 255);
            Serial.print(color);
          }
        }

        break;
      }

    case CONTROL_AUTOMAT:
      {
        if (showMenuThisTime == 1)
        {
          showMenu(options2Automat);
          showMenuThisTime = 0;
        }
        buttonPressed(options2Automat, 2);

        break;
      }



    case TEMPERATURA:
      {
        if (showMenuThisTime == 1)
        {
          showMenu(options3);
          showMenuThisTime = 0;
        }
        buttonPressed(options3, 2);

        break;
      }
    case INUNDATII:
      {
        break;
      }

    case LOCKED:
      {
        int x;
        x = analogRead (0);


        lcd.setCursor(0, 1);
        lcd.print(millis() / 1000 / 60);
        lcd.print(":");
        lcd.print(millis() / 1000 % 60 / 10);
        lcd.print(millis() / 1000 % 60 % 10);

        lcd.setCursor(12, 0);
        lcd.write(2); lcd.write(3); lcd.write(4);
        lcd.setCursor(12, 1);
        lcd.write(5); lcd.write(6); lcd.write(5);

        if (x < 60) {
          password[counter] = 1;  counter++; delay(500); lockTime = millis();
        }
        else if (x < 200) {
          password[counter] = 2;  counter++; delay(500); lockTime = millis();
        }
        else if (x < 400) {
          password[counter] = 3;  counter++; delay(500); lockTime = millis();
        }
        else if (x < 600) {
          password[counter] = 4;  counter++; delay(500); lockTime = millis();
        }

        lcd.setCursor(8, 1);

        for (int i = 0; i < counter; i++)
        {
          lcd.print("*");
        }

        //Daca se ajunge la 4 elemente
        if (counter == 4) {
          //Daca cele 4 elemente reprezinta parola
          if (password[0] == 1 && password[1] == 1 && password[2] == 1 && password[3] == 1 )
          {
            showMenuThisTime = 1;
            stage = MENIU;
            password[0] == 0; password[1] == 0; password[2] == 0; password[3] == 0;
            counter = 0;
            delay(100);
            break;
          }
          //Daca cele 4 elemente NU reprezinta parola
          else
          {
            password[0] == 0; password[1] == 0; password[2] == 0; password[3] == 0;
            counter = 0;
            delay(100);
            lcd.clear();
          }
        }
        break;
      }
  }
}


/////////////////////////////////////////////////////////
//       FUNCTIONS                                     //
/////////////////////////////////////////////////////////

int buttonPressed(String options[], int sizeOfMenu) {
  int x;
  x = analogRead (0);

  if (x < 60)//Right
  {
    lockTime = millis();
    if (stage == MENIU)
      stage = selected + 1;
    else
      stage = stage * 10 + selected;

    showMenuThisTime = 1;
    selected = 0;
    arrow = 0;

    return 1;
  }
  else if (x < 200)//Up
  {
    lockTime = millis();
    arrow = 0;
    if (selected > 0)
      selected--;
    showMenu(options);
    return 2;
  }
  else if (x < 400)//Down
  {
    lockTime = millis();
    arrow = 1;
    if (selected < sizeOfMenu - 1)
      selected++;
    showMenu(options);
    return 3;
  }
  else if (x < 600)//Left
  {
    lockTime = millis();
    if (stage < 10)
      stage = MENIU;
    else
      stage = stage / 10;

    showMenuThisTime = 1;
    selected = 0;
    arrow = 0;
    return 4;
  }
  else if (x < 800)//Select
  {
    lockTime = millis();
    return 5;
  }
  return 0;
}

void showMenu(String options[])
{
  lcd.clear();
  lcd.setCursor(0, arrow);
  lcd.write(1);

  lcd.setCursor(1, 0);
  lcd.print(options[selected - arrow]);
  lcd.setCursor(1, 1);
  lcd.print(options[selected + 1 - arrow]);
  delay(500);
}

void RGB_color(long int red_light_value, int green_light_value, int blue_light_value)
{
  analogWrite(red_light_pin, red_light_value);
  analogWrite(green_light_pin, green_light_value);
  analogWrite(blue_light_pin, blue_light_value);
}
