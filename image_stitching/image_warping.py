import image_feature_matching as ifm
import seam_finder_DP as sfd
import image_blending as ib

import cv2
import random
import numpy as np

def image_warping(img1, img2, outputfname, NNDR=0.7, ransac_trial=1000) :

    # if (type(img1) == str) :
    #     img1 = cv2.imread(img1)
    #     img2 = cv2.imread(img2)

    matching_keypoint1, matching_keypoint2, number_of_matching = ifm.get_matching_feature(img1, img2, NNDR)
    best_H, _ = RANSAC(matching_keypoint1, matching_keypoint2, number_of_matching, ransac_trial)
    
    [xmin, xmax, ymin, ymax], H_matrix = calculate_bounding_box_and_translate_H(img1, img2, best_H)
    canvas1 = cv2.warpPerspective(img1, H_matrix, (xmax-xmin, ymax-ymin))
    c1 = cv2.warpPerspective(img1, H_matrix, (xmax-xmin, ymax-ymin), borderMode=cv2.BORDER_REFLECT)
    cv2.imwrite('./cavnas1.jpg', canvas1)
    canvas2 = get_img2_canvas(canvas1.shape, img2, xmin, ymin)
    c2 = image_mirroring(canvas2, img2.shape)
    cv2.imwrite('./cavnas1.jpg', c1)
    cv2.imwrite('./canvas2.jpg', c2)
    _, overlap_box = get_overlap_image(canvas1, canvas2)
    merge = image_mosaicing(canvas1, canvas2)
    cv2.imwrite('./mosaicing.jpg', merge) 

    seam = sfd.seam_finder(merge[overlap_box[2]:overlap_box[3], overlap_box[0]:overlap_box[1]], True) 
    stitched_image = seam_stitching(canvas1, canvas2, merge, seam, overlap_box[0], overlap_box[2], xmin)
    cv2.imwrite('./usingSeamfinder.jpg', stitched_image)

    blending_image = ib.image_blending(canvas1, canvas2, c1, c2, stitched_image, seam, overlap_box, xmin)
    cv2.imwrite('./blending_image.jpg', blending_image)
    
    return blending_image

def RANSAC(kp1, kp2, number_of_matching, trial) :

    best_H = np.zeros((3, 3))
    best_inlier_ratio = 0

    for i in range(trial) :
        random_idx = random.sample(range(0, number_of_matching), 4)
        random_matching_keypoint = []                                                           # random_matching_keypoint : 4x2x3 행렬. kp1은 [x, y, 1]
                                                                                                # [[kp1, kp2],
        for idx in range(len(random_idx)) :                                                     #  [kp1, kp2],                         
            random_matching_keypoint.append([kp1[random_idx[idx]], kp2[random_idx[idx]]])       #  [kp1, kp2],
                                                                                                #  [kp1, kp2]]
        H = compute_homography(random_matching_keypoint)

        homography_transformation_value = []
        for index in range(number_of_matching) :
            transformation = np.dot(H, kp1[index])
            transformation = transformation/transformation[2]
            homography_transformation_value.append(transformation)

        diff = []
        for index in range(len(homography_transformation_value)) :
            diff.append(np.sqrt((homography_transformation_value[index][0] - kp2[index][0]) ** 2
                              + (homography_transformation_value[index][1] - kp2[index][1]) ** 2
                              + (homography_transformation_value[index][2] - kp2[index][2]) ** 2))

        inlier_number = 0

        for error in diff :
            if error <= 3 :
                inlier_number = inlier_number + 1
        
        inlier_ratio = inlier_number / len(diff)

        if inlier_ratio > best_inlier_ratio :
            best_inlier_ratio = inlier_ratio
            best_inlier_num = inlier_number

            best_H = H
     
    print('inlier 비율 : ', best_inlier_ratio)        

    return best_H, best_inlier_num

