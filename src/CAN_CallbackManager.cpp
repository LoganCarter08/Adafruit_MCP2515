#include "CAN_CallbackManager.hpp"
#include <Arduino.h>
#include <SPI.h>
#include <bitset>
#include <algorithm>

CAN_CallbackManager::CAN_CallbackManager() : mMCP(MCP2515_CS, MCP2515_MOSI, MCP2515_MISO, MCP2515_SCK)
{
}

void CAN_CallbackManager::setup()
{
    if (!mMCP.begin(CAN_BAUDRATE, TESTING_CAN_BUS))
    {
        // todo. Add error on screen about not being able to connect to CAN.
        Serial.println("Error initializing MCP2515.");
        while (1)
            delay(10);
    }
    Serial.println("MCP2515 chip found");

    if (TESTING_CAN_BUS)
    {
        while (!Serial)
            delay(10);
    }

    std::function<void(int)> onReceive = [this](int aPacketSize)
    { this->handleMessage(aPacketSize); };

    mMCP.onReceive(MCP2515_INT, onReceive);
}

void CAN_CallbackManager::addSubscription(can_field::CAN_Fields aTopic, std::function<void(AnyUnit)> aCallback, uint16_t aUpdateInterval)
{
    auto found = mSubscriptions.find(aTopic);
    std::vector<std::shared_ptr<CallbackStatus>> vec = {};
    if (found != mSubscriptions.end())
    {
        vec = found->second;
    }

    CallbackStatus call;
    call.mFunc = aCallback;
    call.mUpdateInterval = aUpdateInterval;
    vec.push_back(std::make_shared<CallbackStatus>(call));

    mSubscriptions[aTopic] = vec;
}

void CAN_CallbackManager::loop()
{
    /*
        So this is a little wonky. Needs extensive testing. But wrap any time
        we access the current messages with no interrupts that way it can't be
        edited while we use. We want noInterrupts for as little time as possible
        so there isn't a chance of delaying receiving messages.
    */
    noInterrupts();
    std::vector<can_field::CAN_Fields> keys;
    keys.reserve(mCurrentMessages.size());
    std::transform(mCurrentMessages.begin(), mCurrentMessages.end(), std::back_inserter(keys),
                   [](auto const &pair)
                   { return pair.first; });
    interrupts();

    /*
        Go through all messages we have stored and send them.
    */
    for (can_field::CAN_Fields key : keys)
    {
        auto found = mSubscriptions.find(key);
        if (found == mSubscriptions.end())
            continue;

        noInterrupts();
        AnyUnit value = mCurrentMessages[key];
        mCurrentMessages.erase(key);
        interrupts();

        for (auto callbackStatus : found->second)
        {
            callbackStatus->call(value);
        }
    }
}

void CAN_CallbackManager::handleMessage(int packetSize)
{
    unsigned long long message = 0;
    int i = 0;
    while (mMCP.available())
    {
        uint8_t val = mMCP.read();
        message = message | (static_cast<unsigned long long>(val) << (56 - (8 * i)));
        i++;
    }

    // Serial.println(std::bitset<64>(message).to_string().c_str());
    std::vector<MessageUnit> fields = mPhoneBook.call(mMCP.packetId(), message);

    // now we have the list of MessageUnits and we will need to call our subs with this.
    for (MessageUnit field : fields)
    {
        mCurrentMessages[field.mFieldName] = field.mValue;
    }
}

void CAN_CallbackManager::setContacts(std::unordered_map<long, MessageBook> aContacts)
{
    mPhoneBook.setContacts(aContacts);
}

MCP2515 *CAN_CallbackManager::getMCP2515()
{
    return &mMCP;
}