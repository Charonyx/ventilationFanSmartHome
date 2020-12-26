#include <Wire.h>
#include <hd44780.h>
#include <hd44780_I2Cexp.h>
hd44780_I2Cexp lcd;
int  sec = 0, minutes = 0, hours = 0;
int state = 0;
int steps = 0;
int status;// , address;
const int LCD_COLS = 16;
const int LCD_ROWS = 2;
int i;
int ldrR = 0, prevldr = 0;
byte device_LCD = 0x25;//, data, x, motor = 0x26;
int fan = 2;
int ldr = 3;
int sw  = 4;
int led1 = 8;
int led2 = 9;
int ther = A1;

#define RT0 10000 // Ω
#define B 3977    // K
//--------------------------------------

#define VCC 5   //Supply voltage
#define R 100000 //R=100KΩ

//Variables
float RT, VR, ln, TX, T0, VRT;

unsigned long last_time = 0 , last_time1 = 0;
ISR(TIMER1_COMPA_vect)
{
  if ((ldrR == 1 || TX > 25.00) && state == 0) {
    state = 1;
  }
  if ((ldrR == 1 || TX <= 25.00) &&  state == 1) {
    state = 0;
  }
  if (digitalRead(sw) == 0 && state == 1) {
    state = 0;

  }
  //    if (state == 0)
  //    {
  //      state = 1; //ทวนเข็ม
  //    }
  //    else if (state == 1)
  //    {
  //      state = 2; //ตามเข็ม
  //    }
  //    else state = 1;
  //  }
  //
  //  if (digitalRead(11) == LOW)
  //  {
  //    state = 3;
  //  }
}
void setup()
{
  Wire.begin();
  pinMode(fan, OUTPUT);
  pinMode(ldr, INPUT);

  pinMode(sw, INPUT_PULLUP);
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  //pinMode(11, INPUT_PULLUP);
  T0 = 25 + 273.15; //Temperature T0 from datasheet, conversion from Celsius to kelvin


  //interrupt 120 Hz
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 15624; //129
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12) | (1 << CS10);
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
  Serial.begin(115200);
}

void loop()
{
  status = lcd.begin(LCD_COLS, LCD_ROWS);
  if (status) {
    hd44780::fatalError(status);
  }

  //////////
  VRT = analogRead(A1);//
  VRT = (5.00 / 1023.00) * VRT;
  VR = VCC - VRT;
  RT = VRT / (VR / R); //Resistance of RT

  ln = log(RT / RT0);
  TX = (1 / ((ln / B) + (1 / T0))); //Temp from thermistor

  TX = (TX - 273.15) + 38; //to C

  Serial.print("Temperature:");
  Serial.print("\t");
  Serial.println(TX);
  // Serial.print("C\t\t");
  // Serial.print(TX + 273.15); //Conversion to Kelvin
  // Serial.print("K\t\t");
  // Serial.print((TX * 1.8) + 32); //Conversion to Fahrenheit
  // Serial.println("F");



  ////////////////////////////////////////////////// CHECK LDR //////////////////////////////////////////////////
  ldrR = 0;
  ldrR = digitalRead(ldr);

  if (prevldr == 0 && ldrR == 1) {
    state = 1;
    //Serial.println(ldrR);
    digitalWrite(led2, HIGH); //1
    //delay(500);
  }
  else {
    digitalWrite(led2, LOW); //0
    //delay(500);
  }
  prevldr = ldrR;


  lcd.setCursor(4, 0);
  lcd.print(TX);
  if (state == 0) {
    lcd.setCursor(4, 1);
    lcd.print("TURN OFF");
  }
  else if (state == 1) {
    lcd.setCursor(4, 1);
    lcd.print("TURN ON");
  }

  Serial.println(state);
  if (state == 0) {
    digitalWrite(fan, 0);
    digitalWrite(led1, 0);
  }
  else if (state == 1) {
    digitalWrite(fan, 1);
    digitalWrite(led1, 1);
  }
  //  else if (state == 2) {
  //    Serial.print("eiei");
  //  }

}
