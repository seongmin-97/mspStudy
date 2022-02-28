import image_feature_matching as ifm
import image_warping as iw

import cv2
import math
import numpy as np
from collections import deque

def image_stitching(fname_list, outputfname, NNDR=0.5, trial=500) :

    img_list = read_image_list(fname_list)
    matching_graph, best_H = calculate_matching_graph(img_list, NNDR, trial)

    print("매칭 관계 : ", matching_graph)
    print("-------------------------------")

    output = all_image_warping(img_list, matching_graph, NNDR, trial)
    cv2.imwrite(outputfname, output)
    return None

def read_image_list(fname_list) :

    img_list = []

    for fname in fname_list :
        img_list.append(cv2.imread(fname))

    return img_list

def all_image_warping(img_list, matching_graph, NNDR, trial) :

    visited = [False] * len(img_list)
    visited[0] = True
    queue = deque([0])
    end_warping = []
    start = 0

    while queue :
        img_pair = queue.popleft()
        # print(matching_graph[img_pair])

        # warping
        for pair in matching_graph[img_pair] :
            # 이미 와핑이 된 경우인지 확인 
            # ex) [0, 3]이 완료되었으면 [3, 0]은 안해도 된다.
            if [pair[1], pair[0]] in end_warping :
                continue
            print(pair)
            if start == 0 :
                result = iw.image_warping(img_list[pair[0]], img_list[pair[1]], '', NNDR, trial)
                start = 1
            else :
                result = iw.image_warping(img_list[pair[1]], result, '', NNDR, trial)
            end_warping.append(pair)
        # BFS
        for data in matching_graph[img_pair] :
            if not visited[data[1]] :
                queue.append(data[1])
                visited[data[1]] = True

    return result

def calculate_matching_graph(img_list, NNDR, trial) :

    matching_graph = []
    n_f, n_i, best_H = calculate_number_of_feature_and_H(img_list, NNDR, trial)

    for i in range(len(img_list)) :
        node_info = []

        for j in range(len(img_list)) :
            if isMatching(n_f[i][j], n_i[i][j]) :
                node_info.append([i, j])

        matching_graph.append(node_info)
        
    return matching_graph, best_H

def calculate_number_of_feature_and_H(img_list, NNDR, trial) :
    
    n_f = []
    n_i = []
    best_H_list = []
    
    for row in range(len(img_list)) :
        
        num_matching = []
        num_inlier = []
        # best_H_row = []
        
        for column in range(len(img_list)) :
            
            if row == column :
                num_matching.append(0)
                num_inlier.append(0)
                # best_H_row.append([[0, 0, 0],[0, 0, 0],[0, 0, 0]])
            elif row > column :
                num_matching.append(n_f[column][row])
                num_inlier.append(n_i[column][row])

                # if n_f[column][row] < 4 :
                    # best_H_row.append([[0, 0, 0],[0, 0, 0],[0, 0, 0]])
                # else :
                    # best_H, best_inlier_num = iw.RANSAC(matching_keypoint1, matching_keypoint2, len(matching_keypoint1), trial)
                    # best_H_row.append(best_H)
            else :
                matching_keypoint1, matching_keypoint2, number_of_matching = ifm.get_matching_feature(img_list[row], img_list[column], NNDR)
                
                if number_of_matching < 4 :
                    num_matching.append(number_of_matching)
                    num_inlier.append(0)
                    # best_H_row.append([[0, 0, 0],[0, 0, 0],[0, 0, 0]])
                else :
                    best_H, best_inlier_num = iw.RANSAC(matching_keypoint1, matching_keypoint2, len(matching_keypoint1), trial)

                    num_matching.append(number_of_matching)
                    num_inlier.append(best_inlier_num)
                    # best_H_row.append(best_H)

        n_f.append(num_matching)
        n_i.append(num_inlier)
        # best_H_list.append(best_H_row)

        print("피쳐 개수, H 계산 중...", (row+1)/len(img_list))
    
    for image in img_list :
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

    matching_probability = 1.0 / (1 + (f_when_p_m0 * p_m0) / (f_when_p_m1 * p_m1 + 0.000000000000001))

    if matching_probability > p_min :
        return True
    else :
        return False


# fname_list = ['./data/school2.jpg' ,'./data/school1.jpg', './data/school3.jpg', './data/school4.jpg']
fname_list = ['./data/school1.jpg', './data/school2.jpg', './data/school3.jpg', './data/school5.jpg',
              './data/school8.jpg']


image_stitching(fname_list, './output.jpg', NNDR=0.7, trial=1000)