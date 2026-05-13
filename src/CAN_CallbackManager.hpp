#ifndef CAN_CALLBACKMANAGER_HPP
#define CAN_CALLBACKMANAGER_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <functional>
#include "CAN_Fields.hpp"
#include <memory>
#include "Arduino.h"
#include "AnyUnit.hpp"
#include <Adafruit_MCP2515.h>
#include "Config.hpp"
#include "CAN_Test.hpp"
#include "PhoneBook.hpp"
#include "Message.hpp"

struct CallbackStatus
{
    std::function<void(AnyUnit)> mFunc;
    AnyUnit mLastValue;
    long mLastUpdateTime{0};
    uint16_t mUpdateInterval{0};

    void call(AnyUnit aField)
    {
        long currentTime = millis();
        if (mLastValue.toString() == aField.toString())
        {
            return;
        }
        else if (mUpdateInterval == 0 || mLastUpdateTime == 0 || mLastUpdateTime + mUpdateInterval < currentTime)
        {
            mFunc(aField);
            mLastUpdateTime = currentTime;
            mLastValue = aField;
        }
    }
};

class CAN_CallbackManager
{
public:
    CAN_CallbackManager(); // add real constructor later
    void addSubscription(can_field::CAN_Fields aTopic, std::function<void(AnyUnit)> aCallback, uint16_t aUpdateInterval = 0);
    void handleMessage(int packetSize);
    void setup();
    void loop();
#ifdef TESTING_CAN_BUS
    void sendTests()
    {
        mCAN_Test.sendTests();
    }
#endif

private:
    std::unordered_map<can_field::CAN_Fields, std::vector<std::shared_ptr<CallbackStatus>>> mSubscriptions;
    Adafruit_MCP2515 mMCP;
    PhoneBook mPhoneBook;
    // this should be a map because we only want to show the current most field. Old should be stale.
    std::unordered_map<can_field::CAN_Fields, AnyUnit> mCurrentMessages{};
#ifdef TESTING_CAN_BUS
    CAN_Test mCAN_Test;
#endif
};

#endif