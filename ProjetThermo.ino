#include <Wire.h>
#include <U8glib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <AM2320.h>
AM2320 th;

U8GLIB_SSD1306_128X32 u8g(U8G_I2C_OPT_NONE);  // I2C / TWI

#define VBATPIN A9

//DeviceAddress deviceAddress;
#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

byte Cptr = 0;
float measuredvbat;
int zob;
float tmp, hum;

void setup() {
  //Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  display.display();
  delay(1000);
}

void loop() {
  display.dim(true);
  if (Cptr == 10) {
    measuredvbat = analogRead(VBATPIN);
    measuredvbat *= 2;    // we divided by 2, so multiply back
    measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
    measuredvbat /= 1024; // convert to voltage
    Cptr = 0;
    zob = measuredvbat * 100;
    if (zob > 421) zob = 421; // Chargé à bloc
    if (zob < 356) zob = 355; // Déchargé...!
    zob = map(zob, 355, 421, 28, 1);
  }
  
  u8g.firstPage();
  do {    
    draw();
  } while(u8g.nextPage());
  delay(1000);
  th.Read();
  Cptr += 1;
}

void draw(void) {
  u8g.setFont(u8g_font_fub30n);
  
  tmp = th.t;
  hum = th.h;
  
  // Serial.print("Temp = "); Serial.println(tmp);
  
  if ((tmp < 10) and (tmp >= 0)) { // Si temp entre 0° et 9°;
    u8g.setPrintPos(30, 32);
  }
  else if ((tmp < 0) and (tmp > -10)) { // Si temp entre -9° et -1°;
    u8g.setPrintPos(20, 32);
  }
  else if (tmp <= -10) { // Si temp inférieure ou égale à -10°;
    u8g.setPrintPos(0, 32);
  }
  else {
    u8g.setPrintPos(10, 32);  // Si temp normale (sup à 9°);
  }
  if (Cptr > 7) {
    u8g.print(hum,1);
    u8g.drawCircle(99, 5, 3);
    u8g.drawCircle(99, 5, 2);
    u8g.drawLine(110, 2, 95, 20);
    u8g.drawCircle(106, 16, 3);
    u8g.drawCircle(106, 16, 2);
  }
  else {
    u8g.print(tmp,1);
    u8g.drawCircle(100, 5, 4);                // °
    u8g.drawCircle(100, 5, 3);                // °
  }
  u8g.drawFrame(118, 2, 10, 30);            // Corps accu
  u8g.drawFrame(120, 0, 6, 2);              // Téton accu
  u8g.setColorIndex(0);                     // Black pixel 
  u8g.drawBox(121, 1, 4, 2);                // Vidage téton pou' faire bien !
  u8g.setColorIndex(1);                     // White pixel
  u8g.drawBox(119, 2 + zob, 8, 29 - zob);   // Remplissage charge accu
  //Serial.print("zob = "); Serial.println(zob);
  //Serial.print("vbat = "); Serial.print(measuredvbat); Serial.println("v");

  if ((zob == 28) or (zob == 1)) {
    if (Cptr % 2 == 0) {
      u8g.drawBox(119, 3, 8, 28);
      u8g.setFont(u8g_font_profont10r);
      u8g.setPrintPos(91, 32);
      u8g.print(measuredvbat); u8g.print("v");
      }
    else {
      u8g.setColorIndex(0);
      u8g.drawBox(119, 3, 8, 28);
      u8g.setColorIndex(1);
      u8g.setFont(u8g_font_profont10r);
      u8g.setPrintPos(91, 32);
      u8g.print(measuredvbat); u8g.print("v");
    }
  }
}
