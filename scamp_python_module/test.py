
import io
import math
import time
import scamp
import tkinter as tk
from PIL import Image, ImageTk

record_frame_num = 0
record_image_num = 0
def process_packet(packet):
    global DisplayCanvas
    global DisplayImage
    global record_frame_num
    global record_image_num
    if packet['type']=='data':
        lc = packet['loopcounter']
        if lc == record_frame_num:
            print('same frame', lc, record_frame_num)
        else:
            print('new frame', lc, record_frame_num)
            record_image_num = 0
        record_frame_num = lc
        datatype = packet['datatype']
        j_w = record_image_num % 4
        j_h = math.floor(record_image_num / 4)
        # print(lc, datatype)
        
        if datatype == 'TEXT':
            print('[%d] text: %s' % (lc, repr(packet['text'])))
            # None
            
        elif datatype == 'SCAMP5_AOUT':
            w = packet['width']
            h = packet['height']
            print('[%d] aout %dx%d >> %d' % (lc, w, h, packet['channel']))
            img = Image.frombytes('L', (w, h), packet['buffer']).transpose(Image.FLIP_LEFT_RIGHT).transpose(Image.ROTATE_180)
            DisplayImage[record_image_num] = ImageTk.PhotoImage(img)
            DisplayCanvas[record_image_num].create_image(j_w*W, j_h*H, image=DisplayImage[record_image_num], anchor=tk.NW)
            record_image_num += 1
        elif datatype == 'SCAMP5_DOUT':
            w = packet['width']
            h = packet['height']
            print('[%d] dout %dx%d >> %d' % (lc, w, h, packet['channel']))
            img = Image.frombytes('L', (w, h), packet['buffer']).transpose(Image.FLIP_LEFT_RIGHT).transpose(Image.ROTATE_180)
            DisplayImage[record_image_num] = ImageTk.PhotoImage(img)
            DisplayCanvas[record_image_num].create_image(j_w*W, j_h*H, image=DisplayImage[record_image_num], anchor=tk.NW)
            record_image_num += 1
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

#design the layout of images
N_Display = 10
w_num = 4
h_num = 3
DisplayImage = []
DisplayCanvas = []
j_w = 0
j_h = 0
canvas = tk.Canvas(tk_root, width=W * w_num, height=H * h_num, bd=0)
for i in range(N_Display):
    image = ImageTk.PhotoImage(Image.frombytes(mode='L', size=(W, H), data=bytes(W*H)))
    DisplayImage.append(image)
    DisplayCanvas.append(canvas)
    canvas.pack()
    j_w = i%4
    j_h = math.floor(i/4)
    canvas.create_image(j_w*W, j_h*H, image=DisplayImage[i], anchor=tk.NW)
############################

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
