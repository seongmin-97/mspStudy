#include "header.h"


void jpeg_encoder(char* input_file_name, char* dpcm_info, int* zigzag_image) {
	unsigned char* lena_image = NULL;
	char* level_shift_image = NULL;
	double* dct_image = NULL;
	int* quantization_image = NULL;

	lena_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	level_shift_image = (char*)malloc(sizeof(char) * WIDTH * HEIGHT);
	dct_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	quantization_image = (int*)malloc(sizeof(int) * WIDTH * HEIGHT);

	// 이미지 읽기
	read_image(lena_image, input_file_name);
	
	// level_shift
	printf("level shift \n");
	level_shifting_128_encoder(lena_image, level_shift_image);
	print_block_char(level_shift_image, 0, 0);
	print_block_char(level_shift_image, 0, 1);

	free(lena_image);

	// dct
	printf("dct \n");
	DCT_char(8, level_shift_image, dct_image);
	print_block_double(dct_image, 0, 0);
	print_block_double(dct_image, 0, 1);

	free(level_shift_image);

	// 양자화
	printf("quantization \n");
	quantization_jpeg(dct_image, quantization_image);
	print_block_int(quantization_image, 0, 0);
	print_block_int(quantization_image, 0, 1);
	free(dct_image);

	// DPCM
	DPCM(quantization_image, dpcm_info);
	printf("DC계수 \n");
	print_block_int(quantization_image, 32, 32);
	printf("\n");
	zigzag_scan(quantization_image, zigzag_image);
	free(quantization_image);
	
	int skip_block = 32 * 64 + 32;
	for (int i = skip_block * 64; i < skip_block * 64 + 64; i++) {
		printf("%d ",*(zigzag_image + i));
	}

	printf("\n\n");

	for (int i = 0; i < 64; i++) {
		printf("%d ", *(zigzag_image + i));
	}
	printf("\n\n");

	for (int i = 64; i < 128; i++) {
		printf("%d ", *(zigzag_image + i));
	}
	printf("\n\n");

}

void jpeg_decoder(char* output_file_name, char* dpcm_info, int* zigzag_image) {
	int* decode_dpcm = NULL;
	int* decode_quantization = NULL;
	char* idct_image = NULL;
	unsigned char* recon_image = NULL;

	decode_dpcm = (int*)malloc(sizeof(int) * WIDTH * HEIGHT / 64);
	decode_quantization = (int*)malloc(sizeof(int) * WIDTH * HEIGHT);
	idct_image = (char*)malloc(sizeof(char) * WIDTH * HEIGHT);
	recon_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	printf("\n\nDC계수 복원 \n");
	decode_DPCM(dpcm_info, decode_dpcm);

	printf("\n\nDC계수와 zigzag scan을 이용해 양자화 행렬 복원 \n");
	decode_zigzag_scan(decode_dpcm, zigzag_image, decode_quantization);
	print_block_int(decode_quantization, 0, 0);
	print_block_int(decode_quantization, 0, 1);
	print_block_int(decode_quantization, 32, 32);

	printf("\n\n양자화 계수 곱하고 IDCT 한 번에 수행 \n\n");
	quantization_IDCT_jpeg(decode_quantization, idct_image);
	print_block_char(idct_image, 0, 0);
	print_block_char(idct_image, 0, 1);

	printf("\nlevel shift 128 \n\n");
	level_shifting_128_decoder(idct_image, recon_image);
	print_block_unsigned_char(recon_image, 0, 0);
	print_block_unsigned_char(recon_image, 0, 1);

	printf("\n save recon file");
	save_unsigned_char_to_image(recon_image, output_file_name);
	
	free(decode_dpcm);
	free(decode_quantization);
	free(idct_image);
	free(recon_image);
}

void level_shifting_128_encoder(unsigned char* input, char* output) {
	// 저장
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(output + i) = *(input + i) - 128;
	}
}

