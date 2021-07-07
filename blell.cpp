//
// Created by dekel on 7/8/21.
//
#include "blell.hpp"
#include <string>
#include <optional>
#include <boost/algorithm/hex.hpp>

std::optional<blell> parseMessage(std::string && message){
    struct blell blell{};

    if(!message.starts_with("LL Data: ")){
        return std::nullopt;
    }
    else{
        std::cout << "dsadas";
        std::string bytesStr = message.substr(9);
        //Remove all whitespaces from string
        std::string::iterator end_pos = std::remove(bytesStr.begin(), bytesStr.end(), ' ');
        bytesStr.erase(end_pos, bytesStr.end());
        //char messageBytes[sizeof (struct blell)]{0};
        std::vector<uint8_t> messageBytes(bytesStr.size()/2); //Since it's HEX string
        std::string hash = boost::algorithm::unhex(bytesStr);
        std::copy(hash.begin(), hash.end(), messageBytes.begin());
        //access_address not returned by the btlejack
        size_t toCopy = messageBytes.size() - blell.l2cap.length;
        memcpy(&blell.header, messageBytes.data(), toCopy);
        //Continue parsing according to blell.l2cap.length
        blell.l2cap.bleatt.data.assign(messageBytes.begin() + toCopy, messageBytes.begin() + toCopy + blell.l2cap.length);
    }

    return blell;
}
#include "blell.hpp"
