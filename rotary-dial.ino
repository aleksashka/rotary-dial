// This is a simple project for my child's busybox

// Arduino is connected to a rotary dialer (DIAL_PIN) and is monitoring dialing
// by checking if circuit is closed or opened (with debounce delay). At each
// iteration of dialing the final digit all preceding digits are shown on an LED
// display.
// After digitTimeout digit is finalized and next dial is counted to the next
// digit.
// After textTimeout LCD is reset to it's initial state (digits are erased).
// After ledTimeout LED backlight of LCD is turned off.

#include <stdlib.h>
#include <LiquidCrystal.h>

#define LCD_LED    9
#define RGB_LED   11
#define LCD_BRIGHT 6
#define DIAL_PIN   A2  // Pin for normally opened rotary dialer

LiquidCrystal lcd(8,7,5,4,3,2);
int count = 0;            // Current digit
int pos = 0;              // Current position for LCD
char count_char[2];       // Char to print digit on LCD

bool reading = false;     // Current reading of state
bool lastRead = false;    // Previous reading
bool lcd_led_on = false;  // True if LCD has LED on
bool lcd_in_use = false;  // True if LCD has non-default text
long lastChangeTime = 0;  // Time when last change happened
int debounceDelay = 10;   // Minimum time between short circuits
int digitTimeout = 100;   // Minimum time between digits
int textTimeout = 2000;   // Time before deleting non-default text
int ledTimeout =  5000;   // Time before turning LED off

void setup()
{
  Serial.begin(9600);
  pinMode(DIAL_PIN, INPUT_PULLUP);
  pinMode(LCD_LED, OUTPUT);
  pinMode(RGB_LED, OUTPUT);
  pinMode(LCD_BRIGHT, OUTPUT);
  analogWrite(LCD_LED, 0);
  analogWrite(RGB_LED, 0);
  analogWrite(LCD_BRIGHT,100);
  lcd.begin(16, 2);
  reset_lcd();
}

void reset_lcd()
{
  lcd.clear();
  lcd.print("Max is the best!");
}

void loop()
{
  if ( lcd_led_on ) {
    // Bother only if LCD's LED is on
    if ( lcd_in_use ) {
      // Bother only if LCD has some non-default text
      if ( (millis() - lastChangeTime) > textTimeout) {
        // Idle timeout expired, so reset
        Serial.println("Text timeout expired");
        reset_lcd();
        pos = 0;
        lcd_in_use = false;
      }

      if ( (millis() - lastChangeTime) > digitTimeout) {
        // interDigitTimeout has passed
        if ( count ) {
          // Some number was dialed, so change position
          Serial.println("done");
          count = 0;                // Reset digit
          pos++;                    // Move to the right
          if ( pos > 15 ) {         // Carriage return if number is too long
            pos = 0;
          }
          lcd.setCursor(pos,1);
          lcd.print(" ");
        }
      }
    } else if ( (millis() - lastChangeTime) > ledTimeout) {
      // if LCD LED timeout expired, then turn LED off
      Serial.println("LED timeout expired");
      lcd_led_on = false;
      analogWrite(LCD_LED,0);
    }
  }

  if ( (millis() - lastChangeTime) > debounceDelay) {
    // Read values only after debounceDelay expires
    reading = !digitalRead(DIAL_PIN);     // Invert reading
    if ( reading != lastRead ) {          // Something has changed
      lastChangeTime = millis();          // Keep track of time
      lastRead = reading;                 // Keep track of readings
      if ( reading ) {                    // If circuit has closed
        if ( !lcd_in_use ) {
          lcd_in_use = lcd_led_on = true;
          analogWrite(LCD_LED,255);
        }
        if ( !count ) {                   // Dialing has just started (0)
          lcd.setCursor(pos,1);
          lcd.print("1");
        }
        analogWrite(RGB_LED, 150);
      } else {                            // If circuit has opened
        analogWrite(RGB_LED,0);
        count++;                          // Increment digit
        itoa(count % 10, count_char, 10); // Last digit of integer to char
        lcd.setCursor(pos,1);
        lcd.print(count_char);
        Serial.print(count % 10, DEC);
        Serial.print(" ");
      }
    }
  }
}
