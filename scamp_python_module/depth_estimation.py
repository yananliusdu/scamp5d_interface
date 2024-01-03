# Created by Yanan Liu on 10:57 03/01/2024
# Location: Your Location
# Log: Your Log Information
# Version: Your Version Information


from PIL import Image
import os

# 定义颜色映射
colors = [(255, 0, 0), (0, 255, 0), (0, 0, 255), (255, 255, 0), (255, 0, 255), (0, 255, 255), (128, 128, 128), (0, 0, 0)]

# 设置图像尺寸和文件夹路径
width, height = 256, 256
folder_path = r'D:\PPA\Scamp5d_Download\scamp5d_host_1_7_0\scamp5d_host\bin\depth'

# 创建一个新的空白图像
combined_image = Image.new("RGB", (width, height), "white")
def set_border_black(image):
    """ 将图像边缘像素设置为黑色 """
    width, height = image.size
    for x in range(width):
        image.putpixel((x, 0), 0)         # 上边界
        image.putpixel((x, height - 1), 0) # 下边界
    for y in range(height):
        image.putpixel((0, y), 0)         # 左边界
        image.putpixel((width - 1, y), 0) # 右边界
    return image


# 遍历每个图像文件
for i in range(1, 9):
    image_path = os.path.join(folder_path, f"{i}.bmp")
    image = Image.open(image_path).convert('L')
    image = set_border_black(image)

    # 将当前图像的边缘叠加到combined_image上
    for x in range(width):
        for y in range(height):
            # 如果当前像素是边缘（非白色），使用对应的颜色
            # print(image.getpixel((x, y)))
            if image.getpixel((x, y)) == 255:
                combined_image.putpixel((x, y), colors[i-1])

# 保存叠加后的图像
combined_image.save("combined_image.bmp")
combined_image.show()