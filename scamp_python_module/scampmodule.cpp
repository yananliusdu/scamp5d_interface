
#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include <scamp5d_tcp.h>
#include <scamp5d_usb.h>
#include <vs_packet_decoder.hpp>

#include "scampmodule_packet_switch.hpp"


scamp5d_interface *box = NULL;
std::list<std::pair<uint8_t*,size_t>> packet_list;


static void packet_sink_callback(uint8_t*packet_buffer,size_t packet_size){
    packet_list.emplace_back();
    packet_list.back().first = packet_buffer;
    packet_list.back().second = packet_size;
}


PyObject *scamp_open_usb(PyObject *self, PyObject *args){
    const char *str;
    int result;

    if(!PyArg_ParseTuple(args, "s", &str)){
        return NULL;
    }

    box = new scamp5d_usb();
    result = box->open(str,0);

    if(result==0){
        box->on_free_packet(packet_sink_callback);
        box->routine();
    }

    return PyLong_FromLong(result);
}


PyObject *scamp_open_tcp(PyObject *self, PyObject *args){
    const char *str = "127.0.0.1";
    const unsigned int value = 27888;
    int result;

    if(!PyArg_ParseTuple(args, "sI", &str, &value)){
        return NULL;
    }

    box = new scamp5d_tcp();
    result = box->open(str,value);

    if(result==0){
        box->on_free_packet(packet_sink_callback);
        box->routine();
    }

    return PyLong_FromLong(result);
}


PyObject *scamp_close(PyObject *self, PyObject *args){
    int result = -1;
    if(box!=NULL){
        box->routine();
        result = box->close();
        delete box;
        box = NULL;
        while(packet_list.size()>0){
            free(packet_list.front().first);
            packet_list.pop_front();
        }
    }
    return PyLong_FromLong(result);
}


PyObject *scamp_routine(PyObject *self, PyObject *args){
    if(box!=NULL){
        box->routine();
    }
    Py_INCREF(Py_None);
    return Py_None;
}


PyObject *scamp_get_packet_raw(PyObject *self, PyObject *args){
    if(box==NULL){
        Py_INCREF(Py_None);
        return Py_None;
    }
    box->routine();
    if(packet_list.size()==0){
        Py_INCREF(Py_None);
        return Py_None;
    }
    auto buffer = packet_list.front().first;
    auto n_bytes = packet_list.front().second;
    packet_list.pop_front();
    auto p = PyByteArray_FromStringAndSize((char*)buffer,n_bytes);
    free(buffer);
    return p;
}



PyObject *scamp_get_packet(PyObject *self, PyObject *args){
    // if(!PyArg_ParseTuple(args, "Y", &packet)){
    //     Py_INCREF(Py_None);
    //     return Py_None;
    // }
    // auto n_bytes = (size_t)PyByteArray_Size((PyObject*)packet);
    // auto data = (uint8_t*)PyByteArray_AsString((PyObject*)packet);
    if(box==NULL){
        Py_INCREF(Py_None);
        return Py_None;
    }
    box->routine();
    if(packet_list.size()==0){
        Py_INCREF(Py_None);
        return Py_None;
    }
    auto buffer = packet_list.front().first;
    auto n_bytes = packet_list.front().second;
    packet_list.pop_front();

    PyObject* ret = decode_packet(buffer,n_bytes);
    
    free(buffer);
    return ret;
}


PyObject *scamp_send_message(PyObject *self, PyObject *args){
    uint8_t code;
    uint8_t arg;
    int data;

    if(!PyArg_ParseTuple(args, "bbi", &code, &arg, &data)){
        return NULL;
    }

    box->post_message(VS_MSG_ROUTE_M0,code,arg,data);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject *scamp_send_gui_value(PyObject *self, PyObject *args){
    uint8_t item_id;
    int value;

    if(!PyArg_ParseTuple(args, "bi", &item_id, &value)){
        return NULL;
    }

    box->post_message(VS_MSG_ROUTE_M0,VS_MSG_GUI_UPDATE,item_id,value);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject *scamp_send_image(PyObject *self, PyObject *args){
    Py_buffer buf;
    int width,height,n_bits;
    if(!PyArg_ParseTuple(args, "y*iii", &buf, &width, &height, &n_bits)){
        return NULL;
    }

    if(buf.len==3*width*height){
        box->post_image_rgb((uint8_t*)buf.buf,width,height,n_bits);
    }else{
        box->post_image((uint8_t*)buf.buf,width,height,n_bits);
    }

    PyBuffer_Release(&buf);

    Py_INCREF(Py_None);
    return Py_None;
}


PyObject *scamp_send_file(PyObject *self, PyObject *args){
    Py_buffer buf;
    if(!PyArg_ParseTuple(args, "y*", &buf)){
        return NULL;
    }

    box->post_file((uint8_t*)buf.buf,buf.len);

    PyBuffer_Release(&buf);

    Py_INCREF(Py_None);
    return Py_None;
}


//------------------------------------------------------------------------------


static const PyMethodDef ScampMethods[] = {
    {"open_usb",  scamp_open_usb, METH_VARARGS, "open a USB direct connection."},
    {"open_tcp",  scamp_open_tcp, METH_VARARGS, "open a TCP connection to a proxy."},
    {"close",  scamp_close, METH_VARARGS, "close the connection."},
    {"get_packet",  scamp_get_packet, METH_VARARGS, "get a new packet if there is one."},
    {"get_packet_raw",  scamp_get_packet_raw, METH_VARARGS, "get a new packet as bytearray if there is one."},
    {"send_gui_value",  scamp_send_gui_value, METH_VARARGS, "send the value of a gui item."},
    {"send_message",  scamp_send_message, METH_VARARGS, "send a message."},
    {"send_image",  scamp_send_image, METH_VARARGS, "send an greyscale/rgb image to respond a request."},
    {"send_file",  scamp_send_file, METH_VARARGS, "send a file to respond a request."},
    {"routine",  scamp_routine, METH_VARARGS, "processing routine."},
    {NULL, NULL, 0, NULL}
};


static struct PyModuleDef scampmodule = {
    PyModuleDef_HEAD_INIT,
    "scamp",
    "to interface with SCAMP vision systems",
    -1,
    (PyMethodDef*)ScampMethods
};


#define EXPORT_CONSTANT(m)  PyModule_AddIntConstant(p, #m, m)

PyMODINIT_FUNC PyInit_scamp(void){
    PyObject*p = PyModule_Create(&scampmodule);

    EXPORT_CONSTANT(VS_MSG_PACKET);
    EXPORT_CONSTANT(VS_MSG_HOST_ON);
    EXPORT_CONSTANT(VS_MSG_HOST_DC);
    EXPORT_CONSTANT(VS_MSG_GUI_UPDATE);
    EXPORT_CONSTANT(VS_MSG_USER_VALUE);
    EXPORT_CONSTANT(VS_MSG_HOST_VALUE);

    return p;
}

