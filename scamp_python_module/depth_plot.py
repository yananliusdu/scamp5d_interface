from PIL import Image
import numpy as np
import os

# 图像文件夹路径
folder_path = r'D:\PPA\Scamp5d_Download\scamp5d_host_1_7_0\scamp5d_host\bin\depth_test'  # 替换为你的图像文件夹路径

# 获取所有图像文件
image_files = sorted([f for f in os.listdir(folder_path) if f.endswith('.BMP')])
num_images = len(image_files)
if num_images != 32:
    raise ValueError(f"Expected 32 images, but found {num_images}")

# 读取第一张图像以获取尺寸信息
image_path = os.path.join(folder_path, image_files[0])
sample_image = Image.open(image_path)
width, height = sample_image.size

# 初始化一个数组来存储32位图像的数据
combined_data = np.zeros((height, width), dtype=np.uint32)

# 将每张图像的数据添加到相应的位位置
for bit_position, image_file in enumerate(image_files):
    image_path = os.path.join(folder_path, image_file)
    image = Image.open(image_path).convert('L')
    image_data = np.array(image) // 255  # 将像素值转换为0或1
    combined_data |= image_data << bit_position

# 将NumPy数组转换为图像
combined_image = Image.fromarray(combined_data, mode='I')

# 保存和显示图像
output_path = r'D:\PPA\Scamp5d_Download\scamp5d_host_1_7_0\scamp5d_host\bin\depth_test'  # 修改为你的保存路径
combined_image.save(output_path)
combined_image.show()

output_path  # 返回保存的图像路径
