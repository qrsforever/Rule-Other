/***************************************************************************
 *  DataChannel.cpp - Data Channel Impl
 *
 *  Created: 2018-06-15 17:43:34
 *
 *  Copyright QRS
 ****************************************************************************/

#include "DataChannel.h"

namespace HB {

std::string& stringTrim(std::string& text)
{
    if (!text.empty()) {
        text.erase(0, text.find_first_not_of((" \n\r\t\f\v")));
        text.erase(text.find_last_not_of((" \n\r\t\f\v")) + 1);
    }
    return text;
}

} /* namespace HB */
