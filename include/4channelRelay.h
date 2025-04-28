#ifndef FOUR_CHANNEL_RELAY_H
#define FOUR_CHANNEL_RELAY_H

#include <stdint.h>
#include <stdbool.h>

// Pin definitions for first relay module
#define RELAY1_PIN 4  // IN1
#define RELAY2_PIN 5  // IN2
#define RELAY3_PIN 6  // IN3
#define RELAY4_PIN 7  // IN4

// Pin definitions for second relay module
#define RELAY5_PIN 8  // IN1 of second module
#define RELAY6_PIN 9  // IN2 of second module
#define RELAY7_PIN 10 // IN3 of second module
#define RELAY8_PIN 11 // IN4 of second module

// Enums for relay channels
typedef enum {
    RELAY_CHANNEL_1 = 1,
    RELAY_CHANNEL_2 = 2,
    RELAY_CHANNEL_3 = 3,
    RELAY_CHANNEL_4 = 4,
    RELAY_CHANNEL_5 = 5,
    RELAY_CHANNEL_6 = 6,
    RELAY_CHANNEL_7 = 7,
    RELAY_CHANNEL_8 = 8
} RelayChannel;

// Enum for relay modules
typedef enum {
    RELAY_MODULE_1 = 0,
    RELAY_MODULE_2 = 1
} RelayModule;

// Bit masks for each relay
#define RELAY1_MASK 0x01  // 0000 0001
#define RELAY2_MASK 0x02  // 0000 0010
#define RELAY3_MASK 0x04  // 0000 0100
#define RELAY4_MASK 0x08  // 0000 1000
#define RELAY5_MASK 0x10  // 0001 0000
#define RELAY6_MASK 0x20  // 0010 0000
#define RELAY7_MASK 0x40  // 0100 0000
#define RELAY8_MASK 0x80  // 1000 0000

typedef struct {
    uint8_t relay1_pin;
    uint8_t relay2_pin;
    uint8_t relay3_pin;
    uint8_t relay4_pin;
    uint8_t states;  // Using bits: 0000DCBA where A=relay1, B=relay2, etc.
} FourChannelRelay;

// New struct for dual relay modules
typedef struct {
    FourChannelRelay module1;
    FourChannelRelay module2;
    uint8_t combined_states;  // All 8 relays: HGFEDCBA where A=relay1, B=relay2, etc.
} DualRelayModules;

// Original single module functions
void relay_init(FourChannelRelay* relay);
void relay_set(FourChannelRelay* relay, uint8_t channel, bool state);
bool relay_get_state(FourChannelRelay* relay, uint8_t channel);
void relay_toggle(FourChannelRelay* relay, uint8_t channel);
void relay_all(FourChannelRelay* relay, bool state);
void relay_set_pattern(FourChannelRelay* relay, uint8_t pattern);

// New dual module functions
void dual_relay_init(DualRelayModules* modules);
void dual_relay_set(DualRelayModules* modules, RelayChannel channel, bool state);
bool dual_relay_get_state(DualRelayModules* modules, RelayChannel channel);
void dual_relay_toggle(DualRelayModules* modules, RelayChannel channel);
void dual_relay_all(DualRelayModules* modules, bool state);
void dual_relay_set_pattern(DualRelayModules* modules, uint8_t pattern);
void dual_relay_set_module(DualRelayModules* modules, RelayModule module, uint8_t pattern);

#endif
