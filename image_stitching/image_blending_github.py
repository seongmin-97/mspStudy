import numpy as np
from skimage.color import rgb2gray
from imageio import imread
from scipy.signal import convolve2d
import scipy
from scipy.ndimage.filters import convolve1d
import matplotlib.pyplot as plt
import os
import cv2
MIN_DIM = 16
GRAY = 1
RGB = 2
MAX_COLOR = 255
TRANSFORM_MATRIX = np.array([[0.299, 0.587, 0.114],
                           [0.596, -0.275, -0.321],
                           [0.212, -0.523, 0.311]])


def read_image(filename, representation):
    """
    reads an image file and converts it into a given representation.
    :param filename: the filename of an image on disk (could be grayscale or RGB)
    :param representation: representation code, either 1 or 2 defining whether the output should be a grayscale
    image (1) or an RGB image (2). If the input image is grayscale, we won’t call it with representation = 2.
    :return: The image. Converted if specified.
    """
    if representation != GRAY and representation != RGB: return
    img = imread(filename)
    img = img.astype(np.float64) / MAX_COLOR
    if representation == GRAY:
        return rgb2gray(img)
    return img


def build_gaussian_pyramid(im, max_levels, filter_size):
    """
    constructs a Gaussian pyramid of a given image.
    :param im:  a grayscale image with double values in [0, 1]
    :param max_levels:  the maximal number of levels1 in the resulting pyramid
    :param filter_size:  the size of the Gaussian filter_vec (an odd scalar that represents a squared filter_vec)
    to be used in constructing the pyramid filter_vec
    :return: pyr - the resulting pyramid as a standard python array, filter_vec - row vector of shape (1,
    filter_size) used for the pyramid construction
    """
    filter_vec = create_filter(filter_size)
    pyr = []
    for i in range(max_levels):
        if im.shape[0] < MIN_DIM or im.shape[1] < MIN_DIM:
            break
        pyr.append(im)
        im = reduce(im, filter_vec)
    return pyr, filter_vec


def reduce(im, filter_vec):
    """
    reduces a given image by 2
    :param im: the image to reduce
    :param filter: the filter_vec to blur the image with
    :return: the image after reduction
    """
    img = convolve1d(convolve1d(im, filter_vec[0], mode='constant').T, filter_vec[0], mode='constant').T
    return img[::2, ::2]


def build_laplacian_pyramid(im, max_levels, filter_size):
    """
    :param im:
    :param max_levels:
    :param filter_size:
    :return:
    """
    gauss_pyr, filter_vec = build_gaussian_pyramid(im.copy(), max_levels, filter_size)
    pyr = []
    for i in range(len(gauss_pyr) - 1):
        laplacian = gauss_pyr[i] - cv2.resize(expand(gauss_pyr[i + 1].copy(), filter_vec * 2.0), dsize=(len(gauss_pyr[i][0]), len(gauss_pyr[i])))
        pyr.append(laplacian)
    pyr.append(gauss_pyr[-1])
    return pyr, filter_vec


def expand(im, filter_vec):
    """
    expands a given image by 2
    :param im: the image to reduce
    :param filter_vec: the filter_vec to blur the image with
    :return: the image after expanding
    """
    length, width = im.shape
    filter_vec = filter_vec.copy()
    expanded_im = np.insert(im, np.arange(1, length + 1), 0, axis=0)
    expanded_im = np.insert(expanded_im, np.arange(1, width + 1), 0, axis=1)
    return convolve1d(convolve1d(expanded_im, filter_vec[0], mode='constant').T, filter_vec[0], mode='constant').T

def create_filter(filter_size):
    """
    creates a filter_vec in a given size with the binomial coefficients
    :param filter_size: the size of the filter_vec (odd size)
    :return: the created filter_vec in size 'filter_size'
    """
    if filter_size == 1:
        return np.asarray([[1]])
    basic_filter = np.array([[1, 2, 1]])
    k = int((filter_size - 1 ) / 2)
    output_filter = basic_filter.copy()
    for i in range(k - 1):
        output_filter = convolve2d(output_filter, basic_filter)
    return output_filter / (2 ** (2 * k))


def laplacian_to_image(lpyr, filter_vec, coeff):
    """
    reconstruct an image from its Laplacian Pyramid.
    :param lpyr: the Laplacian Pyramid
    :param filter_vec: the filter_vec to blur the image with
    :param coeff: list to multiply each level i of the laplacian pyramid by its corresponding coefficient
    coeff[i].
    :return: the reconstructed image
    """
    im = lpyr[-1] * coeff[-1]
    for i in range(len(lpyr) - 2, -1, -1):
        lpyr[i] *= coeff[i]
        im = cv2.resize(expand(im, filter_vec * 2.0), dsize=(len(lpyr[i][0]), len(lpyr[i]))) + lpyr[i]
    return im


def stretch_image(im):
    """
    stretching a given image to values in [0,1]
    :param im: the image to stretch
    :return: the stretched image
    """
    return (im - np.min(im)) / (np.max(im) - np.min(im))


