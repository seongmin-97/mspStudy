import math
import numpy as np

def estimate_rotation_matrix_from_homography(h, f) :
    # 모두 같은 카메라로 찍었음을 가정한다.
    r = h.copy()

    r[0][2] = r[0][2] / f
    r[1][2] = r[1][2] / f
    r[2][0] = r[2][0] * f
    r[2][1] = r[2][1] * f

    return r

def estimate_focal_from_homography(h):

    f1 = None
    f0 = None

    d1 = h[2][0] * h[2][1]
    d2 = (h[2][1] - h[2][0]) * (h[2][1] + h[2][0])
    v1 = -(h[0][0] * h[0][1] + h[1][0] * h[1][1]) / d1
    v2 = (h[0][0] * h[0][0] + h[1][0] * h[1][0] - h[0][1] * h[0][1] - h[1][1] * h[1][1]) / d2
    if (v1 < v2):
      temp = v1
      v1 = v2
      v2 = temp
    if (v1 > 0 and v2 > 0):
      f1 = math.sqrt(v1 if abs(d1) > abs(d2) else v2)
    elif (v1 > 0):
      f1 = math.sqrt(v1)
    else:
      return 0

    d1 = h[0][0] * h[1][0] + h[0][1] * h[1][1]
    d2 = h[0][0] * h[0][0] + h[0][1] * h[0][1] - h[1][0] * h[1][0] - h[1][1] * h[1][1]
    v1 = -h[0][2] * h[1][2] / d1
    v2 = (h[1][2] * h[1][2] - h[0][2] * h[0][2]) / d2
    if (v1 < v2):
      temp = v1
      v1 = v2
      v2 = temp
    if (v1 > 0 and v2 > 0):
      f0 = math.sqrt(v1 if abs(d1) > abs(d2) else v2)
    elif (v1 > 0):
      f0 = math.sqrt(v1)
    else:
      return 0

    if (math.isinf(f1) or math.isinf(f0)):
      return 0

    return math.sqrt(f1 * f0)
    