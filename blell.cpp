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

std::optional<blell> parseMessage(std::string & message){
    struct blell blell{};

    if(!message.starts_with("LL Data: ")){
        return std::nullopt;
    }
    else{
        std::string bytesStr = message.substr(9);
        //Remove all whitespaces from string
        std::string::iterator end_pos = std::remove(bytesStr.begin(), bytesStr.end(), ' ');
        bytesStr.erase(end_pos, bytesStr.end());
        char messageBytes[sizeof (struct blell)]{};
        std::string unHexed = boost::algorithm::unhex(bytesStr);
        std::copy(unHexed.begin(), unHexed.end(), messageBytes);

        //access_address not returned by the btlejack
        size_t toCopy = sizeof(blell.header) + sizeof(blell.l2cap) - sizeof(blell.l2cap.bleatt.data);
        memcpy(&blell.header, messageBytes, toCopy);
        //Continue parsing according to blell.l2cap.length
        memcpy(&blell.l2cap.bleatt.data, messageBytes + toCopy, blell.l2cap.length);
    }

    return blell;
}

uint16_t blell::toUint16(const uint8_t * value) const
{
    return value[1] * 256 + value[0];
}

std::ostream & operator << (std::ostream & os, const struct blell blell){
    os << "Link Layer ID: ";
    os << ToString(static_cast<llid>(blell.header.llid));
    os << std::endl << "Payload Length: " << +blell.header.length << std::endl;
    os << "L2CAP Channel Identifier: ";
    os << (blell.l2cap.cid == ATTRIBUTE_PROTOCOL ? "Attribute Protocol" : "Not supported");
    os << std::endl << "L2CAP Payload Length: " << blell.l2cap.length << std::endl;
    os << "Authentication Signature: ";
    os << (blell.l2cap.bleatt.opCode.authenticationSignature ? "True (signed by the sender)"
                                                             : "False (not signed by the sender)");
    os << std::endl << "Method: ";
    uint16_t handle = blell.toUint16(&blell.l2cap.bleatt.data[0]);
    switch(blell.l2cap.bleatt.opCode.method) {
        case BT_ATT_OP_ERROR_RSP:
            os << "Error Write To Handle";
            break;
        case BT_ATT_OP_FIND_INFO_REQ:
            os << "Service Discovery Request" << std::endl;
            os << "Starting Handle: " << handle << std::endl;
            os << "Ending handle: " << blell.toUint16(&blell.l2cap.bleatt.data[2]);
            break;
        case BT_ATT_OP_FIND_INFO_RSP:
            os << "Service Discovery Response" << std::endl;
            os << "UUID Format: " << (blell.l2cap.bleatt.data[0] == 1 ? "16-bit UUIDs" : "128-bit UUIDs");
            os << std::endl << "Handle: " << blell.toUint16(&blell.l2cap.bleatt.data[1]);
            os << std::endl << "UUID: 0x" << std::hex << blell.toUint16(&blell.l2cap.bleatt.data[3]);
            break;
        case BT_ATT_OP_READ_REQ:
            os << "Read Request" << std::endl;
            os << "Read Handle: #" << handle;
            break;
        case BT_ATT_OP_READ_RSP:
            os << "Read Response" << std::endl;
            os << "Handle Response: " << blell.l2cap.bleatt.data;
            break;
        case BT_ATT_OP_WRITE_REQ:
            os << "WRITE Request" << std::endl;
            os << "Handle: #" << handle << std::endl;
            os << "Data: " << (blell.l2cap.bleatt.data + 2);
            break;
        case BT_ATT_OP_WRITE_RSP:
            os << "WRITE response" << std::endl;
            os << "Handle Response: " << blell.l2cap.bleatt.data;
            break;
    }

    return os;
}