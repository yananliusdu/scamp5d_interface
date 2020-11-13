

#include <vs_packet_decoder.hpp>
#include "scampmodule_packet_switch.hpp"


// setup all callback functions in the class constructor
class python_module_packet_switch:public vs_packet_decoder{

public:
    python_module_packet_switch();

    PyObject* ret;

};


python_module_packet_switch::python_module_packet_switch(){

    case_text([this](const char*text,size_t length){
        ret = Py_BuildValue("{ss,si,ss,si,si, ss#}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","TEXT",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "text",text,length
        );
    });

    case_analog([this](int width,int height,uint8_t*bitmap_buffer){
        ret = Py_BuildValue("{ss,si,ss,si,si, si,si,sy#}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","SCAMP5_AOUT",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "width",width,
            "height",height,
            "buffer",bitmap_buffer,size_t(width*height)
        );
    });

    case_digital([this](int width,int height,uint8_t*bitmap_buffer){
        ret = Py_BuildValue("{ss,si,ss,si,si, si,si,sy#}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","SCAMP5_DOUT",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "width",width,
            "height",height,
            "buffer",bitmap_buffer,size_t(width*height)
        );
    });

    case_boundingbox([this](const vs_array<uint8_t>&data){
        PyObject *lst = PyList_New(data.n_rows());
        for(int i=0;i<data.n_rows();i++){
            PyObject *vec = Py_BuildValue("(ii)",data(i,0),data(i,1));
            PyList_SET_ITEM(lst, i, vec);
        }
        ret = Py_BuildValue("{ss,si,ss,si,si, si,si,sO}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","BBOX",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "n_rows",(int)data.n_rows(),
            "n_cols",(int)data.n_cols(),
            "data",lst
        );
    });

    case_points([this](const vs_array<uint8_t>&data){
        PyObject *lst = PyList_New(data.n_rows());
        for(int i=0;i<data.n_rows();i++){
            PyObject *vec = Py_BuildValue("(ii)",data(i,0),data(i,1));
            PyList_SET_ITEM(lst, i, vec);
        }
        ret = Py_BuildValue("{ss,si,ss,si,si, si,si,sO}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","EVENTS",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "n_rows",(int)data.n_rows(),
            "n_cols",(int)data.n_cols(),
            "data",lst
        );
    });

    case_data_int8([this](const vs_array<int8_t>&data){
        PyObject *lst = PyList_New(data.n_rows());
        for(int i=0;i<data.n_rows();i++){
            PyObject *row = PyList_New(data.n_cols());
            for(int j=0;j<data.n_cols();j++){
                PyList_SET_ITEM(row, j, PyLong_FromLong(data(i,j)));
            }
            PyList_SET_ITEM(lst, i, row);
        }
        ret = Py_BuildValue("{ss,si,ss,si,si, si,si,sO}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","INT8",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "n_rows",(int)data.n_rows(),
            "n_cols",(int)data.n_cols(),
            "data",lst
        );
    });

    case_data_int16([this](const vs_array<int16_t>&data){
        PyObject *lst = PyList_New(data.n_rows());
        for(int i=0;i<data.n_rows();i++){
            PyObject *row = PyList_New(data.n_cols());
            for(int j=0;j<data.n_cols();j++){
                PyList_SET_ITEM(row, j, PyLong_FromLong(data(i,j)));
            }
            PyList_SET_ITEM(lst, i, row);
        }
        ret = Py_BuildValue("{ss,si,ss,si,si, si,si,sO}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","INT16",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "n_rows",(int)data.n_rows(),
            "n_cols",(int)data.n_cols(),
            "data",lst
        );
    });

    case_data_int32([this](const vs_array<int32_t>&data){
        PyObject *lst = PyList_New(data.n_rows());
        for(int i=0;i<data.n_rows();i++){
            PyObject *row = PyList_New(data.n_cols());
            for(int j=0;j<data.n_cols();j++){
                PyList_SET_ITEM(row, j, PyLong_FromLong(data(i,j)));
            }
            PyList_SET_ITEM(lst, i, row);
        }
        ret = Py_BuildValue("{ss,si,ss,si,si, si,si,sO}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","INT32",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "n_rows",(int)data.n_rows(),
            "n_cols",(int)data.n_cols(),
            "data",lst
        );
    });

    case_data_float([this](const vs_array<float>&data){
        PyObject *lst = PyList_New(data.n_rows());
        for(int i=0;i<data.n_rows();i++){
            PyObject *row = PyList_New(data.n_cols());
            for(int j=0;j<data.n_cols();j++){
                PyList_SET_ITEM(row, j, PyFloat_FromDouble(data(i,j)));
            }
            PyList_SET_ITEM(lst, i, row);
        }
        ret = Py_BuildValue("{ss,si,ss,si,si, si,si,sO}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","FLOAT",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "n_rows",(int)data.n_rows(),
            "n_cols",(int)data.n_cols(),
            "data",lst
        );
    });

    case_request_image([this](const char*filepath,int32_t n_bits){
        ret = Py_BuildValue("{ss,si,ss,si,si, ss,ss,si}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","REQUEST",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "filetype","IMAGE",
            "filepath",filepath,
            "n_bits",int(n_bits)
        );
    });

    case_request_file([this](const char*filepath,int32_t chunksize){
        ret = Py_BuildValue("{ss,si,ss,si,si, ss,ss,si}",
            "type","data",
            "size",int(get_packet_header()->payload_size),
            "datatype","REQUEST",
            "loopcounter",(int)get_loop_counter(),
            "channel",(int)get_data_channel(),
            "filetype","FILE",
            "filepath",filepath,
            "chunksize",int(chunksize)
        );
    });
    
}


PyObject *decode_packet(uint8_t*packet_buffer,size_t packet_size){
    static python_module_packet_switch packet_switch;
    static uint32_t packet_counter = 0;

    auto r = packet_switch.decode_packet(packet_buffer,packet_size,packet_counter++);

    if(r==python_module_packet_switch::done){
        return packet_switch.ret;
    }else{
        auto header = (vs_packet_header_t*)packet_buffer;
        int payload_size = header->payload_size;
        auto payload = packet_buffer + sizeof(vs_packet_header_t);
        return Py_BuildValue("{ss,si,sy#}",
            "type","unknown",
            "size",payload_size,
            "payload",payload,payload_size
        );
    }
}

