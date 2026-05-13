/*
    Class that will be a map of messages from this sender. Base class that will be derived by children.
*/

#ifndef MESSAGE_BOOK_HPP
#define MESSAGE_BOOK_HPP

#include "Message.hpp"
#include <unordered_map>

class MessageBook
{
public:
    MessageBook(const std::unordered_map<int, Message> aMessages)
    {
        mMessages = aMessages;
    };

    MessageBook(const std::unordered_map<int, Message> aMessages, MessageParam aID_Location)
    {
        mMessages = aMessages;
        mID_Location = aID_Location;
    };

    std::vector<MessageUnit> handleMessage(unsigned long long aMsg) const
    {
        // some message don't support ID. In this case there should only
        // be one message in the book and it should be set to 0 as the ID.
        if (mID_Location.mFieldName == can_field::NOT_USED)
        {
            return mMessages.find(0)->second.parseMessage(aMsg);
        }

        uint8_t msgID = aMsg >> (64 - mID_Location.mStartBit - mID_Location.mBitWidth);

        auto const msg = mMessages.find(msgID);
        // lookup the reciever and give them the raw message to parse.
        if (msg != mMessages.end())
        {
            return msg->second.parseMessage(aMsg);
        }

        Serial.println("No message found");
        return {};
    }

private:
    std::unordered_map<int, Message> mMessages;
    MessageParam mID_Location;
};

#endif