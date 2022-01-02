#define _CRT_SECURE_NO_WARNINGS
#define WIDTH 512
#define HEIGHT 512

#include <stdio.h>
#include <malloc.h>
#include <math.h>

// Scailing �Ű������� �������
// �Է� ���� ���, ��� ���� ���, �Է� ���� �ȼ� ��, �Է� ���� �ȼ� ��, ��� ���� �ȼ� ��, ��� ���� �ȼ� �� 
// LPF �� ��(0 <= ���x, ��� �Է��� ���), mode(1 : S&H, 2 : Bilinear, 3 : B-spline, 4 : Cubic Conv)
int Scailing(char*, char*, int, int, int, int, int, int);

// LPF �Լ�, ���� ����, ���� ����, LPF �� ��, ��� (1 : ���� ����, 2 : ���� ����)
// LPF, sampling frequency 1, cut off frequency 0.25, FIR, 7 tap, Hamming (7��)
int LPF(unsigned char*, int, int, int, int);

// ���� �̹���, ���� �̹���, ���� ������ (�� �̹����� ���簢���� ���� ��� ����)
double MSE(unsigned char*, unsigned char*, int);
// �̹��� ���� �����, MSE ��� ����Ʈ���� �� ����
int calculate_image_MSE(char *, char *);

// mode(1 : S&H, 2 : Bilinear, 3 : B-spline, 4 : Cubic Conv)
int lena512to1000to512(int);
int lena512to400to512NoLPF(int);
int lena512to945(int);
int lena512to298NoLPF(int);

// LPF tap ��, mode(1 : S&H, 2 : Bilinear, 3 : B-spline, 4 : Cubic Conv)
int lena512to400to512UsingLPF(int, int);
int lena512to298UsingLPF(int, int);


int main(void) {

	// mode 2�� Bilinear
	int mode = 2;

	lena512to1000to512(mode);
	lena512to400to512UsingLPF(9, mode);
	lena512to400to512NoLPF(mode);
	lena512to945(mode);
	lena512to298UsingLPF(9, mode);
	lena512to298NoLPF(mode);

	return 0;
}

int lena512to1000to512(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo1000.img", WIDTH, HEIGHT, 1000, 1000, 0, mode);
	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo1000.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena1000To512.img", 1000, 1000, WIDTH, HEIGHT, 0, mode);

	printf("lena �̹����� 1000x1000���� Ű�� �� �̸� �ٽ� 512x512�� �ٿ����ϴ�. (lena1000To512.img)\n");

	calculate_image_MSE("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena1000To512.img");

	return 1;
}

int lena512to400to512NoLPF(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400.img", WIDTH, HEIGHT, 400, 400, 0, mode);
	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512.img", 400, 400, WIDTH, HEIGHT, 0, mode);

	printf("lena �̹����� 400x400���� ���� �� �̸� �ٽ� 512x512�� �÷Ƚ��ϴ�. (lena400To512.img)\n");

	calculate_image_MSE("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512.img");

	return 1;
}

int lena512to400to512UsingLPF(int LPF_tap_num, int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400LPF.img", WIDTH, HEIGHT, 400, 400, LPF_tap_num, mode);
	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400LPF.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512LPF.img", 400, 400, WIDTH, HEIGHT, 0, mode);

	printf("lena �̹����� LPF�� ����� �� 400x400���� ���� �� �̸� �ٽ� 512x512�� �÷Ƚ��ϴ�. (lena400To512LPF.img)\n");

	calculate_image_MSE("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512LPF.img");

	return 1;
}

int lena512to945(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo945.img", WIDTH, HEIGHT, 945, 945, 0, mode);

	printf("lena �̹����� 945x945���� �÷Ƚ��ϴ�. (lenaTo945.img)\n");

	return 1;
}

int lena512to298NoLPF(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo298.img", WIDTH, HEIGHT, 298, 298, 0, mode);

	printf("lena �̹����� 298x298���� �ٿ����ϴ�. (lenaTo298.img)\n");

	return 1;
}

