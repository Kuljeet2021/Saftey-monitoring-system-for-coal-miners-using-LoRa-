#include <SoftwareSerial.h>
#include <SFE_BMP180.h>
#include <Wire.h>
// #include "GY521.h"
#include <DFRobot_DHT11.h>
#define CH4Sensor A0      //MQ2
#define Co2Sensor A1      //MQ135
#define CH4Sensor_thr 10  //MQ2
#define Co2Sensor_thr 11  //MQ135
DFRobot_DHT11 DHT;
#define Buzzer 13
#define DHT11_PIN 4
#define Key 2
#define ALTITUDE 1655.0  //
SFE_BMP180 pressure;
// GY521 sensor(0x68);

uint32_t counter = 0;
SoftwareSerial wireless(5, 6);  //Rx, Tx


void setup() {
  Serial.begin(115200);
  wireless.begin(9600);
  Serial.println(__FILE__);


  pinMode(Buzzer, OUTPUT);
  digitalWrite(Buzzer, LOW);
  pinMode(Co2Sensor, INPUT);
  pinMode(CH4Sensor, INPUT);

  pinMode(Co2Sensor_thr, INPUT);
  pinMode(CH4Sensor_thr, INPUT);
  Wire.begin();

  pinMode(Key, INPUT_PULLUP);


  if (pressure.begin())
    Serial.println("BMP180 init success");
  else {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while (1)
      ;  // Pause forever.
  }

  delay(100);

  attachInterrupt(digitalPinToInterrupt(Key), Alert, FALLING);
}
int temp_threshold = 40;
int humi_threshold = 60;
int pressure_threshold = 1000;
int Me_threshold = 300;
int co2_threshold = 3000;


unsigned long t1 = 0;
bool Okflag = false;
//PanicAlert, Methane_alert,CO_alert,Temp, Hum, Presure, Me, CO
bool PanicAlert = false;
bool ComeBackAlert = false;
String str = "";
void loop() {

  if (digitalRead(Key) == LOW) {
    PanicAlert = true;
  }
  if (wireless.available()) {
    String instr = wireless.readStringUntil('#');
    if (instr[0] == '1') {
      Serial.println("Panic Feedback");
      PanicAlert = false;
    }
    if (instr[0] == '2') {
      Serial.println("Come Back ALert");
      ComeBackAlert = true;
      while (true) {
        digitalWrite(Buzzer, !digitalRead(Buzzer));
        delay(300);
      }
    }
  }
  str = "";

  if (PanicAlert == true) {
    Serial.println("Panic Alert");
    str = "1";
    digitalWrite(Buzzer, !digitalRead(Buzzer));
  } else {
    str = "0";
    digitalWrite(Buzzer, LOW);
  }
  if (digitalRead(CH4Sensor_thr) == LOW) {
    Serial.println("Methane Alert");
    str += "1";
    digitalWrite(Buzzer, HIGH);
    delay(500);
    digitalWrite(Buzzer, LOW);
  } else {
    str += "0";
  }

  if (digitalRead(Co2Sensor_thr) == LOW) {
    Serial.println("CO2 Alert");
    str += '1';
    digitalWrite(Buzzer, HIGH);
    delay(500);
    digitalWrite(Buzzer, LOW);
  } else
    str += '0';


  DHT.read(DHT11_PIN);
  Serial.print("temp:");
  int t = DHT.temperature;
  int h = DHT.humidity;
  Serial.print(t);

  Serial.print("  humi:");
  Serial.println(h);
  digitalWrite(Buzzer, LOW);
  float B_pressure = Pressure();
  Serial.println("Pressure : " + String(B_pressure));

  float Me_value = methane();
  float Co2_Value = Co2();
  str += ',';
  str += String(t) + "," + String(h) + "," + String(B_pressure) + "," + String(Me_value) + "," + String(Co2_Value) + "#";
  Serial.println("===============");
  Serial.println(str);
  wireless.println(str);
  Serial.println("===============");

  if (t > temp_threshold || h > humi_threshold || B_pressure > pressure_threshold)  // || Me_value> Me_threshold || Co2_Value>co2_threshold )
  {
    Serial.println("Situation out of control");
    digitalWrite(Buzzer, HIGH);
    delay(500);
    digitalWrite(Buzzer, LOW);

  } else
    delay(2500);
}
void Alert() {
  Serial.println("Panic Alert");
  PanicAlert = true;
  // wireless.println("XXX");
  digitalWrite(Buzzer, HIGH);
}


