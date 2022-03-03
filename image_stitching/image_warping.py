import image_feature_matching as ifm
import cv2
import random
import numpy as np

def image_warping(img1, img2, outputfname, NNDR=0.7, trial=1000) :

    matching_keypoint1, matching_keypoint2, number_of_matching = ifm.get_matching_feature(img1, img2, NNDR)
    best_H, _ = RANSAC(matching_keypoint1, matching_keypoint2, number_of_matching, trial)
    [xmin, xmax, ymin, ymax], H_matrix = calculate_bounding_box_and_translate_H(img1, img2, best_H)
    canvas = cv2.warpPerspective(img1, H_matrix, (xmax-xmin, ymax-ymin))
    result = image_mosaicing(canvas, img2, abs(xmin), abs(ymin))
    
    if len(outputfname) == 0 :
        return result
    else :
        cv2.imwrite(outputfname, result)
        return result

def image_warping_using_fname(fname1, fname2, outputfname, NNDR=0.8, ransac_trial=1000) :

    img1 = cv2.imread(fname1)
    img2 = cv2.imread(fname2)

    matching_keypoint1, matching_keypoint2, number_of_matching = ifm.get_matching_feature(fname1, fname2, NNDR)
    best_H, _ = RANSAC(matching_keypoint1, matching_keypoint2, number_of_matching, ransac_trial)

    [xmin, xmax, ymin, ymax], H_matrix = calculate_bounding_box_and_translate_H(img1, img2, best_H)
    canvas = cv2.warpPerspective(img1, H_matrix, (xmax-xmin, ymax-ymin))
    result = image_mosaicing(canvas, img2, abs(xmin), abs(ymin))

    cv2.imwrite(outputfname, result)

    return result

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

def image_mosaicing(canvas1, img2, x_offset, y_offset, mode=2) :
    
    if mode == 1 :
        canvas1[int(abs(y_offset)):img2.shape[0]+int(abs(y_offset)), int(abs(x_offset)):img2.shape[1]+int(abs(x_offset)), :] = img2[:, :, :]
    elif mode == 2 :
        for i in range(len(img2)) :
            for j in range(len(img2[0])) :
                for k in range(len(img2[0][0])) :
                    if canvas1[i+int(abs(y_offset))][j+int(abs(x_offset))][k] > img2[i][j][k] :
                        canvas1[i+int(abs(y_offset))][j+int(abs(x_offset))] = canvas1[i+int(abs(y_offset))][j+int(abs(x_offset))]
                    else :
                        canvas1[i+int(abs(y_offset))][j+int(abs(x_offset))] = img2[i][j]

    return canvas1