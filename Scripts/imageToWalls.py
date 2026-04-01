import sys
import cv2

width = 16
height = 16
dimensions = (height, width)
bw_array = []
bw_value = 0

input_file = input("Image file: ")

img_original = cv2.imread(input_file, 0)
img_resized = cv2.resize(img_original, dimensions, interpolation = cv2.INTER_AREA)
(thresh, img_bw) = cv2.threshold(img_resized, 128, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)

# Uncomment below lines to see the image treshold on original size
#(thresh, img_bw_original_size) = cv2.threshold(img_original, 128, 255, cv2.THRESH_BINARY | cv2.THRESH_OTSU)
#cv2.imshow("Frame", img_bw_original_size)
#cv2.waitKey(0)

for i in range(height):
    bw_value = 0
    for j in range(width):
        k = img_bw[i, j]
        if k == 0:
            bw_value |= (1 << (width - 1 - j))
    bw_array.append(bw_value)

for index, bw_value in enumerate(bw_array):
    if index == len(bw_array) - 1:
        print(f"0b{bw_value:016b}", end='')
    else:
        print(f"0b{bw_value:016b},", end='')