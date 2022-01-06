#define _CRT_SECURE_NO_WARNINGS
#define WIDTH 512
#define HEIGHT 512

#include <stdio.h>
#include <malloc.h>
#include <math.h>

// Scailing �Ű������� �������
// �Է� ���� ���, ��� ���� ���, �Է� ���� �ȼ� ��, �Է� ���� �ȼ� ��, ��� ���� �ȼ� ��, ��� ���� �ȼ� �� 
// LPF �� ��(0 <= ���x, ��� �Է��� ���), cut_off frequency(�Ҽ��� �Ʒ��� �� �ڸ� ������), mode(1 : S&H, 2 : Bilinear, 3 : Cubic Conv, 4 : cubic b spline)
int Scailing(char*, char*, int, int, int, int, int, int, int);

// LPF �Լ�, ���� ����, ���� ����, LPF �� ��, cut_off frequency(�Ҽ��� �Ʒ��� �� �ڸ� ������) ��� (1 : ���� ����, 2 : ���� ����)
int LPF(unsigned char*, int, int, int, int, int);

// ���� �̹���, ���� �̹���, ���� ������ (�� �̹����� ���簢���� ���� ��� ����)
double MSE(unsigned char*, unsigned char*, int);
// �̹��� ���� �����, MSE ��� ����Ʈ���� �� ����
int calculate_image_MSE(char *, char *);

// mode(1 : S&H, 2 : Bilinear, 3 : Cubic Conv, 4 : cubic b spline)
int lena512to1000to512(int);
int lena512to400to512NoLPF(int);
int lena512to945(int);
int lena512to298NoLPF(int);

// LPF tap ��, mode(1 : S&H, 2 : Bilinear, 3 : Cubic Conv, 4 : cubic b spline)
int lena512to400to512UsingLPF(int, int);
int lena512to298UsingLPF(int, int);


int main(void) {

	// mode 3�� Cubic Convolution
	int mode = 3;

	lena512to1000to512(mode);
	lena512to400to512UsingLPF(11, mode);
	lena512to400to512NoLPF(mode);
	lena512to945(mode);
	lena512to298UsingLPF(11, mode);
	lena512to298NoLPF(mode);

	return 0;
}

int lena512to1000to512(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo1000.img", WIDTH, HEIGHT, 1000, 1000, 0, 0, mode);
	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo1000.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena1000To512.img", 1000, 1000, WIDTH, HEIGHT, 0, 0, mode);

	printf("lena �̹����� 1000x1000���� Ű�� �� �̸� �ٽ� 512x512�� �ٿ����ϴ�. (lena1000To512.img)\n");

	calculate_image_MSE("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena1000To512.img");

	return 1;
}

int lena512to400to512NoLPF(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400.img", WIDTH, HEIGHT, 400, 400, 0, 0, mode);
	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512.img", 400, 400, WIDTH, HEIGHT, 0, 0, mode);

	printf("lena �̹����� 400x400���� ���� �� �̸� �ٽ� 512x512�� �÷Ƚ��ϴ�. (lena400To512.img)\n");

	calculate_image_MSE("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512.img");

	return 1;
}

int lena512to400to512UsingLPF(int LPF_tap_num, int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400LPF.img", WIDTH, HEIGHT, 400, 400, LPF_tap_num, 40, mode);
	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400LPF.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512LPF.img", 400, 400, WIDTH, HEIGHT, 0, 0, mode);

	printf("lena �̹����� LPF�� ����� �� 400x400���� ���� �� �̸� �ٽ� 512x512�� �÷Ƚ��ϴ�. (lena400To512LPF.img)\n");

	calculate_image_MSE("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512LPF.img");

	return 1;
}

int lena512to945(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo945.img", WIDTH, HEIGHT, 945, 945, 0, 0, mode);

	printf("lena �̹����� 945x945���� �÷Ƚ��ϴ�. (lenaTo945.img)\n");

	return 1;
}

int lena512to298NoLPF(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo298.img", WIDTH, HEIGHT, 298, 298, 0, 0, mode);

	printf("lena �̹����� 298x298���� �ٿ����ϴ�. (lenaTo298.img)\n");

	return 1;
}

int lena512to298UsingLPF(int LPF_tap_num, int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo298LPF.img", WIDTH, HEIGHT, 298, 298, LPF_tap_num, 25, mode);

	printf("lena �̹����� LPF�� ����� �� 298x298���� �ٿ����ϴ�. (lenaTo298LPF.img)\n");

	return 1;
}

