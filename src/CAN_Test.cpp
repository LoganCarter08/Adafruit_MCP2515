#include "CAN_Test.hpp"
#include <cstdlib>
#include <Arduino.h>
#include <bitset>

CAN_Test::CAN_Test(Adafruit_MCP2515 *aMCP)
{
    mMCP = aMCP;
}

CAN_Test::CAN_Test()
{
}

void CAN_Test::setup(Adafruit_MCP2515 *aMCP)
{
    mMCP = aMCP;
}

/*
    Right now the ecu I'm using to test with only supports a max of 16 bit values,
    so this might need redone to handle any standard c int type for longer values.
*/
std::tuple<uint16_t, uint16_t> CAN_Test::split16Bit(uint16_t aSixteen)
{
    if (BYTE_FLIP)
    {
        return std::make_tuple<uint16_t, uint16_t>((uint8_t)(aSixteen >> 8), (uint8_t)(aSixteen & 0xFF));
    }

    return std::make_tuple<uint16_t, uint16_t>((uint8_t)(aSixteen & 0xFF), (uint8_t)(aSixteen >> 8));
}

void CAN_Test::sendTests()
{
    // dummy data for now.
    mCurrentRPM = ((mCurrentRPM + 1) % 8000);
    mSpeed = (mSpeed + 1) % 225; // 225 kmh is about 150 mph

    auto splitSpeed = split16Bit(mSpeed);
    auto splitRPM = split16Bit(mCurrentRPM);

    mMCP->beginPacket(1);
    mMCP->write(1); // message ID
    mMCP->write(std::get<1>(splitRPM));
    mMCP->write(std::get<0>(splitRPM));
    mMCP->write(std::get<1>(splitSpeed)); // Wheel speed in kmh
    mMCP->write(std::get<0>(splitSpeed));
    mMCP->write(99);  // ECT in celsius
    mMCP->write(138); // MAP in kpa
    mMCP->endPacket();

    // delay(2000);
}