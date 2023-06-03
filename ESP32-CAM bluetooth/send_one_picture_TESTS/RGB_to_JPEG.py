from PIL import Image
import numpy as np
# image_RGB = np.array([[[18, 18, 20], [12, 12, 14], [7, 7, 9], [9, 9, 11], [15, 15, 17], [15, 15, 17], [8, 8, 10], [1, 1, 3], [6, 6, 8], [6, 6, 8], [6, 6, 8], [7, 7, 9], [7, 7, 9], [8, 8, 10]]])
# image = Image.fromarray(image_RGB.astype('uint8')).convert('RGB')
# image.save('image.jpg')

fileName = './FRAMESIZE_96X96.RGB565'
with open(fileName, mode='rb') as file:
    fileContent = file.read()

print(len(fileContent))