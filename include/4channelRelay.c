#include "4channelRelay.h"
#include <Arduino.h>

void relay_init(FourChannelRelay* relay) {
    // Initialize pins
    relay->relay1_pin = RELAY1_PIN;
    relay->relay2_pin = RELAY2_PIN;
    relay->relay3_pin = RELAY3_PIN;
    relay->relay4_pin = RELAY4_PIN;

    // Set pins as outputs
    pinMode(relay->relay1_pin, OUTPUT);
    pinMode(relay->relay2_pin, OUTPUT);
    pinMode(relay->relay3_pin, OUTPUT);
    pinMode(relay->relay4_pin, OUTPUT);

    // Initialize all relays to OFF state
    relay->states = 0x00;  // All bits set to 0 (OFF)

    // Set physical pins (HIGH because it's active-low)
    digitalWrite(relay->relay1_pin, HIGH);
    digitalWrite(relay->relay2_pin, HIGH);
    digitalWrite(relay->relay3_pin, HIGH);
    digitalWrite(relay->relay4_pin, HIGH);
}

// Helper function to get mask for a channel (1-8)
static uint8_t get_mask_for_channel(uint8_t channel) {
    switch(channel) {
        case RELAY_CHANNEL_1: return RELAY1_MASK;
        case RELAY_CHANNEL_2: return RELAY2_MASK;
        case RELAY_CHANNEL_3: return RELAY3_MASK;
        case RELAY_CHANNEL_4: return RELAY4_MASK;
        case RELAY_CHANNEL_5: return RELAY5_MASK;
        case RELAY_CHANNEL_6: return RELAY6_MASK;
        case RELAY_CHANNEL_7: return RELAY7_MASK;
        case RELAY_CHANNEL_8: return RELAY8_MASK;
        default: return 0;
    }
}

// Helper function to get pin for a channel (1-4) in a single module
static uint8_t get_pin_for_channel(FourChannelRelay* relay, uint8_t channel) {
    switch(channel) {
        case RELAY_CHANNEL_1: return relay->relay1_pin;
        case RELAY_CHANNEL_2: return relay->relay2_pin;
        case RELAY_CHANNEL_3: return relay->relay3_pin;
        case RELAY_CHANNEL_4: return relay->relay4_pin;
        default: return 0;
    }
}

void relay_set(FourChannelRelay* relay, uint8_t channel, bool state) {
    uint8_t mask = get_mask_for_channel(channel);
    uint8_t pin = get_pin_for_channel(relay, channel);

    if (mask == 0 || pin == 0) return;

    if (state) {
        relay->states |= mask;    // Set bit
    } else {
        relay->states &= ~mask;   // Clear bit
    }

    digitalWrite(pin, !state);  // Inverted because it's active-low
}

bool relay_get_state(FourChannelRelay* relay, uint8_t channel) {
    uint8_t mask = get_mask_for_channel(channel);
    return (relay->states & mask) != 0;
}

void relay_toggle(FourChannelRelay* relay, uint8_t channel) {
    uint8_t mask = get_mask_for_channel(channel);
    relay->states ^= mask;  // XOR to toggle bit
    uint8_t pin = get_pin_for_channel(relay, channel);
    digitalWrite(pin, !(relay->states & mask));  // Inverted for active-low
}

void relay_all(FourChannelRelay* relay, bool state) {
    uint8_t pattern = state ? 0x0F : 0x00;  // 0x0F = 0000 1111
    relay_set_pattern(relay, pattern);
}

void relay_set_pattern(FourChannelRelay* relay, uint8_t pattern) {
    relay->states = pattern & 0x0F;  // Only use lower 4 bits

    // Update physical pins
    digitalWrite(relay->relay1_pin, !(pattern & RELAY1_MASK));
    digitalWrite(relay->relay2_pin, !(pattern & RELAY2_MASK));
    digitalWrite(relay->relay3_pin, !(pattern & RELAY3_MASK));
    digitalWrite(relay->relay4_pin, !(pattern & RELAY4_MASK));
}

