import cv2
import numpy as np

import seam_finder_DP as sfd
import image_warping as iw
# 1. seam 없이 진행하거나
# 2. warping 할 때 seam을 지속적으로 구하고, mask도 계속 같이 transform 하면서 mask 저장
def image_blending(mirroring_list, H_matrix_list, boundingBox, mask_list) :

    canvas = []
    for i, image in enumerate(mirroring_list) :
        H_matrix_list[i] = H_matrix_list[i].astype('float32')
        warping = cv2.warpPerspective(image, H_matrix_list[i], (boundingBox[1]-boundingBox[0], boundingBox[3]-boundingBox[2]), borderMode=cv2.BORDER_REFLECT)
        canvas.append(warping)
        cv2.imwrite('./canvas'+str(i)+'.jpg', warping)

    #     transform_coordinate = H_matrix_list[i]@np.array([0, 0, 1])
    #     x_offset.append(transform_coordinate[0])
    # print(x_offset)

    for i in range(len(canvas)) :
        cv2.imwrite('./cut'+str(i)+'.jpg', canvas[i] * mask_list[i])
        cv2.imwrite('./mask'+str(i)+'.jpg', mask_list[i] * 255)

    gaussian_pyramid_list = []
    for i in range(len(canvas)) :
        gaussian_pyramid_list.append(get_gaussian_pyramid(canvas[i]))

    laplacian_pyramid_list = []
    for i in range(len(canvas)) :
        laplacian_pyramid_list.append(get_laplacian_pyramid(gaussian_pyramid_list[i]))

    mask_gaussian_pyramid_list = []
    for i in range(len(canvas)) :
        mask_gaussian_pyramid_list.append(get_gaussian_pyramid(mask_list[i]))

    blending_laplacian_pyramid = get_blending_laplacian_pyramid(laplacian_pyramid_list, mask_gaussian_pyramid_list)
    blending_image = reconstruct(blending_laplacian_pyramid)

    cv2.imwrite('./result1.jpg', blending_image)
    
#     for pairs in matching_pair :
#         for pair in pairs :
#             # 겹치는 pair 삭제
#             for idx, i in enumerate(matching_pair[pair[1]]) :
#                 if i == [pair[1], pair[0]] :
#                     del matching_pair[pair[1]][idx]
# # pair 별로 seam 찾아야 함
# # 겹칠 때 어떻게 해야 하는지 살펴야 함
# # x_offset 역할은 어떻게..?
#     i = 0
#     mask = []
#     for i in range(len(canvas)) :
#         mask.append(np.zeros_like(canvas[1]))

    
    # for pairs in matching_pair :
    #     for pair in pairs :
    #         print(str(pair)+'overlap...')
    #         overlapMask, overlap_box = sfd.get_overlap_image(canvas[pair[0]], canvas[pair[1]])
    #         if len(overlap_box) == 0 :
    #             continue
    #         cv2.imwrite('./overlap'+str(pair[0])+str(pair[1])+'.jpg', overlapMask*255)
    #         mosaic = iw.image_mosaicing(canvas[pair[0]], canvas[pair[1]])
    #         seam = sfd.seam_finder(mosaic[overlap_box[2]:overlap_box[3]+1, overlap_box[0]:overlap_box[1]]+1, True, i)

    #         if x_offset[pair[0]] > x_offset[pair[1]] :    
    #             mask[pair[0]] = get_mask(canvas[pair[0]], seam, overlapMask, overlap_box[0], overlap_box[2], 0)    
    #             mask[pair[1]] = get_mask(canvas[pair[1]], seam, overlapMask, overlap_box[0], overlap_box[2], 1)    
    #         else :
    #             mask[pair[0]] = get_mask(canvas[pair[0]], seam, overlapMask, overlap_box[0], overlap_box[2], 1)    
    #             mask[pair[1]] = get_mask(canvas[pair[1]], seam, overlapMask, overlap_box[0], overlap_box[2], 0)    

    #         canvas[pair[0]] = canvas[pair[0]] * mask[pair[0]]
    #         canvas[pair[1]] = canvas[pair[1]] * mask[pair[1]]
    #         i += 1
    

    # if x_offset == 0 :
    #     x = 1
    # else :
    #     x = 0

    # mask1 = get_mask(canvas1, seam, overlapBox[0], overlapBox[2], x_offset)
    # mask2 = get_mask(canvas2, seam, overlapBox[0], overlapBox[2], x)

    # mask1_gaussian_pyramid = get_gaussian_pyramid(mask1)
    # mask2_gaussian_pyramid = get_gaussian_pyramid(mask2)
    
    # blending_laplacian_pyramid = get_blending_laplacian_pyramid(img1_laplacian_pyramid, mask1_gaussian_pyramid, img2_laplacian_pyramid, mask2_gaussian_pyramid)
    # blending_image = reconstruct(blending_laplacian_pyramid)


    return 