int lena512to298UsingLPF(int LPF_tap_num, int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo298LPF.img", WIDTH, HEIGHT, 298, 298, LPF_tap_num, mode);

	printf("lena �̹����� LPF�� ����� �� 298x298���� �ٿ����ϴ�. (lenaTo298LPF.img)\n");

	return 1;
}

int Scailing(char* input_filepath, char* output_filepath, int input_width, int input_height, int output_width, int output_height, int LPF_tap_num, int mode) {

	// �ʿ��� ���� ����
	float reciprocal_scail_ratio_width = (float) input_width / (float) output_width;
	float reciprocal_scail_ratio_height = (float) input_height / (float) output_height;

	FILE* input_file = NULL;
	FILE* output_file = NULL;

	unsigned char* input_image = NULL;
	unsigned char* mid_image = NULL;
	unsigned char* result_image = NULL;

	input_image = (unsigned char*)malloc(sizeof(unsigned char) * input_width * input_height);
	mid_image = (unsigned char*)malloc(sizeof(unsigned char) * output_width * input_height);
	result_image = (unsigned char*)malloc(sizeof(unsigned char) * output_width * output_height);

	// ���� �߻� ��
	if (input_image == NULL || mid_image == NULL || result_image == NULL) {
		printf("malloc error");
		return -1;
	}

	// raw ���� �о����
	input_file = fopen(input_filepath, "rb");

	// ���� �߻� ��
	if (input_file == NULL) {
		printf("load img error");
		return -1;
	}
	// raw ���� ������ ����, ���� �ݱ�
	fread(input_image, sizeof(unsigned char), input_width * input_height, input_file);
	fclose(input_file);


	// scailing
	// S&H
	if (mode == 1) {
		// LPF ����ϴ� ���
		if (LPF_tap_num > 0) {
			LPF(input_image, input_width, input_height, LPF_tap_num, 1);
		}
		// ���� ����
		for (int row = 0; row < input_height; row++) {
			for (int column = 0; column < output_width; column++) {
				*(mid_image + row * output_width + column) = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
			}
		}

		// LPF ����ϴ� ���
		if (LPF_tap_num > 0) {
			LPF(mid_image, output_width, input_height, LPF_tap_num, 2);
		}

		// ���� ����
		for (int row = 0; row < output_height; row++) {
			for (int column = 0; column < output_width; column++) {
				*(result_image + row * output_width + column) = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
			}
		}
	}
	// Bilinear
	else if (mode == 2) {
		// LPF ����ϴ� ���
		if (LPF_tap_num > 0) {
			LPF(input_image, input_width, input_height, LPF_tap_num, 1);
		}
		// ���� ����
		for (int row = 0; row < input_height; row++) {
			for (int column = 0; column < output_width; column++) {
				int value_x = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
				int value_x_plus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
				double t = column * reciprocal_scail_ratio_width - floor(column * reciprocal_scail_ratio_width);

				*(mid_image + row * output_width + column) = (1 - t) * value_x + t * value_x_plus_1;
				
			}
		}
		// LPF ����ϴ� ���
		if (LPF_tap_num > 0) {
			LPF(mid_image, output_width, input_height, LPF_tap_num, 2);
		}

		// ���� ����
		for (int row = 0; row < output_height; row++) {
			for (int column = 0; column < output_width; column++) {
				int value_x = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
				int value_x_plus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
				double t = row * reciprocal_scail_ratio_height - floor(row * reciprocal_scail_ratio_height);

				*(result_image + row * output_width + column) = (1 - t) * value_x + t * value_x_plus_1;			
			}
		}
	}


	// ��� ����
	output_file = fopen(output_filepath, "wb");

	if (output_file == NULL) {
		printf("save result error");
	}

	fwrite(result_image, sizeof(unsigned char), output_width * output_height, output_file);
	fclose(output_file);

	// �޸� ����
	free(input_image);
	free(mid_image);
	free(result_image);

	return 1;
}

