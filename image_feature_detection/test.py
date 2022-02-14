import cv2
import pysift

image = cv2.imread('./data/box.png', 0)
print("입력 이미지의 데이터 타입 : ", type(image))
keypoints, descriptors = pysift.computeKeypointsAndDescriptors(image)
print("결과 keypoints 데이터 타입 : ", type(keypoints))
print("결과 descriptors 데이터 타입 : ", type(descriptors))