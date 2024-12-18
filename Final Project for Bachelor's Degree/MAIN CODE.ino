#define BLYNK_TEMPLATE_ID "TMPL6asET1GRq"
#define BLYNK_TEMPLATE_NAME "Try"
#define BLYNK_AUTH_TOKEN "yWDUfC1G8XyaKOHYYZgBzJJxL1tzBNL-"

#define BLYNK_PRINT Serial

#define DEBUG

// Masukkan Library ===================
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "RTClib.h"
// ====================================

// RTC Obyek ==========================
RTC_DS3231 myRtc;
unsigned int nowHour = 0, nowMinute = 0, nowSecond = 0;
uint16_t nowYear = 0;
uint8_t nowMonth = 0, nowDay = 0;
unsigned long int timerRtc = 0;
bool waktuSiram = true;
#define JAM_SIRAM          7
#define MENIT_SIRAM        0
#define I2C_SDA           21
#define I2C_SCL           22
// ====================================

// Connect BLYNK dan WIFI =============
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "Helmi";
char pass[] = "password";
// ====================================
BlynkTimer timer;

//Deklarasi Sensor =====================
#define IN1           14
#define CSMSENSORPIN  39
// =====================================

// Setting Timer Penyiraman ============
#define WAKTU_MTR_TURUN   15000
#define WAKTU_MTR_NAIK    15000
// =====================================

// Fuzzy Aim ===========================
double percentSoilMoistureSens = 0.0;
// =====================================


bool flagSiram = false;
float dataSiram = 0.0;


// Step Siram ==========================
bool flagMotorTurun = false;
bool flagMotorSiram = false;
bool flagMotorNaik = false;

unsigned long int timerMotorTurun = 0;
unsigned long int timerMotorSiram = 0;
unsigned long int timerMotorNaik = 0;
// =====================================

//Sensor Kelembapan Tanah ==============
double AnalogValue = 0;
double VWC = 0;
double SoilMoisturePercent = 0;
//======================================

void(* resetFunc) (void) = 0;

//RTC BLYNK ================================================================
void clockDisplay() {
  String currentTime = (String)nowHour + ":" + (String)nowMinute + ":" + (String)nowSecond;
  String currentDate = (String)nowDay + "/" + (String)nowMonth + "/" + (String)nowYear;
  Serial.print("Current Date : ");
  Serial.print(currentDate);
  Serial.print(", ");
  Serial.println(currentTime);
  Blynk.virtualWrite(V4, currentTime);
  Blynk.virtualWrite(V5, currentDate);
}
//============================================================================

// BACA Sensor Soil Moisture =================================================
void read_MoistureSens(){
  AnalogValue = analogRead(CSMSENSORPIN);
  VWC = 0.000000291826*pow(AnalogValue, 2) - 0.00217032*AnalogValue + 4.03144;
  SoilMoisturePercent = 351.14*VWC - 0.5479;

  // Fuzzy Aim ================================================
  percentSoilMoistureSens = (double)SoilMoisturePercent;
  // ==========================================================
}
// ===========================================================================

//Monitoring Sensor ==========================================================
void sendSensor() {
  Blynk.virtualWrite(V2, SoilMoisturePercent);
  Serial.print("Soil Moisture % : ");
  Serial.println(SoilMoisturePercent);
}
//==============================================================================

void setup() {
  Serial.begin(115200);

  pinMode(IN1, OUTPUT);

  // Setting Modul RTC
  delay(200);
  Wire.begin(I2C_SDA, I2C_SCL);
  if (! myRtc.begin()) {
    Serial.println("RTC : Couldn't find RTC");
    Serial.flush();
    delay(5000);
    resetFunc();
  }
  
  if (myRtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    myRtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  delay(200);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(1000L, clockDisplay);
  timer.setInterval(1000L, sendSensor);
}

void loop() {
  // Timer 1s untuk baca data RTC dan Sensor Soil Moisture
  if((millis() - timerRtc) > 800){
    RTC_GetTime();
    read_MoistureSens();
    timerRtc = millis();
  }

  if((WiFi.status() == WL_CONNECTED)){
    Blynk.run();
    timer.run();
  }

  if(nowHour == JAM_SIRAM && nowMinute == MENIT_SIRAM){ // Jika JAM dan MENIT siram sudah sama dengan RTC
    if(waktuSiram == true){
      Serial.println("Siram Dulu");
      if (flagSiram == false) {
        Serial.println("Flag Siram False");
        if(SoilMoisturePercent < 40){ // Memeriksa apakah kelembaban kurang dari 40%
          Serial.println("Siram Dulu  222");
          // Fuzzy
          dataSiram = Defuzzyfikasi_Algorithm();
          if (dataSiram > 0.0) {
            // Hidupkan Motor
            Serial.println("Hidupkan Motor");
            flagSiram = true;
            flagMotorTurun = true;
            flagMotorNaik = false;
            flagMotorSiram = false;
            timerMotorTurun = millis();
          }
        }
      }
      waktuSiram = false;
    }
  }
  else waktuSiram = true;

  //Menurunkan Motor Untuk Menyiram ========================================
  if (flagSiram == true) {
    if (flagMotorTurun == true) {
      // Turunkan Motor
      Serial.println("NYIRAM TURUN");
      digitalWrite(IN1, HIGH);
      if ((millis() - timerMotorTurun) >= WAKTU_MTR_TURUN) {
        Serial.println("Motor sudah turun");
        digitalWrite(IN1, LOW);
        flagMotorTurun = false;
        flagMotorSiram = true;
        flagMotorNaik = false;

        timerMotorTurun = millis();
        timerMotorSiram = millis();
      }
    }
    //=========================================================================

    //Fase Penyiraman ===========================================================    
    if (flagMotorSiram == true) {
      Serial.println("NYIRAM NYIRAM");
      digitalWrite(IN1, LOW);
      if ((millis() - timerMotorSiram) >= (uint16_t)(dataSiram * 1000.0)) {
        flagMotorTurun = false;
        flagMotorSiram = false;
        flagMotorNaik = true;

        timerMotorSiram = millis();
        timerMotorNaik = millis();
      }
    }
    //============================================================================

    //Motor Naik Selesai Menyiram ==============================
    if (flagMotorNaik == true) {
      Serial.println("NYIRAM NAIK");
      digitalWrite(IN1, HIGH);
      if ((millis() - timerMotorNaik) >= WAKTU_MTR_NAIK) {
        digitalWrite(IN1, LOW);

        flagMotorTurun = false;
        flagMotorSiram = false;
        flagMotorNaik = false;
        flagSiram = false;
        Serial.println("Proses Siram Selesai");
        timerMotorNaik = millis();
      }
    }
    //===========================================================
  }
}