def reconstruct(blending_laplacian_pyramid) :

    blending_laplacian_pyramid = blending_laplacian_pyramid[::-1]
    recon_result = np.array([])
    
    for i in range(len(blending_laplacian_pyramid)-1) :

        if i == 0 :
            resize = image_upsampling(blending_laplacian_pyramid[i], blending_laplacian_pyramid[i+1].shape)
        else :
            resize = image_upsampling(recon_result, blending_laplacian_pyramid[i+1].shape)
        resize = np.float32(resize)
        recon_result = cv2.add(resize, np.float32(blending_laplacian_pyramid[i+1]))

    return recon_result

def get_blending_laplacian_pyramid(img_laplacian_pyramid_list, mask_gaussian_pyramid_list) :

    blending = []

    for pyramid_idx in range(len(img_laplacian_pyramid_list[0])) :
        unit = np.zeros_like(img_laplacian_pyramid_list[0][pyramid_idx])
        for img_idx in range(len(img_laplacian_pyramid_list)) :
            unit = unit + img_laplacian_pyramid_list[img_idx][pyramid_idx] * mask_gaussian_pyramid_list[img_idx][pyramid_idx]
        blending.append(unit)

    # for i in range(len(img1_laplacian_pyramid)) :
        # blending.append(img1_laplacian_pyramid[i]*(mask1_gaussian_pyramid[i]) + img2_laplacian_pyramid[i]*(mask2_gaussian_pyramid[i]))
    
    return blending

def get_mask(canvas, seam, xmin, ymin, x_offset) :

    mask = np.zeros_like(canvas)
    index = 0

    if x_offset == 0 :
        for row in range(mask.shape[0]) :
            for column in range(mask.shape[1]) :
                if (canvas[row][column] == np.array([0, 0, 0])).all() :
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
                if (canvas[row][column] == np.array([0, 0, 0])).all() :
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
            gaussian_pyramid_n_minus_1 = np.float32(image_upsampling(gaussian_pyramid[i-1], gaussian_pyramid_n.shape))
            laplacian_pyramid.append(cv2.subtract(gaussian_pyramid_n, gaussian_pyramid_n_minus_1))

    laplacian_pyramid = laplacian_pyramid[::-1]

    return laplacian_pyramid

def get_gaussian_pyramid(image) :

    
    gaussian_pyramid = []
    image = image.astype(np.float32)
    gaussian_pyramid.append(image)

    i = 0
    while len(gaussian_pyramid) < 5 :

        blurred = cv2.GaussianBlur(gaussian_pyramid[i], (5, 5), 1)

        blurred = image_downsampling(blurred)
        gaussian_pyramid.append(blurred)

        i += 1

    return gaussian_pyramid

def image_downsampling(image) :

    resized_image = cv2.resize(image, dsize=(0, 0), fx=0.5, fy=0.5, interpolation=cv2.INTER_LINEAR)

    return resized_image

def image_upsampling(image, size) :

    resized_image = cv2.pyrUp(image)
    resized_image = cv2.resize(image, dsize=(size[1], size[0]))

    return resized_image