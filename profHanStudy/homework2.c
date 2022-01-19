#include "header.h"


int lena512to1000to512(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo1000nopreempasis.img", WIDTH, HEIGHT, 1000, 1000, 0, 0, mode);
	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo1000nopreempasis.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena1000To512nopreempasis.img", 1000, 1000, WIDTH, HEIGHT, 0, 0, mode);

	printf("lena 이미지를 1000x1000으로 키운 후 이를 다시 512x512로 줄였습니다. (lena1000To512.img)\n");

	calculate_image_MSE("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena1000To512nopreempasis.img");

	return 1;
}

int lena512to400to512NoLPF(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400.img", WIDTH, HEIGHT, 400, 400, 0, 0, mode);
	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512.img", 400, 400, WIDTH, HEIGHT, 0, 0, mode);

	printf("lena 이미지를 400x400으로 줄인 후 이를 다시 512x512로 늘렸습니다. (lena400To512.img)\n");

	calculate_image_MSE("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512.img");

	return 1;
}

int lena512to400to512UsingLPF(int LPF_tap_num, int cut_off, int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400LPFnopreempasis.img", WIDTH, HEIGHT, 400, 400, LPF_tap_num, cut_off, mode);
	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo400LPFnopreempasis.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512LPFnopreempasis.img", 400, 400, WIDTH, HEIGHT, 0, 0, mode);

	printf("lena 이미지를 LPF를 사용한 후 400x400으로 줄인 후 이를 다시 512x512로 늘렸습니다. (lena400To512LPF.img)\n");

	calculate_image_MSE("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena400To512LPFnopreempasis.img");

	return 1;
}

int lena512to945(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo945.img", WIDTH, HEIGHT, 945, 945, 0, 0, mode);

	printf("lena 이미지를 945x945으로 늘렸습니다. (lenaTo945.img)\n");

	return 1;
}

int lena512to298NoLPF(int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo298.img", WIDTH, HEIGHT, 298, 298, 0, 0, mode);

	printf("lena 이미지를 298x298으로 줄였습니다. (lenaTo298.img)\n");

	return 1;
}

int lena512to298UsingLPF(int LPF_tap_num, int cut_off, int mode) {

	Scailing("C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lena.img", "C:\\Users\\korea_1\\profHanStudy\\profHanStudy\\lenaTo298LPF.img", WIDTH, HEIGHT, 298, 298, LPF_tap_num, cut_off, mode);

	printf("lena 이미지를 LPF를 사용한 후 298x298으로 줄였습니다. (lenaTo298LPF.img)\n");

	return 1;
}

