
#ifndef VS_PROTOCOL_HPP
#define VS_PROTOCOL_HPP

#include <cstdint>
#include <cstdlib>
#include <cstring>

#include <functional>

#define SCAMP5_PE_ROWS  		256
#define SCAMP5_PE_COLS  		256
#define SCAMP5_IO_BUFFER_SIZE	8192

#define SCAMP5D_USB_VID			0x1FC9
#define SCAMP5D_USB_PID			0x008A


#define VS_PACKET_TYPE_STR      0x01
#define VS_PACKET_TYPE_WSTR     0x02
#define VS_PACKET_TYPE_MSG      0x07
#define VS_PACKET_TYPE_DATA     0x10
#define VS_PACKET_TYPE_SCAMP   	0x11
#define VS_PACKET_TYPE_RAW      0xFF

#define VS_DATA_MSG        	    1
#define VS_DATA_GUI     		2
#define VS_DATA_SCAMP5_AOUT     3
#define VS_DATA_SCAMP5_DOUT     4
#define VS_DATA_SCAMP5_AABB     5
#define VS_DATA_SCAMP5_EVENTS   6
#define VS_DATA_SCAMP5_BITSTACK 7
#define VS_DATA_TEXT           	8
#define VS_DATA_INT8            9
#define VS_DATA_INT16           10
#define VS_DATA_INT32           11
#define VS_DATA_FLOAT           12
#define VS_DATA_DOTMAT          13
#define VS_DATA_STYLE     		14
#define VS_DATA_FILEPATH		15
#define VS_DATA_FLASH_PAGE      16
#define VS_DATA_FLASH_STREAM    17
#define VS_DATA_BULK_M4    		18
#define VS_DATA_BULK_M0    		19
#define VS_DATA_REQUEST    		20
#define VS_DATA_SCAMP5_IMAGE_PATCH	33

#pragma pack(push)
#pragma pack(1)

struct vs_packet_header_t{
    uint8_t signature[3];
    uint8_t packet_type;
    uint32_t payload_size:24;
    uint32_t checksum:8;
};

struct vs_data_header_t{
    uint32_t loop_counter;
    uint8_t type;
    uint8_t version;
    uint16_t dim_size[3];
    uint32_t channel;
};

#pragma pack(pop)


//------------------------------------------------------------------------------


class vs_protocol{

protected:

    int32_t error;
    uint32_t version;

    uint8_t* packet_buffer;
    size_t packet_buffer_size;


public:

    static const uint32_t PAYLOAD_SIZE_LIMIT;
    static const uint8_t PACKET_SIGNATURE[3];

    static bool check_header(const void*p);
    static void encode_header(void*p,uint8_t packet_type,size_t payload_bytes);

    static inline size_t get_header_size(){
        return sizeof(vs_packet_header_t);
    }

    vs_protocol();

    inline void bind_buffer(void*buffer,size_t bytes){
        packet_buffer = (uint8_t*)buffer;
        packet_buffer_size = bytes;
    }

    inline uint8_t* get_buffer(){
        return packet_buffer;
    }

    inline size_t get_buffer_size(){
        return packet_buffer_size;
    }

};


//------------------------------------------------------------------------------


class vs_protocol_in:public vs_protocol{

protected:

    int32_t packet_state;
    size_t packet_size;
    size_t packet_cur;

    std::function<void(vs_protocol_in*,uint8_t*,size_t)> packet_callback;
    std::function<void(vs_protocol_in*,uint8_t*,size_t)> header_callback;


public:

    uint32_t signature_counter;
    uint32_t header_counter;

    vs_protocol_in();

    void reset();
    int input(const uint8_t*chunk,size_t bytes);

    inline vs_packet_header_t* get_packet_header(){
        return (vs_packet_header_t*)packet_buffer;
    }

    inline uint8_t* get_packet_payload(){
        return packet_buffer + sizeof(vs_packet_header_t);
    }

    inline size_t get_packet_size(){
        return packet_size;
    }
    
    inline void on_header(std::function<void(vs_protocol_in*p,uint8_t*packet_buffer,size_t packet_length)> cb){
        header_callback = cb;
    }

    inline void on_packet(std::function<void(vs_protocol_in*p,uint8_t*packet_buffer,size_t packet_length)> cb){
        packet_callback = cb;
    }

};


//------------------------------------------------------------------------------


class vs_protocol_out:public vs_protocol{

protected:

    uint8_t*start_ptr;
    uint8_t*buffer_ptr;
    uint8_t pkt_type;

    std::function<void(vs_protocol_out*,uint8_t*,size_t)> dispatch_callback;


public:

    vs_protocol_out(){
        start_ptr = NULL;
        buffer_ptr = NULL;
        pkt_type = 0;
    }

    void dispatch();

    inline void reset_buffer(){
        start_ptr = NULL;
        buffer_ptr = NULL;
        pkt_type = 0;
    }

    inline void packet_begin(uint8_t packet_type){
        if(buffer_ptr==NULL){
            start_ptr = packet_buffer;
            buffer_ptr = packet_buffer + sizeof(vs_packet_header_t);
        }else{
            start_ptr = buffer_ptr;
            buffer_ptr = buffer_ptr + sizeof(vs_packet_header_t);
        }
        pkt_type = packet_type;
    }

    inline void* packet_allocate(size_t bytes){
        uint8_t*p = buffer_ptr;
        buffer_ptr += bytes;
        return p;
    }

    inline void packet_load(const void*data,size_t bytes){
        memcpy(buffer_ptr,data,bytes);
        buffer_ptr += bytes;
    }

    inline void packet_end(void){
        size_t packet_size = buffer_ptr - start_ptr;
        size_t payload_size = packet_size - sizeof(vs_packet_header_t);
        encode_header(start_ptr,pkt_type,payload_size);
    }

    inline size_t get_packet_size(void){
        size_t packet_size = buffer_ptr - start_ptr;
        return packet_size;
    }

    inline void on_dispatch(std::function<void(vs_protocol_out*p,uint8_t*packet_buffer,size_t packet_length)> cb){
        dispatch_callback = cb;
    }

};


//------------------------------------------------------------------------------


#endif
