#include <Arduino.h>
#include "4channelRelay.h"

// Create a dual relay module instance
DualRelayModules relays;

void setup() {
    Serial.begin(115200);

    // Initialize both relay modules
    dual_relay_init(&relays);
    Serial.println("Dual 4-Channel Relay Modules Initialized");
}

void loop() {
    // Example 1: Cycle through all 8 relays individually
    Serial.println("Example 1: Cycling through all 8 relays individually");
    for(RelayChannel channel = RELAY_CHANNEL_1; channel <= RELAY_CHANNEL_8; channel = (RelayChannel)(channel + 1)) {
        Serial.print("Turning ON relay ");
        Serial.println(channel);
        dual_relay_set(&relays, channel, true);
        delay(1000);

        Serial.print("Turning OFF relay ");
        Serial.println(channel);
        dual_relay_set(&relays, channel, false);
        delay(1000);
    }

    // Example 2: Control all relays at once
    Serial.println("Example 2: Turning all relays ON");
    dual_relay_all(&relays, true);
    delay(2000);

    Serial.println("Turning all relays OFF");
    dual_relay_all(&relays, false);
    delay(2000);

    // Example 3: Set specific patterns using uint8_t
    Serial.println("Example 3: Setting alternating pattern (10101010)");
    dual_relay_set_pattern(&relays, 0xAA);  // Binary: 10101010
    delay(2000);

    Serial.println("Setting inverse pattern (01010101)");
    dual_relay_set_pattern(&relays, 0x55);  // Binary: 01010101
    delay(2000);

    // Example 4: Control modules separately
    Serial.println("Example 4: Module 1 ON, Module 2 OFF");
    dual_relay_set_module(&relays, RELAY_MODULE_1, 0x0F);  // All ON for module 1
    dual_relay_set_module(&relays, RELAY_MODULE_2, 0x00);  // All OFF for module 2
    delay(2000);

    Serial.println("Module 1 OFF, Module 2 ON");
    dual_relay_set_module(&relays, RELAY_MODULE_1, 0x00);  // All OFF for module 1
    dual_relay_set_module(&relays, RELAY_MODULE_2, 0x0F);  // All ON for module 2
    delay(2000);

    // Turn everything off before next cycle
    dual_relay_all(&relays, false);
    delay(1000);
}
