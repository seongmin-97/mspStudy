import image_feature_matching as ifm
import image_warping as iw

import numpy as np
import math

def image_stitching(fname_list, NNDR=0.5, trial=500) :

    matching_graph, best_H = calculate_matching_graph(fname_list, NNDR)

    print("매칭 관계 : ", matching_graph)

    for i in range(len(matching_graph)) :
        for j in range(len(matching_graph[0])) :
            if matching_graph[i][j] == 1 :
                pass
            else :
                continue

    return None

def calculate_matching_graph(fname_list, NNDR) :

    matching_graph = np.zeros((len(fname_list), len(fname_list)))
    n_f, n_i, best_H = calculate_number_of_feature_and_H(fname_list, NNDR)

    for i in range(len(fname_list)) :
        for j in range(len(fname_list)) :
            if i == j :
                matching_graph[i][j] = 0
            elif isMatching(n_f[i][j], n_i[i][j]) :
                matching_graph[i][j] = 1
            else :
                matching_graph[i][j] = 0

    return matching_graph, best_H

def calculate_number_of_feature_and_H(fname_list, NNDR) :
    
    n_f = []
    n_i = []
    best_H_list = []
    
    for row in range(len(fname_list)) :
        
        num_matching = []
        num_inlier = []
        best_H_row = []
        
        for column in range(len(fname_list)) :
            
            if row == column :
                num_matching.append(0)
                num_inlier.append(0)
                best_H_row.append([[0, 0, 0],[0, 0, 0],[0, 0, 0]])
            elif row > column :
                num_matching.append(n_f[column][row])
                num_inlier.append(n_i[column][row])
                best_H_row.append(best_H_list[column][row])
            else :
                matching_keypoint1, matching_keypoint2, number_of_matching = ifm.get_matching_feature(fname_list[row], fname_list[column], NNDR)
                
                if number_of_matching < 4 :
                    num_matching.append(number_of_matching)
                    num_inlier.append(0)
                    best_H_row.append([[0, 0, 0],[0, 0, 0],[0, 0, 0]])
                else :
                    best_H, best_inlier_num = iw.RANSAC(matching_keypoint1, matching_keypoint2, len(matching_keypoint1), 1000)

                    num_matching.append(number_of_matching)
                    num_inlier.append(best_inlier_num)
                    best_H_row.append(best_H)

        n_f.append(num_matching)
        n_i.append(num_inlier)
        best_H_list.append(best_H_row)

        print("피쳐 개수, H 계산 중...", (row+1)/len(fname_list))
    
    for image in fname_list :
        kp, _ = ifm.get_feature_SIFT(image, False)
        n_f.append(len(kp))

    return n_f, n_i, best_H_list

def isMatching(n_f, n_i) :
    
    p1 = 0.6 # 
    p0 = 0.1
    p_m1 = 0.000001
    p_m0 = 1-p_m1
    p_min = 0.999

    f_when_p_m1 = math.comb(n_f, n_i) * (p1 ** n_i) * ((1 - p1) ** (n_f - n_i))
    f_when_p_m0 = math.comb(n_f, n_i) * (p0 ** n_i) * ((1 - p0) ** (n_f - n_i))

    matching_probability = 1.0 / (1 + (f_when_p_m0 * p_m0) / (f_when_p_m1 * p_m1))

    if matching_probability > p_min :
        return True
    else :
        return False


fname_list = ['./data/school2.jpg', './data/school1.jpg' ,'./data/school3.jpg', './data/school6.jpg', 
              './data/school8.jpg', './data/school7.jpg', './data/school4.jpg', './data/school5.jpg']

image_stitching(fname_list, NNDR=0.5, trial=500)