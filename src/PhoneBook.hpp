/*
    What is a way to create an easy to edit map for sender ID to message look up?
        Maybe just a static map?
*/
#ifndef PHONE_BOOK_HPP
#define PHONE_BOOK_HPP

#include <unordered_map>
#include <unordered_set>
#include "Message.hpp"
#include "MessageBook.hpp"
#include "CAN_Config.hpp"

class PhoneBook
{

public:
    PhoneBook() {};

    std::vector<MessageUnit> call(long aFrom, unsigned long long aMsg)
    {
        auto const contact = mContacts.find(aFrom);
        // lookup the reciever and give them the raw message to parse.
        if (contact != mContacts.end())
        {
            return contact->second.handleMessage(aMsg);
        }

        Serial.print("No contact found for: ");
        Serial.println(aFrom);
        return {};
    }

    void setContacts(std::unordered_map<long, MessageBook> aContacts)
    {
        mContacts = aContacts;
    }

private:
    std::unordered_map<long, MessageBook> mContacts = {};
};

#endif