int Scailing(char* input_filepath, char* output_filepath, int input_width, int input_height, int output_width, int output_height, int LPF_tap_num, int cut_off, int mode) {

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
			LPF(input_image, input_width, input_height, LPF_tap_num, cut_off, 1);
		}
		// ���� ����
		for (int row = 0; row < input_height; row++) {
			for (int column = 0; column < output_width; column++) {
				*(mid_image + row * output_width + column) = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
			}
		}

		// LPF ����ϴ� ���
		if (LPF_tap_num > 0) {
			LPF(mid_image, output_width, input_height, LPF_tap_num, cut_off, 2);
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
			LPF(input_image, input_width, input_height, LPF_tap_num, cut_off, 1);
		}

		// ���� ����
		for (int row = 0; row < input_height; row++) {
			for (int column = 0; column < output_width; column++) {
				unsigned char value_x, value_x_plus_1;
				double t;
				if ((int)(column * reciprocal_scail_ratio_width) + 1 > input_width - 1) {
					value_x = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					t = column * reciprocal_scail_ratio_width - (int) (column * reciprocal_scail_ratio_width);

					*(mid_image + row * output_width + column) = round((1 - t) * value_x + t * value_x_plus_1);
				}
				else {
					value_x = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					t = column * reciprocal_scail_ratio_width - (int) (column * reciprocal_scail_ratio_width);

					*(mid_image + row * output_width + column) = round((1 - t) * value_x + t * value_x_plus_1);
				}
			}
		}
		// LPF ����ϴ� ���
		if (LPF_tap_num > 0) {
			LPF(mid_image, output_width, input_height, LPF_tap_num, cut_off, 2);
		}

		// ���� ����
		for (int row = 0; row < output_height; row++) {
			for (int column = 0; column < output_width; column++) {
				unsigned char value_x, value_x_plus_1;
				double t;
				if ((int)(row * reciprocal_scail_ratio_height) + 1 > input_height - 1) {
					value_x = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int) (row * reciprocal_scail_ratio_height);

					*(result_image + row * output_width + column) = round((1 - t) * value_x + t * value_x_plus_1);
				}
				else {
					value_x = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int) (row * reciprocal_scail_ratio_height);

					*(result_image + row * output_width + column) = round((1 - t) * value_x + t * value_x_plus_1);
				}		
			}
		}
	}
	// Cubic Convolution
	else if (mode == 3) {
		// alpha ���� -0.5
		float a = -0.5;
		// LPF ����ϴ� ���
		if (LPF_tap_num > 0) {
			LPF(input_image, input_width, input_height, LPF_tap_num, cut_off, 1);
		}

		// ���� ����
		for (int row = 0; row < input_height; row++) {
			for (int column = 0; column < output_width; column++) {
				unsigned char value_x_minus_1, value_x, value_x_plus_1, value_x_plus_2;
				double t;
				// �������� value_x_minus_1�� �Ѿ�� ���
				if ((int)(column * reciprocal_scail_ratio_height) - 1 < 0) {
					value_x_minus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x_plus_2 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 2);
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				} // �������� value_x_plus_2�� �Ѿ�� ���
				else if ((int)(column * reciprocal_scail_ratio_height) + 2 > input_width - 1) {
					value_x_minus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x_plus_2 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				} // �������� value_x_plus_1�� 2 ��� �Ѿ�� ���
				else if ((int)(column * reciprocal_scail_ratio_height) + 1 > input_width - 1) {
					value_x_minus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_2 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				}
				else {
					value_x_minus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x_plus_2 = *(input_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 2);
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				}

				*(mid_image + row * output_width + column) = round(value_x_minus_1 * (a * pow(t, 3) - 2 * a * pow(t, 2) + a * t)
																+ value_x * ((a + 2) * pow(t, 3) - (a + 3) * pow(t, 2) + 1)
																+ value_x_plus_1 * (-(a + 2) * pow(t, 3) + (2*a + 3) * pow(t, 2) - a * t)
																+ value_x_plus_2 * (-a * pow(t,3) + a * pow(t, 2)));
			}
		}
		// LPF ����ϴ� ���
		if (LPF_tap_num > 0) {
			LPF(mid_image, output_width, input_height, LPF_tap_num, cut_off, 2);
		}

		// ���� ����
		for (int row = 0; row < output_height; row++) {
			for (int column = 0; column < output_width; column++) {
				unsigned char value_x_minus_1, value_x, value_x_plus_1, value_x_plus_2;
				double t;
				// �������� value_x_minus_1�� �Ѿ�� ���
				if ((int) (row * reciprocal_scail_ratio_height) - 1 < 0) {
					value_x_minus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x_plus_2 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) + 2) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				} // �������� value_x_plus_2�� �Ѿ�� ���
				else if ((int) (row * reciprocal_scail_ratio_height) + 2 > input_height - 1) {
					value_x_minus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x_plus_2 = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				} // �������� value_x_plus_1�� 2 ��� �Ѿ�� ���
				else if ((int)(row * reciprocal_scail_ratio_height) + 1 > input_height - 1) {
					value_x_minus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_2 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				}
				else {
					value_x_minus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x = *(mid_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x_plus_2 = *(mid_image + ((int)(row * reciprocal_scail_ratio_height) + 2) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				}

				*(result_image + row * output_width + column) = round(value_x_minus_1 * (a * pow(t, 3) - 2 * a * pow(t, 2) + a * t)
																	+ value_x * ((a + 2) * pow(t, 3) - (a + 3) * pow(t, 2) + 1)
																	+ value_x_plus_1 * (-(a + 2) * pow(t, 3) + (2 * a + 3) * pow(t, 2) - a * t)
																	+ value_x_plus_2 * (-a * pow(t, 3) + a * pow(t, 2)));
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

int LPF(unsigned char * input_image, int width, int height, int tap_num, int cut_off, int mode) {
	//�ʿ��� ���� ����
	int location_doing_convolution;
	float* filter = NULL;
	unsigned char* result_image = NULL;

	filter = (float*)malloc(sizeof(float) * tap_num);
	result_image = (unsigned char*)malloc(sizeof(unsigned char) * width * height);

	float* mid_tap = filter + (tap_num - 1) / 2;
	// LPF, sampling frequency 1, cut off frequency 0.25, FIR, 7 tap, Hamming
	if (tap_num == 7) {
		*(filter) = -0.0087;
		*(filter + 1) = 0.0000;
		*(filter + 2) = 0.2518;
		*(filter + 3) = 0.5138;
		*(filter + 4) = 0.2518;
		*(filter + 5) = 0.0000;
		*(filter + 6) = -0.0087;
	}
	// LPF, sampling frequency 1, cut off frequency 0.25, FIR, 9 tap, Hamming
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
	}
	// LPF, sampling frequency 1, cut off frequency 0.4, FIR, 11 tap, Hamming
	else if (tap_num == 11 && cut_off == 40) {
		*(filter) = 0.0000;
		*(filter + 1) = -0.0079;
		*(filter + 2) = 0.0402;
		*(filter + 3) = -0.1033;
		*(filter + 4) = 0.1708;
		*(filter + 5) = 0.8005;
		*(filter + 6) = 0.1708;
		*(filter + 7) = -0.1033;
		*(filter + 8) = 0.0402;
		*(filter + 9) = -0.0079;
		*(filter + 10) = 0.0000;
	}
	else if (tap_num == 11 && cut_off == 25) {
		*(filter) = 0.0051;
		*(filter + 1) = 0.0000;
		*(filter + 2) = -0.0419;
		*(filter + 3) = 0.0000;
		*(filter + 4) = 0.2885;
		*(filter + 5) = 0.4968;
		*(filter + 6) = 0.2885;
		*(filter + 7) = 0.0000;
		*(filter + 8) = -0.0419;
		*(filter + 9) = 0.0000;
		*(filter + 10) = 0.0051;
	}
	
	// mode 1 : ���� LPF, mode 2 : ���� LPF
	if (mode == 1) {
		for (int row = 0; row < height; row++) {
			for (int column = 0; column < width; column++) {
				float subResister = 0.0000;
				for (int tap = -((tap_num - 1) / 2); tap < (tap_num - 1) / 2 + 1; tap++) {
					// mid_tap�� ���, LPF ���� ��� �κ��� 0���� �����ؼ� ���� �� ���������� ����� ������ ���ĺ���
					location_doing_convolution = column + tap;
					// �̷���
					if (location_doing_convolution < 0) {
						subResister += *(input_image + row * width + column - tap) * *(mid_tap + tap);
					}
					else if (location_doing_convolution > width - 1) {
						subResister += *(input_image + row * width + column - tap) * *(mid_tap + tap);
					} // �̷��� ���ص� �Ǵ� �⺻ �κ�
					else {
						subResister += *(input_image + row * width + location_doing_convolution) * *(mid_tap + tap);
					}
				}
				*(result_image + row * width + column) = (unsigned char) round(subResister);
			}
		}
	}
	else if (mode == 2) {
		for (int column = 0; column < width; column++) {
			for (int row = 0; row < height; row++) {
				float subResister = 0.0000;
				for (int tap = -((tap_num - 1) / 2); tap < (tap_num - 1) / 2 + 1; tap++) {
					location_doing_convolution = row + tap;
					// �̷���
					if (location_doing_convolution < 0) {
						subResister += *(input_image + (row - tap) * width + column) * *(mid_tap + tap);
					}
					else if (location_doing_convolution > height - 1) {
						subResister += *(input_image + (row - tap) * width + column) * *(mid_tap + tap);
					} // �̷��� ���ص� �Ǵ� �⺻ �κ�
					else {
						subResister += *(input_image + location_doing_convolution * width + column) * *(mid_tap + tap);
					}
				}
				*(result_image + row * width + column) = (unsigned char) round(subResister);
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