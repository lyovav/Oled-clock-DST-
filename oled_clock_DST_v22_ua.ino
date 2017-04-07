/*
 * 128X64 Blue OLED LCD LED Display 0.96" I2C
 * DS3231 AT24C32 IIC Module
 * 
 */

#include <OLED_I2C.h>         // Подключение библиотеки для дисплея
#include <Wire.h>
#include <Time.h>      // для часов
#include <TimeLib.h>
#include <Timezone.h>  //DST

//UA Time Zone (Ukraine, Kiev)
TimeChangeRule myDST = {"EDT", Last, Sun, Mar, 3, +60};    //Переход на летнее время
TimeChangeRule mySTD = {"EST", Last, Sun, Nov, 4, -60};     //Переход на зимнее время
Timezone myTZ(myDST, mySTD);

TimeChangeRule *tcr;        //pointer to the time change rule, use to get TZ abbrev
time_t utc, local;

#include <DS1307RTC.h>

const int vb=A0;
int volt=0;
float vbat=0;


OLED  myOLED(SDA, SCL, 8);    
extern uint8_t MegaNumbers[]; // Подключение больших шрифтов
extern uint8_t SmallFont[];   // Подключение маленьких шрифтов
extern uint8_t UkrFont[];     // Подключение укр. шрифтов



///// температура ..
float get3231Temp(){
  byte tMSB, tLSB; 
  float temp3231;

  Wire.beginTransmission(0x68);
  Wire.write(0x11);
  Wire.endTransmission();
  Wire.requestFrom(0x68, 2);

  if(Wire.available()) {
    tMSB = Wire.read(); //2's complement int portion
    tLSB = Wire.read(); //fraction portion

    temp3231 = (tMSB & B01111111); //do 2's math on Tmsb
    temp3231 += ( (tLSB >> 6) * 0.25 ); //only care about bits 7 & 8
  }
  else {
    //oh noes, no data!
  }

  return temp3231;
}
 
void setup() {
  myOLED.begin();
    Serial.begin(115200);

   // myTZ.writeRules(100);    //write rules to EEPROM address 100
    
    setSyncProvider(RTC.get);   // the function to get the time from the RTC
    if(timeStatus()!= timeSet) 
        Serial.println("Unable to sync with the RTC");
    else
        Serial.println("RTC has set the system time");  
}




 
void loop(){


     
    Serial.println();
    utc = now();
    local = myTZ.toLocal(utc, &tcr);


    
    
  tmElements_t tm;
 // (RTC.read(tm)) ; //читаем данные из часов
  myOLED.setBrightness(10); // устанавливаем яркость. от 0 до 255. При 0 свечение в половину яркости.
  myOLED.setFont(SmallFont); //Ставим маленький шрифт
  myOLED.setFont(UkrFont); //Используем укроязычный шрифт
 // myOLED.printNumI(tm.Wday, 16, 0);

//Serial.print((weekday()));
 Serial.print(day());
switch (weekday()) // В этой переменной содержится значение дня недели. Начиная с воскресенья
  {
    case 1:     myOLED.print("YTLSKZ", CENTER, 0);    break; //воскресенье
    case 2:     myOLED.print("GJYTLSKJR", CENTER, 0);    break; // Отображение дня недели, тут понедельник
    case 3:     myOLED.print("DSDNJHJR", CENTER, 0);        break; //вторник
    case 4:     myOLED.print("CTHTLF", CENTER, 0);          break; // итд
    case 5:     myOLED.print("XTNDTH", CENTER, 0);        break;
    case 6:     myOLED.print("G'ZNYBWZ", CENTER, 0);        break;
    case 7:     myOLED.print("CE<JNF", CENTER, 0);        break;
 
   }

  myOLED.printNumI(get3231Temp(), 5, 0);// выводим температуру микросхемы часов  

  
  myOLED.print("C", 19, 0);
  myOLED.setFont(MegaNumbers); //Подключаем большие шрифты для часов.
  if(hour() < 10)
  {
  myOLED.print("0", 4, 13); 
  myOLED.printNumI(hour(), 26, 13);
  }
  else
  {
    myOLED.printNumI(hour(), 4, 13);
  }
  myOLED.print("/", 51, 13);  // Отображение двоеточия
  if (minute()<10){                  //далее отображаем минуты, 
    myOLED.print("0", 75, 13);        //если минут менее 10, то отображаем первым 0
    myOLED.printNumI(minute(), 97, 13);
  }
  else {
  myOLED.printNumI(minute(), 75, 13);
  }

  myOLED.setFont(SmallFont);
  if (second()<10){                  //далее отображаем секунды, 
    myOLED.print("0", 114, 52);        //если секунд менее 10, то отображаем первым 0
    myOLED.printNumI(second(), 120, 52);
  }
  else {
  myOLED.printNumI(second(), 114, 52);
  }

  
  myOLED.setFont(SmallFont);
  if (day()<10){
    myOLED.print("0", 40, 57);
    myOLED.printNumI(day() , 46, 57);// Отображение даты 
  }
  else {
  myOLED.printNumI(day() , 40, 57); // Отображение даты
  }
 
  myOLED.print("." , 52, 57);// Отображение даты

  if (month()<10){
  myOLED.print("0" , 56, 57);// Отображение даты  
  myOLED.printNumI(month() , 62, 57);// Отображение даты  
  }
  else{
  myOLED.printNumI(month() , 56, 57);// Отображение даты
  }
  
  myOLED.print("." , 68, 57);// Отображение даты
  myOLED.printNumI(year(), 73, 57);// Отображение даты
  myOLED.update();
  delay(500);
  myOLED.setFont(MegaNumbers);                     // Скрытие двоеточия
  myOLED.print("-", 51, 12);
  myOLED.update();
  delay(500);

 myOLED.setFont(SmallFont);
volt=analogRead(vb);
vbat=volt/1024.0*5;
myOLED.print(String(vbat,2), 8, 57);// Отображение напряжения питания
myOLED.print("V", 0, 57);


} 




