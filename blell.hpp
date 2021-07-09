//
// Created by dekel on 7/8/21.
//
#ifndef BTLEJACK_PARSER_BLELL_HPP
#define BTLEJACK_PARSER_BLELL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <cstdint>

#define BT_ATT_MAX_VALUE_LEN	512

struct bleattOpCode{
    uint8_t method : 6;
    uint8_t command : 1;
    uint8_t authenticationSignature : 1;
}__attribute__((packed));

struct bleatt{
    struct bleattOpCode opCode;
    uint8_t data[BT_ATT_MAX_VALUE_LEN];
}__attribute__((packed));

struct blel2cap{
    uint16_t length; //payload length
    uint16_t cid; //Channel identifiers - Attribute Protocol (ATTRIBUTE_PROTOCOL) supported only
    struct bleatt bleatt;
}__attribute__((packed));

struct llheader{
    uint8_t llid : 2; //Link Layer Data PDU or Link Layer control PDU. For more info view BLE Vol6 Section 2.4
    uint8_t nesn : 1; //Next Expected Sequence Number
    uint8_t sn   : 1; //Sequence Number
    uint8_t md   : 1; //More Data
    uint8_t rfu  : 3;
    uint8_t length;  //The Length field indicates the size, in octets, of the Payload and MIC, if included.
}__attribute__((packed));

struct blell{
    uint32_t access_address;
    struct llheader header;
    struct blel2cap l2cap;
    uint8_t crc[3];

    friend std::ostream & operator << (std::ostream & os, const struct blell blell);

private:
    uint16_t toUint16(const uint8_t * value) const;
}__attribute__((packed));

std::optional<blell> parseMessage(std::string & message);

#endif //BTLEJACK_PARSER_BLELL_HPP