def remove_outlier(kp1, kp2, number_of_matching, trial) :

    best_inlier_ratio = 0

    for i in range(trial) :
        random_idx = random.sample(range(0, number_of_matching), 4)
        random_matching_keypoint = []                                                           # random_matching_keypoint : 4x2x3 행렬. kp1은 [x, y, 1]
                                                                                                # [[kp1, kp2],
        for idx in range(len(random_idx)) :                                                     #  [kp1, kp2],                         
            random_matching_keypoint.append([kp1[random_idx[idx]], kp2[random_idx[idx]]])       #  [kp1, kp2],
                                                                                                #  [kp1, kp2]]
        H = compute_homography(random_matching_keypoint)

        homography_transformation_value = []
        for index in range(number_of_matching) :
            transformation = np.dot(H, kp1[index])
            transformation = transformation/transformation[2]
            homography_transformation_value.append(transformation)


        diff = []
        for index in range(len(homography_transformation_value)) :
            diff.append(np.sqrt((homography_transformation_value[index][0] - kp2[index][0]) ** 2
                              + (homography_transformation_value[index][1] - kp2[index][1]) ** 2
                              + (homography_transformation_value[index][2] - kp2[index][2]) ** 2))

        inlier1 = []
        inlier2 = []
        inlier_number = 0
        for index in range(len(diff)) :
            if diff[index] <= 3 :
                inlier_number = inlier_number + 1
        
        inlier_ratio = inlier_number / len(diff)

        if inlier_ratio > best_inlier_ratio :
            best_inlier_ratio = inlier_ratio
            best_diff = diff
            
            for index in range(len(best_diff)) :
                if best_diff[index] <= 3 :
                    inlier1.append(kp1[index])
                    inlier2.append(kp2[index])

    return inlier1, inlier2
        
def compute_homography(random_matching_keypoint) :
    
    A = []

    if len(random_matching_keypoint) != 4 :
        return None

    for idx in range(4) :
        u = random_matching_keypoint[idx][0][0]
        v = random_matching_keypoint[idx][0][1]
        u_prime = random_matching_keypoint[idx][1][0]
        v_prime = random_matching_keypoint[idx][1][1]

        A.append([0, 0, 0, -u, -v, -1, v_prime * u, v_prime * v, v_prime])
        A.append([u, v, 1, 0, 0, 0, -u_prime * u, -u_prime * v, -u_prime])

    _, _, V = np.linalg.svd(A)

    H = np.reshape(V[8], (3, 3))
    H = H / H[2][2]

    return H

def calculate_bounding_box_and_translate_H(img1, img2, best_H) :

    h1, w1 = img1.shape[:2]
    h2, w2 = img2.shape[:2]

    corner1 = np.array([[0, 0] ,[0, h1], [w1, h1], [w1, 0]], dtype='float32').reshape(-1, 1, 2)
    corner2 = np.array([[0, 0] ,[0, h2], [w2, h2], [w2, 0]], dtype='float32').reshape(-1, 1, 2)

    corner1_transform = cv2.perspectiveTransform(corner1, best_H)
    corner = np.concatenate((corner1_transform, corner2), axis=0)

    [xmin, ymin] = corner.min(axis=0).ravel()
    [xmax, ymax] = corner.max(axis=0).ravel()

    xmin = int(xmin - 0.5)
    ymin = int(ymin - 0.5)
    xmax = int(xmax + 0.5)
    ymax = int(ymax + 0.5)

    bounding_box = [xmin, xmax, ymin, ymax]

    Ht = np.array([[1, 0, -bounding_box[0]],
                    [0, 1, -bounding_box[2]],
                    [0, 0, 1]])

    H_matrix = Ht.dot(best_H)

    return bounding_box, H_matrix

def get_overlap_image(canvas1, canvas2) :

    overlap = np.zeros_like(canvas1)

    x = []
    y = []

    for i in range(len(canvas2)) :
            for j in range(len(canvas2[0])) :
                if (canvas1[i][j] != np.array([0, 0, 0])).any() and (canvas2[i][j] != np.array([0, 0, 0])).any() :
                    overlap[i][j][:] = canvas1[i][j][:]
                    y.append(i)
                    x.append(j)

    return overlap[min(x):max(x), min(y):max(y)], [min(x), max(x), min(y), max(y)]

