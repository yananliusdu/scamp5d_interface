# -*- coding: utf-8 -*-
#python3
#Author: Yanan Liu
# @Time    : 31/12/2020 17:35
# @Software: PyCharm 
# @File    : multifunctions

import cv2
import numpy as np

def saving_crop_img(crop_size, x, y, img, save_path):
    start_x = int(crop_size / 2 * x - crop_size / 4)  # start pixel for cropping
    start_y = int(crop_size / 2 * y - crop_size / 4)
    if start_x < 0:
        start_x = 0
    if start_x >= 192:
        start_x = 192
    if start_y < 0:
        start_y = 0
    if start_y >= 192:
        start_y = 192
    crop_img = img[start_y:start_y + crop_size, start_x:start_x + crop_size]
    # gray_img_crop = cv2.cvtColor(crop_img, cv2.COLOR_BGR2GRAY)
    tile = np.tile(crop_img, (4, 4))
    cv2.imwrite(save_path, tile)
    return crop_img


def saving_trajectory_data(data, file):
    file.write(str(data) + '\n')


