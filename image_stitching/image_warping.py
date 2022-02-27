
import cv2
import random
import numpy as np

def image_warping(fname1, fname2, outputfname, H) :
    
    img1 = cv2.imread(fname1)
    img2 = cv2.imread(fname2)

    #best_H, _ = RANSAC(matching_keypoint1, matching_keypoint2, number_of_matching, ransac_trial)
    best_H, x_offset, y_offset = translate_homography(H, img1)
    
    img1_homography_transformation_to_img2 = homography_transformation(best_H, img1)
    bounding_box = homography_coordinate_box(img1_homography_transformation_to_img2)

    canvas1 = cv2.warpPerspective(img1, best_H, (max(int(bounding_box[3]), img2.shape[0]+int(abs(y_offset))), max(img2.shape[1]+int(abs(x_offset)), int(bounding_box[1]))), flags=cv2.INTER_LINEAR)    
    #cv2.imwrite('./homography.jpg', canvas1)

    result = image_mosaicing(canvas1, img2, x_offset, y_offset)
    
    if len(outputfname) == 0 :
        return result
    else :
        cv2.imwrite(outputfname, result)
        return result

def temporary_image_warping(fname1, fname2, NNDR=0.8, ransac_trial=1000) :
    
    import image_feature_matching as ifm

    img1 = cv2.imread(fname1)
    img2 = cv2.imread(fname2)

    matching_keypoint1, matching_keypoint2, number_of_matching = ifm.get_matching_feature(fname1, fname2, NNDR)
    best_H, best_inlier_num = RANSAC(matching_keypoint1, matching_keypoint2, number_of_matching, ransac_trial)

    h1, w1 = img1.shape[:2]
    h2, w2 = img2.shape[:2]

    pts1 = np.array([[0, 0] ,[0, h1], [w1, h1], [w1, 0]], dtype='float32').reshape(-1, 1, 2)
    pts2 = np.array([[0, 0] ,[0, h2], [w2, h2], [w2, 0]], dtype='float32').reshape(-1, 1, 2)

    pts1_ = cv2.perspectiveTransform(pts1, best_H)
    pts = np.concatenate((pts1_, pts2), axis=0)

    print('------------------------')
    print(pts.min(axis=0))
    print(pts.min(axis=0).ravel())

    [xmin, ymin] = pts.min(axis=0).ravel()
    [xmax, ymax] = pts.max(axis=0).ravel()

    xmin = int(xmin - 0.5)
    ymin = int(ymin - 0.5)
    xmax = int(xmax + 0.5)
    ymax = int(ymax + 0.5)

    t = [-xmin, -ymin]

    Ht = np.array([[1, 0, t[0]],
                    [0, 1, t[1]],
                    [0, 0, 1]])

    result = cv2.warpPerspective(img1, Ht.dot(best_H), (xmax-xmin, ymax-ymin))
    result[t[1]:h2+t[1], t[0]:w2+t[0], :] = img2[:, :, :]

    cv2.imwrite('./temporary_result.jpg', result)

    # print("결과" ,bounding_box)


    # canvas1 = cv2.warpPerspective(img1, best_H, (1000, 1000), flags=cv2.INTER_LINEAR)
    # result = image_mosaicing(canvas1, img2, x_offset, y_offset)
    # print("결과 그리기 시작")

    cv2.imwrite('./temporary_result.jpg', result)

    return result

def calculate_bounding_box(translated_image, image, x_offset, y_offset, H) :

    translated_image = np.array(translated_image)

    # x min, x max, y min, y max
    image_bounding_box = []
    translated_image_bounding_box = []
    result_bounding_box = []

    image_bounding_box.append(x_offset)
    image_bounding_box.append(image.shape[1] + x_offset)
    image_bounding_box.append(y_offset)
    image_bounding_box.append(image.shape[0] + y_offset)

    size_matrix = np.array([[1, translated_image.shape[1], 1, translated_image.shape[1]],
                            [1, 1, translated_image.shape[0], translated_image.shape[0]],
                            [1, 1, 1, 1]])
    print(H.shape, size_matrix.shape)
    calculate_edge = np.matmul(H, size_matrix)
    calculate_edge = calculate_edge / calculate_edge[2][3]

    translated_image_bounding_box.append(min(calculate_edge[0]))
    translated_image_bounding_box.append(max(calculate_edge[0]))
    translated_image_bounding_box.append(min(calculate_edge[1]))
    translated_image_bounding_box.append(max(calculate_edge[1]))

    print("img2 : ", image_bounding_box)
    print("transform : ", translated_image_bounding_box)

    result_bounding_box.append(min(image_bounding_box[0], translated_image_bounding_box[0]))
    result_bounding_box.append(max(image_bounding_box[1], translated_image_bounding_box[1]))
    result_bounding_box.append(min(image_bounding_box[2], translated_image_bounding_box[2]))
    result_bounding_box.append(max(image_bounding_box[3], translated_image_bounding_box[3]))

    return result_bounding_box


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

def homography_transformation(H, image) : 

    result = [] 
    
    width = image.shape[1]
    height = image.shape[0]

    for row in range(height) :
        rows = []
        
        for column in range(width) :
            position = np.array([row, column, 1])
            transformation = np.matmul(H, position)
            transformation = transformation/transformation[2]
            rows.append(transformation)
        
        result.append(rows)

    return result    

def translate_homography(best_H, img1) :

    img1_homography_transformation_to_img2 = homography_transformation(best_H, img1)
    print("sdfsdf", np.array(img1_homography_transformation_to_img2).shape)
    x_offset = 0
    y_offset = 0

    inverse = np.linalg.inv(best_H)

    translate = np.matmul(inverse, np.array([0, 0, 1]))

    translate_trnasformation = np.array([[1, 0, translate[0]/translate[2]],
                                         [0, 1, translate[1]/translate[2]],
                                         [0, 0, 1]])
    best_H = np.matmul(translate_trnasformation, best_H)
    best_H = best_H / best_H[2][2]

    return best_H, abs(x_offset), abs(y_offset)

def image_mosaicing(canvas1, img2, x_offset, y_offset) :
    
    # canvas1[int(abs(y_offset)):img2.shape[0]+int(abs(y_offset)), int(abs(x_offset)):img2.shape[1]+int(abs(x_offset)), :] = img2[:, :, :]

    for i in range(len(img2)) :
        for j in range(len(img2[0])) :
            for k in range(len(img2[0][0])) :
                if canvas1[i+int(abs(y_offset))][j+int(abs(x_offset))][k] > img2[i][j][k] :
                    canvas1[i+int(abs(y_offset))][j+int(abs(x_offset))] = canvas1[i+int(abs(y_offset))][j+int(abs(x_offset))]
                else :
                    canvas1[i+int(abs(y_offset))][j+int(abs(x_offset))] = img2[i][j]

    return canvas1

temporary_image_warping('./data/school3.jpg', './temporary_result.jpg')