// 양자화 관련 함수는 전에 있던걸 그대로 자료형반 바꿔서 사용하자.
void DCT_char(int dimension, char* input_image, double* output_image) {
	double* mid_image = NULL;
	double* basis_vector = NULL;
	// 내적 계산 결과를 임시 저장할 포인터
	double* dot_product = NULL;

	mid_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);

	basis_vector = (double*)malloc(sizeof(double) * dimension * dimension);
	dot_product = (double*)malloc(sizeof(double) * dimension);


	// basis vector 계산, 벡터가 v개, 각 벡터는 v개 원소 (v는 DIMENTION 상수)
	for (int v = 0; v < dimension; v++) {
		double c;

		if (v == 0) {
			c = 1 / sqrt(2);
		}
		else {
			c = 1.0;
		}
		for (int i = 0; i < dimension; i++) {
			*(basis_vector + v * dimension + i) = 1.0 / 2.0 * c * cos((2 * i + 1) * v * M_PI / (2.0 * dimension));
		}
	}

	// 가로 DCT
	for (int row = 0; row < HEIGHT; row++) {
		for (int column = 0; column < WIDTH; column) {
			// 내적 계산 v로 각 벡터들 접근, i로 각 벡터들의 원소들 접근
			for (int v = 0; v < dimension; v++) {
				// 일단 각 원소를 하나하나 곱하고
				for (int i = 0; i < dimension; i++) {
					*(dot_product + i) = *(input_image + row * WIDTH + column + i) * *(basis_vector + v * dimension + i);
				}
				// 곱한 내역이 저장되어 있는 dot_product의 원소를 모두 더해서 저장!
				double dot_product_result = 0;
				for (int i = 0; i < dimension; i++) {
					dot_product_result += *(dot_product + i);
				}
				*(mid_image + row * WIDTH + column + v) = dot_product_result;
			}
			// 8개씩 내적 계산
			column = column + dimension;
		}
	}

	// 세로 DCT
	for (int column = 0; column < WIDTH; column++) {
		for (int row = 0; row < HEIGHT; row) {
			// 내적 계산 v로 각 벡터들 접근, i로 각 벡터들의 원소들 접근
			for (int v = 0; v < dimension; v++) {
				// 일단 각 원소를 하나하나 곱하고
				for (int i = 0; i < dimension; i++) {
					*(dot_product + i) = *(mid_image + (row + i) * WIDTH + column) * *(basis_vector + v * dimension + i);
				}
				// 곱한 내역이 저장되어 있는 dot_product의 원소를 모두 더해서 저장!
				double dot_product_result = 0;
				for (int i = 0; i < dimension; i++) {
					dot_product_result += *(dot_product + i);
				}
				*(output_image + (row + v) * WIDTH + column) = dot_product_result;
			}
			// 8개씩 내적 계산
			row = row + dimension;
		}
	}

	free(mid_image);
	free(basis_vector);
	free(dot_product);

	return 0;
}

void quantization_jpeg(double* dct_image, int* output_image) {

	int* coef = NULL;
	coef = (int*)malloc(sizeof(int) * 8 * 8);

	// 어떻게 넣을지 몰라 일단 일일이... 넣었습니다.
	*(coef) = 16;
	*(coef + 1) = 11;
	*(coef + 2) = 10;
	*(coef + 3) = 16;
	*(coef + 4) = 24;
	*(coef + 5) = 40;
	*(coef + 6) = 51;
	*(coef + 7) = 61;

	*(coef + 8) = 12;
	*(coef + 9) = 12;
	*(coef + 10) = 14;
	*(coef + 11) = 19;
	*(coef + 12) = 26;
	*(coef + 13) = 58;
	*(coef + 14) = 60;
	*(coef + 15) = 55;

	*(coef + 16) = 14;
	*(coef + 17) = 13;
	*(coef + 18) = 16;
	*(coef + 19) = 24;
	*(coef + 20) = 40;
	*(coef + 21) = 57;
	*(coef + 22) = 69;
	*(coef + 23) = 56;

	*(coef + 24) = 14;
	*(coef + 25) = 17;
	*(coef + 26) = 22;
	*(coef + 27) = 29;
	*(coef + 28) = 51;
	*(coef + 29) = 87;
	*(coef + 30) = 80;
	*(coef + 31) = 62;

	*(coef + 32) = 18;
	*(coef + 33) = 22;
	*(coef + 34) = 37;
	*(coef + 35) = 56;
	*(coef + 36) = 68;
	*(coef + 37) = 109;
	*(coef + 38) = 103;
	*(coef + 39) = 77;

	*(coef + 40) = 24;
	*(coef + 41) = 35;
	*(coef + 42) = 55;
	*(coef + 43) = 64;
	*(coef + 44) = 81;
	*(coef + 45) = 104;
	*(coef + 46) = 113;
	*(coef + 47) = 92;

	*(coef + 48) = 49;
	*(coef + 49) = 64;
	*(coef + 50) = 78;
	*(coef + 51) = 87;
	*(coef + 52) = 103;
	*(coef + 53) = 121;
	*(coef + 54) = 120;
	*(coef + 55) = 101;

	*(coef + 56) = 72;
	*(coef + 57) = 92;
	*(coef + 58) = 95;
	*(coef + 59) = 98;
	*(coef + 60) = 112;
	*(coef + 61) = 100;
	*(coef + 62) = 103;
	*(coef + 63) = 99;

	// 양자화
	// row column은 dct 이미지의 행과 열, i, j는 블록 내의 행과 열
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// 나누기
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					*(output_image + (row + i) * WIDTH + column + j) = (int)(*(dct_image + (row + i) * WIDTH + column + j) / *(coef + i * 8 + j));
				}
			}
			column = column + 8;
		}
		row = row + 8;
	}

	free(coef);
}

