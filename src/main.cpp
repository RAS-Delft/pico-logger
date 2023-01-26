#include <Arduino.h>
#include <ArduinoJson.h>
#include "Streaming.h"

uint8_t voltage_pins[] = {26, 27};
uint8_t current_pins[] = {28};

// the setup routine runs once when you press reset:
void setup() {
    Serial.begin(115200);
    // initialize the digital pin as an output.
    pinMode(LED_BUILTIN, OUTPUT);

    for (auto pin: voltage_pins) {
        pinMode(pin, INPUT);
    }
    for (auto pin: current_pins) {
        pinMode(pin, INPUT);
    }
    Serial.println("Finish set-up");
}

// the loop routine runs over and over again forever:
void loop() {
    StaticJsonDocument<200> doc;
    JsonArray voltages = doc.createNestedArray("voltages");
    JsonArray currents = doc.createNestedArray("currents");

    Serial.println("Ping");
    digitalWrite(LED_BUILTIN, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(500);               // wait for half a second
    digitalWrite(LED_BUILTIN, LOW);    // turn the LED off by making the voltage LOW
    delay(500);               // wait for half a second

    for (unsigned int i = 0; i < sizeof(voltage_pins); ++i) {
        int value = analogRead(voltage_pins[i]);
        voltages.add(value);
        Serial << "Voltage value " << i << " = " << value << endl;
    }
    for (unsigned int i = 0; i < sizeof(current_pins); ++i) {
        int value = analogRead(current_pins[i]);
        currents.add(value);
        Serial << "Current value " << i << " = " << value << endl;
    }
    serializeJson(doc, Serial);
    Serial.println();
}