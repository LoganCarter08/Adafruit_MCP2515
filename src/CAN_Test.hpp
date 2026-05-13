#ifndef CAN_TEST_HPP
#define CAN_TEST_HPP

#include <cinttypes>
#include <Adafruit_MCP2515.h>
#include <CAN_Config.hpp>
#include <tuple>

class CAN_Test
{
public:
    CAN_Test();
    CAN_Test(Adafruit_MCP2515 *aMCP);
    void setup(Adafruit_MCP2515 *aMCP);
    void sendTests();
    std::tuple<uint16_t, uint16_t> split16Bit(uint16_t aSixteen);

private:
    uint16_t mCurrentRPM{0};
    uint16_t mSpeed{0};
    uint8_t mGear{0};
    long mToggledTurns;
    bool mTurnsOn;
    long mLastFuelUpdate{0};
    long mLastHeadlightUpdate{0};

    Adafruit_MCP2515 *mMCP;
};

#endif