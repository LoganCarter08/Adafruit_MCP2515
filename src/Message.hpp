#ifndef MESSAGE_HPP
#define MESSAGE_HPP

#include <vector>
#include "CAN_Fields.hpp"
#include <cinttypes>
#include "AnyUnit.hpp"
#include <Arduino.h>
#include <bitset>

namespace message
{
    enum ENDIANNESS
    {
        BIG,
        LITTLE
    };
}

struct MessageUnit
{
    AnyUnit mValue;
    can_field::CAN_Fields mFieldName;

    MessageUnit(can_field::CAN_Fields aFieldName, AnyUnit aValue)
    {
        mFieldName = aFieldName;
        mValue = aValue;
    }
};

struct MessageParam
{
    can_field::CAN_Fields mFieldName;
    uint8_t mStartBit;
    uint8_t mBitWidth; // this feels like a clunky solution, but allows for tighter bit packing if user does want that.
    units::UnitType mUnitType;
    message::ENDIANNESS mEndian;

    union
    {
        units::PressureUnit mPressureUnit;
        units::TemperatureUnit mTemperatureUnit;
        units::SpeedUnit mSpeedUnit;
    };

    /*
        True delegated constructors, a constructor that calls another constructor,
        aren't supported in this Arduino version of C++, so just used a generic
        function for that.
    */
    void delegatedConstructor(can_field::CAN_Fields aFieldName,
                              uint8_t aStartBit,
                              uint8_t aBitWidth,
                              units::UnitType aUnitType = units::UnitType::UNITLESS,
                              message::ENDIANNESS aEndian = message::LITTLE)
    {
        mFieldName = aFieldName;
        mStartBit = aStartBit;
        mBitWidth = aBitWidth;
        mUnitType = aUnitType;
        mEndian = aEndian;
    }

    MessageParam(can_field::CAN_Fields aFieldName,
                 uint8_t aStartBit,
                 uint8_t aBitWidth,
                 units::UnitType aUnitType = units::UnitType::UNITLESS,
                 message::ENDIANNESS aEndian = message::LITTLE)
    {
        delegatedConstructor(aFieldName, aStartBit, aBitWidth, aUnitType, aEndian);
    }

    MessageParam(can_field::CAN_Fields aFieldName,
                 uint8_t aStartBit,
                 uint8_t aBitWidth,
                 units::PressureUnit aPressureUnit,
                 message::ENDIANNESS aEndian = message::LITTLE)
    {
        mPressureUnit = aPressureUnit;
        delegatedConstructor(aFieldName, aStartBit, aBitWidth, units::UnitType::PRESSURE, aEndian);
    }

    MessageParam(can_field::CAN_Fields aFieldName,
                 uint8_t aStartBit,
                 uint8_t aBitWidth,
                 units::TemperatureUnit aTemperatureUnit,
                 message::ENDIANNESS aEndian = message::LITTLE)
    {
        mTemperatureUnit = aTemperatureUnit;
        delegatedConstructor(aFieldName, aStartBit, aBitWidth, units::UnitType::TEMPERATURE, aEndian);
    }

    MessageParam(can_field::CAN_Fields aFieldName,
                 uint8_t aStartBit,
                 uint8_t aBitWidth,
                 units::SpeedUnit aSpeedUnit,
                 message::ENDIANNESS aEndian = message::LITTLE)
    {
        mSpeedUnit = aSpeedUnit;
        delegatedConstructor(aFieldName, aStartBit, aBitWidth, units::UnitType::SPEED, aEndian);
    }

    MessageParam()
    {
        delegatedConstructor(can_field::CAN_Fields::NOT_USED, 0, 0, units::UnitType::UNITLESS);
    }
};

class Message
{
public:
    Message(std::vector<MessageParam> aParameters)
    {
        mParameters = aParameters;
    }

    std::vector<MessageUnit> parseMessage(unsigned long long aMsgData) const
    {
        std::vector<MessageUnit> parsedData = {};
        for (MessageParam param : mParameters)
        {
            AnyUnit unit;

            // shift the unneeded bits off the right side and keep the new 16 on the right.
            uint16_t value = aMsgData >> (64 - param.mStartBit - param.mBitWidth);

            if (param.mBitWidth == 16 && BYTE_FLIP)
            {
                // sometimes bytes are flipped in transmissions. If so then flip them back.
                value = (value << 8) | (value >> 8);
            }
            else
            {
                // & with 0000000011111111 to remove first 8 digits
                value = value & 0xFF;
            }

            if (param.mUnitType == units::PRESSURE)
            {
                unit = AnyUnit(value, param.mPressureUnit);
            }
            else if (param.mUnitType == units::TEMPERATURE)
            {
                unit = AnyUnit(value, param.mTemperatureUnit);
            }
            else if (param.mUnitType == units::SPEED)
            {
                unit = AnyUnit(value, param.mSpeedUnit);
            }
            else
            {
                unit = AnyUnit(value);
            }

            parsedData.push_back(MessageUnit(param.mFieldName, unit));
        }
        return parsedData;
    }

private:
    std::vector<MessageParam> mParameters;
};

#endif