void DPCM(int* input_image, char* output) {
	
	int* dc_coefficient = NULL;
	dc_coefficient = (int*)malloc(sizeof(int) * WIDTH * HEIGHT / 64);

	// 일단 DC 계수를 읽어오자.
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// output_dc는 WIDTH x HEIGHT / 64개의 값을 저장 가능 
			// 이것을 64x64 행렬이라 생각해보자.
			// 그런데, row, column은 각각 512, 512까지 접근
			// 어짜피 8의 배수에만 접근하고, output_dc의 행/열과 8배만큼 차이나므로 row, column에 / 8을 해서 접근하자.
			*(dc_coefficient + row / 8 * 64 + column / 8) = *(input_image + row * WIDTH + column);

			column += 8;
		}
		row += 8;
	}
	
	// 프린트문
	printf("DC 계수 \n\n");
	for (int i = 0; i < 11; i++) {
		printf("dc 계수 : %d \n", *(dc_coefficient + i));
	}

	// 차분값을 구해서 저장하자.
	// 반복문 안에 계속 i=0을 비교하는 비교 구문을 넣으면 너무 비효율적이지 않을까?
	*(output) = *(dc_coefficient);
	printf("\n\n");
	printf("DC 차분 계수 \n\n");
	printf("dc 차분 계수 : %d - 0 = %d\n", *(output), *(output));
	// 나머지는 반복문
	for (int i = 1; i < WIDTH * HEIGHT / 64; i++) {
		*(output + i) = *(dc_coefficient + i) - *(dc_coefficient + i - 1);
		
		if (i < 11) {
			printf("dc 차분 계수 : %d - % d =  %d\n", *(dc_coefficient + i), *(dc_coefficient + i - 1), *(output + i));
		}
	}

	free(dc_coefficient);
}

void zigzag_scan(int* input, int* output) {

	// zigzag scan이 한 블록 될 때마다 +1 (포인터 주소 조절용
	int finish_block = 0;

	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {

			// index는 지그재그 스캔을 저장할 output에 참고
			int index = 0;
			// block_row, block_column은 input값의 지그재그 스캔을 도움
			int block_row = 0;
			int block_column = 0;

			// 일단 맨 첫번째 값은 먼저 저장
			*(output + finish_block * 64) = *(input + row * WIDTH + column);

			while (index < 64) {

				if (index > 34) {
					// 아래로 한 번 이동
					index += 1;
					block_row += 1;
				}
				else {
					// 오른쪽으로 한 번 이동
					index += 1;
					block_column += 1;
				}

				*(output + finish_block * 64 + index) = *(input + (row + block_row) * WIDTH + column + block_column);

				// 왼쪽 아래 대각선 이동
				while (block_column > 0 && block_row < 7) {
					block_column -= 1;
					block_row += 1;
					index += 1;
					*(output + finish_block * 64 + index) = *(input + (row + block_row) * WIDTH + column + block_column);
				}

				if (index > 34) {
					// 오른쪽으로 한 번 이동
					index += 1;
					block_column += 1;
				}
				else {
					// 아래로 한 번 이동
					index += 1;
					block_row += 1;
				}
				*(output + finish_block * 64 + index) = *(input + (row + block_row) * WIDTH + column + block_column);

				// 8x8에서 마지막 zigzag는 오른쪽 위 대각선 이동 직전이다.
				if (index == 63) {
					break;
				}
				// 오른쪽 위 대각선 이동
				while (block_row > 0 && block_column < 7) {
					block_column += 1;
					block_row -= 1;
					index += 1;
					*(output + finish_block * 64 + index) = *(input + (row + block_row) * WIDTH + column + block_column);
				}
			}
			finish_block += 1;
			column += 8;
		}
		row += 8;
	}
	
}

