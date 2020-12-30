import sys
import math
import time
import scamp
import tkinter as tk
from PIL import Image, ImageTk

sys.path.insert(1,
                'C:\\PycharmWorkspace\\HelloPythonDeeplearning\\scamp5d_interface\\scamp_python_module\\coopeliaAPI\\')
sys.path.insert(1, 'C:\\PycharmWorkspace\\HelloPythonDeeplearning\\VisualisationProject\\')
import b0RemoteApi
import numpy as np
import cv2
from data_visulisation_function import *

shared_path_test_image = 'C:\\CDT project\\DeepCNN\\carTracking\\car_xyrV1\\results\\dataforSCAMP\\scamp5d_host\\bin\\shared_file\\shared_image_test_0.bmp'
shared_path_shown_image = 'C:\\CDT project\\DeepCNN\\carTracking\\car_xyrV1\\results\\dataforSCAMP\\scamp5d_host\\bin\\shared_file\\shared_image_show_0.bmp'
record_frame_num = 0
record_image_num = 0
label_num = 8
show_img_res = 256
target_step = 0.02
circle_radius = 30

new_frame = False
client = None
target = 0
activeVisionSensor = 0
scamp_detected_x = -1
scamp_detected_y = -1
x_prediction = []
y_prediction = []


def process_packet(packet):
    global DisplayCanvas
    global DisplayImage
    global record_frame_num
    global record_image_num
    global new_frame
    global scamp_detected_x
    global scamp_detected_y
    global x_prediction
    global y_prediction

    if packet['type'] == 'data':
        lc = packet['loopcounter']
        if lc == record_frame_num:
            print('same frame', lc, record_frame_num)
            new_frame = False
        else:
            print('new frame', lc, record_frame_num)
            record_image_num = 0
            new_frame = True
        record_frame_num = lc
        datatype = packet['datatype']
        j_w = record_image_num % 4
        j_h = math.floor(record_image_num / 4)
        # print(lc, datatype)

        if datatype == 'TEXT':
            print('[%d] text: %s' % (lc, repr(packet['text'])))
            result = packet['text']

            if 'predictions:' in result:
                result = result.split()
                x_prediction = result[1:9]
                y_prediction = result[9:17]
                x_prediction = [int(i) for i in x_prediction]
                y_prediction = [int(i) for i in y_prediction]
                scamp_detected_x = x_prediction.index(max(x_prediction))
                scamp_detected_y = 7 - y_prediction.index(max(y_prediction))
                print(scamp_detected_x, scamp_detected_y)

        elif datatype == 'SCAMP5_AOUT':
            w = packet['width']
            h = packet['height']
            print('[%d] aout %dx%d >> %d' % (lc, w, h, packet['channel']))
            img = Image.frombytes('L', (w, h), packet['buffer']).transpose(Image.FLIP_LEFT_RIGHT).transpose(
                Image.ROTATE_180)
            DisplayImage[record_image_num] = ImageTk.PhotoImage(img)
            DisplayCanvas[record_image_num].create_image(j_w * W, j_h * H, image=DisplayImage[record_image_num],
                                                         anchor=tk.NW)
            record_image_num += 1
        elif datatype == 'SCAMP5_DOUT':
            w = packet['width']
            h = packet['height']
            print('[%d] dout %dx%d >> %d' % (lc, w, h, packet['channel']))
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


def coopelia_api_ini():
    # with b0RemoteApi.RemoteApiClient('b0RemoteApi_CoppeliaSim_Python', 'b0RemoteApi', 60) as client:
    global client
    global target
    global activeVisionSensor
    client = b0RemoteApi.RemoteApiClient('b0RemoteApi_CoppeliaSim_Python', 'b0RemoteApi', 60)
    client.simxStartSimulation(client.simxServiceCall())
    client.simxAddStatusbarMessage('Hello from PyCharm Python', client.simxDefaultPublisher())
    res, activeVisionSensor = client.simxGetObjectHandle('Vision_sensor', client.simxServiceCall())
    # res, resolution, image = client.simxGetVisionSensorImage(activeVisionSensor, False, client.simxServiceCall())
    res, target = client.simxGetObjectHandle('Quadricopter_target', client.simxServiceCall())
    # print('target', target)
    # time.sleep(1)


def api_image_process_motion_control(x, y):
    dim = (64, 64)
    ## image capturing from Coppeliasim
    res, resolution, image = client.simxGetVisionSensorImage(activeVisionSensor, False, client.simxServiceCall())
    img = np.frombuffer(image, dtype=np.ubyte)
    img.resize([resolution[0], resolution[1], 3])
    img = np.rot90(img, 2)
    img = np.fliplr(img)
    img = cv2.cvtColor(img, cv2.COLOR_RGB2BGR)
    gray_img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    cv2.imwrite(shared_path_shown_image, gray_img)
    gray64 = cv2.resize(gray_img, dim, cv2.INTER_NEAREST)
    tile = np.tile(gray64, (4, 4))
    cv2.imwrite(shared_path_test_image, tile)

    # motion control
    if x >= 0 and y >= 0:
        res, target_pos = client.simxGetObjectPosition(target, -1, client.simxServiceCall())
        # print(target_pos)
        ctr_pos0 = target_pos[0] + (x - 3.5) * target_step
        ctr_pos1 = target_pos[1] - (y - 3.5) * target_step
        set_pos = target_pos
        set_pos[0] = ctr_pos0
        set_pos[1] = ctr_pos1
        client.simxSetObjectPosition(target, -1, set_pos, client.simxServiceCall())

        # visualisation
        x_img = round(show_img_res / label_num * (x + 0.5))
        y_img = round(show_img_res / label_num * (y + 0.5))
        show_img = cv2.circle(img, (x_img, y_img), radius=circle_radius, color=(255, 0, 255), thickness=3)
        show_img = plotting_prediction_curve(x_prediction, y_prediction, show_img, 3)
        cv2.imshow('show_img', show_img)
        cv2.waitKey(1)


def api_main_process():
    scamp.routine()
    while True:
        packet = scamp.get_packet()
        if packet is None:
            break
        else:
            process_packet(packet)
            if new_frame:
                api_image_process_motion_control(scamp_detected_x, scamp_detected_y)
    tk_root.update_idletasks()
    tk_root.after(1, api_main_process)


Connection_Type = 'USB'

# GUI design
tk_root = tk.Tk()
tk_root.title('SCAMP-5d Python Host App')
W = 256
H = 256
# design the layout of images
N_Display = 8
w_num = 4
h_num = 2
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

# main_process()
coopelia_api_ini()
api_main_process()
tk_root.mainloop()

if Send_Msg_On_Quit:
    scamp.send_message(scamp.VS_MSG_HOST_DC, 0, 0)

time.sleep(0.05)
scamp.close()
cv2.destroyAllWindows()
print('End.')
exit()
