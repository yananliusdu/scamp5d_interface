import numpy as np
import cv2

import seaborn as sns
from matplotlib import pyplot as plt

from glob import glob as gglob

from matplotlib.colors import ListedColormap, LinearSegmentedColormap

folder_path = r'D:\PPA\Scamp5d_Download\scamp5d_host_1_7_0\scamp5d_host\bin\depth_test\*.BMP'

image_paths = sorted(gglob(folder_path), reverse=False)
image_values = list(range(len(image_paths)))

depth_map = np.zeros(shape=(256, 256), dtype=int)
for p, v in zip(image_paths, image_values):
    gray_img = cv2.imread(p, cv2.IMREAD_GRAYSCALE)
    value_img = np.ma.array(np.full_like(gray_img, fill_value=v),
                            mask=(gray_img == 0) | (depth_map > 0))
    depth_map = depth_map + value_img.filled(0)

# 使用Seaborn默认的连续颜色映射
current_cmap = sns.color_palette("Spectral", as_cmap=True)
# 获取当前colormap的颜色列表
current_cmap_array = current_cmap(np.arange(current_cmap.N))
# 将 0 值的颜色设置为黑色
current_cmap_array[0] = np.array([0, 0, 0, 1])
# 创建一个新的colormap对象
custom_cmap = LinearSegmentedColormap.from_list('custom_cmap', current_cmap_array)

sns.heatmap(data=depth_map, square=True, cmap=custom_cmap,
            xticklabels=False, yticklabels=False, )
# sns.heatmap(depth_map, annot=True, cmap='RdBu_r')
plt.show()
