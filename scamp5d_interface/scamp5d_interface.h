/*!

\file

\ingroup VS_HOST_MODULE

\author Jianing Chen

*/

#ifndef SCAMP5D_INTERFACE_H
#define SCAMP5D_INTERFACE_H

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <list>
#include <algorithm>
#include <functional>

#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "vs_message.hpp"
#include "vs_protocol.hpp"
#include "vs_protocol_gui.hpp"


/*!
Base API for access a Scamp5d vision system
*/
class scamp5d_interface{

public:

    static const size_t PACKET_BUFFER_SIZE = 400000;

    scamp5d_interface();
    virtual ~scamp5d_interface();

    /*!
    @brief open the device interface.

    Uses of the two arguments depend of any inheriting class of this base class.
    */
    int open(const char*arg1,uint32_t arg2);

    /*!
    @brief close the device interface
    */
    int close();

    /*!
    @brief routine function which process all internal mechanisms such as IO polls.
    */
    void routine();

    /*!
    @brief write data to the device, this function will copy the data
    */
    void write(const void*buffer,size_t bytes);

    /*!
    @brief send data to the device, this function will not copy the data
    */
    void send(const void*buffer,size_t bytes);

    /*!
    @brief send data to the device and 'free' the buffer, this function will not copy the data
    */
    void send_and_free(void*buffer,size_t bytes);

    /*!
    @brief send a text string packet
    */
    void post_string(const char*str,size_t length);

    /*!
    @brief send a message packet
    */
    void post_message(uint16_t route,uint8_t code,uint8_t arg=0,int32_t value=0);

    /*!
    @brief send an array of messages in a packet
    */
    void post_message(const vs_message_t*msg,size_t num=1);

    /*!
    @brief send raw binary data, use only when a file request is received
    */
    void post_file(const uint8_t*buffer, size_t n_bytes, uint32_t lc=0);

    /*!
    @brief send a grey scale for responding to a image request
    */
    void post_image(const uint8_t*buffer, size_t w, size_t h, uint8_t n_bits,uint32_t lc=0);

    /*!
    @brief convert a 24-bit rgb to grey scale for responding to a image request
    */
    void post_image_rgb(const uint8_t*buffer, size_t w, size_t h, uint8_t n_bits,uint32_t lc=0);

    /*!
    @brief setup a callback to handle all received packets
    */
    void on_receive_packet(std::function<void(const uint8_t*packet_buffer,size_t packet_size)> cb){
        if(cb!=NULL){
            cb_receive_packet = cb;
        }else{
            cb_receive_packet = [this](const uint8_t*packet_buffer,size_t packet_size){
                //printf("[scamp5d_interface] on_receive_packet: %d bytes\n",packet_size);
            };
        }
    }

    /*!
    @brief setup a callback to free received packets
    */
    void on_free_packet(std::function<void(uint8_t*packet_buffer,size_t packet_size)> cb){
        if(cb!=NULL){
            cb_free_packet = cb;
        }else{
            cb_free_packet = [this](uint8_t*packet_buffer,size_t packet_size){
                free(packet_buffer);
            };
        }
    }

    /*!
    @brief get the number of packet signature encountered in the IO stream
    */
    inline uint32_t get_signature_counter(){
        return in_protocol.signature_counter;
    }

    /*!
    @brief get the number of packet header encountered in the IO stream
    */
    inline uint32_t get_header_counter(){
        return in_protocol.header_counter;
    }

    /*!
    @brief get the number of packet received
    */
    inline uint32_t get_packet_counter(){
        return packet_counter;
    }

    /*!
    @brief a free-to-use lock for coordinating multi-threaded application
    */
    inline void session_lock(){
        session_mux.lock();
    }

    /*!
    @brief a free-to-use lock for coordinating multi-threaded application
    */
    inline void session_unlock(){
        session_mux.unlock();
    }


protected:

    struct queue_entry{
        uint8_t*buffer;
        size_t length;
        size_t cursor;
        bool need_free;
    };

    std::mutex session_mux;
    uint32_t packet_counter;

    std::list<queue_entry> in_queue;
    std::mutex in_queue_mux;

    std::list<queue_entry> out_queue;
    std::mutex out_queue_mux;

    vs_protocol_in in_protocol;
    vs_protocol_out out_protocol;

    std::function<void(const uint8_t*packet_buffer,size_t packet_size)> cb_receive_packet;
    std::function<void(uint8_t*packet_buffer,size_t packet_size)> cb_free_packet;

    void qout(uint8_t*buffer,size_t bytes,bool do_free);

    virtual int _driver_open(const char*arg1,uint32_t arg2) = 0;
    virtual int _driver_close() = 0;
    virtual int _driver_routine() = 0;

};

#endif
