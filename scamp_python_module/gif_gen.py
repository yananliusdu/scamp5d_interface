from PIL import Image
import os

# 图像文件夹路径
folder_path = r'D:\PPA\Scamp5d_Download\scamp5d_host_1_7_0\scamp5d_host\bin\depth_test'
output_gif_path = r'D:\PPA\Scamp5d_Download\scamp5d_host_1_7_0\scamp5d_host\bin\depth_test\animated.gif'

# 获取文件夹中所有图像文件的路径
image_paths = [os.path.join(folder_path, file) for file in sorted(os.listdir(folder_path)) if file.endswith('.BMP')]

# 读取图像
images = [Image.open(image_path) for image_path in image_paths]

# 将第一张图像以外的其余图像保存为一个序列
images[0].save(output_gif_path, save_all=True, append_images=images[1:], optimize=False, duration=100, loop=0)

print(f"GIF saved at {output_gif_path}")
