/* Comment this out to disable prints and save space */
// #define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "TMPL3achzzElE"
#define BLYNK_TEMPLATE_NAME "CoalMinersSafety"
#define BLYNK_AUTH_TOKEN "l2aUlyOv1nHOfKYLEhyM8Yk-Koz6MVyi"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <LiquidCrystal_I2C.h>
#define fb_key D5
#define cb_key D6
int lcdColumns = 16;
int lcdRows = 2;
//LiquidCrystal_I2C lcd(0x3F, lcdColumns, lcdRows);
 LiquidCrystal_I2C lcd(0x27, lcdColumns, lcdRows);
// Your WiFi credentials.
// Set password to "" for open networks.
char ssid[] ="Project"; // "ElectroEra"; //
char pass[] = "123456789"; //"mywifipassword";//

// char ssid[] = "ElectroEra"; //
// char pass[] = "mywifipassword";//
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(fb_key,INPUT_PULLUP);
  pinMode(cb_key,INPUT_PULLUP);
  
   lcd.init();
     lcd.backlight();
     lcd.clear();

     
   lcd.setCursor(0, 0);  // pos, line
   lcd.print("Connecting Net");
   lcd.setCursor(0,1);
   lcd.print(ssid);
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);
  lcd.setCursor(0, 1);  // pos, line
   lcd.print("Connected....");
   delay(3000);
   lcd.clear();
   lcd.print("Waiting...");
   

}
// 00000,32,63,10.11,12.13,972.90#
// Panic_alert,Methane_alert,CO_alert,On_Head_or_NO,Fell or Not, Temp, Hum, Presure, Me, CO
void loop() {
  // put your main code here, to run repeatedly:
 Blynk.run();
 if(digitalRead(fb_key)==LOW){
    Serial.println("11111#");
    lcd.clear();
    lcd.print("Panic feedBack ");
    lcd.setCursor(0,1);
    lcd.print("Sent");
    delay(1000);
 }
 if(digitalRead(cb_key)==LOW){
    Serial.println("22222#");
    lcd.clear();
    lcd.print("Panic CallBack ");
    lcd.setCursor(0,1);
    lcd.print("Sent");
    delay(1000);
 }
 if(Serial.available()){
  String instr=Serial.readStringUntil('#');

  if(instr[0]=='1'){
        Blynk.logEvent("alert", String("Panic_alert!"));// + temp);
        lcd.clear();lcd.print("Panic Alert..!!!!");
        delay(600);
  }
  
  if(instr[1]=='1'){
        Blynk.logEvent("alert", String("Methane_alert!"));// + temp);
        lcd.clear();lcd.print("Methane_Alert..!!!!");
        delay(600);
  }
    if(instr[2]=='1'){
        Blynk.logEvent("alert", String("CO2_alert!"));// + temp);
         lcd.clear();lcd.print("CO2_alert..!!!!");
         delay(600);
  }
  
   

  int idx_1=instr.indexOf(',');
  // Serial.println(instr);
  // Serial.print("idx_1 " + String(idx_1));
  int idx_2=instr.indexOf(',',idx_1+1);  
  // Serial.print("idx_2 " + String(idx_2));
  int idx_3=instr.indexOf(',',idx_2+1);  
  // Serial.print("idx_3 " + String(idx_3));
  int idx_4=instr.indexOf(',',idx_3+1);  
  // Serial.print("idx_4 " + String(idx_4));
  int idx_5=instr.indexOf(',',idx_4+1);  
  // Serial.print("idx_5 " + String(idx_5));
  int idx_6=instr.indexOf('#',idx_5+1);  
  // Serial.print("idx_6 " + String(idx_6));

  String Str_temp=instr.substring(idx_1+1, idx_2);
  // Serial.println("Str_temp : "+ Str_temp);
  String Str_Hum=instr.substring(idx_2+1, idx_3);
  // Serial.println("Str_Hum : "+ Str_Hum);
  String Str_Pres=instr.substring(idx_3+1, idx_4);
  // Serial.println("Str_Pres : "+ Str_Pres);
  String Str_Me=instr.substring(idx_4+1, idx_5);
  // Serial.println("Str_Me : "+ Str_Me);
  String Str_CO2=instr.substring(idx_5+1, idx_6);
  // Serial.println("Str_CO2 : "+ Str_CO2);
   delay(100);
   while(Serial.available()){
    
    char ch=Serial.read();
  }

  lcd.setCursor(0, 0);
  lcd.print("T:");
  int temp=Str_temp.toInt();  
  Blynk.virtualWrite(V0,temp);
  lcd.print(temp);
  lcd.print(" ");

  int hum=Str_Hum.toInt();    
  Blynk.virtualWrite(V1,hum);
  lcd.print("H:");
  lcd.print(hum);
  lcd.print(" ");

  int pres=Str_Pres.toFloat();
  Blynk.virtualWrite(V2,pres);
  lcd.print("P:");
  lcd.print(pres);

  lcd.setCursor(0,1);
  int Me=Str_Me.toInt();
  lcd.print("CH4:");
  lcd.print(Me);
  lcd.print(" ");
    

  Blynk.virtualWrite(V3,Me);
  int CO2=Str_CO2.toInt();
  lcd.print("Co2:");
  lcd.print(CO2);
  Blynk.virtualWrite(V4,CO2);
  
 }
 
//  Blynk.virtualWrite(V5, millis() / 1000);
}
