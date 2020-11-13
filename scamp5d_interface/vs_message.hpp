
#ifndef VS_MESSAGE_HPP
#define VS_MESSAGE_HPP

#include <cstdint>

#pragma pack(push)
#pragma pack(1)
struct vs_message_t{
    uint16_t route;
    uint8_t code[2];

    union{
        int8_t int8[4];
        int16_t int16[2];
        int32_t int32[1];
        uint8_t uint8[4];
        uint16_t uint16[2];
        uint32_t uint32[1];
    } data;

    inline void clear(){
    	uint64_t*p = (uint64_t*)this;
    	*p = 0;
    }

};
#pragma pack(pop)


#define VS_MSG_ROUTE_HOST      	0xCCDE
#define VS_MSG_ROUTE_DEVICE    	0xDECC
#define VS_MSG_ROUTE_M0       	0xD0CC
#define VS_MSG_ROUTE_M4       	0xD4CC
#define VS_MSG_ROUTE_APP       	VS_MSG_ROUTE_M0
#define VS_MSG_ROUTE_SYS       	VS_MSG_ROUTE_M4

#define VS_MSG_DEVICE_ON       	100
#define VS_MSG_DEVICE_DC       	101
#define VS_MSG_DEVICE_LC       	102
#define VS_MSG_DATA_STALL		120
#define VS_MSG_PACKET          	127
#define VS_MSG_USB_CONNECT     	128
#define VS_MSG_USB_DISCONNECT  	129
#define VS_MSG_HOST_ON         	130
#define VS_MSG_HOST_DC         	131
#define VS_MSG_GUI_REQUEST     	132
#define VS_MSG_GUI_UPDATE      	133
#define VS_MSG_USER_VALUE      	134
#define VS_MSG_HOST_VALUE      	135
#define VS_MSG_GUI_FEEDBACK    	136
#define VS_MSG_START_RECORDING  137
#define VS_MSG_STOP_RECORDING  	138
#define VS_MSG_RECORD_FRAME  	139

#define VS_MSG_START_M0        144
#define VS_MSG_STOP_M0         145
#define VS_MSG_FLASH_STREAM_BEGIN     146
#define VS_MSG_FLASH_STREAM_END       147
#define VS_MSG_REBOOT        	148


#endif
