#include "header.h"

int main(void) {
	
	
	// ���� �̹����� �׳� DCT ���� ���� �񱳱����� �Ա� ���� ��� �ҷ��� �ڵ�
	unsigned char* original_image = NULL;
	double* dct_image = NULL;

	original_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	dct_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	
	read_image(original_image, "./lena.img");
	// ���� �̹��� �� ���
	printf("���� �̹��� \n");
	print_block_unsigned_char(original_image, 0, 0);
	print_block_unsigned_char(original_image, 0, 1);
	// DCT �� �� ���
	printf("���� �̹��� DCT \n");
	DCT(8, original_image, dct_image);
	print_block_double(dct_image, 0, 0);
	print_block_double(dct_image, 0, 1);


	printf("-------------------------------------------\n\n\n");

	char* dpcm_info = NULL;
	int* zigzag_scan = NULL;

	dpcm_info = (char*)malloc(sizeof(char) * WIDTH * HEIGHT / 64);
	zigzag_scan = (int*)malloc(sizeof(int) * WIDTH * HEIGHT);

	printf("JPEG Encoder \n");
	jpeg_encoder("./lena.img", dpcm_info, zigzag_scan);

	printf("-------------------------------------------\n\n\n");

	printf("JPEG Decoder \n");
	jpeg_decoder("./lena_jpeg.img", dpcm_info, zigzag_scan);
	
	printf("-------------------------------------------\n\n\n");

	unsigned char* jpeg_image = NULL;
	jpeg_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	read_image(jpeg_image, "./lena_jpeg.img");

	double PSNR = psnr(original_image, jpeg_image);
	
	calculate_image_MSE("./lena.img", "./lena_jpeg.img");
	printf("PSNR �� : %.3f", PSNR);
	
	free(jpeg_image);
	free(original_image);
	free(dct_image);
}