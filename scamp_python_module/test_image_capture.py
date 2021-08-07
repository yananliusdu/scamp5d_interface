
# this project is used to collect images from the SCAMP
# images are saved locally associated with local time stamp

import io
import cv2
import time
import scamp
import numpy as np
from datetime import datetime
import tkinter as tk
from PIL import Image, ImageTk

image_save_path = 'C:\\CDT project\\DeepCNN\\Hector\\Journal\\image_capture\\'
image_count = 0


def process_packet(packet):
    global DisplayCanvas
    global DisplayImage
    global image_count

    if packet['type']=='data':
    
        lc = packet['loopcounter']
        datatype = packet['datatype']

        # print(lc, datatype)
        
        if datatype == 'TEXT':
            print('[%d] text: %s' % (lc, repr(packet['text'])))
            # None
            
        elif datatype == 'SCAMP5_AOUT':
            w = packet['width']
            h = packet['height']
            print('[%d] aout %dx%d >> %d' % (lc, w, h, packet['channel']))
            img = Image.frombytes('L', (w, h), packet['buffer']).transpose(Image.FLIP_LEFT_RIGHT).transpose(Image.ROTATE_180)

            # datetime object containing current date and time
            now = datetime.now()
            now = str(now)
            now = now.replace(" ", "-")
            now = now.replace(":", "-")
            # datetime object containing current date and time
            # dt_string = now.strftime()
            image_name = image_save_path + now + '.png'
            print(image_name)
            image = np.array(img)
            cv2.imwrite(image_name, image)
            image_count = image_count + 1

            DisplayImage[0] = ImageTk.PhotoImage(img)
            DisplayCanvas[0].create_image(0, 0, image=DisplayImage[0], anchor=tk.NW)

        elif datatype == 'SCAMP5_DOUT':
            w = packet['width']
            h = packet['height']
            print('[%d] dout %dx%d >> %d' % (lc, w, h, packet['channel']))
            img = Image.frombytes('L', (w, h), packet['buffer']).transpose(Image.FLIP_LEFT_RIGHT).transpose(Image.ROTATE_180)
            DisplayImage[1] = ImageTk.PhotoImage(img)
            DisplayCanvas[1].create_image(0, 0, image=DisplayImage[1], anchor=tk.NW)

        elif datatype == 'INT16':
            print('[%d] int16 %dx%d >> %d' % (lc, packet['n_rows'], packet['n_cols'], packet['channel']))

        elif datatype == 'INT32':
            print('[%d] int32 %dx%d >> %d' % (lc, packet['n_rows'], packet['n_cols'], packet['channel']))
            # print(packet['data'])

        elif datatype == 'FLOAT':
            print('[%d] float %dx%d >> %d' % (lc, packet['n_rows'], packet['n_cols'], packet['channel']))

        elif datatype == 'REQUEST':

            if packet['filetype']== 'IMAGE':
                print('[%d] request image %s %d' % (lc, repr(packet['filepath']), packet['n_bits']))
                img = Image.open(packet['filepath']).transpose(Image.FLIP_TOP_BOTTOM)
                scamp.send_image(img.tobytes(), img.width, img.height, packet['n_bits'])

            elif packet['filetype']== 'FILE':
                print('[%d] request file %s' % (lc, repr(packet['filepath'])))
                with open(packet['filepath'], "rb") as f:
                    scamp.send_file(f.read())

    else:
        print('packet: type=%s, size=%d' % (packet['type'], packet['size']))


def slider_1_callback(value):
    scamp.send_gui_value(0, int(value))


def check_1_callback():
    global Check_1_Var
    global Send_Msg_On_Quit
    if Check_1_Var.get():
        scamp.send_message(scamp.VS_MSG_HOST_ON, 0, 0)
        Send_Msg_On_Quit = True
    else:
        scamp.send_message(scamp.VS_MSG_HOST_DC, 0, 0)
        Send_Msg_On_Quit = False


def main_process():

    scamp.routine()

    while True:
        packet = scamp.get_packet()
        if packet is None:
            break
        else:
            process_packet(packet)

    tk_root.update_idletasks()

    tk_root.after(1, main_process)


# Entry Point

Connection_Type = 'USB'

tk_root = tk.Tk()
tk_root.title('Scamp5d Python App')

W = 256
H = 256

N_Display = 2
DisplayImage = []
DisplayCanvas = []
for i in range(N_Display):
    image = ImageTk.PhotoImage(Image.frombytes(mode='L', size=(W, H), data=bytes(W*H)))
    DisplayImage.append(image)
    canvas = tk.Canvas(tk_root, width=W, height=H, bd=0)
    DisplayCanvas.append(canvas)
    canvas.pack()
    canvas.create_image(0, 0, image=DisplayImage[i], anchor=tk.NW)

Slider_1 = tk.Scale(tk_root, from_=-100, to=100, orient=tk.HORIZONTAL, command=slider_1_callback)
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
    scamp.open_tcp('127.0.0.1',27888)

main_process()
tk_root.mainloop()

if Send_Msg_On_Quit:
    scamp.send_message(scamp.VS_MSG_HOST_DC, 0, 0)

time.sleep(0.05)
scamp.close()
print('End.')

exit()