int run_length_code(int* input, char* output) {
	int idx = 0;

	for (int block = 0; block < 512 * 512 / 64; block++) {
		unsigned char count_zero = 0;
		for (int data = 0; data < 64; data++) {
			if (data == 0) {
				continue;
			}
			else if (*(input + block * 64 + data) == 0) {
				if (count_zero == 0) {
					break;
				}
				count_zero += 1;
			}
			else  {
				*(output + idx) = count_zero;
				*(output + idx + 1) = *(input + block * 64 + data);
				count_zero = 0;
				idx += 2;
			}
		}
	}
}



void decode_DPCM(char* input, int* output) {

	*(output) = *(input);
	printf("\n\n");
	printf("DC값 복원 \n\n");
	printf("dc 복원 계수 : %d\n", *(output));

	for (int i = 1; i < WIDTH * HEIGHT / 64; i++) {
		*(output + i) = *(input + i) + *(output + i - 1);

		if (i < 11) {
			printf("dc 복원 계수 : %d\n", *(output + i));
		}
	}

}

void decode_zigzag_scan(int* dc_coefficient, int* zigzag_scan, int* output) {
	

	// zigzag scan이 한 블록 될 때마다 +1 (포인터 주소 조절용
	int finish_block = 0;

	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {

			// index는 각 블록을 데이터에 저장할 output에 참고
			int index = 0;
			// block_row, block_column은 input값의 지그재그 스캔을 도움
			int block_row = 0;
			int block_column = 0;

			// 일단 dc 계수 저장
			*(output + (row + block_row) * WIDTH + column + block_column) = *(dc_coefficient + finish_block);

			while (index < 64) {

				if (index > 34) {
					// 아래로 한 번 이동
					index += 1;
					block_row += 1;
				}
				else {
					// 오른쪽으로 한 번 이동
					index += 1;
					block_column += 1;
				}

				*(output + (row + block_row) * WIDTH + column + block_column) = *(zigzag_scan + finish_block * 64 + index);

				// 왼쪽 아래 대각선 이동
				while (block_column > 0 && block_row < 7) {
					block_column -= 1;
					block_row += 1;
					index += 1;

					*(output + (row + block_row) * WIDTH + column + block_column) = *(zigzag_scan + finish_block * 64 + index);
				}

				if (index > 34) {
					// 오른쪽으로 한 번 이동
					index += 1;
					block_column += 1;
				}
				else {
					// 아래로 한 번 이동
					index += 1;
					block_row += 1;
				}

				*(output + (row + block_row) * WIDTH + column + block_column) = *(zigzag_scan + finish_block * 64 + index);

				// 8x8에서 마지막 zigzag는 오른쪽 위 대각선 이동 직전이다.
				if (index == 63) {
					break;
				}
				// 오른쪽 위 대각선 이동
				while (block_row > 0 && block_column < 7) {
					block_column += 1;
					block_row -= 1;
					index += 1;
					*(output + (row + block_row) * WIDTH + column + block_column) = *(zigzag_scan + finish_block * 64 + index);
				}
			}
			finish_block += 1;
			column += 8;
		}
		row += 8;
	}
}

