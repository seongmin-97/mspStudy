import cv2
import math
import numpy as np

img = cv2.imread('./data/museum1.jpg')
def cylindrical_projection(img) :
    rows = img.shape[0]
    cols = img.shape[1]

    #f = cols / (2 * math.tan(np.pi / 8))
    result = np.zeros_like(img)
    center_x = int(cols / 2)
    center_y = int(rows / 2)
    alpha = math.pi / 4
    f = cols / (2 * math.tan(alpha / 2))
    x_list, y_list = np.indices((img.shape[0], img.shape[1]), dtype=np.float32)

    for  y in range(rows):
        for x in range(cols):
            theta = math.atan((x- center_x )/ f)
            point_x = f * math.tan( (x-center_x) / f) + center_x
            point_y = (y-center_y) / math.cos(theta) + center_y
            x_list[y][x] = point_x
            y_list[y][x] = point_y
            if point_x >= cols or point_x < 0 or point_y >= rows or point_y < 0:
                pass
            else:
                result[y , x, :] = img[int(point_y) , int(point_x), :]
    
    test = cv2.remap(img, x_list, y_list, interpolation=cv2.INTER_LINEAR, borderMode=cv2.BORDER_REFLECT)
    return result, test

# def image_mirroring(image) :
    
#     len_row = []
#     len_column = []
#     for row in range(image.shape[0]) :
#         for column in range(image.shape[1]) :
#             pixel = 0
#             if (image[row][column] != np.array([0, 0, 0])).any() :
#                 pixel += 1
#         len_row.append(pixel)
#         pixel = 0
#     for column in range(image.shape[1]) :
#         for row in range(image.shape[0]) :
#             pixel = 0
#             if (image[row][column] != np.array([0, 0, 0])).any() :
#                 pixel += 1
#         len_column.append(pixel)

def cylindricalWarp(img):
    """This function returns the cylindrical warp for a given image and intrinsics matrix K"""
    h_,w_ = img.shape[:2]
    # pixel coordinates
    K = np.array([[1100,0,w_/2],[0,1100,h_/2],[0,0,1]]) # mock intrinsics
    y_i, x_i = np.indices((h_,w_))
    X = np.stack([x_i,y_i,np.ones_like(x_i)],axis=-1).reshape(h_*w_,3) # to homog
    Kinv = np.linalg.inv(K) 
    X = Kinv.dot(X.T).T # normalized coords
    # calculate cylindrical coords (sin\theta, h, cos\theta)
    A = np.stack([np.sin(X[:,0]),X[:,1],np.cos(X[:,0])],axis=-1).reshape(w_*h_,3)
    B = K.dot(A.T).T # project back to image-pixels plane
    # back from homog coords
    B = B[:,:-1] / B[:,[-1]]
    # make sure warp coords only within image bounds
    B[(B[:,0] < 0) | (B[:,0] >= w_) | (B[:,1] < 0) | (B[:,1] >= h_)] = -1
    B = B.reshape(h_,w_,-1)
    
    img_rgba = cv2.cvtColor(img,cv2.COLOR_BGR2BGRA) # for transparent borders...
    # warp the image according to cylindrical coords
    return cv2.remap(img_rgba, B[:,:,0].astype(np.float32), B[:,:,1].astype(np.float32), cv2.INTER_AREA, borderMode=cv2.BORDER_REFLECT)
cylindrical_projection(cv2.imread('./data/museum1.jpg'))