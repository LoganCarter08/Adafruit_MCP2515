#ifndef CAN_CONFIG_HPP
#define CAN_CONFIG_HPP

#define MCP2515_CS 20
#define MCP2515_MOSI 11
#define MCP2515_MISO 12
#define MCP2515_SCK 10
#define MCP2515_INT 21
#define CAN_BAUDRATE 500000

#define BYTE_FLIP true

// unit outputs.
#define DEFAULT_SPEED_UNIT units::MPH
#define DEFAULT_TEMPERATURE_UNIT units::FAHRENHEIT
#define DEFAULT_PRESSURE_UNIT units::PSI

#define TESTING_CAN_BUS true

#endif
