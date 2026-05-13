#ifndef ANYUNIT_HPP
#define ANYUNIT_HPP

#include <cinttypes>
#include <string>
#include "CAN_Config.hpp"
#include <format>

#ifndef DEFAULT_SPEED_UNIT
#define DEFAULT_SPEED_UNIT units::KPH
#endif

#ifndef DEFAULT_TEMPERATURE_UNIT
#define DEFAULT_TEMPERATURE_UNIT units::CELSIUS
#endif

#ifndef DEFAULT_PRESSURE_UNIT
#define DEFAULT_PRESSURE_UNIT units::KPA
#endif

namespace units
{
    enum UnitType : uint8_t
    {
        PRESSURE,
        SPEED,
        TEMPERATURE,
        UNITLESS
    };

    // kpa, m, Lambda, °C, kph
    enum SpeedUnit : uint8_t
    {
        KPH,
        MS,
        MPH
    };

    enum TemperatureUnit : uint8_t
    {
        CELSIUS,
        FAHRENHEIT,
        KELVIN
    };

    enum PressureUnit : uint8_t
    {
        BAR,
        KPA,
        PSI
    };

    struct UnitUnionSelection
    {
        UnitType mUnitType{NULL};
        union
        {
            units::SpeedUnit mSpeedUnit;
            units::TemperatureUnit mTemperatureUnit;
            units::PressureUnit mPressureUnit;
        };
    };
}

struct UnitlessType
{
    double mValue;

    UnitlessType()
    {
    }

    UnitlessType(double aVal)
    {
        mValue = aVal;
    }

    double getValue() const
    {
        return mValue;
    }

    std::string toString(uint8_t aNumDec = 0)
    {
        return std::string(String(mValue, aNumDec).c_str());
    }

    bool toBool()
    {
        return mValue != 0;
    }
};

struct SpeedType : public UnitlessType
{
    units::SpeedUnit mCurrentUnit;

    SpeedType(units::SpeedUnit aCurrentUnit, double aValue)
    {
        mCurrentUnit = aCurrentUnit;
        mValue = convert(aValue);
    }

    double convert(double aValue, units::SpeedUnit aNewUnit = DEFAULT_SPEED_UNIT)
    {
        if (mCurrentUnit == aNewUnit)
            return aValue;

        double retVal = 0;

        if (mCurrentUnit == units::KPH)
        {
            if (aNewUnit == units::MS)
            {
                retVal = aValue * 0.277778;
            }
            else if (aNewUnit == units::MPH)
            {
                retVal = aValue * 0.621371;
            }
        }
        else if (mCurrentUnit == units::MS)
        {
            if (aNewUnit == units::KPH)
            {
                retVal = aValue * 3.6;
            }
            else if (aNewUnit == units::MPH)
            {
                retVal = aValue * 2.23694;
            }
        }
        else if (mCurrentUnit == units::MPH)
        {
            if (aNewUnit == units::KPH)
            {
                retVal = aValue * 1.60934;
            }
            else if (aNewUnit == units::MS)
            {
                retVal = aValue * 0.44704;
            }
        }

        mCurrentUnit = aNewUnit;
        return retVal;
    }
};

struct TemperatureType : public UnitlessType
{
    units::TemperatureUnit mCurrentUnit;

    TemperatureType(units::TemperatureUnit aCurrentUnit, double aValue)
    {
        mCurrentUnit = aCurrentUnit;
        mValue = convert(aValue);
    }

    double convert(double aValue, units::TemperatureUnit aNewUnit = DEFAULT_TEMPERATURE_UNIT)
    {
        if (mCurrentUnit == aNewUnit)
            return aValue;

        double retVal = 0;

        if (mCurrentUnit == units::CELSIUS)
        {
            if (aNewUnit == units::FAHRENHEIT)
            {
                retVal = (aValue * (9.0 / 5.0)) + 32;
            }
            else if (aNewUnit == units::KELVIN)
            {
                retVal = aValue + 273.15;
            }
        }
        else if (mCurrentUnit == units::FAHRENHEIT)
        {
            if (aNewUnit == units::CELSIUS)
            {
                retVal = (aValue - 32) * (5.0 / 9.0);
            }
            else if (aNewUnit == units::KELVIN)
            {
                retVal = ((aValue - 32) * (5.0 / 9.0)) + 273.15;
            }
        }
        else if (mCurrentUnit == units::KELVIN)
        {
            if (aNewUnit == units::CELSIUS)
            {
                retVal = aValue - 273.15;
            }
            else if (aNewUnit == units::FAHRENHEIT)
            {
                retVal = ((aValue - 273.15) * (9.0 / 5.0)) + 32;
            }
        }

        mCurrentUnit = aNewUnit;

        return retVal;
    }
};

