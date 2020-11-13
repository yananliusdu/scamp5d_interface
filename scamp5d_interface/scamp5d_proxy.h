/*!

\file
\ingroup VS_HOST_MODULE

A TCP proxy for relaying data to any connected Scamp5d TCP Interface (::scamp5d_tcp). 
Based on libuv.

\author Jianing Chen

*/
#ifndef SCAMP5D_PROXY_H
#define  SCAMP5D_PROXY_H

#include "scamp5d_interface.h"


/*!
API Class of the proxy
*/
class scamp5d_proxy{

public:

    scamp5d_proxy();
    ~scamp5d_proxy();

    /*!
    @brief open TCP proxy
    */
    void open(const char*ip,int port);

    /*!
    @brief close TCP proxy
    */
    void close();

    /*!
    @brief process TCP proxy
    */
    void routine();

    /*!
    @brief broadcast data to all connected clients and callback when done

    The third argument given to the callback function is the result. 0 means successful. 
    */
    void broadcast(uint8_t*data,size_t bytes,std::function<void(uint8_t*,size_t,int)> cb);

    /*!
    @brief broadcast data to all connected clients and optionally 'free' the data buffer

    If \p free_data is set to \c true, the data buffer will be freed after the broadcast is finished. 
    */
    void broadcast(uint8_t*data,size_t bytes,bool free_data);

    /*!
    @brief callback to execute when a packet is received from a client.
    */
    void on_receive_packet(std::function<void(const uint8_t*buffer,size_t bytes)> cb){
        cb_receive_packet = cb;
    };

    /*!
    @brief callback to execute when a string need to be displayed
    */
    void on_print(std::function<void(const char*str,int type)> cb){
        cb_print = cb;
    };

    /*!
    @brief get number of connected clients 
    */
    inline size_t get_client_count(){
        return client_list.size();
    };

    /*!
    @brief get size of the data in dispatch queue
    */
    inline size_t get_bytes_in_queue(){
        return bytes_in_queue;
    }


protected:

    static const size_t CLIENT_PACKET_BUFFER_SIZE = 200*1024;

    class client{

    public:
        void *tcp;
        void *server;
        int state;
        vs_protocol_in in_protocol;
        uint8_t *in_buffer;

    };


    client* create_client();
    void delete_client(client*p);
    void read_client(client*p);

    char print_buffer[256];
    void *context;
    void *tcp;
    std::list<client*> client_list;
    std::atomic<size_t> bytes_in_queue;
    std::function<void(const char*,int)> cb_print;
    std::function<void(const uint8_t*,size_t)> cb_receive_packet;

};


#endif