/*
 * Dual Relay Module Functions
 * These functions manage two 4-channel relay modules as a single 8-channel system
 * using a single uint8_t to represent all 8 relay states
 */

// Initialize both relay modules
void dual_relay_init(DualRelayModules* modules) {
    // Initialize first module with default pins
    relay_init(&modules->module1);

    // Initialize second module with different pins
    modules->module2.relay1_pin = RELAY5_PIN;
    modules->module2.relay2_pin = RELAY6_PIN;
    modules->module2.relay3_pin = RELAY7_PIN;
    modules->module2.relay4_pin = RELAY8_PIN;

    // Set pins as outputs
    pinMode(modules->module2.relay1_pin, OUTPUT);
    pinMode(modules->module2.relay2_pin, OUTPUT);
    pinMode(modules->module2.relay3_pin, OUTPUT);
    pinMode(modules->module2.relay4_pin, OUTPUT);

    // Initialize all relays to OFF state
    modules->module2.states = 0x00;

    // Set physical pins (HIGH because it's active-low)
    digitalWrite(modules->module2.relay1_pin, HIGH);
    digitalWrite(modules->module2.relay2_pin, HIGH);
    digitalWrite(modules->module2.relay3_pin, HIGH);
    digitalWrite(modules->module2.relay4_pin, HIGH);

    // Initialize combined states
    modules->combined_states = 0x00;
}

// Set a specific relay (1-8) across both modules
void dual_relay_set(DualRelayModules* modules, RelayChannel channel, bool state) {
    uint8_t mask = get_mask_for_channel(channel);

    // Update combined state
    if (state) {
        modules->combined_states |= mask;
    } else {
        modules->combined_states &= ~mask;
    }

    // Determine which module to use
    if (channel <= RELAY_CHANNEL_4) {
        // Channels 1-4 are on module 1
        relay_set(&modules->module1, channel, state);
    } else {
        // Channels 5-8 are on module 2
        // Convert to 1-4 range for the second module
        uint8_t module2_channel = channel - 4;
        relay_set(&modules->module2, module2_channel, state);
    }
}

// Get state of a specific relay (1-8)
bool dual_relay_get_state(DualRelayModules* modules, RelayChannel channel) {
    uint8_t mask = get_mask_for_channel(channel);
    return (modules->combined_states & mask) != 0;
}

// Toggle a specific relay (1-8)
void dual_relay_toggle(DualRelayModules* modules, RelayChannel channel) {
    bool current_state = dual_relay_get_state(modules, channel);
    dual_relay_set(modules, channel, !current_state);
}

// Set all relays to the same state
void dual_relay_all(DualRelayModules* modules, bool state) {
    uint8_t pattern = state ? 0xFF : 0x00;  // All 8 bits
    dual_relay_set_pattern(modules, pattern);
}

// Set all relays according to a pattern
void dual_relay_set_pattern(DualRelayModules* modules, uint8_t pattern) {
    modules->combined_states = pattern;

    // Update module 1 (lower 4 bits)
    relay_set_pattern(&modules->module1, pattern & 0x0F);

    // Update module 2 (upper 4 bits)
    relay_set_pattern(&modules->module2, (pattern >> 4) & 0x0F);
}

// Set relays for a specific module
void dual_relay_set_module(DualRelayModules* modules, RelayModule module, uint8_t pattern) {
    if (module == RELAY_MODULE_1) {
        // Update module 1 (lower 4 bits)
        relay_set_pattern(&modules->module1, pattern & 0x0F);

        // Update combined states (preserve upper 4 bits)
        modules->combined_states = (modules->combined_states & 0xF0) | (pattern & 0x0F);
    } else {
        // Update module 2 (upper 4 bits)
        relay_set_pattern(&modules->module2, pattern & 0x0F);

        // Update combined states (preserve lower 4 bits)
        modules->combined_states = (modules->combined_states & 0x0F) | ((pattern & 0x0F) << 4);
    }
}
