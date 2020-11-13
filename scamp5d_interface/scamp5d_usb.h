/*!

\file

\ingroup VS_HOST_MODULE

\author Jianing Chen

*/
#ifndef SCAMP5D_USB_H
#define SCAMP5D_USB_H


#include "scamp5d_interface.h"


/*!
Scamp5d Vision System Interface via USB
*/
class scamp5d_usb:public scamp5d_interface{

public:

    scamp5d_usb();
    ~scamp5d_usb();

protected:

    std::thread *out_thread;
    std::atomic<bool> out_busy;
    std::thread *in_thread;
    std::atomic<bool> in_busy;

    // implement all virtual functions

    void *icp;
    int _driver_open(const char*arg1,uint32_t arg2);
    int _driver_close();
    int _driver_routine();

};

#endif
