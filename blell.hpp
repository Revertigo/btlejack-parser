//
// Created by dekel on 7/8/21.
//
#ifndef BTLEJACK_PARSER_BLELL_HPP
#define BTLEJACK_PARSER_BLELL_HPP

#include <iostream>
#include <vector>
#include <string>
#include <optional>
#include <stdint.h>

enum btattMethod{
    BT_ATT_OP_READ_REQ = 0x0a,
    BT_ATT_OP_READ_RSP,
    BT_ATT_OP_WRITE_REQ = 0x12,
    BT_ATT_OP_WRITE_RSP
};
//Channel Identifier
enum cid{
    ATT = 4, //Attribute protocol
    SIGNALING_CHANNEL,
    SMP //Security Manager Protocol
};

//Link Layer Identifier
enum llid{
    RESERVED,
    DATA_PDU_CONTINUATION,
    DATA_PDU_START,
    CONTROL_PDU
};

struct bleattOpCode{
    uint8_t method : 6;
    uint8_t command : 1;
    uint8_t authenticationSignature : 1;
}__attribute__((packed));

struct bleatt{
    struct bleattOpCode opCode;
    std::vector<uint8_t> data;
    //uint8_t data[2]; //TODO: Should be dynamically allocated
}__attribute__((packed));

struct blel2cap{
    uint16_t length;
    uint16_t cid; //Channel identifiers - Attribute Protocol (ATT) supported only
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

    friend std::ostream & operator << (std::ostream & os, const struct blell blell){
        os << "Link Layer ID: ";
        os << (blell.header.llid == CONTROL_PDU ? "Control PDU" : "Data PDU");
        os << std::endl << "Payload Length: " << +blell.header.length << std::endl;
        os << "L2CAP Channel Identifier ";
        os << (blell.l2cap.cid == ATT ? "Attribute Protocol" : "Not supported");
        os << std::endl << "L2CAP Payload Length: " << blell.l2cap.length << std::endl;
        os << "Authentication Signature: ";
        os << (blell.l2cap.bleatt.opCode.authenticationSignature ? "True (signed by the sender)"
                                                                 : "False (not signed by the sender)");
        os << std::endl << "Method: ";
        uint16_t data = blell.l2cap.bleatt.data[1] * 256 + blell.l2cap.bleatt.data[0];
        if(blell.l2cap.bleatt.opCode.method == BT_ATT_OP_READ_REQ){
            os << "Read Request";
            os << std::endl << "Read Handle: #" << data;
        }
        else if(blell.l2cap.bleatt.opCode.method == BT_ATT_OP_READ_RSP){
            os << "Read Response";
            os << std::endl << "Handle Response: " << data;
        }
        else{
            os << "Not Supported";
        }

        return os;
    }
}__attribute__((packed));

std::optional<blell> parseMessage(std::string && message);

#endif //BTLEJACK_PARSER_BLELL_HPP
