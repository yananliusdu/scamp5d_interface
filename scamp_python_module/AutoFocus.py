
## Yanan Liu
## Autofocus
## 25/12/2023 @506

import sys
import math
import time
import scamp
import tkinter as tk
from PIL import Image, ImageTk

import serial
import time
from multifunctions import *
import threading

record_frame_num = 0
record_image_num = 0
frame_count = 0

new_frame = False
client = None
target = 0
total_pixel = 0

max_pixel_num = 0
min_pixel_num = 900
focus_cmd = 0
lens_cmd = 0

time_delay_lens = 0.05
lens_step = 50
lens_cmd_start = 300
focus_scale_min = 0.9
focus_scale_max = 1.2

# Open serial port with specified settings
ser = serial.Serial('COM4', 9600, timeout=1, parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE, bytesize=serial.EIGHTBITS)

# Hexadecimal commands for different levels
commands = {
    0: bytearray([0x53, 0x31, 0x01, 0x02, 0x00, 0x00, 0x87, 0x3E]),
    50: bytearray([0x53, 0x31, 0x01, 0x02, 0x32, 0x00, 0xB9, 0x3E]),
    100: bytearray([0x53, 0x31, 0x01, 0x02, 0x64, 0x00, 0xEB, 0x3E]),
    150: bytearray([0x53, 0x31, 0x01, 0x02, 0x96, 0x00, 0x1D, 0x3E]),
    200: bytearray([0x53, 0x31, 0x01, 0x02, 0xC8, 0x00, 0x4F, 0x3E]),
    250: bytearray([0x53, 0x31, 0x01, 0x02, 0xFA, 0x00, 0x81, 0x3E]),
    300: bytearray([0x53, 0x31, 0x01, 0x02, 0x2C, 0x01, 0xB4, 0x3E]),
    350: bytearray([0x53, 0x31, 0x01, 0x02, 0x5E, 0x01, 0xE6, 0x3E]),
    400: bytearray([0x53, 0x31, 0x01, 0x02, 0x90, 0x01, 0x18, 0x3E]),
    450: bytearray([0x53, 0x31, 0x01, 0x02, 0xC2, 0x01, 0x4A, 0x3E]),
    500: bytearray([0x53, 0x31, 0x01, 0x02, 0xF4, 0x01, 0x7C, 0x3E]),
    550: bytearray([0x53, 0x31, 0x01, 0x02, 0x26, 0x02, 0xAF, 0x3E]),
    600: bytearray([0x53, 0x31, 0x01, 0x02, 0x58, 0x02, 0xE1, 0x3E]),
    650: bytearray([0x53, 0x31, 0x01, 0x02, 0x8A, 0x02, 0x13, 0x3E]),
    700: bytearray([0x53, 0x31, 0x01, 0x02, 0xBC, 0x02, 0x45, 0x3E]),
    750: bytearray([0x53, 0x31, 0x01, 0x02, 0xEE, 0x02, 0x77, 0x3E]),
    800: bytearray([0x53, 0x31, 0x01, 0x02, 0x20, 0x03, 0xAA, 0x3E]),
    850: bytearray([0x53, 0x31, 0x01, 0x02, 0x52, 0x03, 0xDC, 0x3E]),
    900: bytearray([0x53, 0x31, 0x01, 0x02, 0x84, 0x03, 0x0E, 0x3E]),
    950: bytearray([0x53, 0x31, 0x01, 0x02, 0xB6, 0x03, 0x40, 0x3E]),
    1000: bytearray([0x53, 0x31, 0x01, 0x02, 0xE8, 0x03, 0x72, 0x3E]),
}


def send_command(level):
    command = commands.get(level)
    if command:
        ser.write(command)
        print(f"Command for level {level} sent.")
    else:
        print(f"No command found for level {level}")
    # time.sleep(0.035)

def read_response():
    while True:
        if ser.in_waiting > 0:
            response = ser.read(ser.in_waiting)
            print("Response received:", response)
            break
        # time.sleep(0.1)

def run_for_duration(duration):
    """
    Runs the specified code for a given duration.

    Parameters:
    - duration: The duration in seconds for which to run the code.
    - code_to_run: A function that contains the code to be executed.
    """
    end_time = time.time() + duration
    while time.time() < end_time:
        packet = scamp.get_packet()
        # if packet is not None:
        #     loopcounter = packet.get('loopcounter', 'default_value')
        #     print('loopcounter test', loopcounter)
    time.sleep(0.02) # in case the buffer is empty