def render_pyramid(pyr, levels):
    """
    creates a single image of all the pyramid images
    :param pyr: a Gaussian or Laplacian pyramid
    :param levels: the number of levels2 to present in the result ≤ max_levels
    :return: a single black image in which the pyramid levels of the given pyramid pyr are stacked
    horizontally
    """
    res = stretch_image(pyr[0])
    for i in range(1, levels):
        pyr[i] = stretch_image(pyr[i])
        zeros_to_add = pyr[0].shape[0] - pyr[i].shape[0]
        im = np.append(pyr[i], np.repeat([[0]] * zeros_to_add, pyr[i].shape[1], axis=1), axis=0)
        res = np.concatenate((res, im), axis=1)
    return res


def display_pyramid(pyr, levels):
    """
    displays 'levels' images from pyr in one image
    :param pyr: a Gaussian or Laplacian pyramid
    :param levels: the number of levels2 to present in the result ≤ max_levels
    """
    to_display = render_pyramid(pyr, levels)
    plt.imshow(to_display, cmap='gray')
    plt.axis("off")
    plt.show()


def scale_images(im1, im2, im3):
    """
    scales 3 given images to the same size (according to the max width/height of the 3 images)
    :param im1 - im3:  the given images to scale
    :return: an array of the 3 scaled images
    """
    new_h = max(im1.shape[0], im2.shape[0], im3.shape[0])
    new_w = max(im1.shape[1], im2.shape[1], im3.shape[1])
    scaled = []
    for im in [im1, im2, im3]:
        if im.shape[0] < new_h:  # padds the image length with zeros
            zeros_to_add = new_h - im.shape[0]
            im = np.append(im, np.repeat([[0]] * zeros_to_add, im.shape[1], axis=1), axis=0)
        if im.shape[1] < new_w:  # padds the image height with zeros
            zeros_to_add = new_w - im.shape[1]
            im = np.append(im, np.repeat([[0]] * zeros_to_add, im.shape[0], axis=0), axis=1)
        scaled.append(im)
    return scaled


def pyramid_blending(im1, im2, mask, max_levels, filter_size_im, filter_size_mask):
    """
    :param im1: 1st grayscale image to be blended
    :param im2: 2nd grayscale image to be blended
    :param mask: a boolean mask containing True and False representing which parts  of im1 and im2 should
    appear in the resulting image.
    :param max_levels: the max_levels parameter to generate the Gaussian and Laplacian pyramids.
    :param filter_size_im: the size of the Gaussian filter_vec for the construction the construction of the
    Laplacian pyramids of im1 and im2.
    :param filter_size_mask: the size of the Gaussian filter_vec for the construction of the Gaussian pyramid
    of mask.
    :return: the blended image
    """
    mask = mask.astype(np.float64)
    im1, im2, mask = scale_images(im1, im2, mask)
    L1, filter_vec = build_laplacian_pyramid(im1, max_levels, filter_size_im)
    L2, _ = build_laplacian_pyramid(im2, max_levels, filter_size_im)
    Gm, _ = build_gaussian_pyramid(mask, max_levels, filter_size_mask)
    L_out = []
    for i in range(len(L1)):
        L_out.append(Gm[i] * L1[i] + (1 - Gm[i]) * L2[i])
    coeff = [1 for _ in range(len(L_out))]
    im_blend = laplacian_to_image(L_out, filter_vec, coeff)
    return np.clip(im_blend, 0, 1)

def blending_example(im1_name, im2_name, mask_name):
    """
    my blending example
    :param im1: 1st grayscale image to be blended (background image)
    :param im2: 2nd grayscale image to be blended
    :param mask: a boolean mask containing True and False             representing which parts  of im1 and im2 should
    appear in the resulting image. True pixels will be taken from im1. 
    :return: the two images to blend and the mask to decide how to blend them
    """
    im1 = read_image(relpath(im1_name), 2)
    im2 = read_image(relpath(im2_name), 2)
    mask = read_image(relpath(mask_name), 1)
    mask[np.where(mask <= 0.5)] = False
    mask[np.where(mask > 0.5)] = True
    im_blend = im1.copy()
    mask = mask.astype(np.bool)
    for i in range(3):
        im_blend[:, :, i] = pyramid_blending(im1[:, :, i], im2[:, :, i], mask, 5,
                                       filter_size_im=5, filter_size_mask=3)
    images = [im1, im2, mask, im_blend]
    for i, im in enumerate(images):
        plt.subplot(2, 2, i + 1)
        plt.imshow(images[i], cmap='gray')
        plt.axis("off")
    plt.show()
    return im1, im2, mask.astype(np.bool), im_blend

def relpath(filename):
    return os.path.join(os.path.dirname(__file__), filename)


def blending_example1():
    """
    1st blending example
    :return: the two images to blend, the mask and the blended image
    """
    return blending_example("coffee.jpg", "water.jpg", "coffee_mask.jpg")



def blending_example2():
    """
    2nd blending example
    :return: the two images to blend, the mask and the blended image
    """
    return blending_example("nyc.jpg", "puddle.jpg", "puddle_mask.jpg")

blending_example('./canvas1.jpg', './canvas2.jpg', './mask1.jpg')