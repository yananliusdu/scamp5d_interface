
#include <cstdlib>
#include "scamp5d_tcp.h"
#include "scamp5d_usb.h"
#include "scamp5d_proxy.h"
#include "vs_packet_decoder.hpp"
#include "debug_functions.h"


#define ENABLE_PROXY


scamp5d_proxy *proxy;
scamp5d_interface*box;
vs_packet_decoder*packet_switch;

std::atomic<bool> quit(false);
bool host_on = false;
char filepath[256];
uint8_t bitmap24_buffer[256*256*3];


void setup_packet_switch(){

    // text are from the 'vs_post_text' function
    packet_switch->case_text([&](const char*text,size_t length){
        auto sn = packet_switch->get_packet_sn();
        auto lc = packet_switch->get_loop_counter();
        printf("packet[%d]: text, lc=%d: \n%s\n",sn,lc,text);
        //        // report back some user values
        //        box->post_message(VS_MSG_ROUTE_APP,VS_MSG_USER_VALUE,23,sn);
    });

    // boundingbox are from the 'scamp5_output_boundingbox' function
    packet_switch->case_boundingbox([&](const vs_array<uint8_t>&data){
        auto sn = packet_switch->get_packet_sn();
        auto lc = packet_switch->get_loop_counter();
        printf("packet[%d]: aabb, lc=%d, { %d, %d, %d, %d }\n",sn,lc,data(0,0),data(0,1),data(1,0),data(1,1));
    });

    // data from the 'scamp5_output_analog' and the 'scamp5_output_digital' function
    packet_switch->case_analog([&](int width,int height,uint8_t*bitmap_buffer){
        auto sn = packet_switch->get_packet_sn();
        auto lc = packet_switch->get_loop_counter();
        printf("packet[%d]: aout, lc=%d, width=%d, height=%d\n",sn,lc,width,height);
    });

    // data from the 'scamp5_output_analog' and the 'scamp5_output_digital' function
    packet_switch->case_digital([&](int width,int height,uint8_t*bitmap_buffer){
        auto sn = packet_switch->get_packet_sn();
        auto lc = packet_switch->get_loop_counter();
        printf("packet[%d]: dout, lc=%d, width=%d, height=%d\n",sn,lc,width,height);
    });

    // ponits are data from the 'scamp5_output_events' function
    packet_switch->case_points([&](const vs_array<uint8_t>&data){
        auto sn = packet_switch->get_packet_sn();
        auto lc = packet_switch->get_loop_counter();
        printf("packet[%d]: points, lc=%d, num_points=%d\n",sn,lc,data.get_row_size());
    });

    packet_switch->case_raw([&](const uint8_t*payload,size_t bytes){
        auto sn = packet_switch->get_packet_sn();
        auto lc = packet_switch->get_loop_counter();
        printf("packet[%d]: raw, lc=%d, payload bytes=%d, [ %2.2X, %2.2X, %2.2X, %2.2X ... ]\n",sn,lc,bytes,
            payload[0],payload[1],payload[2],payload[3]);
    });

    packet_switch->case_data_int32([&](const vs_array<int32_t>&data){
        auto sn = packet_switch->get_packet_sn();
        auto lc = packet_switch->get_loop_counter();
        auto channel = packet_switch->get_data_channel();
        printf("packet[%d]: data array, lc=%d, int32[%d][%d], channel: %d\n",sn,lc,data.get_row_size(),data.get_col_size(),channel);
    });

}


void input_loop(){

    printf("<press Q to quit>\n");

    while(quit==false){
        char c = conio_getch();
        switch(c){

        case 'q':
            quit = true;
            printf("quit\n");
            break;

        case 's':// for diagnostic purpose
            printf("signature_counter: %d, packet_counter: %d\n",
                box->get_signature_counter(),box->get_packet_counter());
            break;

        case 'h':// manually toggle whether the device should behave as if the host app running
            if(host_on){
                box->post_message(VS_MSG_ROUTE_APP,VS_MSG_HOST_DC);
                printf("host off\n");
            }else{
                box->post_message(VS_MSG_ROUTE_APP,VS_MSG_HOST_ON);
                printf("host on\n");
            }
            host_on = !host_on;
            break;

        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
            printf("move first user slider: %d\n",c - '0');
            box->post_message(VS_MSG_ROUTE_APP,VS_MSG_GUI_UPDATE,VS_GUI_USER_ITEM_0,c - '0');
            break;
        }
    }
}


int main(int argc,char*argv[]){
    int r;
    char ip_str[] = "192.168.1.101";

    // use the IP address in argument if possible
    if(argc>=2){
        strcpy(ip_str,argv[1]);
    }


    printf("Scamp5d Application Example\n");

    // a thread to handle console keyboard input
    std::thread input_thread(input_loop);

    // 'scamp5d_packet_switch' the object to decode the incoming packets and pass them
    // to the corresponding handler function registered.
    packet_switch = new vs_packet_decoder;

    setup_packet_switch();

    /// Initialize the device interface
    box = new scamp5d_tcp();
    r = box->open("127.0.0.1",27888);
    if(r){
        fprintf(stderr,"<Error> failed to open device!\n");
        exit(-1);
    }else{
        printf("<Device Ready>\n------------------------------------------------\n");
    }


#ifdef ENABLE_PROXY

    /// Start the TCP Proxy
    proxy = new scamp5d_proxy;
    proxy->open(ip_str,27725);

    /// Setup how the interface handles packet received from the device
    // Here the "free_packet" callback is used, which needs to 'free' the packet after use.
    box->on_free_packet([&](uint8_t*packet,size_t packet_size){

        // process the packet
        packet_switch->decode_packet(packet,packet_size,box->get_packet_counter());

        // forward to proxy, and deal with proxy bottle neck
        const size_t warning_size = 512*1024;
        const size_t discard_size = 10*1024*1024;
        size_t queue_size = proxy->get_bytes_in_queue();

        if(queue_size>discard_size){
            printf("[scamp5d_proxy] warning: queue over %d bytes, packet discarded!!!\n",queue_size);
            // dont broadcast, just free
            free(packet);
        }else{
            if(queue_size>warning_size){
                printf("[scamp5d_proxy] warning: over %d bytes of data queued!\n",queue_size);
            }
            // broadcast through the proxy and free the packet
            proxy->broadcast(packet,packet_size,true);
        }

    });

    /// Setup how the proxy handles packet received from client
    proxy->on_receive_packet([&](const uint8_t*packet,size_t bytes){
        //printf("client -> device: %d, %s\n",bytes,HEX_STR(packet,(bytes>16)? 16:bytes));
        printf("client -> device: %d bytes\n",bytes);
        box->write(packet,bytes);
    });

    /// Main Loop
    while(quit==false){
        box->routine();
        proxy->routine();
    }

    proxy->close();
    delete proxy;

#else

    box->on_receive_packet([&](const uint8_t*packet,size_t packet_size){
        packet_switch->decode_packet(packet,packet_size,box->get_packet_counter());
    });

    while(quit==false){
        box->routine();
    }

#endif

    box->close();
    delete box;

    delete packet_switch;

    input_thread.join();

    return 0;
}
