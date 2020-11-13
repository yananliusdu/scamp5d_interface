#pragma once

#include "scamp5d_interface.h"
#include <string>

class scamp5d_vcom:public scamp5d_interface{

public:

    scamp5d_vcom();
    ~scamp5d_vcom();

    // implement all virtual functions

    int _connect_(const char*port_name);
    void _disconnect_();
    bool _is_connected_();
    int _set_block_io_(bool k);
    int _send_(const void*data,size_t bytes);
    bool _send_done_();
    int _receive_(void*buffer,size_t buffer_size,size_t*bytes_received);
    
protected:
    void *serial_api;

};
