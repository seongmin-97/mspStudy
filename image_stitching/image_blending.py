import cv2
import numpy as np

def image_blending(canvas1, canvas2, merge, seam, xmin, ymin, x_offset) :

    if type(canvas1) == str :
        canvas1 = cv2.imread(canvas1)
        canvas2 = cv2.imread(canvas2)

    mask1 = get_mask(canvas1, seam, xmin, ymin, x_offset)
    if x_offset == 0 :
        x = 1
    else :
        x = 0
    mask2 = get_mask(canvas2, seam, xmin, ymin, x)

    cv2.imwrite('./mask1.jpg', mask1*200)
    cv2.imwrite('./mask2.jpg', mask2*200)

    img1_gaussian_pyramid = get_gaussian_pyramid(canvas1)
    img2_gaussian_pyramid = get_gaussian_pyramid(canvas2)
    
    img1_laplacian_pyramid = get_laplacian_pyramid(img1_gaussian_pyramid)
    img2_laplacian_pyramid = get_laplacian_pyramid(img2_gaussian_pyramid)

    mask1_gaussian_pyramid = get_gaussian_pyramid(mask1)
    mask2_gaussian_pyramid = get_gaussian_pyramid(mask2)

    for i in range(len(mask1_gaussian_pyramid)) :
        cv2.imwrite('./gaussian1mask'+str(i)+'.jpg', mask1_gaussian_pyramid[i]*200)
        cv2.imwrite('./gaussian2mask'+str(i)+'.jpg', mask2_gaussian_pyramid[i]*200)
        cv2.imwrite('./img1gaussianpyramid'+str(i)+'.jpg', img1_gaussian_pyramid[i])
        cv2.imwrite('./img2gaussianpyramid'+str(i)+'.jpg', img2_gaussian_pyramid[i])
        cv2.imwrite('./img1laplacianpyramid'+str(i)+'.jpg', img1_laplacian_pyramid[i])
        cv2.imwrite('./img2laplacianpyramid'+str(i)+'.jpg', img2_laplacian_pyramid[i])

    blending_laplacian_pyramid = get_blending_laplacian_pyramid(img1_laplacian_pyramid, img2_laplacian_pyramid, mask1_gaussian_pyramid, mask2_gaussian_pyramid)
    blending_image = reconstruct(blending_laplacian_pyramid)
    

    return blending_image

def reconstruct(blending_laplacian_pyramid) :

    blending_laplacian_pyramid = blending_laplacian_pyramid[::-1]
    recon_result = np.array([])
    
    for i in range(len(blending_laplacian_pyramid)-1) :

        if i == 0 :
            resize = image_upsampling(blending_laplacian_pyramid[i], blending_laplacian_pyramid[i+1].shape[:2])
        else :
            resize = image_upsampling(recon_result, blending_laplacian_pyramid[i+1].shape[:2])
        resize = np.float32(resize)
        recon_result = cv2.add(resize, np.float32(blending_laplacian_pyramid[i+1]))

    return recon_result

def get_blending_laplacian_pyramid(img1_laplacian_pyramid, img2_laplacian_pyramid, mask1_gaussian_pyramid, mask2_gaussian_pyramid) :

    blending = []

    for i in range(len(img1_laplacian_pyramid)) :
        blending.append(img1_laplacian_pyramid[i]*(mask1_gaussian_pyramid[i]) + img2_laplacian_pyramid[i]*(mask2_gaussian_pyramid[i]))
    
    for i in range(len(blending)) :
        cv2.imwrite('./blendinglaplacianpyramid'+str(i)+'.jpg', blending[i])
    return blending

def get_mask(merge, seam, xmin, ymin, x_offset) :

    mask = np.zeros_like(merge)
    index = 0

    if x_offset == 0 :
        for row in range(mask.shape[0]) :
            for column in range(mask.shape[1]) :
                if (merge[row][column] == np.array([0, 0, 0])).all() :
                    mask[row][column] = np.array([0, 0, 0])
                elif row <= ymin-1 or row > ymin+len(seam)-1 :
                    mask[row][column] = np.array([1, 1, 1])
                else :
                    if column <= xmin+seam[index] :
                        mask[row][column] = np.array([0, 0, 0])
                    else :
                        mask[row][column] = np.array([1, 1, 1])
            if row > ymin-1 and row <= ymin+len(seam)-1 :
                index += 1 
    else :
        for row in range(mask.shape[0]) :
            for column in range(mask.shape[1]) :
                if (merge[row][column] == np.array([0, 0, 0])).all() :
                    mask[row][column] = np.array([0, 0, 0])
                elif row <= ymin-1 or row > ymin+len(seam)-1 :
                    mask[row][column] = np.array([1, 1, 1])
                else :
                    if column <= xmin+seam[index] :
                        mask[row][column] = np.array([1, 1, 1])
                    else :
                        mask[row][column] = np.array([0, 0, 0])
            if row > ymin-1 and row <= ymin+len(seam)-1 :
                index += 1 

    return mask

def get_laplacian_pyramid(gaussian_pyramid) :

    laplacian_pyramid = []
    gaussian_pyramid = gaussian_pyramid[::-1]

    for i in range(len(gaussian_pyramid)) :
        if i == 0 :
            laplacian_pyramid.append(gaussian_pyramid[i])
        else :
            gaussian_pyramid_n = np.float32(gaussian_pyramid[i])
            gaussian_pyramid_n_minus_1 = np.float32(image_upsampling(gaussian_pyramid[i-1], gaussian_pyramid_n.shape[:2]))
            laplacian_pyramid.append(cv2.subtract(gaussian_pyramid_n, gaussian_pyramid_n_minus_1))

    laplacian_pyramid = laplacian_pyramid[::-1]

    return laplacian_pyramid

def get_gaussian_pyramid(image) :

    gaussian_pyramid = []
    image = image.astype(np.float32)
    gaussian_pyramid.append(image)

    i = 0
    while len(gaussian_pyramid) < 10 :

        if i == 0 :
            blurred = cv2.GaussianBlur(image, (5, 5), 0.7)
        else :
            blurred = cv2.GaussianBlur(gaussian_pyramid[i], (5, 5), 0.7)
        
        blurred = image_downsampling(blurred)
        gaussian_pyramid.append(blurred)

        i += 1

    return gaussian_pyramid

def image_downsampling(image) :

    resized_image = cv2.resize(image, dsize=(0, 0), fx=0.85, fy=0.85, interpolation=cv2.INTER_CUBIC)

    return resized_image

def image_upsampling(image, size) :

    resized_image = cv2.resize(image, dsize=size[::-1], interpolation=cv2.INTER_LINEAR)
    
    return resized_image