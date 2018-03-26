#include <stdlib.h>
#include <LiquidCrystal.h>

#define LCD_POWER  7
#define LCD_BRIGHT 6
#define DIAL_PIN   A2  // Pin for normally opened rotary dialer

LiquidCrystal lcd(12,10,5,4,3,2);
int count = 0;               // Current digit
int pos = 0;                 // Current position for LCD
char count_char[2];          // Char to print digit on LCD

bool reading = false;        // Current reading of state
bool lastRead = false;       // Previous reading
bool lcd_on = false;         // True if LCD has any digits
long lastChangeTime = 0;     // Time when last change happened
int debounceDelay = 10;      // Minimum time between short circuits
int interDigitTimeout = 100; // Minimum time between digits
int idleTimeout = 5000;      // Time before turning LCD off

void setup()
{
  Serial.begin(9600);
  pinMode(DIAL_PIN, INPUT_PULLUP);
  pinMode(LCD_POWER, OUTPUT);
  pinMode(LCD_BRIGHT, OUTPUT);
  digitalWrite(LCD_POWER, HIGH);
  analogWrite(LCD_BRIGHT,30);
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
  if ( lcd_on ) {
    // Bother only if lcd is on
    if ( (millis() - lastChangeTime) > idleTimeout) {
      // Idle timeout expired, so reset
      Serial.println("Idle timeout expired");
      reset_lcd();
      pos = 0;
      lcd_on = false;
    }

    if ( (millis() - lastChangeTime) > interDigitTimeout) {
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
  }

  if ( (millis() - lastChangeTime) > debounceDelay) {
    // Read values only after debounceDelay expires
    reading = !digitalRead(DIAL_PIN);     // Invert reading
    if ( reading != lastRead ) {          // Something has changed
      lastChangeTime = millis();          // Keep track of time
      lastRead = reading;                 // Keep track of readings
      if ( reading ) {                    // If short circuit
        lcd_on = true;
        count++;                          // Increment digit
        itoa(count % 10, count_char, 10); // Integer to char
        lcd.setCursor(pos,1);
        lcd.print(count_char);
        Serial.print(count % 10, DEC);
        Serial.print(" ");
      }
    }
  }
}
