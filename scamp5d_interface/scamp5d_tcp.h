/*!

\file

\ingroup VS_HOST_MODULE

\author Jianing Chen

*/
#ifndef SCAMP5D_TCP_H
#define SCAMP5D_TCP_H


#include "scamp5d_interface.h"


/*!
Scamp5d Vision System Interface via TCP
*/
class scamp5d_tcp:public scamp5d_interface{

public:

    scamp5d_tcp();
    ~scamp5d_tcp();
    

protected:

    void*connection;

    // implement all virtual functions

    int _driver_open(const char*arg1,uint32_t arg2);
    int _driver_close();
    int _driver_routine();

};

#endif
