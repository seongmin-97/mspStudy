from unittest import result
import numpy as np
import cv2 

def sean_finder(image) :

    energy = calculate_energy_of_image(image)

    return result

def calculate_energy_of_image(image) :
    
    image = image.astype('float32')
    energy = np.zeros((image.shape[0], image.shape[1]))

    for row in range(image.shape[0]) :
        for column in range(image.shape[1]) :
            
            if row == 0 and column == 0 :
                delta_x = (image[row][column][0] - image[row][column+1][0]) ** 2 + (image[row][column][1] - image[row][column+1][1]) ** 2 + (image[row][column][2] - image[row][column+1][2]) ** 2
                delta_y = (image[row][column][0] - image[row+1][column][0]) ** 2 + (image[row][column][1] - image[row+1][column][1]) ** 2 + (image[row][column][2] - image[row+1][column][2]) ** 2
            elif row == 0 and column == image.shape[1] - 1 :
                delta_x = (image[row][column-1][0] - image[row][column][0]) ** 2 + (image[row][column-1][1] - image[row][column][1]) ** 2 + (image[row][column-1][2] - image[row][column][2]) ** 2
                delta_y = (image[row][column][0] - image[row+1][column][0]) ** 2 + (image[row][column][1] - image[row+1][column][1]) ** 2 + (image[row][column][2] - image[row+1][column][2]) ** 2

            elif row == image.shape[0] - 1 and column == 0 :
                delta_x = (image[row][column][0] - image[row][column+1][0]) ** 2 + (image[row][column][1] - image[row][column+1][1]) ** 2 + (image[row][column][2] - image[row][column+1][2]) ** 2
                delta_y = (image[row-1][column][0] - image[row][column][0]) ** 2 + (image[row-1][column][1] - image[row][column][1]) ** 2 + (image[row-1][column][2] - image[row][column][2]) ** 2
            elif row == image.shape[0] - 1 and column == image.shape[1] - 1 :
                delta_x = (image[row][column-1][0] - image[row][column][0]) ** 2 + (image[row][column-1][1] - image[row][column][1]) ** 2 + (image[row][column-1][2] - image[row][column][2]) ** 2
                delta_y = (image[row-1][column][0] - image[row][column][0]) ** 2 + (image[row-1][column][1] - image[row][column][1]) ** 2 + (image[row-1][column][2] - image[row][column][2]) ** 2

            elif row == image.shape[0] - 1 :
                delta_x = (image[row][column-1][0] - image[row][column+1][0]) ** 2 + (image[row][column-1][1] - image[row][column+1][1]) ** 2 + (image[row][column-1][2] - image[row][column+1][2]) ** 2
                delta_y = (image[row-1][column][0] - image[row][column][0]) ** 2 + (image[row-1][column][1] - image[row][column][1]) ** 2 + (image[row-1][column][2] - image[row][column][2]) ** 2
            elif column == image.shape[1] - 1 :
                delta_x = (image[row][column-1][0] - image[row][column][0]) ** 2 + (image[row][column-1][1] - image[row][column][1]) ** 2 + (image[row][column-1][2] - image[row][column][2]) ** 2
                delta_y = (image[row-1][column][0] - image[row+1][column][0]) ** 2 + (image[row-1][column][1] - image[row+1][column][1]) ** 2 + (image[row-1][column][2] - image[row+1][column][2]) ** 2

            elif row == 0 :
                delta_x = (image[row][column-1][0] - image[row][column+1][0]) ** 2 + (image[row][column-1][1] - image[row][column+1][1]) ** 2 + (image[row][column-1][2] - image[row][column+1][2]) ** 2
                delta_y = (image[row][column][0] - image[row+1][column][0]) ** 2 + (image[row][column][1] - image[row+1][column][1]) ** 2 + (image[row][column][2] - image[row+1][column][2]) ** 2
            elif column == 0 :
                delta_x = (image[row][column][0] - image[row][column+1][0]) ** 2 + (image[row][column][1] - image[row][column+1][1]) ** 2 + (image[row][column][2] - image[row][column+1][2]) ** 2
                delta_y = (image[row-1][column][0] - image[row+1][column][0]) ** 2 + (image[row-1][column][1] - image[row+1][column][1]) ** 2 + (image[row-1][column][2] - image[row+1][column][2]) ** 2
                
            else :
                delta_x = (image[row][column-1][0] - image[row][column+1][0]) ** 2 + (image[row][column-1][1] - image[row][column+1][1]) ** 2 + (image[row][column-1][2] - image[row][column+1][2]) ** 2
                delta_y = (image[row-1][column][0] - image[row+1][column][0]) ** 2 + (image[row-1][column][1] - image[row+1][column][1]) ** 2 + (image[row-1][column][2] - image[row+1][column][2]) ** 2

            energy[row][column] = delta_x + delta_y

    return energy

def lowest_energy_seam(energy) :
    return lowest_energy_seam

image = cv2.imread('./data/school1.jpg')

energy = calculate_energy_of_image(image)

cv2.imwrite('./result.jpg', energy)