struct PressureType : public UnitlessType
{
    units::PressureUnit mCurrentUnit;

    PressureType(units::PressureUnit aCurrentUnit, double aValue)
    {
        mCurrentUnit = aCurrentUnit;
        mValue = convert(aValue);
    }

    double convert(double aValue, units::PressureUnit aNewUnit = DEFAULT_PRESSURE_UNIT)
    {
        if (mCurrentUnit != units::PressureUnit::KPA)
        {
            Serial.println(mCurrentUnit);
            Serial.println(aNewUnit);
            Serial.println();
        }

        if (mCurrentUnit == aNewUnit)
            return aValue;

        double retVal = 0;

        if (mCurrentUnit == units::BAR)
        {
            if (aNewUnit == units::KPA)
            {
                retVal = aValue * 100;
            }
            else if (aNewUnit == units::PSI)
            {
                retVal = aValue * 14.5038;
            }
        }
        else if (mCurrentUnit == units::KPA)
        {
            if (aNewUnit == units::BAR)
            {
                retVal = aValue * 0.01;
            }
            else if (aNewUnit == units::PSI)
            {
                retVal = aValue * 0.145038;
            }
        }
        else if (mCurrentUnit == units::PSI)
        {
            if (aNewUnit == units::BAR)
            {
                retVal = aValue * 0.0689476;
            }
            else if (aNewUnit == units::KPA)
            {
                retVal = aValue * 6.89476;
            }
        }

        mCurrentUnit = aNewUnit;
        return retVal;
    }
};

struct AnyUnit
{
    units::UnitType mType;
    union
    {
        SpeedType mSpeed;
        UnitlessType mUnitless;
        PressureType mPressure;
        TemperatureType mTemperature;
    };

    std::string toString(uint8_t aNumDec = 0)
    {
        switch (mType)
        {
        case units::SPEED:
            return mSpeed.toString(aNumDec);
        case units::UNITLESS:
            return mUnitless.toString(aNumDec);
        case units::PRESSURE:
            return mPressure.toString(aNumDec);
        case units::TEMPERATURE:
            return mTemperature.toString(aNumDec);
        }
        return "";
    }

    double getValue() const
    {
        switch (mType)
        {
        case units::SPEED:
            return mSpeed.getValue();
        case units::UNITLESS:
            return mUnitless.getValue();
        case units::PRESSURE:
            return mPressure.getValue();
        case units::TEMPERATURE:
            return mTemperature.getValue();
        }
        return 0;
    }

    bool toBool()
    {
        switch (mType)
        {
        case units::SPEED:
            return mSpeed.toBool();
        case units::UNITLESS:
            return mUnitless.toBool();
        case units::PRESSURE:
            return mPressure.toBool();
        case units::TEMPERATURE:
            return mTemperature.toBool();
        }
        return false;
    }

    void convert(units::TemperatureUnit aUnit)
    {
        if (mType == units::TEMPERATURE)
        {
            mTemperature.mValue = mTemperature.convert(mTemperature.getValue(), aUnit);
        }
    }

    void convert(units::PressureUnit aUnit)
    {
        if (mType == units::PRESSURE)
        {
            mPressure.mValue = mPressure.convert(mPressure.mValue, aUnit);
        }
    }

    void convert(units::SpeedUnit aUnit)
    {
        if (mType == units::SPEED)
        {
            mSpeed.mValue = mSpeed.convert(mSpeed.mValue, aUnit);
        }
    }

    AnyUnit()
    {
    }

    AnyUnit(double aVal)
    {
        mType = units::UNITLESS;
        mUnitless = UnitlessType(aVal);
    }

    AnyUnit(double aVal, units::SpeedUnit aUnit)
    {
        mType = units::SPEED;
        mSpeed = SpeedType(aUnit, aVal);
    }

    AnyUnit(double aVal, units::PressureUnit aUnit)
    {
        mType = units::PRESSURE;
        mPressure = PressureType(aUnit, aVal);
    }

    AnyUnit(double aVal, units::TemperatureUnit aUnit)
    {
        mType = units::TEMPERATURE;
        mTemperature = TemperatureType(aUnit, aVal);
    }

    bool operator>(const AnyUnit &other) const
    {
        return getValue() > other.getValue();
    }

    bool operator<(const AnyUnit &other) const
    {
        return getValue() < other.getValue();
    }

    bool operator==(const AnyUnit &other) const
    {
        return getValue() == other.getValue();
    }
};

#endif