void quantization_IDCT_jpeg(int* input_image, char* output_image_char) {

	double* mid_image = NULL;
	double* output_image = NULL;

	double* basis_vector = NULL;
	// 내적 계산 결과를 임시 저장할 포인터
	double* dot_product = NULL;

	int* coef = NULL;
	coef = (int*)malloc(sizeof(int) * 8 * 8);

	mid_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	output_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);


	basis_vector = (double*)malloc(sizeof(double) * 8 * 8 * 8 * 8);
	dot_product = (double*)malloc(sizeof(double) * 8 * 8);


	// 어떻게 넣을지 몰라 일단 일일이... 넣었습니다.
	*(coef) = 16;
	*(coef + 1) = 11;
	*(coef + 2) = 10;
	*(coef + 3) = 16;
	*(coef + 4) = 24;
	*(coef + 5) = 40;
	*(coef + 6) = 51;
	*(coef + 7) = 61;

	*(coef + 8) = 12;
	*(coef + 9) = 12;
	*(coef + 10) = 14;
	*(coef + 11) = 19;
	*(coef + 12) = 26;
	*(coef + 13) = 58;
	*(coef + 14) = 60;
	*(coef + 15) = 55;

	*(coef + 16) = 14;
	*(coef + 17) = 13;
	*(coef + 18) = 16;
	*(coef + 19) = 24;
	*(coef + 20) = 40;
	*(coef + 21) = 57;
	*(coef + 22) = 69;
	*(coef + 23) = 56;

	*(coef + 24) = 14;
	*(coef + 25) = 17;
	*(coef + 26) = 22;
	*(coef + 27) = 29;
	*(coef + 28) = 51;
	*(coef + 29) = 87;
	*(coef + 30) = 80;
	*(coef + 31) = 62;

	*(coef + 32) = 18;
	*(coef + 33) = 22;
	*(coef + 34) = 37;
	*(coef + 35) = 56;
	*(coef + 36) = 68;
	*(coef + 37) = 109;
	*(coef + 38) = 103;
	*(coef + 39) = 77;

	*(coef + 40) = 24;
	*(coef + 41) = 35;
	*(coef + 42) = 55;
	*(coef + 43) = 64;
	*(coef + 44) = 81;
	*(coef + 45) = 104;
	*(coef + 46) = 113;
	*(coef + 47) = 92;

	*(coef + 48) = 49;
	*(coef + 49) = 64;
	*(coef + 50) = 78;
	*(coef + 51) = 87;
	*(coef + 52) = 103;
	*(coef + 53) = 121;
	*(coef + 54) = 120;
	*(coef + 55) = 101;

	*(coef + 56) = 72;
	*(coef + 57) = 92;
	*(coef + 58) = 95;
	*(coef + 59) = 98;
	*(coef + 60) = 112;
	*(coef + 61) = 100;
	*(coef + 62) = 103;
	*(coef + 63) = 99;


	int dimension = 8;
	// basis vector 계산, 벡터가 v개, 각 벡터는 v개 원소 (v는 DIMENTION 상수)
	for (int i = 0; i < dimension; i++) {
		for (int j = 0; j < dimension; j++) {
			for (int u = 0; u < dimension; u++) {
				for (int v = 0; v < dimension; v++) {
					double c_v;
					double c_u;

					if (v == 0) {
						c_v = 1 / sqrt(2);
					}
					else {
						c_v = 1.0;
					}

					if (u == 0) {
						c_u = 1 / sqrt(2);
					}
					else {
						c_u = 1.0;
					}// basis를 4차원이라 했을 때, 가장 먼저 접근할 수 있는 i는 이미지 데이터의 가로축, j는 이미지 데이터의 세로 축, u는 주파수의 가로축, v는 주파수의 세로축, i는 이미지 데이터의 가로축, j는 이미지 데이터의 세로 축
					*(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v) = c_v * c_u / (dimension / 2.0) * cos((2 * i + 1) * u * M_PI / 16.0) * cos((2 * j + 1) * v * M_PI / 16.0);
				}
			}
		}
	}

	// block IDCT
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// 내적 2차원 내적? 이므로 차근차근 가보자 먼저 블록 데이터에 i, j로 접근 (가로, 세로축)
			for (int i = 0; i < dimension; i++) {
				for (int j = 0; j < dimension; j++) {
					// 하나하나 곱해서 더하자. 데이터에 주파수 데이터에 u, v로 접근 (주파수 가로, 주파수 세로축)
					// dot_product에 계산 결과물 임시 저장
					for (int u = 0; u < dimension; u++) {
						for (int v = 0; v < dimension; v++) {
							*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
							*(dot_product + u * dimension + v) = *(dot_product + u * dimension + v) * *(coef + u * dimension + v);
						}
					}
					// 쌍쌍이 곱한 값, (dot_product)를 모두 더해 내적 계산을 완료한다.
					// 이 값을 블록 기준, i, j에 저장한다.
					double dot_product_result = 0;
					for (int i = 0; i < dimension * dimension; i++) {
						dot_product_result += *(dot_product + i);
					}
					*(output_image + (row + i) * WIDTH + column + j) = dot_product_result;
				}
			}
			column = column + dimension;
		}
		row = row + dimension;
	}

	// 복사 & clipping
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		if (*(output_image + i) < -128) {
			*(output_image_char + i) = -128;
		}
		else if (*(output_image + i) > 127) {
			*(output_image_char + i) = 127;
		}
		else {
			*(output_image_char + i) = round(*(output_image + i));
		}
	}

	free(mid_image);
	free(output_image);
	free(basis_vector);
	free(dot_product);
	free(coef);
	return 0;
}

void level_shifting_128_decoder(char* input, unsigned  char* output) {
	// 저장
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(output + i) = *(input + i) + 128;
	}
}
