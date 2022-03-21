import cv2
import numpy as np

# 두 이미지에 대해서만 가능한 코드

def gain_based_esposure_compensation(canvas1, canvas2, overlap_mask) :

    I12, N12 = calculate_mean_intensity_overlap(overlap_mask * canvas1)
    I21, N21 = calculate_mean_intensity_overlap(overlap_mask * canvas2)

    cv2.imwrite('./c1_overlap.jpg', canvas1*overlap_mask)
    cv2.imwrite('./c2_overlap.jpg', canvas2*overlap_mask)
    b = calculate_b_matrix(N12, N21)
    A = calculate_A_matrix(I12, I21, N12, N21, b)
    g = get_gain(A, b)

    # canvas1에 g[0], canvas2에 g[1]을 곱해서 사용
    return g 

def get_gain(A, b) :
    return np.dot(np.linalg.inv(A), np.transpose(b))

def calculate_A_matrix(I12, I21, N12, N21, b) :

    A = np.zeros((2, 2)) 

    A[0][0] = b[0] + (0.02 * I12 ** 2 * N12)
    A[0][1] = -0.02 * I12 * I21 * N12
    A[1][0] = -0.02 * I21 * I12 * N21
    A[1][1] = b[1] + (0.02 * I21 ** 2 * N21)

    return A

def calculate_b_matrix(N12, N21) :

    b = np.array([0, 0])

    b[0] = 100 * (N12)
    b[1] = 100 * (N21)

    return b

def calculate_mean_intensity_overlap(overlap) :

    intensity = 0
    N = 0

    for row in range(overlap.shape[0]) :
        for column in range(overlap.shape[1]) :
            intensity = intensity + calculate_intensity(overlap[row][column])
            if (overlap[row][column] != np.array([0, 0, 0])).any() :
                N = N + 1

    return intensity / N, N

def calculate_intensity(pixel) :
    return np.sqrt(pixel[0] ** 2 + pixel[1] ** 2 + pixel[2] ** 2)