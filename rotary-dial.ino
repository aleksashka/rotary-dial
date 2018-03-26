#include <stdlib.h>
#include <LiquidCrystal.h>

#define LCD_POWER  7
#define LCD_BRIGHT 6
#define DIAL_PIN   A2

LiquidCrystal lcd(12,10,5,4,3,2);
int count_int = 0;
uint8_t count_dec;
int pos = 0;
char count_char[2];

void setup()
{
  Serial.begin(9600);
  pinMode(DIAL_PIN, INPUT_PULLUP);
  pinMode(LCD_POWER, OUTPUT);
  pinMode(LCD_BRIGHT, OUTPUT);
  digitalWrite(LCD_POWER, HIGH);
  analogWrite(LCD_BRIGHT,30);
  lcd.begin(16, 2);
  //lcd.print("hello, world!");
  //lcd.setCursor(0,1);
}

void print_num()
{
  lcd.setCursor(pos,0);
  lcd.print(count_char);
}

void loop()
{
  count_dec = count_int % 10;
  itoa(count_dec, count_char, 10);
  
  //Serial.print("count_int = ");
  //Serial.println(count_int);
  //Serial.print("count_char = ");
  //Serial.println(count_char);
  
  print_num();
  if (!digitalRead(DIAL_PIN)) {count_int++;}
  pos++;
  if (pos>15) {pos = 0;}
  delay(200);
}