def process_packet(packet):
    global DisplayCanvas
    global DisplayImage
    global record_frame_num, record_image_num
    global new_frame
    global frame_count
    global total_pixel
    if packet['type'] == 'data':
        lc = packet['loopcounter']
        if lc == record_frame_num:
            # print('same frame', lc, record_frame_num)
            new_frame = False
        else:
            # print('new frame', lc, record_frame_num)
            record_image_num = 0
            new_frame = True
            frame_count += 1
        record_frame_num = lc
        datatype = packet['datatype']
        j_w = record_image_num % 4
        j_h = math.floor(record_image_num / 4)
        # print(lc, datatype)

        if datatype == 'TEXT':
            print('[%d] text: %s' % (lc, repr(packet['text'])))
            result = packet['text']
            if 'total_pixel' in result:
                result = result.split()
                total_pixel= int(result[-1])
                # print('total_pixel', total_pixel)

        elif datatype == 'SCAMP5_AOUT':
            w = packet['width']
            h = packet['height']
            # print('[%d] aout %dx%d >> %d' % (lc, w, h, packet['channel']))
            img = Image.frombytes('L', (w, h), packet['buffer']).transpose(Image.FLIP_LEFT_RIGHT).transpose(
                Image.ROTATE_180)
            DisplayImage[record_image_num] = ImageTk.PhotoImage(img)
            DisplayCanvas[record_image_num].create_image(j_w * W, j_h * H, image=DisplayImage[record_image_num],
                                                         anchor=tk.NW)
            record_image_num += 1
        elif datatype == 'SCAMP5_DOUT':
            w = packet['width']
            h = packet['height']
            # print('[%d] dout %dx%d >> %d' % (lc, w, h, packet['channel']))
            img = Image.frombytes('L', (w, h), packet['buffer']).transpose(Image.FLIP_LEFT_RIGHT).transpose(
                Image.ROTATE_180)
            DisplayImage[record_image_num] = ImageTk.PhotoImage(img)
            DisplayCanvas[record_image_num].create_image(j_w * W, j_h * H, image=DisplayImage[record_image_num],
                                                         anchor=tk.NW)
            record_image_num += 1
        elif datatype == 'INT16':
            print('[%d] int16 %dx%d >> %d' % (lc, packet['n_rows'], packet['n_cols'], packet['channel']))

        elif datatype == 'INT32':
            print('[%d] int32 %dx%d >> %d' % (lc, packet['n_rows'], packet['n_cols'], packet['channel']))
            # print(packet['data'])

        elif datatype == 'FLOAT':
            print('[%d] float %dx%d >> %d' % (lc, packet['n_rows'], packet['n_cols'], packet['channel']))

        elif datatype == 'REQUEST':
            if packet['filetype'] == 'IMAGE':
                print('[%d] request image %s %d' % (lc, repr(packet['filepath']), packet['n_bits']))
                img = Image.open(packet['filepath']).transpose(Image.FLIP_TOP_BOTTOM)
                scamp.send_image(img.tobytes(), img.width, img.height, packet['n_bits'])

            elif packet['filetype'] == 'FILE':
                print('[%d] request file %s' % (lc, repr(packet['filepath'])))
                with open(packet['filepath'], "rb") as f:
                    scamp.send_file(f.read())
    else:
        print('packet: type=%s, size=%d' % (packet['type'], packet['size']))


def slider_1_callback(value):
    scamp.send_gui_value(1, int(value))


def check_1_callback():
    global Check_1_Var
    global Send_Msg_On_Quit
    if Check_1_Var.get():
        scamp.send_message(scamp.VS_MSG_HOST_ON, 0, 0)
        Send_Msg_On_Quit = True
    else:
        scamp.send_message(scamp.VS_MSG_HOST_DC, 0, 0)
        Send_Msg_On_Quit = False

