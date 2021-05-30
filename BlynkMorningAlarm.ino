#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <WidgetRTC.h>
#include <TimeLib.h>

// Blynk authorization token
char auth[] = "LZ1HGjOwEzkCLYhAvdPCPkgEkoY4QWAL";

// Set WiFi credentials
char ssid[] = "7";
char pass[] = "panther143";

// Pin definitions
int RELAY = D4;
int STATE = D8;

// Global variables
long timerHours = 0;
long timerMins = 0;
long totalTime = 0;
long startTime = 0;
int timerFlag = 0;

// Initialize RTC object
WidgetRTC rtc;

// Initialize timer object
BlynkTimer timer;

// Initialize terminak object
WidgetTerminal terminal(V1);

// Manual override to turn LEDs ON/OFF
BLYNK_WRITE(V0)
{
  if (param.asInt() == 1) {
    digitalWrite(RELAY, HIGH);
  }
  else {
    digitalWrite(RELAY, LOW);
  }
}

// Timer hour input
BLYNK_WRITE(V2)
{
  // Hours converted to seconds
  timerHours = param.asInt()*3600;
}

// Timer minute input
BLYNK_WRITE(V3)
{
  // Minutes converted to seconds
  timerMins = param.asInt()*60;
}

BLYNK_WRITE(V4)
{
  if (timerFlag == 0) {
    timerFlag = 1;
    // Total timeout duration in milliseconds
    totalTime = timerHours + timerMins;
    terminal.println("The timer is set for " + String(timerHours/3600) + " hours and " + String(timerMins/60) + " minutes.");
    totalTime *= 1000;
    startTime = millis();
    digitalWrite(STATE, HIGH);
    terminal.flush();
  }
}

BLYNK_WRITE(V5)
{
  // Reset timer and all relavent parameters
  timerFlag, startTime, totalTime = 0;
}

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

void setup()
{
  // Debug console
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  setSyncInterval(10 * 60); // RTC sync interval in seconds (10 minutes)
  
  // Initialize pins
  pinMode(RELAY, OUTPUT);
  pinMode(STATE, OUTPUT);
  digitalWrite(RELAY, LOW);
}

void loop()
{
  Blynk.run();
  if (timerFlag == 1) {
    if (millis() - startTime >= totalTime) {
      digitalWrite(RELAY, HIGH);
      if (millis() - startTime >= totalTime + 200) {
        digitalWrite(RELAY, LOW);
        digitalWrite(STATE, LOW);
        timerFlag = 0;
        startTime = 0;
        terminal.clear();
      }
    }
  }
}
