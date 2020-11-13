/*!

\file

\ingroup VS_HOST_MODULE

\author Jianing Chen

*/

#ifndef VS_PACKET_DECODER_H
#define VS_PACKET_DECODER_H


#include <list>
#include <string>
#include <array>
#include <functional>

#include "vs_protocol.hpp"
#include "vs_message.hpp"
#include "vs_array.hpp"

/*!
The class for handling data packet received from the vision system
*/
class vs_packet_decoder{

public:

    static void decode_aout(uint8_t*result,const uint8_t*source,int rows,int cols,int orientation = 2);
    static void decode_dout(uint8_t*result,const uint8_t*source,int rows,int cols,int orientation = 2);


    enum result{ done, no_callback, packet_type_unknown, data_type_unknown };

    typedef std::function<void(int width,int height,uint8_t*bitmap_buffer)> image_callback;
    typedef std::function<void(vs_array<uint8_t> const& data)> vector_callback;
    typedef std::function<void(int width,int height,std::list<uint8_t*> const& bitmap_buffer_array)> bitstack_callback;
    typedef std::function<void(const vs_message_t*msg,size_t num)> message_callback;
    typedef std::function<void(const char*text,size_t length)> text_callback;
    typedef std::function<void(const wchar_t*text,size_t length)> wstr_callback;
    typedef std::function<void(const char*text,int32_t arg)> request_callback;
    typedef std::function<void(vs_array<int8_t> const& data)> data_int8_callback;
    typedef std::function<void(vs_array<int16_t> const& data)> data_int16_callback;
    typedef std::function<void(vs_array<int32_t> const& data)> data_int32_callback;
    typedef std::function<void(vs_array<float> const& data)> data_float_callback;
    typedef std::function<void(const uint8_t*packet,size_t packet_size)> raw_callback;

    vs_packet_decoder();

    /*!
        \brief process a packet's content and call any matched callback function.
        \param packet_buffer    pointer to a packet
        \param packet_size      size of the packet
        \param sn               serial number of the packet (optional)
        \return returns the result of process
    */
    result decode_packet(const uint8_t*packet_buffer,size_t packet_size,uint32_t sn = 0);// return true if delievered to any callback functions successfully

    /*!
        \brief callback for message packet (from ::vs_post_message)
    */
    void case_message(message_callback func);

    /*!
        \brief callback for text packet (from ::vs_post_text)
    */
    void case_text(text_callback func);

    /*!
        \brief callback for SCAMP5 AREG image packet (from ::scamp5_output_image)
    */
    void case_analog(image_callback func);

    /*!
        \brief callback for SCAMP5 DREG image packet (from ::scamp5_output_image)
    */
    void case_digital(image_callback func);

    /*!
        \brief callback for SCAMP5 boundingbox packet (from ::scamp5_output_boundingbox)
    */
    void case_boundingbox(vector_callback func);

    /*!
        \brief callback for SCAMP5 events coordinates image packet (from ::scamp5_output_events)
    */
    void case_points(vector_callback func);

    /*!
        \brief callback for SCAMP5 events bistack image packet (from ::scamp5_output_bitstack_begin)
    */
    void case_bitstack(bitstack_callback func);

    /*!
        \brief callback for dotmat image packet (from ::vs_post_dotmat)
    */
    void case_dotmat(image_callback func);

    /*!
        \brief callback for int8 array packet (from ::vs_post_int8)
    */
    void case_data_int8(data_int8_callback func);

    /*!
        \brief callback for int16 array packet (from ::vs_post_int16)
    */
    void case_data_int16(data_int16_callback func);

    /*!
        \brief callback for int32 array packet (from ::vs_post_int32)
    */
    void case_data_int32(data_int32_callback func);

    /*!
        \brief callback for floating point value array packet (from ::vs_post_float)
    */
    void case_data_float(data_float_callback func);

    /*!
    \brief packet callback for GUI setup information
    */
    void case_gui(raw_callback func);

    /*!
    \brief packet callback for image request, the integer argument given is the number of bits starting from the highest bit
    */
    void case_request_image(request_callback func);

    /*!
    \brief packet callback for file request, the integer argument given is the maximum bytes allowed in a chunk when sending
    */
    void case_request_file(request_callback func);

    /*!
    \brief packet constructed with unknow/raw packet_type
    */
    void case_raw(raw_callback func);

    /*!
    \brief get the loop counter value on the device when the packet was dispatched
    */
    uint32_t get_loop_counter();

    /*!
    \brief get the display handle value of the image packet
    */
    uint32_t get_display_handle();

    /*!
    \brief get the data channel value set by the device for the data packet
    */
    uint32_t get_data_channel();


    inline uint32_t get_packet_sn(){
        if(data_packet){
            return packet_sn;
        }
        return 0;
    }
    
    inline const vs_packet_header_t* get_packet_header(){
        if(data_packet){
            return &data_packet->header;
        }
        return NULL;
    }


protected:

    struct vs_data_packet_map{
        vs_packet_header_t header;
        vs_data_header_t data_def;
        uint8_t data[65536];// this size is a placeholder, packets are open ended
    };

    uint32_t packet_sn;
    vs_data_packet_map *data_packet;

    raw_callback func_raw;
    raw_callback func_gui;
    request_callback func_request_image;
    request_callback func_request_file;
    image_callback func_analog;
    image_callback func_digital;
    image_callback func_dotmat;
    vector_callback func_aabb;
    vector_callback func_points;
    bitstack_callback func_bitstack;
    message_callback func_message;
    text_callback func_text;
    wstr_callback func_wstr;
    data_int8_callback func_data_int8;
    data_int16_callback func_data_int16;
    data_int32_callback func_data_int32;
    data_float_callback func_data_float;

};


#endif
