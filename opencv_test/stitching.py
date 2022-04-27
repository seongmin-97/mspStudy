 #!/usr/bin/env python
 
'''
 Stitching sample
 ================
 
 Show how to use Stitcher API from python in a simple way to stitch panoramas
 or scans.
 '''
 
 # Python 2/3 compatibility
from __future__ import print_function

import os

import numpy as np
import cv2 as cv

import argparse
import sys

modes = (cv.Stitcher_PANORAMA, cv.Stitcher_SCANS)

parser = argparse.ArgumentParser(prog='stitching.py', description='Stitching sample.')
parser.add_argument('dir', help = 'input images directory')

__doc__ += '\n' + parser.format_help()

def load_images_from_dir(dir):
  imgs = []
  for filename in sorted(os.listdir(dir)):
    img = cv.imread(os.path.join(dir, filename))
    if img is not None:
      imgs.append(img)
  return imgs

def main():
    args = parser.parse_args()

    # read input images
    imgs = load_images_from_dir(args.dir)

    stitcher = cv.Stitcher.create(cv.Stitcher_PANORAMA)
    status, pano = stitcher.stitch(imgs)

    if status != cv.Stitcher_OK:
        print("Can't stitch images, error code = %d" % status)
        sys.exit(-1)

    cv.imwrite('result.jpg', pano)
    print("stitching completed successfully.")

    print('Done')


if __name__ == '__main__':
    print(__doc__)
    main()
    cv.destroyAllWindows()