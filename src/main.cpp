#include <Arduino.h>
#include <ArduinoJson.h>
#include "Streaming.h"

const double ANALOG_MAX = 1023;
const double NOM_VOLTAGE = 3.3;
const double scaler = NOM_VOLTAGE/ANALOG_MAX;
uint8_t voltage_pins[] = {26, 27};
const double voltage_conversion[] = {7.8/1.0 * scaler, 1.0 * scaler};
uint8_t current_pins[] = {28};
const double current_conversion[] = {1680.0/1000.0 * scaler / 0.185};
const double current_bias[] = {-2.456 / 0.185};

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
        double value = voltage_conversion[i] * static_cast<double>(int_value);
        voltages.add(value);
    }
    for (unsigned int i = 0; i < sizeof(current_pins); ++i) {
        int int_value = analogRead(current_pins[i]);
        double value = current_conversion[i] * static_cast<double>(int_value) + current_bias[i];
        currents.add(value);
    }
    serializeJson(doc, Serial);
    Serial.println();
}