def auto_focus_process():
    scamp.routine()
    # 初始对焦过程
    max_pixel_num = 0
    focus_cmd = 0
    lens_cmd = lens_cmd_start
    while lens_cmd < 1001:
        send_command(lens_cmd)
        run_for_duration(time_delay_lens)
        packet = scamp.get_packet()
        if packet is not None:
            datatype = packet['datatype']
            if datatype == 'TEXT':
                process_packet(packet)
                if total_pixel > max_pixel_num*1.1:
                    max_pixel_num = total_pixel
                    focus_cmd = lens_cmd
            else:
                process_packet(packet)
            lens_cmd += lens_step

    # 设置到最佳焦距
    send_command(focus_cmd)
    print('focused')

    # 持续监测对焦
    while True:
        run_for_duration(time_delay_lens)
        packet = scamp.get_packet()
        if packet is not None:
            # datatype = packet['datatype']
            # if datatype == 'TEXT':
            process_packet(packet)
            if (total_pixel < min_pixel_num) or(total_pixel < max_pixel_num * focus_scale_min) or (total_pixel > max_pixel_num * focus_scale_max):  # 如果像素数量下降超过10%，重新对焦
                max_pixel_num = 0
                for lens_cmd in range(lens_cmd_start, 1001, lens_step):
                    send_command(lens_cmd)
                    run_for_duration(time_delay_lens)
                    packet = scamp.get_packet()
                    if packet is not None:
                        datatype = packet['datatype']
                        if datatype == 'TEXT':
                            process_packet(packet)
                            if total_pixel > max_pixel_num*1.1:
                                max_pixel_num = total_pixel
                                focus_cmd = lens_cmd
                        else:
                            process_packet(packet)
                send_command(focus_cmd)

# UI update
def main_process():
    tk_root.update_idletasks()
    tk_root.after(0.1, main_process)

def api_main_process():
    scamp.routine()
    # global total_pixel, max_pixel_num, focus_cmd
    # max_pixel_num =0
    # focus_cmd = 0
    # for lens_cmd in range(0,1001,100):
    #     send_command(lens_cmd)
    #     time.sleep(0.05)
    #     packet = scamp.get_packet()
    #     if packet is None:
    #         break
    #     else:
    #         process_packet(packet)
    #         if (total_pixel > max_pixel_num):
    #             max_pixel_num = total_pixel
    #             focus_cmd = lens_cmd
    #
    # send_command(focus_cmd)
    # time.sleep(0.04)

    while True:
        packet = scamp.get_packet()
        if packet is None:
            pass
        else:
            process_packet(packet)
            if new_frame:
                pass

Connection_Type = 'USB'

# GUI design
tk_root = tk.Tk()
tk_root.title('SCAMP-5d Python Host App')
W = 256
H = 256
# design the layout of images
N_Display = 12
w_num = 4
h_num = 3
DisplayImage = []
DisplayCanvas = []
j_w = 0
j_h = 0
canvas = tk.Canvas(tk_root, width=W * w_num, height=H * h_num, bd=0)
for i in range(N_Display):
    image = ImageTk.PhotoImage(Image.frombytes(mode='L', size=(W, H), data=bytes(W * H)))
    DisplayImage.append(image)
    DisplayCanvas.append(canvas)
    canvas.pack()
    j_w = i % 4
    j_h = math.floor(i / 4)
    canvas.create_image(j_w * W, j_h * H, image=DisplayImage[i], anchor=tk.NW)
############################

Slider_1 = tk.Scale(tk_root, from_=0, to=2141, orient=tk.HORIZONTAL, command=slider_1_callback)
Slider_1.pack()

Send_Msg_On_Quit = False
if Connection_Type == 'USB':
    Check_1_Var = tk.IntVar()
    Check_1 = tk.Checkbutton(tk_root, text="vs_gui_is_on()", variable=Check_1_Var, command=check_1_callback)
    Check_1.pack()
    print('open USB connection...')
    scamp.open_usb('0')
else:
    print('open TCP connection...')
    scamp.open_tcp('127.0.0.1', 27888)

threading.Thread(target=auto_focus_process, daemon=True).start()
tk_root.after(1, main_process)
tk_root.mainloop()


if Send_Msg_On_Quit:
    scamp.send_message(scamp.VS_MSG_HOST_DC, 0, 0)

time.sleep(0.05)
scamp.close()
ser.close()
cv2.destroyAllWindows()
print('End.')
exit()
