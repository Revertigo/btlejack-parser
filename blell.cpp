//
// Created by dekel on 7/8/21.
//
#include "blell.hpp"
#include <string>
#include <optional>
#include <boost/algorithm/hex.hpp>
#include <boost/preprocessor.hpp>

#define X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE(r, data, elem)    \
    case elem : return BOOST_PP_STRINGIZE(elem);

#define DEFINE_ENUM_WITH_STRING_CONVERSIONS(name, enumerators)                \
    enum name {                                                               \
        BOOST_PP_SEQ_ENUM(enumerators)                                        \
    };                                                                        \
                                                                              \
    inline const char* ToString(name v)                                       \
    {                                                                         \
        switch (v)                                                            \
        {                                                                     \
            BOOST_PP_SEQ_FOR_EACH(                                            \
                X_DEFINE_ENUM_WITH_STRING_CONVERSIONS_TOSTRING_CASE,          \
                name,                                                         \
                enumerators                                                   \
            )                                                                 \
            default: return "[Unknown " BOOST_PP_STRINGIZE(name) "]";         \
        }                                                                     \
    }

//Link Layer Identifier
DEFINE_ENUM_WITH_STRING_CONVERSIONS(llid, (RESERVED)(DATA_PDU_CONTINUATION)
                                                            (DATA_PDU_START)(CONTROL_PDU))

//https://github.com/dlenski/ttblue/blob/master/att-types.h
enum btattMethod{
    BT_ATT_OP_ERROR_RSP = 1,
    BT_ATT_OP_FIND_INFO_REQ = 4,
    BT_ATT_OP_FIND_INFO_RSP,
    BT_ATT_OP_READ_REQ = 0x0a,
    BT_ATT_OP_READ_RSP,
    BT_ATT_OP_WRITE_REQ = 0x12,
    BT_ATT_OP_WRITE_RSP
};

//Channel Identifier
enum cid{
    ATTRIBUTE_PROTOCOL = 4, //Attribute protocol
    SIGNALING_CHANNEL,
    SMP     //Security Manager Protocol
};

//PDU types
enum pduPacketType{
    CONNECT_REQ = 5
};

std::optional<blell> parseMessage(std::string & message){
    struct blell blell{};

    std::string startStr{"LL Data: "};
    if(!message.starts_with(startStr)){
        return std::nullopt;
    }

    std::string bytesStr = message.substr(startStr.size());
    //Remove all whitespaces from string
    std::string::iterator end_pos = std::remove(bytesStr.begin(), bytesStr.end(), ' ');
    bytesStr.erase(end_pos, bytesStr.end());
    char messageBytes[sizeof (struct blell)]{};
    std::string unHexed = boost::algorithm::unhex(bytesStr);
    std::copy(unHexed.begin(), unHexed.end(), messageBytes);

    //We don't want to parse a connection packet
    if((messageBytes[0] & 0x0f) == CONNECT_REQ){
        return std::nullopt;
    }
    //access_address not returned by the btlejack tool
    size_t toCopy = sizeof(blell.header) + sizeof(blell.l2cap) - sizeof(blell.l2cap.bleatt.data);
    memcpy(&blell.header, messageBytes, toCopy);
    //Continue parsing according to blell.l2cap.length
    memcpy(&blell.l2cap.bleatt.data, messageBytes + toCopy,
           std::min<uint16_t>(blell.l2cap.length, sizeof(blell.l2cap.bleatt.data)));


    return blell;
}

uint16_t blell::toUint16(const uint8_t * value)
{
    return value[1] * 256 + value[0];
}

std::ostream & operator << (std::ostream & os, const struct blell blell){
    os << "Link Layer ID: ";
    os << ToString(static_cast<llid>(blell.header.llid)) << std::endl;
    os << "Payload Length: " << +blell.header.length << std::endl;
    os << "L2CAP Channel Identifier: ";
    os << (blell.l2cap.cid == ATTRIBUTE_PROTOCOL ? "Attribute Protocol" : "Not supported");
    os << std::endl << "L2CAP Payload Length: " << +blell.l2cap.length << std::endl;
    os << "Authentication Signature: ";
    os << (blell.l2cap.bleatt.opCode.authenticationSignature ? "True (signed by the sender)"
                                                             : "False (not signed by the sender)");
    os << std::endl << "Method: ";

    switch(blell.l2cap.bleatt.opCode.method) {
        case BT_ATT_OP_ERROR_RSP: {
            os << "Permissions Error";
            break;
        }
        case BT_ATT_OP_FIND_INFO_REQ: {
            os << "Service Discovery Request" << std::endl;
            uint16_t startHandle = blell.toUint16(&blell.l2cap.bleatt.data[0]);
            os << "Starting Handle: 0x" << std::hex << startHandle << std::endl;
            uint16_t endHandle = blell.toUint16(&blell.l2cap.bleatt.data[2]);
            os << "Ending handle: 0x" << std::hex << endHandle << std::dec;
            break;
        }
        case BT_ATT_OP_FIND_INFO_RSP: {
            os << "Service Discovery Response" << std::endl;
            os << "UUID Format: " << (blell.l2cap.bleatt.data[0] == 1 ? "16-bit UUIDs" : "128-bit UUIDs");
            uint16_t handle = blell.toUint16(&blell.l2cap.bleatt.data[1]);
            os << std::endl << "Handle: 0x" << std::hex << handle;
            uint16_t uuid = blell.toUint16(&blell.l2cap.bleatt.data[3]);
            os << std::endl << "UUID: 0x" << std::hex << uuid << std::dec;
            break;
        }
        case BT_ATT_OP_READ_REQ: {
            os << "Read Request" << std::endl;
            uint16_t handle = blell.toUint16(&blell.l2cap.bleatt.data[0]);
            os << "Read Handle: 0x" << std::hex << handle << std::dec;
            break;
        }
        case BT_ATT_OP_READ_RSP: {
            os << "Read Response" << std::endl;
            os << "Handle Response: " << blell.l2cap.bleatt.data;
            break;
        }
        case BT_ATT_OP_WRITE_REQ: {
            os << "WRITE Request" << std::endl;
            uint16_t handle = blell.toUint16(&blell.l2cap.bleatt.data[0]);
            os << "Handle: 0x" << std::hex << handle << std::dec << std::endl;
            os << "Data: " << (blell.l2cap.bleatt.data + 2);
            break;
        }
        case BT_ATT_OP_WRITE_RSP: {
            os << "WRITE response" << std::endl;
            os << "Handle Response: " << blell.l2cap.bleatt.data;
            break;
        }
        default:
        {
            os << "Not supported, 0x" << +blell.l2cap.bleatt.opCode.method << std::endl;
        }
    }

    return os;
}