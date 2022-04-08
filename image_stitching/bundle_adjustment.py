import numpy as np

def bundle_adjustment(matching_pair, H) :
    # matching pair[0]과 H 연산 하면 matching pair[1] 예측치
    H = np.array(H)
    H = H.reshape(-1, )
    H = H[:8]

    X = matching_pair[0]
    X_dash = predict(X, H)
    jacobian = get_jacobian_matrix(X, X_dash, H)
    mue = 0.003
    previous_error = np.mean(get_error_vector(X, X_dash))
    best_H = H

    iter_num = 0
    print('bundle adjustment 수행 전', previous_error)

    while True :
        
        jacobian = get_jacobian_matrix(X, X_dash, H)
        variance = get_varience_H(jacobian, mue, X, X_dash)
        
        H = H - variance
        X_dash = predict(X, H)
        error = np.mean(get_error_vector(X, X_dash))

        if error > previous_error :
            mue = mue * 1.05
        else :
            mue = 0.003

        iter_num += 1
        previous_error = error

        if iter_num % 10 == 0 :
            print('error : ', error)
        if error < 10**-1 :
            print('bundle adjustment 수행 결과', error)
            return H_transform_3x3(H)
        elif iter_num > 1000 :
            print('bundle adjustment 수행 결과', error)
            return H_transform_3x3(best_H)

def get_varience_H(jacobian, mue, X, X_dash) :

    jacobian_transpose = np.transpose(jacobian)
    jacobian_square = np.matmul(jacobian_transpose, jacobian)
    diagonal_matrix = np.identity(n = 8)
    diag = np.diagonal(jacobian_square)

    for i in range(8) :
        diagonal_matrix[i][i] = diag[i]

    inverse_matrix = np.linalg.inv(jacobian_square + mue * diagonal_matrix)
    inverse_matrix_multiply = np.matmul(inverse_matrix, jacobian_transpose)
    error_vector = get_error_vector(X, X_dash)

    varience = np.matmul(inverse_matrix_multiply, error_vector)

    return varience.reshape(-1, )

def get_jacobian_matrix(X, X_dash, H) :

    jacobian = np.zeros((len(X), 8))
    
    # a = H00, b = H01, c = H02, ... h = H21 (H22 = 1)
    for i, [x, y, _] in enumerate(X) :

        x_dash = X_dash[i][0]
        y_dash = X_dash[i][1]

        ax_by_c = H[0] * x + H[1] * y + H[2]
        dx_ey_f = H[3] * x + H[4] * y + H[5]
        gx_hy_1 = H[6] * x + H[7] * y + 1

        derivative_a = -2 * (x / gx_hy_1) * (x_dash - ax_by_c / gx_hy_1) 
        derivative_b = -2 * (y / gx_hy_1) * (x_dash - ax_by_c / gx_hy_1) 
        derivative_c = -2 / gx_hy_1 * (x_dash - ax_by_c / gx_hy_1) 

        derivative_d = -2 * (x / gx_hy_1) * (y_dash - dx_ey_f / gx_hy_1) 
        derivative_e = -2 * (y / gx_hy_1) * (y_dash - dx_ey_f / gx_hy_1) 
        derivative_f = -2 / gx_hy_1 * (y_dash - dx_ey_f / gx_hy_1)

        derivative_g = 2 * x * (x_dash - ax_by_c / gx_hy_1) * (ax_by_c / (gx_hy_1 ** 2)) + 2 * x * (y_dash - dx_ey_f / gx_hy_1) * (dx_ey_f / (gx_hy_1 ** 2))
        derivative_h = 2 * y * (x_dash - ax_by_c / gx_hy_1) * (ax_by_c / (gx_hy_1 ** 2)) + 2 * y * (y_dash - dx_ey_f / gx_hy_1) * (dx_ey_f / (gx_hy_1 ** 2))

        jacobian[i] = np.array([derivative_a, derivative_b, derivative_c, derivative_d, derivative_e, derivative_f, derivative_g, derivative_h])

    return jacobian

def predict(x, H) :

    H_matrix = H_transform_3x3(H)
    predict_vector = np.zeros((len(x), 3))

    for i in range(len(x)) :
        predict_vector[i] = np.matmul(H_matrix, np.array(x[i]))

    return predict_vector

def get_error_vector(X, X_dash) :

    error_vector = np.zeros((len(X), 1))

    for i in range(len(X)) :
        error_vector[i][0] = get_error(X[i], X_dash[i])

    return error_vector

def get_error(X, X_dash) :

    error = 0
    for i in range(len(X)) :
        error += np.sqrt((X[i] - X_dash[i]) ** 2)

    return error

def H_transform_3x3(H) :

    H_matrix = np.zeros((1, 9)).reshape(-1, )
    H_matrix[0:8] = H
    H_matrix[8] = 1
    H_matrix = H_matrix.reshape(3, 3)

    return H_matrix