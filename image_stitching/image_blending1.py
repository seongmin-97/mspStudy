
import cv2
import numpy as np
apple = cv2.imread('cavnas1.jpg')
orange = cv2.imread('canvas2.jpg')
print(apple.shape)
print(orange.shape)

# generate Gaussian pyramid for apple
apple_copy = apple.copy()
gp_apple = [apple_copy]
for i in range(6):
    apple_copy = cv2.pyrDown(apple_copy)
    gp_apple.append(apple_copy)


# generate Gaussian pyramid for orange
orange_copy = orange.copy()
gp_orange = [orange_copy]
for i in range(6):
    orange_copy = cv2.pyrDown(orange_copy)
    gp_orange.append(orange_copy)

# generate Laplacian Pyramid for apple
apple_copy = gp_apple[5]
lp_apple = [apple_copy]
for i in range(5, 0, -1):
    gaussian_expanded = cv2.pyrUp(gp_apple[i])
    gaussian_expanded = cv2.resize(gaussian_expanded, dsize=(gp_apple[i-1].shape[1], gp_apple[i-1].shape[0]))
    laplacian = cv2.subtract(gp_apple[i-1], gaussian_expanded)
    lp_apple.append(laplacian)
    cv2.imwrite('asdfasdf'+str(i)+'.jpg', laplacian)
# generate Laplacian Pyramid for orange
orange_copy = gp_orange[5]
lp_orange = [orange_copy]
for i in range(5, 0, -1):
    gaussian_expanded = cv2.pyrUp(gp_orange[i])
    gaussian_expanded = cv2.resize(gaussian_expanded, dsize=(gp_orange[i-1].shape[1], gp_orange[i-1].shape[0]))
    laplacian = cv2.subtract(gp_orange[i-1], gaussian_expanded)
    lp_orange.append(laplacian)
    cv2.imwrite('asdforange'+str(i)+'.jpg', laplacian)
# Now add left and right halves of images in each level
mask = cv2.imread('./mask1.jpg')
mask_copy = mask.copy() / 200
gp_mask = []
for i in range(6) :
    mask_copy = cv2.pyrDown(mask_copy)
    mask_copy = cv2.resize(mask_copy, dsize=(lp_orange[i].shape[1], lp_orange[i].shape[0]))
    gp_mask.append(mask_copy)
apple_orange_pyramid = []
n = 0

for i in range(6) :
    print('orange', lp_orange[i].shape)
    print('apple', lp_apple[i].shape)
    print('mask', gp_mask[i].shape)
for apple_lap, orange_lap, mask_gp in zip(lp_apple, lp_orange, gp_mask):
    cols, rows, ch = apple_lap.shape
    laplacian = apple_lap * mask_gp + orange_lap * (1-mask_gp)
    apple_orange_pyramid.append(laplacian)

    n += 1

# now reconstruct
apple_orange_reconstruct = apple_orange_pyramid[0]
for i in range(1, 6):
    apple_orange_reconstruct = cv2.pyrUp(apple_orange_reconstruct)
    apple_orange_reconstruct = cv2.resize(apple_orange_reconstruct, dsize=(apple_orange_pyramid[i].shape[1], apple_orange_pyramid[i].shape[0]))
    apple_orange_reconstruct = cv2.add(apple_orange_pyramid[i], apple_orange_reconstruct)
    print(apple_orange_reconstruct.shape)


cv2.imwrite("./blending.jpg", apple_orange_reconstruct)