int Scailing(char* input_filepath, char* output_filepath, int input_width, int input_height, int output_width, int output_height, int LPF_tap_num, int cut_off, int mode) {

	// 필요한 변수 선언
	float reciprocal_scail_ratio_width = (float) input_width / (float) output_width;
	float reciprocal_scail_ratio_height = (float) input_height / (float) output_height;

	FILE* input_file = NULL;
	FILE* output_file = NULL;

	unsigned char* input_image = NULL;
	unsigned char* result_image = NULL;
	
	float* input_float_image = NULL;
	float* mid_float_image = NULL;
	float* result_float_image = NULL;

	input_image = (unsigned char*)malloc(sizeof(unsigned char) * input_width * input_height);
	result_image = (unsigned char*)malloc(sizeof(unsigned char) * output_width * output_height);

	input_float_image = (float*)malloc(sizeof(float) * input_width * input_height);
	mid_float_image = (float*)malloc(sizeof(float) * output_width * input_height);
	result_float_image = (float*)malloc(sizeof(float) * output_width * output_height);

	// raw 파일 읽어오기
	input_file = fopen(input_filepath, "rb");

	// 에러 발생 시
	if (input_file == NULL) {
		printf("load img error");
		return -1;
	}
	// raw 파일 변수에 저장, 파일 닫기
	fread(input_image, sizeof(unsigned char), input_width * input_height, input_file);
	fclose(input_file);

	// 더 큰 자료형을 사용해 중간의 자료형 변환 과정을 줄여 정확도를 높여보자.

	for (int i = 0; i < input_width * input_height; i++) {
		*(input_float_image + i) = *(input_image + i);
	}


	// scailing
	// S&H
	if (mode == 1) {
		// LPF 사용하는 경우
		if (LPF_tap_num > 0) {
			LPF(input_float_image, input_width, input_height, LPF_tap_num, cut_off, 1);
		}
		// 가로 방향
		for (int row = 0; row < input_height; row++) {
			for (int column = 0; column < output_width; column++) {
				*(mid_float_image + row * output_width + column) = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
			}
		}

		// LPF 사용하는 경우
		if (LPF_tap_num > 0) {
			LPF(mid_float_image, output_width, input_height, LPF_tap_num, cut_off, 2);
		}

		// 세로 방향
		for (int row = 0; row < output_height; row++) {
			for (int column = 0; column < output_width; column++) {
				*(result_float_image + row * output_width + column) = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
			}
		}
	}
	// Bilinear
	else if (mode == 2) {
		// LPF 사용하는 경우
		if (LPF_tap_num > 0) {
			LPF(input_float_image, input_width, input_height, LPF_tap_num, cut_off, 1);
		}

		// 가로 방향
		for (int row = 0; row < input_height; row++) {
			for (int column = 0; column < output_width; column++) {
				float value_x, value_x_plus_1;
				float t;
				if ((int)(column * reciprocal_scail_ratio_width) + 1 > input_width - 1) {
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					t = column * reciprocal_scail_ratio_width - (int) (column * reciprocal_scail_ratio_width);

					*(mid_float_image + row * output_width + column) = (1 - t) * value_x + t * value_x_plus_1;
				}
				else {
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					t = column * reciprocal_scail_ratio_width - (int) (column * reciprocal_scail_ratio_width);

					*(mid_float_image + row * output_width + column) = (1 - t) * value_x + t * value_x_plus_1;
				}
			}
		}
		// LPF 사용하는 경우
		if (LPF_tap_num > 0) {
			LPF(mid_float_image, output_width, input_height, LPF_tap_num, cut_off, 2);
		}

		// 세로 방향
		for (int row = 0; row < output_height; row++) {
			for (int column = 0; column < output_width; column++) {
				float value_x, value_x_plus_1;
				float t;
				if ((int)(row * reciprocal_scail_ratio_height) + 1 > input_height - 1) {
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height)) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int) (row * reciprocal_scail_ratio_height);

					*(result_float_image + row * output_width + column) = (1 - t) * value_x + t * value_x_plus_1;
				}
				else {
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int) (row * reciprocal_scail_ratio_height);

					*(result_float_image + row * output_width + column) = (1 - t) * value_x + t * value_x_plus_1;
				}		
			}
		}
	}
	// Cubic Convolution
	else if (mode == 3) {
		// alpha 값은 -0.5
		float a = -0.5;
		// LPF 사용하는 경우
		if (LPF_tap_num > 0) {
			LPF(input_float_image, input_width, input_height, LPF_tap_num, cut_off, 1);
		}

		// 가로 방향
		for (int row = 0; row < input_height; row++) {
			for (int column = 0; column < output_width; column++) {
				float value_x_minus_1, value_x, value_x_plus_1, value_x_plus_2;
				float t;
				// 좌측에서 value_x_minus_1이 넘어가는 경우
				if ((int)(column * reciprocal_scail_ratio_height) - 1 < 0) {
					value_x_minus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x_plus_2 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 2);
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				} // 우측에서 value_x_plus_2만 넘어가는 경우
				else if ((int)(column * reciprocal_scail_ratio_height) + 1 == input_width - 1) {
					value_x_minus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x_plus_2 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				} // 우측에서 value_x_plus_1과 2 모두 넘어가는 경우
				else if ((int)(column * reciprocal_scail_ratio_height) + 1 > input_width - 1) {
					value_x_minus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x_plus_2 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 2);
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				}
				else {
					value_x_minus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x_plus_2 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 2);
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				}

				*(mid_float_image + row * output_width + column) = value_x_minus_1 * (a * pow(t, 3) - 2 * a * pow(t, 2) + a * t)
																+ value_x * ((a + 2) * pow(t, 3) - (a + 3) * pow(t, 2) + 1)
																+ value_x_plus_1 * (-(a + 2) * pow(t, 3) + (2*a + 3) * pow(t, 2) - a * t)
																+ value_x_plus_2 * (-a * pow(t,3) + a * pow(t, 2));
			}
		}
		// LPF 사용하는 경우
		if (LPF_tap_num > 0) {
			LPF(mid_float_image, output_width, input_height, LPF_tap_num, cut_off, 2);
		}

		// 세로 방향
		for (int row = 0; row < output_height; row++) {
			for (int column = 0; column < output_width; column++) {
				float value_x_minus_1, value_x, value_x_plus_1, value_x_plus_2;
				float t;
				// 좌측에서 value_x_minus_1이 넘어가는 경우
				if ((int) (row * reciprocal_scail_ratio_height) - 1 < 0) {
					value_x_minus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x_plus_2 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 2) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				} // 우측에서 value_x_plus_2만 넘어가는 경우
				else if ((int) (row * reciprocal_scail_ratio_height) + 1 == input_height - 1) {
					value_x_minus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x_plus_2 = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				} // 우측에서 value_x_plus_1과 2 모두 넘어가는 경우
				else if ((int)(row * reciprocal_scail_ratio_height) + 1 > input_height - 1) {
					value_x_minus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x_plus_2 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 2) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				}
				else {
					value_x_minus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x_plus_2 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 2) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				}

				*(result_float_image + row * output_width + column) = value_x_minus_1 * (a * pow(t, 3) - 2 * a * pow(t, 2) + a * t)
																	+ value_x * ((a + 2) * pow(t, 3) - (a + 3) * pow(t, 2) + 1)
																	+ value_x_plus_1 * (-(a + 2) * pow(t, 3) + (2 * a + 3) * pow(t, 2) - a * t)
																	+ value_x_plus_2 * (-a * pow(t, 3) + a * pow(t, 2));
			}
		}
	}
	// Cubic B-Spline
	else if (mode == 4) {
		// LPF 사용하는 경우
		if (LPF_tap_num > 0) {
			LPF(input_float_image, input_width, input_height, LPF_tap_num, cut_off, 1);
		}
		int filtertapnum = 15;
		// 전처리 필터
		pre_emphasis_filter(input_float_image, input_width, input_height, filtertapnum, 1);
		// 가로 방향
		for (int row = 0; row < input_height; row++) {
			for (int column = 0; column < output_width; column++) {
				float value_x_minus_1, value_x, value_x_plus_1, value_x_plus_2;
				float t;
				// 좌측에서 value_x_minus_1이 넘어가는 경우
				if ((int)(column * reciprocal_scail_ratio_height) - 1 < 0) {
					value_x_minus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x_plus_2 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 2);
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				} // 우측에서 value_x_plus_2만 넘어가는 경우
				else if ((int) (column * reciprocal_scail_ratio_height + 1 == input_width - 1)) {
					value_x_minus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x_plus_2 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				} // 우측에서 value_x_plus_1과 2 모두 넘어가는 경우
				else if ((int)(column * reciprocal_scail_ratio_height) + 1 > input_width - 1) {
					value_x_minus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x_plus_2 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 2);
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				}
				else {
					value_x_minus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) - 1);
					value_x = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width));
					value_x_plus_1 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 1);
					value_x_plus_2 = *(input_float_image + row * input_width + (int)(column * reciprocal_scail_ratio_width) + 2);
					t = column * reciprocal_scail_ratio_width - (int)(column * reciprocal_scail_ratio_width);
				}

				*(mid_float_image + row * output_width + column) = value_x_minus_1 * (1.0 / 6.0 * pow(1 - t, 3))
					+ value_x * (2.0 / 3.0 + 1.0 / 2.0 * pow(t, 3) - pow(t, 2))
					+ value_x_plus_1 * (2.0 / 3.0 - 1.0 / 2.0 * pow(t - 1, 3) - pow((t - 1), 2))
					+ value_x_plus_2 * (1.0 / 6.0 * pow(t, 3));
			}
		}
		// LPF 사용하는 경우
		if (LPF_tap_num > 0) {
			LPF(mid_float_image, output_width, input_height, LPF_tap_num, cut_off, 2);
		}

		// 전처리 필터
		pre_emphasis_filter(mid_float_image, output_width, input_height, filtertapnum, 2);

		// 세로 방향
		for (int row = 0; row < output_height; row++) {
			for (int column = 0; column < output_width; column++) {
				float value_x_minus_1, value_x, value_x_plus_1, value_x_plus_2;
				float t;
				// 좌측에서 value_x_minus_1이 넘어가는 경우
				if ((int)(row * reciprocal_scail_ratio_height) - 1 < 0) {
					value_x_minus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x_plus_2 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 2) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				} // 우측에서 value_x_plus_2만 넘어가는 경우
				else if ((int)(row * reciprocal_scail_ratio_height) + 1 == input_height - 1) {
					value_x_minus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x_plus_2 = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				} // 우측에서 value_x_plus_1과 2 모두 넘어가는 경우
				else if ((int)(row * reciprocal_scail_ratio_height) + 1 > input_height - 1) {
					value_x_minus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x_plus_2 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 2) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				}
				else {
					value_x_minus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) - 1) * output_width + column);
					value_x = *(mid_float_image + (int)(row * reciprocal_scail_ratio_height) * output_width + column);
					value_x_plus_1 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 1) * output_width + column);
					value_x_plus_2 = *(mid_float_image + ((int)(row * reciprocal_scail_ratio_height) + 2) * output_width + column);
					t = row * reciprocal_scail_ratio_height - (int)(row * reciprocal_scail_ratio_height);
				}

				*(result_float_image + row * output_width + column) = value_x_minus_1 * (1.0 / 6.0 * pow(1 - t, 3))
					+ value_x * (2.0 / 3.0 + 1.0 / 2.0 * pow(t, 3) - pow(t, 2))
					+ value_x_plus_1 * (2.0 / 3.0 - 1.0 / 2.0 * pow(t - 1, 3) - pow((t - 1), 2))
					+ value_x_plus_2 * (1.0 / 6.0 * pow(t, 3));
			}
		}
	}

	for (int i = 0; i < output_width * output_height; i++) {
		*(result_image + i) = round(*(result_float_image + i));
	}

	// 결과 저장
	output_file = fopen(output_filepath, "wb");

	if (output_file == NULL) {
		printf("save result error");
	}

	fwrite(result_image, sizeof(unsigned char), output_width * output_height, output_file);
	fclose(output_file);

	// 메모리 해제
	free(input_image);
	free(result_image);
	free(input_float_image);
	free(mid_float_image);
	free(result_float_image);

	return 1;
}

