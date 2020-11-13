
#ifndef SCAMP_MODULE_PACKET_SWITCH_H
#define SCAMP_MODULE_PACKET_SWITCH_H

#define PY_SSIZE_T_CLEAN
#include <Python.h>

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

PyObject *decode_packet(uint8_t*packet_buffer,size_t packet_size);

#endif