float methane() {
  int CH4SensorValue = analogRead(CH4Sensor);

  // Convert the analog reading to voltage
  float voltage = (CH4SensorValue / 1024.0) * 5.0;

  // Convert voltage to ppm (parts per million)
  // float ppm = (voltage - 0.4) * 10.0; // Adjust the coefficients based on your sensor's calibration

  float ppm = 1591.2 * voltage * voltage - 7293.05 * voltage + 8554.54;  // Adjust the coefficients based on your sensor's calibration
  ppm = ppm - 800;
  // Print the ppm value on the serial monitor
  Serial.print("CH4 Concentration: ");
  Serial.print(ppm);
  Serial.println(" ppm");
  return ppm;
}
float Co2() {
  int Co2SensorValue = analogRead(Co2Sensor);
  // Convert the analog reading to voltage
  float voltage = (Co2SensorValue / 1024.0) * 5.0;
  float ppm = 1591.2 * voltage * voltage - 7293.05 * voltage + 8554.54;  // Adjust the coefficients based on your sensor's calibration
  ppm = ppm - 300;
  // Convert voltage to ppm (parts per million)
  // float ppm = (voltage - 0.4) * 10.0; // Adjust the coefficients based on your sensor's calibration

  // Print the ppm value on the serial monitor
  Serial.print("Co2 Concentration: ");
  Serial.print(ppm);
  Serial.println(" ppm");
  return ppm;
}
double Pressure() {
  char status;
  double T, P, p0, a;

  // Loop here getting pressure readings every 10 seconds.

  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:
  //---------------------
  // Serial.println();
  // Serial.print("provided altitude: ");
  // Serial.print(ALTITUDE,0);
  // Serial.print(" meters, ");
  // Serial.print(ALTITUDE*3.28084,0);
  // Serial.println(" feet");
  //---------------------
  // If you want to measure altitude, and not pressure, you will instead need
  // to provide a known baseline pressure. This is shown at the end of the sketch.

  // You must first get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  if (status != 0) {
    // Wait for the measurement to complete:
    delay(status);

    // Retrieve the completed temperature measurement:
    // Note that the measurement is stored in the variable T.
    // Function returns 1 if successful, 0 if failure.

    status = pressure.getTemperature(T);
    if (status != 0) {
      // Print out the measurement:
      //---------------------
      Serial.print("temperature: ");
      Serial.print(T, 2);
      Serial.print(" deg C, ");
      Serial.print((9.0 / 5.0) * T + 32.0, 2);
      Serial.println(" deg F");
      //---------------------
      // Start a pressure measurement:
      // The parameter is the oversampling setting, from 0 to 3 (highest res, longest wait).
      // If request is successful, the number of ms to wait is returned.
      // If request is unsuccessful, 0 is returned.

      status = pressure.startPressure(3);
      if (status != 0) {
        // Wait for the measurement to complete:
        delay(status);

        // Retrieve the completed pressure measurement:
        // Note that the measurement is stored in the variable P.
        // Note also that the function requires the previous temperature measurement (T).
        // (If temperature is stable, you can do one temperature measurement for a number of pressure measurements.)
        // Function returns 1 if successful, 0 if failure.

        status = pressure.getPressure(P, T);
        if (status != 0) {
          // Print out the measurement:
          // //---------------------
          // Serial.print("absolute pressure: ");
          // Serial.print(P,2);
          // Serial.print(" mb, ");
          // Serial.print(P*0.0295333727,2);
          // Serial.println(" inHg");
          //---------------------

          // The pressure sensor returns abolute pressure, which varies with altitude.
          // To remove the effects of altitude, use the sealevel function and your current altitude.
          // This number is commonly used in weather reports.
          // Parameters: P = absolute pressure in mb, ALTITUDE = current altitude in m.
          // Result: p0 = sea-level compensated pressure in mb

          p0 = pressure.sealevel(P, ALTITUDE);  // we're at 1655 meters (Boulder, CO)
          // Serial.print("relative (sea-level) pressure: ");
          // Serial.print(p0,2);
          // Serial.print(" mb, ");
          // Serial.print(p0*0.0295333727,2);
          // Serial.println(" inHg");

          // On the other hand, if you want to determine your altitude from the pressure reading,
          // use the altitude function along with a baseline pressure (sea-level or other).
          // Parameters: P = absolute pressure in mb, p0 = baseline pressure in mb.
          // Result: a = altitude in m.

          a = pressure.altitude(P, p0);
          // Serial.print("computed altitude: ");
          // Serial.print(a,0);
          // Serial.print(" meters, ");
          // Serial.print(a*3.28084,0);
          // Serial.println(" feet");
        } else Serial.println("error retrieving pressure measurement\n");
      } else Serial.println("error starting pressure measurement\n");
    } else Serial.println("error retrieving temperature measurement\n");
  } else Serial.println("error starting temperature measurement\n");

  return P;
}