int LPF(unsigned char * input_image, int width, int height, int tap_num, int mode) {

	//�ʿ��� ���� ����
	int location_doing_convolution;
	float* filter = NULL;
	unsigned char* result_image = NULL;

	filter = (float*)malloc(sizeof(float) * tap_num);
	result_image = (unsigned char*)malloc(sizeof(unsigned char) * width * height);

	// LPF, sampling frequency 1, cut off frequency 0.25, FIR, 9 tap, Hamming
	if (tap_num == 7) {
		*(filter) = -0.0087;
		*(filter + 1) = 0.0000;
		*(filter + 2) = 0.2518;
		*(filter + 3) = 0.5138;
		*(filter + 4) = 0.2518;
		*(filter + 5) = 0.0000;
		*(filter + 6) = -0.0087;
	}
	else if (tap_num == 9) {
		*(filter) = 0.0000;
		*(filter + 1) = -0.0227;
		*(filter + 2) = 0.0000;
		*(filter + 3) = 0.2740;
		*(filter + 4) = 0.4974;
		*(filter + 5) = 0.2740;
		*(filter + 6) = 0.0000;
		*(filter + 7) = -0.0227;
		*(filter + 8) = 0.0000;
		return 0;
	}

	// mode 1 : ���� LPF, mode 2 : ���� LPF
	if (mode == 1) {
		for (int row = 0; row < height; row++) {
			for (int column = 0; column < width; column++) {
				unsigned char subResister = 0;
				for (int tap = 0; tap < tap_num; tap++) {
					location_doing_convolution = column - (tap_num - 1) / 2 + tap;
					// �̷���
					if (location_doing_convolution < 0) {
						subResister += *(input_image + row * width - location_doing_convolution) * *(filter + tap);
					}
					else if (location_doing_convolution >= width) {
						subResister += *(input_image + row * width + location_doing_convolution - 2 * (location_doing_convolution - (width - 1))) * *(filter + tap);
					}
					else {
						subResister += *(input_image + row * width + location_doing_convolution) * *(filter + tap);
					}
				}
				*(result_image + row * width + column) = subResister;
			}
		}
	}
	else if (mode == 2) {
		for (int column = 0; column < width; column++) {
			for (int row = 0; row < height; row++) {
				unsigned char subResister = 0;
				for (int tap = 0; tap < tap_num; tap++) {
					location_doing_convolution = row - (tap_num - 1) / 2 + tap;
					// �̷���
					if (location_doing_convolution < 0) {
						subResister += *(input_image - location_doing_convolution * width + column) * *(filter + tap);
					}
					else if (location_doing_convolution >= height) {
						subResister += *(input_image + (location_doing_convolution - 2 * (location_doing_convolution - (height - 1))) * width + column) * *(filter + tap);
					}
					else {
						subResister += *(input_image + location_doing_convolution * width + column) * *(filter + tap);
					}
				}
				*(result_image + row * width + column) = subResister;
			}
		}
	}
	else {
		printf("LPF function mode input error\n");
		return 0;
	}

	for (int i = 0; i < width * height; i++) {
		*(input_image + i) = *(result_image + i);
	}

	free(result_image);
	free(filter);

	return 1;
}

double MSE(unsigned char * x, unsigned char * y, int size) {

	double error = 0;

	for (int i = 0; i < size * size; i++) {
		error = error + (*(x + i) - *(y + i)) * (*(x + i) - *(y + i));
	}
	return error / (double) (size * size);
}

int calculate_image_MSE(char* input, char* output) {

	double mse;

	FILE* input_file = NULL;
	FILE* output_file = NULL;

	unsigned char* input_image = NULL;
	unsigned char* output_image = NULL;

	input_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	output_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	input_file = fopen(input, "rb");
	output_file = fopen(output, "rb");

	fread(input_image, sizeof(unsigned char), WIDTH * HEIGHT, input_file);
	fclose(input_file);
	fread(output_image, sizeof(unsigned char), WIDTH * HEIGHT, output_file);
	fclose(output_file);

	mse = MSE(input_image, output_image, WIDTH);

	free(input_image);
	free(output_image);

	printf("MSE : %.6f\n", mse);

	return 1;
}