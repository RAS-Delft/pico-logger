#include <Arduino.h>
#include <ArduinoJson.h>
#include "Streaming.h"

const double ANALOG_MAX = 4095;
const double NOM_VOLTAGE = 3.3;
const double scaler = NOM_VOLTAGE/ANALOG_MAX;
uint8_t voltage_pins[] = {26, 27};
const double voltage_conversion[] = {7.8/1.0 * scaler, 1.0 * scaler};
uint8_t current_pins[] = {28};
const double current_conversion[] = {1680.0/1000.0 * scaler / 0.185};
const double current_bias[] = {-2.51 / 0.185};

uint32_t voltage_readings[2] = {0};
uint32_t current_readings[2] = {0};

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
    analogReadResolution(12);  // Set analog resolution to the full 12 bits.
    Serial.println("Finish set-up");
}

// the loop routine runs over and over again forever:
void loop() {
    StaticJsonDocument<200> doc;
    JsonArray voltages = doc.createNestedArray("voltages");
    JsonArray currents = doc.createNestedArray("currents");

    memset(static_cast<void *>(voltage_readings), 0, sizeof(voltage_readings));
    memset(static_cast<void *>(current_readings), 0, sizeof(current_readings));

    const int AVERAGE = 15;
    for (int l = 0; l < AVERAGE; ++l) {
        for (unsigned int i = 0; i < sizeof(voltage_pins); ++i) {
            voltage_readings[i] += analogRead(voltage_pins[i]);
        }
        for (unsigned int i = 0; i < sizeof(current_pins); ++i) {
            current_readings[i] += analogRead(current_pins[i]);
        }
        delay(20);
    }

    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));

    for (unsigned int i = 0; i < sizeof(voltage_pins); ++i) {
        double value = voltage_conversion[i] * (static_cast<double>(voltage_readings[i]) / static_cast<double>(AVERAGE));
        voltages.add(value);
    }
    for (unsigned int i = 0; i < sizeof(current_pins); ++i) {
        double value = current_conversion[i] * (static_cast<double>(current_readings[i]) / static_cast<double>(AVERAGE)) + current_bias[i];
        currents.add(value);
    }
    serializeJson(doc, Serial);
    Serial.println();
}