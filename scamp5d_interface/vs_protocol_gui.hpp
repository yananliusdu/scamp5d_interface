
#ifndef VS_PROTOCOL_GUI_HPP
#define VS_PROTOCOL_GUI_HPP

#include "vs_protocol.hpp"

#define VS_GUI_USER_ITEM_0			0
#define VS_GUI_USER_ITEM_1			1
#define VS_GUI_USER_ITEM_2			2
#define VS_GUI_USER_ITEM_3			3
#define VS_GUI_USER_ITEM_4			4
#define VS_GUI_USER_ITEM_5			5
#define VS_GUI_USER_ITEM_6			6
#define VS_GUI_USER_ITEM_7			7
#define VS_GUI_USER_ITEM_8			8
#define VS_GUI_USER_ITEM_9			9
#define VS_GUI_USER_ITEM_10			10
#define VS_GUI_USER_ITEM_11			11
#define VS_GUI_USER_ITEM_12			12
#define VS_GUI_USER_ITEM_13			13
#define VS_GUI_USER_ITEM_14			14
#define VS_GUI_USER_ITEM_15			15
#define VS_GUI_USER_ITEM_16			16
#define VS_GUI_USER_ITEM_17			17
#define VS_GUI_USER_ITEM_18			18
#define VS_GUI_USER_ITEM_19			19
#define VS_GUI_USER_ITEM_20			20
#define VS_GUI_USER_ITEM_21			21
#define VS_GUI_USER_ITEM_22			22
#define VS_GUI_USER_ITEM_23			23

#define VS_GUI_USER_DISPLAY_0			100
#define VS_GUI_USER_DISPLAY_1			101
#define VS_GUI_USER_DISPLAY_2			102
#define VS_GUI_USER_DISPLAY_3			103
#define VS_GUI_USER_DISPLAY_4			104
#define VS_GUI_USER_DISPLAY_5			105
#define VS_GUI_USER_DISPLAY_6			106
#define VS_GUI_USER_DISPLAY_7			107
#define VS_GUI_USER_DISPLAY_8			108
#define VS_GUI_USER_DISPLAY_9			109
#define VS_GUI_USER_DISPLAY_10			110
#define VS_GUI_USER_DISPLAY_11			111

#define VS_GUI_HOST_VAR_BASE		128
#define VS_GUI_HOST_STATUS			128
#define VS_GUI_RECORDING			129
#define VS_GUI_FRAME_RATE			130
#define VS_GUI_FRAME_GAIN			131
#define VS_GUI_VECTOR_LENGTH		132
#define VS_GUI_IMAGE_LENGTH			133
#define VS_GUI_IMAGE_BLEND			134


#define VS_GUI_NULL    	0
#define VS_GUI_BUTTON  	1
#define VS_GUI_SLIDER  	2
#define VS_GUI_SWITCH	3
#define VS_GUI_CHECKBOX	4
#define VS_GUI_LIST		5


#pragma pack(push)
#pragma pack(1)

typedef struct VS_GUI_ITEM_FORM{
    uint16_t type;
    uint16_t flags;
    uint8_t text[40];
} vs_gui_item_form_t;

typedef struct VS_GUI_BUTTON_FORM{
    uint16_t type;
    uint16_t flags;
    uint8_t text[40];
} vs_gui_button_form_t;

typedef struct VS_GUI_SWITCH_FORM{
    uint16_t type;
    uint16_t flags;
    uint8_t text[38];
    uint16_t init_value;
} vs_gui_switch_form_t;

typedef struct VS_GUI_SLIDER_FORM{
    uint16_t type;
    uint16_t flags;
    uint8_t text[24];
    int32_t range_min;
    int32_t range_max;
    int32_t init_value;
    int32_t step_size;
} vs_gui_slider_form_t;


#define VS_GUI_IMAGE   16
#define VS_GUI_SCOPE   17
#define VS_GUI_BARPLOT 18
#define VS_GUI_SCATTER 19

typedef struct VS_GUI_DISPLAY_FORM{
    uint16_t type;
    uint16_t flags;
    uint8_t text[22];
    uint8_t size[2];
    uint8_t position[2];
    uint8_t palette[20];
} vs_gui_display_form_t;

typedef struct VS_GUI_SCOPE_FORM{
    uint16_t type;
    uint16_t flags;
    uint8_t text[16];
    uint16_t time_window;
    int16_t scale_min;
    int16_t scale_max;
    uint8_t size[2];
    uint8_t position[2];
    uint8_t palette[20];
} vs_gui_scope_form_t;

typedef struct VS_GUI_BARPLOT_FORM{
    uint16_t type;
    uint16_t flags;
    uint8_t text[16];
    uint16_t width;
    int16_t scale_min;
    int16_t scale_max;
    uint8_t size[2];
    uint8_t position[2];
    uint8_t palette[20];
} vs_gui_barplot_form_t;

typedef struct VS_GUI_SCATTER_FORM{
    uint16_t type;
    uint16_t flags;
    uint8_t text[16];
    uint16_t trail;
    int16_t y_min;
    int16_t y_max;
    uint8_t size[2];
    uint8_t position[2];
    uint8_t palette[16];
    int16_t x_min;
    int16_t x_max;
} vs_gui_scatter_form_t;


typedef struct VS_GUI_PACKET{
    vs_packet_header_t header;
    vs_data_header_t def;
    vs_gui_item_form_t item_form[12];
    vs_gui_display_form_t display_form[12];
} vs_gui_packet_t;

#pragma pack(pop)


#endif
