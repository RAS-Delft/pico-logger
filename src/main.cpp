#include <Arduino.h>
#include <ArduinoJson.h>
#include "Streaming.h"

const unsigned int ANALOG_MAX = 65535;
uint8_t voltage_pins[] = {26, 27};
const float voltage_conversion[] = {6.0/ANALOG_MAX, 3.73/ANALOG_MAX};
uint8_t current_pins[] = {28};
const float current_conversion[] = {5.0/ANALOG_MAX};

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

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    delay(200);               // wait for half a second

    for (unsigned int i = 0; i < sizeof(voltage_pins); ++i) {
        int int_value = analogRead(voltage_pins[i]);
        float value = voltage_conversion[i] * static_cast<float>(int_value);
        voltages.add(value);
    }
    for (unsigned int i = 0; i < sizeof(current_pins); ++i) {
        int int_value = analogRead(current_pins[i]);
        float value = current_conversion[i] * static_cast<float>(int_value);
        currents.add(value);
    }
    serializeJson(doc, Serial);
    Serial.println();
}