def seam_stitching(canvas1, canvas2, merge, x_coordinate, xmin, ymin, x_offset) :

    result = np.zeros_like(canvas1)
    # canvas 2가 왼쪽
    index = 0
    if x_offset == 0 :
        for i in range(len(canvas1)) :
            for j in range(len(canvas1[0])) :
                for k in range(len(canvas1[0][0])) :
                    if i <= ymin-1 or i > ymin+len(x_coordinate)-1 :
                        result[i][j][k] = max(canvas1[i][j][k], canvas2[i][j][k])
                    else :
                        if j <= xmin+x_coordinate[index] :
                            if canvas2[i][j].sum() < 10 :
                                result[i][j][k] = canvas1[i][j][k]
                            else :
                                result[i][j][k] = canvas2[i][j][k]
                        else :
                            if canvas1[i][j].sum() < 10 :
                                result[i][j][k] = canvas2[i][j][k]
                            else :
                                result[i][j][k] = canvas1[i][j][k]
            if i > ymin-1 and i <= ymin+len(x_coordinate)-1 :
                index += 1
    else :
        for i in range(len(canvas1)) :
            for j in range(len(canvas1[0])) :
                for k in range(len(canvas1[0][0])) :
                    if i <= ymin-1 or i > ymin+len(x_coordinate)-1  :
                        result[i][j][k] = max(canvas1[i][j][k], canvas2[i][j][k])
                    else :
                        if j <= xmin+x_coordinate[index] :
                            if canvas1[i][j].sum() < 10 :
                                result[i][j][k] = canvas2[i][j][k]
                            else :
                                result[i][j][k] = canvas1[i][j][k]
                        else :
                            if canvas2[i][j].sum() < 10 :
                                result[i][j][k] = canvas1[i][j][k]
                            else :
                                result[i][j][k] = canvas2[i][j][k]
            if i > ymin-1 and i <= ymin+len(x_coordinate)-1 :
                index += 1

    return result

def image_mosaicing(canvas1, canvas2) :

    merge = np.zeros_like(canvas1)

    for i in range(len(canvas2)) :
        for j in range(len(canvas2[0])) :
            for k in range(len(canvas2[0][0])) :
                if canvas1[i][j][k] > canvas2[i][j][k] :
                    merge[i][j] = canvas1[i][j]
                else :
                    merge[i][j] = canvas2[i][j]

    return merge

def get_img2_canvas(shape, img2, xmin, ymin) :
        
    canvas2 = np.zeros(shape)

    for i in range(len(img2)) :
        for j in range(len(img2[0])) :
            for k in range(len(img2[0][0])) :
                canvas2[i+int(abs(ymin))][j+int(abs(xmin))][k] = img2[i][j][k]
    
    return canvas2

def image_mirroring(canvas, shape) :

    mirrored_canvas = canvas.copy()

    left = 0
    right = 0
    top = 0
    bottom = 0

    for row in range(canvas.shape[0]) :
        for column in range(canvas.shape[1]) :
            if (canvas[row][column] != np.array([0, 0, 0])).any() and left == 0 :
                for i in range(shape[1]) :
                    if column-i >= 0 :
                        mirrored_canvas[row][column-i] = canvas[row][column+(i)]
                left = 1
            elif (canvas[row][column] != np.array([0, 0, 0])).any() and right == 0 :
                for i in range(shape[1]) :
                    if column+shape[1]+i < canvas.shape[1] :
                        mirrored_canvas[row][column+shape[1]+i] = canvas[row][column+shape[1]-(i)]
                right = 1
        left = 0
        right = 0

    for column in range(canvas.shape[1]) :
        for row in range(canvas.shape[0]) :
            if (canvas[row][column] != np.array([0, 0, 0])).any() and top == 0 :
                for i in range(shape[0]) :
                    if row-i >= 0 :
                        mirrored_canvas[row-i][column] = canvas[row+(i)][column]
                top = 1
            elif (canvas[row][column] != np.array([0, 0, 0])).any() and bottom == 0 :
                for i in range(shape[0]) :
                    if row+shape[0]+i < canvas.shape[0] :
                        mirrored_canvas[row+shape[0]+i][column] = canvas[row+shape[0]-(i)][column]
                bottom = 1
        top = 0
        right = 0

    return mirrored_canvas