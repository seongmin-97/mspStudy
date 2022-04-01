import image_feature_matching as ifm
import illumination_compensation as ic
import seam_finder_DP as sfd
import image_blending as ib

import cv2
import random
import numpy as np

def image_warping(img1, img2, NNDR=0.7, ransac_trial=500, mask = None, i=0) :

    matching_keypoint1, matching_keypoint2, number_of_matching = ifm.get_matching_feature(img1, img2, NNDR)
    best_H, _ = RANSAC(matching_keypoint1, matching_keypoint2, number_of_matching, ransac_trial)
    
    boundingBox, H_matrix = calculate_bounding_box_and_translate_H(img1, img2, best_H)
    canvas1 = cv2.warpPerspective(img1, H_matrix, (boundingBox[1]-boundingBox[0], boundingBox[3]-boundingBox[2]))
    canvas2 = get_img2_canvas(canvas1.shape, img2, boundingBox[0], boundingBox[2])    

    translate_matrix = np.array([[1, 0, -boundingBox[0]], [0, 1, -boundingBox[2]], [0, 0, 1]])

    overlap_box = sfd.get_overlap_image(canvas1, canvas2)
    merge = image_mosaicing(canvas1, canvas2)
    
    seam = sfd.seam_finder(merge[overlap_box[2]:overlap_box[3], overlap_box[0]:overlap_box[1]], True) 
    # stitched_image = seam_stitching(canvas1, canvas2, seam, overlap_box[0], overlap_box[2], boundingBox[0])
    
    # mask2는 여태까지 모든 마스크의 리스트
    # x_offset 지정
    if mask != None  :

        for i in range(len(mask)) :
            mask[i] = get_img2_canvas(canvas1.shape, mask[i], boundingBox[0], boundingBox[2])
        
        coordinate = np.array([0, 0, 1])@best_H
        if boundingBox[0] < coordinate[0]:
            mask1 = ib.get_mask(canvas1, seam, overlap_box[0], overlap_box[2], 1)
            previous_hole_mask = ib.get_mask(canvas2, seam, overlap_box[0], overlap_box[2], 0)
        else :
            mask1 = ib.get_mask(canvas1, seam, overlap_box[0], overlap_box[2], 0)
            previous_hole_mask = ib.get_mask(canvas2, seam, overlap_box[0], overlap_box[2], 1)
        
        cv2.imwrite('previous_whole_mask'+str(i)+'.jpg', previous_hole_mask*255)
        
        for i in range(len(mask)) :
            mask[i] = cv2.bitwise_and(previous_hole_mask, mask[i])

        mask.append(mask1)
        return merge, H_matrix, translate_matrix, boundingBox, mask
        
    else :
        if overlap_box[0] == 0 :
            x = 1
        else :
            x = 0
        mask1 = ib.get_mask(canvas1, seam, overlap_box[0], overlap_box[2], overlap_box[0])
        mask2 = ib.get_mask(canvas2, seam, overlap_box[0], overlap_box[2], x)
    
    # c1 = cv2.warpPerspective(img1, H_matrix, (xmax-xmin, ymax-ymin), borderMode=cv2.BORDER_REFLECT)
    # c2 = image_mirroring(canvas2, img2.shape)
    # blending_image = ib.image_blending(canvas1, canvas2, c1, c2, stitched_image, seam, overlap_box, xmin)
    
        return merge, H_matrix, translate_matrix, boundingBox, [mask1, mask2]

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

def seam_stitching(canvas1, canvas2, x_coordinate, xmin, ymin, x_offset) :

    result = np.zeros_like(canvas1)
    # canvas 2가 왼쪽
    index = 0
    if x_offset == 0 :
        for i in range(len(canvas1)) :
            for j in range(len(canvas1[0])) :
                for k in range(len(canvas1[0][0])) :
                    if i <= ymin-1 or i > ymin+len(x_coordinate)-1 :
                        if canvas1[i][j].sum() > canvas2[i][j].sum() :
                            result[i][j][k] = canvas1[i][j][k]
                        else :
                            result[i][j][k] = canvas2[i][j][k]
                    else :
                        # 밝기가 어두우면 다른 캔버스의 픽셀로 대체하는 이유는 검은 공백을 메우기 위해서이다.
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
                        if canvas1[i][j].sum() > canvas2[i][j].sum() :
                            result[i][j][k] = canvas1[i][j][k]
                        else :
                            result[i][j][k] = canvas2[i][j][k]
                    else :
                        # 밝기가 어두우면 다른 캔버스의 픽셀로 대체하는 이유는 검은 공백을 메우기 위해서이다.
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
