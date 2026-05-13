#ifndef CONFIG_HPP
#define CONFIG_HPP

#define RA8875_INT 3
#define RA8875_CS 1
#define RA8875_RESET 28

#define LED_PIN 22
#define LED_COUNT 40
#define LED_BRIGHTNESS 30 // 0-100%

#define MCP2515_CS 20
#define MCP2515_MOSI 11
#define MCP2515_MISO 12
#define MCP2515_SCK 10
#define MCP2515_INT 21
#define CAN_BAUDRATE 500000

#define BYTE_FLIP true

#define NUM_DECIMALS 2
#define LOCAL_BAROMETRIC_PRESSURE 14.7

// unit outputs. Not from source, but what you want to see on the screen.
#define DEFAULT_SPEED_UNIT units::MPH
#define DEFAULT_TEMPERATURE_UNIT units::FAHRENHEIT
#define DEFAULT_PRESSURE_UNIT units::PSI

#define TESTING_CAN_BUS true

#endif