int LPF(float * input_image, int width, int height, int tap_num, int cut_off, int mode) {
	//필요한 변수 생성
	int location_doing_convolution;
	float* filter = NULL;
	float* result_image = NULL;

	filter = (float*)malloc(sizeof(float) * tap_num);
	result_image = (float*)malloc(sizeof(float) * width * height);

	float* mid_tap = filter + (tap_num - 1) / 2;
	// LPF, sampling frequency 1, cut off frequency 0.25, FIR, 7 tap, Hamming
	if (tap_num == 7 && cut_off == 25) {
		*(filter) = -0.0087;
		*(filter + 1) = 0.0000;
		*(filter + 2) = 0.2518;
		*(filter + 3) = 0.5138;
		*(filter + 4) = 0.2518;
		*(filter + 5) = 0.0000;
		*(filter + 6) = -0.0087;
	}
	// LPF, sampling frequency 1, cut off frequency 0.40, FIR, 7 tap, Hamming
	else if (tap_num == 7 && cut_off == 40) {
		*(filter) = 0.0080;
		*(filter + 1) = -0.0464;
		*(filter + 2) = 0.1426;
		*(filter + 3) = 0.7917;
		*(filter + 4) = 0.1426;
		*(filter + 5) = -0.0464;
		*(filter + 6) = 0.0080;
	}
	// LPF, sampling frequency 1, cut off frequency 0.25, FIR, 9 tap, Hamming
	else if (tap_num == 9 && cut_off == 25) {
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
	else if (tap_num == 9 && cut_off == 40) {
		*(filter) = -0.0038;
		*(filter + 1) = 0.0218;
		*(filter + 2) = -0.0821;
		*(filter + 3) = 0.1625;
		*(filter + 4) = 0.8031;
		*(filter + 5) = 0.1625;
		*(filter + 6) = -0.0821;
		*(filter + 7) = 0.0218;
		*(filter + 8) = -0.0038;
	}
	else if (tap_num == 11 && cut_off == 45) {
		*(filter) = 0.0051;
		*(filter + 1) = -0.0126;
		*(filter + 2) = 0.0340;
		*(filter + 3) = -0.0635;
		*(filter + 4) = 0.0893;
		*(filter + 5) = 0.8956;
		*(filter + 6) = 0.0893;
		*(filter + 7) = -0.0635;
		*(filter + 8) = 0.0340;
		*(filter + 9) = -0.0126;
		*(filter + 10) = 0.0051;
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
	else if (tap_num == 11 && cut_off == 35) {
		*(filter) = -0.0051;
		*(filter + 1) = 0.0079;
		*(filter + 2) = 0.0131;
		*(filter + 3) = -0.1038;
		*(filter + 4) = 0.2361;
		*(filter + 5) = 0.7036;
		*(filter + 6) = 0.2361;
		*(filter + 7) = -0.1038;
		*(filter + 8) = 0.0131;
		*(filter + 9) = 0.0079;
		*(filter + 10) = -0.0051;
	}
	else if (tap_num == 11 && cut_off == 30) {
		*(filter) = 0.0000;
		*(filter + 1) = 0.0127;
		*(filter + 2) = -0.0248;
		*(filter + 3) = -0.0638;
		*(filter + 4) = 0.2760;
		*(filter + 5) = 0.5997;
		*(filter + 6) = 0.2760;
		*(filter + 7) = -0.0638;
		*(filter + 8) = -0.0248;
		*(filter + 9) = 0.0127;
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
	else if (tap_num == 11 && cut_off == 20) {
		*(filter) = 0.0000;
		*(filter + 1) = -0.0126;
		*(filter + 2) = -0.0247;
		*(filter + 3) = 0.0635;
		*(filter + 4) = 0.2748;
		*(filter + 5) = 0.3981;
		*(filter + 6) = 0.2748;
		*(filter + 7) = 0.0635;
		*(filter + 8) = -0.0247;
		*(filter + 9) = -0.0126;
		*(filter + 10) = 0.0000;
	}
	else if (tap_num == 13 && cut_off == 40) {
		*(filter) = 0.0025;
		*(filter + 1) = 0.0000;
		*(filter + 2) = -0.0145;
		*(filter + 3) = 0.0543;
		*(filter + 4) = -0.1162;
		*(filter + 5) = 0.1750;
		*(filter + 6) = 0.7976;
		*(filter + 7) = 0.1750;
		*(filter + 8) = -0.1162;
		*(filter + 9) = 0.0543;
		*(filter + 10) = -0.0145;
		*(filter + 11) = 0.0000;
		*(filter + 12) = 0.0025;
	}
	else if (tap_num == 15 && cut_off == 40) {
		*(filter) = -0.0035;
		*(filter + 1) = 0.0039;
		*(filter + 2) = 0.0000;
		*(filter + 3) = -0.0205;
		*(filter + 4) = 0.0651;
		*(filter + 5) = -0.1256;
		*(filter + 6) = 0.1792;
		*(filter + 7) = 0.8028;
		*(filter + 8) = 0.1792;
		*(filter + 9) = -0.1256;
		*(filter + 10) = 0.0651;
		*(filter + 11) = -0.0205;
		*(filter + 12) = 0.0000;
		*(filter + 13) = 0.0039;
		*(filter + 14) = -0.0035;
	}
	else if (tap_num == 17 && cut_off == 40) {
	*(filter) = 0.0030;
	*(filter + 1) = -0.0050;
	*(filter + 2) = 0.0067;
	*(filter + 3) = 0.0000;
	*(filter + 4) = -0.0252;
	*(filter + 5) = 0.0721;
	*(filter + 6) = -0.1306;
	*(filter + 7) = 0.1801;
	*(filter + 8) = 0.7979;
	*(filter + 9) = 0.1801;
	*(filter + 10) = -0.1306;
	*(filter + 11) = 0.0721;
	*(filter + 12) = -0.0252;
	*(filter + 13) = 0.0000;
	*(filter + 14) = 0.0067;
	*(filter + 15) = -0.0050;
	*(filter + 16) = 0.0030;
	}
	
	// mode 1 : 가로 LPF, mode 2 : 세로 LPF
	if (mode == 1) {
		for (int row = 0; row < height; row++) {
			for (int column = 0; column < width; column++) {
				float subResister = 0.0000;
				for (int tap = -((tap_num - 1) / 2); tap < (tap_num - 1) / 2 + 1; tap++) {
					// mid_tap을 사용, LPF 탭의 가운데 부분을 0으로 설정해서 조금 더 직관적으로 계산해 오류를 고쳐보자
					location_doing_convolution = column + tap;
					// 미러링
					if (location_doing_convolution < 0) {
						subResister += *(input_image + row * width + column - tap) * *(mid_tap + tap);
					}
					else if (location_doing_convolution > width - 1) {
						subResister += *(input_image + row * width + column - tap) * *(mid_tap + tap);
					} // 미러링 안해도 되는 기본 부분
					else {
						subResister += *(input_image + row * width + location_doing_convolution) * *(mid_tap + tap);
					}
				}
				*(result_image + row * width + column) = subResister;
			}
		}
	}
	else if (mode == 2) {
		for (int column = 0; column < width; column++) {
			for (int row = 0; row < height; row++) {
				float subResister = 0.0000;
				for (int tap = -((tap_num - 1) / 2); tap < (tap_num - 1) / 2 + 1; tap++) {
					location_doing_convolution = row + tap;
					// 미러링
					if (location_doing_convolution < 0) {
						subResister += *(input_image + (row - tap) * width + column) * *(mid_tap + tap);
					}
					else if (location_doing_convolution > height - 1) {
						subResister += *(input_image + (row - tap) * width + column) * *(mid_tap + tap);
					} // 미러링 안해도 되는 기본 부분
					else {
						subResister += *(input_image + location_doing_convolution * width + column) * *(mid_tap + tap);
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

int pre_emphasis_filter(float* input_image, int width, int height, int tap_num, int mode) {
	//필요한 변수 생성
	int location_doing_convolution;
	float a = -0.2679;
	float* filter = NULL;
	float* result_image = NULL;


	filter = (float*)malloc(sizeof(float) * tap_num);
	result_image = (float*)malloc(sizeof(float) * width * height);

	float* mid_tap = filter + (tap_num - 1) / 2;

	for (int tap = -(tap_num - 1) / 2; tap < (tap_num - 1) / 2 + 1; tap++) {
		if (tap < 0) {
			*(mid_tap + tap) = -6.0 * a / (1.0 - a * a) * pow(a, -tap);
		}
		else {
			*(mid_tap + tap) = -6.0 * a / (1.0 - a * a) * pow(a, tap);
		}
	}

	// mode 1 : 가로 , mode 2 : 세로
	if (mode == 1) {
		for (int row = 0; row < height; row++) {
			for (int column = 0; column < width; column++) {
				float subResister = 0.0000;
				for (int tap = -((tap_num - 1) / 2); tap < (tap_num - 1) / 2 + 1; tap++) {
					location_doing_convolution = column + tap;
					// 미러링
					if (location_doing_convolution < 0) {
						subResister += *(input_image + row * width + column - tap) * *(mid_tap + tap);
					}
					else if (location_doing_convolution > width - 1) {
						subResister += *(input_image + row * width + column - tap) * *(mid_tap + tap);
					} // 미러링 안해도 되는 기본 부분
					else {
						subResister += *(input_image + row * width + location_doing_convolution) * *(mid_tap + tap);
					}
				}
				*(result_image + row * width + column) = subResister;
			}
		}
	}
	else if (mode == 2) {
		for (int column = 0; column < width; column++) {
			for (int row = 0; row < height; row++) {
				float subResister = 0.0000;
				for (int tap = -((tap_num - 1) / 2); tap < (tap_num - 1) / 2 + 1; tap++) {
					location_doing_convolution = row + tap;
					// 미러링
					if (location_doing_convolution < 0) {
						subResister += *(input_image + (row - tap) * width + column) * *(mid_tap + tap);
					}
					else if (location_doing_convolution > height - 1) {
						subResister += *(input_image + (row - tap) * width + column) * *(mid_tap + tap);
					} // 미러링 안해도 되는 기본 부분
					else {
						subResister += *(input_image + location_doing_convolution * width + column) * *(mid_tap + tap);
					}
				}
				*(result_image + row * width + column) = subResister;
			}
		}
	}
	else {
		printf("pre empasis filter function mode input error\n");
		return 0;
	}

	for (int i = 0; i < width * height; i++) {
		*(input_image + i) = *(result_image + i);
	}

	free(result_image);
	free(filter);

	return 1;
}
