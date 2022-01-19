#include "header.h"

void block_DCT_cpu_time(void) {
	clock_t start1, start2, start3, start4, start5, start6, end1, end2, end3, end4, end5, end6;

	start1 = clock();
	block_DCT(2, "lena_DCT2x2.img", "lena_DCT2x2_view.img");
	end1 = clock();
	printf("2x2 ��� ���� �ð� : %.3f\n", (float)(end1 - start1)/CLOCKS_PER_SEC);
	start2 = clock();
	block_DCT(4, "lena_DCT4x4.img", "lena_DCT4x4_view.img");
	end2 = clock();
	printf("4x4 ��� ���� �ð� : %.3f\n", (float)(end2 - start2) / CLOCKS_PER_SEC);
	start3 = clock();
	block_DCT(8, "lena_DCT8x8.img", "lena_DCT8x8_view.img");
	end3 = clock();
	printf("8x8 ��� ���� �ð� : %.3f\n", (float)(end3 - start3) / CLOCKS_PER_SEC);
	start4 = clock();
	block_DCT(16, "lena_DCT16x16.img", "lena_DCT16x16_view.img");
	end4 = clock();
	printf("16x16 ��� ���� �ð� : %.3f\n", (float)(end4 - start4) / CLOCKS_PER_SEC);
	start5 = clock();
	block_DCT(32, "lena_DCT32x32.img", "lena_DCT32x32_view.img");
	end5 = clock();
	printf("32x32 ��� ���� �ð� : %.3f\n", (float)(end5 - start5) / CLOCKS_PER_SEC);
	start6 = clock();
	block_DCT(64, "lena_DCT64x64.img", "lena_DCT64x64_view.img");
	end6 = clock();
	printf("64x64 ��� ���� �ð� : %.3f\n", (float)(end6 - start6) / CLOCKS_PER_SEC);
}

int DCT(int dimension, unsigned char* input_image, double* output_image) {
	double* mid_image = NULL;
	double* basis_vector = NULL;
	// ���� ��� ����� �ӽ� ������ ������
	double* dot_product = NULL;

	mid_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);

	basis_vector = (double*)malloc(sizeof(double) * dimension * dimension);
	dot_product = (double*)malloc(sizeof(double) * dimension);


	// basis vector ���, ���Ͱ� v��, �� ���ʹ� v�� ���� (v�� DIMENTION ���)
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

	// ���� DCT
	for (int row = 0; row < HEIGHT; row++) {
		for (int column = 0; column < WIDTH; column) {
			// ���� ��� v�� �� ���͵� ����, i�� �� ���͵��� ���ҵ� ����
			for (int v = 0; v < dimension; v++) {
				// �ϴ� �� ���Ҹ� �ϳ��ϳ� ���ϰ�
				for (int i = 0; i < dimension; i++) {
					*(dot_product + i) = *(input_image + row * WIDTH + column + i) * *(basis_vector + v * dimension + i);
				}
				// ���� ������ ����Ǿ� �ִ� dot_product�� ���Ҹ� ��� ���ؼ� ����!
				double dot_product_result = 0;
				for (int i = 0; i < dimension; i++) {
					dot_product_result += *(dot_product + i);
				}
				*(mid_image + row * WIDTH + column + v) = dot_product_result;
			}
			// 8���� ���� ���
			column = column + dimension;
		}
	}

	// ���� DCT
	for (int column = 0; column < WIDTH; column++) {
		for (int row = 0; row < HEIGHT; row) {
			// ���� ��� v�� �� ���͵� ����, i�� �� ���͵��� ���ҵ� ����
			for (int v = 0; v < dimension; v++) {
				// �ϴ� �� ���Ҹ� �ϳ��ϳ� ���ϰ�
				for (int i = 0; i < dimension; i++) {
					*(dot_product + i) = *(mid_image + (row + i) * WIDTH + column) * *(basis_vector + v * dimension + i);
				}
				// ���� ������ ����Ǿ� �ִ� dot_product�� ���Ҹ� ��� ���ؼ� ����!
				double dot_product_result = 0;
				for (int i = 0; i < dimension; i++) {
					dot_product_result += *(dot_product + i);
				}
				*(output_image + (row + v) * WIDTH + column) = dot_product_result;
			}
			// 8���� ���� ���
			row = row + dimension;
		}
	}
	
	// ��� print ���߿� �Լ��� ������
	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {

			printf("output[%d][%d] = %f \n", row, column, *(output_image + row * WIDTH + column));
		}
	}

	free(mid_image);
	free(basis_vector);
	free(dot_product);

	return 0;
}

int block_DCT(int dimension, char* output_file_name, char* view_file_name) {

	unsigned char* input_image = NULL;
	double* output_image = NULL;
	unsigned char* output_image_unsigned_char = NULL;
	double* basis_vector = NULL;
	// ���� ��� ����� �ӽ� ������ ������
	double* dot_product = NULL;

	input_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	output_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	output_image_unsigned_char = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	// block�� ��� 4���� ������
	// ���θ� ���� ���� 2�����̹Ƿ�
	basis_vector = (double*)malloc(sizeof(double) * dimension * dimension * dimension * dimension);
	dot_product = (double*)malloc(sizeof(double) * dimension * dimension);

	read_image(input_image, "./lena.img");

	// basis vector ���, ���Ͱ� v��, �� ���ʹ� v�� ���� (v�� DIMENTION ���)
	for (int u = 0; u < dimension; u++) {
		for (int v = 0; v < dimension; v++) {
			// basis�� 4�����̶� ���� ��, ���� ���� ������ �� �ִ� u�� ���ļ��� ������, v�� ���ļ��� ������, i�� �̹��� �������� ������, j�� �̹��� �������� ���� ��
			for (int i = 0; i < dimension; i++) {
				for (int j = 0; j < dimension; j++) {
					*(basis_vector + u * dimension * dimension * dimension + v * dimension * dimension + i * dimension + j) =  cos((2 * i + 1) * u * M_PI / (2.0 * dimension)) * cos((2 * j + 1) * v * M_PI / (2.0 * dimension));
				}
			}
		}
	}
	
	// block DCT
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// ���� 2���� ����? �̹Ƿ� �������� ������ ���� ���ļ� �����Ϳ� u, v�� ����(u�� ���ļ� ������, v�� ���ļ� ������)
			for (int u = 0; u < dimension; u++) {
				for (int v = 0; v < dimension; v++) {
					// �ϳ��ϳ� ���ؼ� ������. �����Ϳ� �̹��� ��� �����Ϳ� i, j�� ���� (������, ������)
					// dot_product�� ��� ����� �ӽ� ����
					for (int i = 0; i < dimension; i++) {
						for (int j = 0; j < dimension; j++) {
							*(dot_product + i * dimension + j) = *(input_image + (row + i) * WIDTH + column + j) * *(basis_vector + u * dimension * dimension * dimension + v * dimension * dimension + i * dimension + j);
						}
					}
					// �ֽ��� ���� �� (dot_product)�� ��� ���� ���� ����� �Ϸ�
					// ����� u,v�� �� ���� �����Ѵ�.
					double dot_product_result = 0;
					for (int i = 0; i < dimension * dimension; i++) {
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
						}
						dot_product_result += c_v * c_u / (dimension / 2.0) * *(dot_product + i);
					}
					*(output_image + (row + u) * WIDTH + column + v) = dot_product_result;
				}
			}
			column = column + dimension;
		}
		row = row + dimension;
	}

	
	// ��� ����
	save_double_to_image(output_image, output_file_name);

	// ���� & clipping
	double max = *(output_image);
	double min = *(output_image);
	
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(output_image + i) = *(output_image + i) * *(output_image + i) * 30;
		if (max < *(output_image + i)) {
			max = *(output_image + i);
		}
		if (min > *(output_image + i)) {
			min = *(output_image + i);
		}
	}

	// printf("%f %f \n", min, max);
	// min�� 0.00000���� Ȯ�� -> �ִ밪�� ������� �����ָ� �ȴ�.
	// �̸� 0 - 255�� ������.


	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(output_image_unsigned_char + i) = round(log(*(output_image + i) + 1) * 255.0 / log(max+1));
	}
	
	save_unsigned_char_to_image(output_image_unsigned_char, view_file_name);

	free(input_image);
	free(output_image);
	free(output_image_unsigned_char);
	free(basis_vector);
	free(dot_product);

	return 0;
}

int IDCT(int dimension, double* input_image, unsigned char* output_image_unsigned_char) {
	double* mid_image = NULL;
	double* output_image = NULL;

	double* basis_vector = NULL;
	// ���� ��� ����� �ӽ� ������ ������
	double* dot_product = NULL;

	mid_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	output_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);

	basis_vector = (double*)malloc(sizeof(double) * dimension * dimension * dimension * dimension);
	dot_product = (double*)malloc(sizeof(double) * dimension * dimension);


	// basis vector ���, ���Ͱ� v��, �� ���ʹ� v�� ���� (v�� DIMENTION ���)
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
					}// basis�� 4�����̶� ���� ��, ���� ���� ������ �� �ִ� i�� �̹��� �������� ������, j�� �̹��� �������� ���� ��, u�� ���ļ��� ������, v�� ���ļ��� ������, i�� �̹��� �������� ������, j�� �̹��� �������� ���� ��
					*(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v) = c_v * c_u / (dimension / 2.0) * cos((2 * i + 1) * u * M_PI / 16.0) * cos((2 * j + 1) * v * M_PI / 16.0);
				}
			}
		}
	}

	// block IDCT
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// ���� 2���� ����? �̹Ƿ� �������� ������ ���� ��� �����Ϳ� i, j�� ���� (����, ������)
			for (int i = 0; i < dimension; i++) {
				for (int j = 0; j < dimension; j++) {
					// �ϳ��ϳ� ���ؼ� ������. �����Ϳ� ���ļ� �����Ϳ� u, v�� ���� (���ļ� ����, ���ļ� ������)
					// dot_product�� ��� ����� �ӽ� ����
					for (int u = 0; u < dimension; u++) {
						for (int v = 0; v < dimension; v++) {
							*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
						}
					}
					// �ֽ��� ���� ��, (dot_product)�� ��� ���� ���� ����� �Ϸ��Ѵ�.
					// �� ���� ��� ����, i, j�� �����Ѵ�.
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

	// view ���� ����
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(output_image_unsigned_char + i) = round(*(output_image + i));
	}

	free(mid_image);
	free(output_image);
	free(basis_vector);
	free(dot_product);

	return 0;
}

int vertical_horizontal_DCT(void) {

	int dimension = 8;

	FILE* vertical_file = NULL;
	FILE* horizontal_file = NULL;
	FILE* vertical_result_file = NULL;
	FILE* horizontal_result_file = NULL;
	FILE* vertical_view_file = NULL;
	FILE* horizontal_view_file = NULL;

	unsigned char* vertical_image = NULL;
	unsigned char* horizontal_image = NULL;
	double* vertical_result_image = NULL;
	double* horizontal_result_image = NULL;
	unsigned char* vertical_result_image_unsigned_char = NULL;
	unsigned char* horizontal_result_image_unsigned_char = NULL;
	double* basis_vector = NULL;
	double* dot_product = NULL;

	vertical_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	horizontal_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	vertical_result_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	horizontal_result_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	vertical_result_image_unsigned_char = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	horizontal_result_image_unsigned_char = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	basis_vector = (double*)malloc(sizeof(double) * dimension * dimension * dimension * dimension);
	dot_product = (double*)malloc(sizeof(double) * dimension * dimension);

	// vertical image ����
	// 4pixel�� �ݺ�
	for (int row = 0; row < HEIGHT; row++) {
		for (int column = 0; column < WIDTH; column) {
			*(vertical_image + row * WIDTH + column) = 255;
			*(vertical_image + row * WIDTH + column + 1) = 255;
			*(vertical_image + row * WIDTH + column + 2) = 255;
			*(vertical_image + row * WIDTH + column + 3) = 255;
			*(vertical_image + row * WIDTH + column + 4) = 0;
			*(vertical_image + row * WIDTH + column + 5) = 0;
			*(vertical_image + row * WIDTH + column + 6) = 0;
			*(vertical_image + row * WIDTH + column + 7) = 0;
			column = column + 8;
		}
	}

	// horizontal image ����
	// 4pixel�� �ݺ�
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column++) {
			*(horizontal_image + row * WIDTH + column) = 255;
			*(horizontal_image + (row + 1) * WIDTH + column) = 255;
			*(horizontal_image + (row + 2) * WIDTH + column) = 255;
			*(horizontal_image + (row + 3) * WIDTH + column) = 255;
			*(horizontal_image + (row + 4) * WIDTH + column) = 0;
			*(horizontal_image + (row + 5) * WIDTH + column) = 0;
			*(horizontal_image + (row + 6) * WIDTH + column) = 0;
			*(horizontal_image + (row + 7) * WIDTH + column) = 0;
		}
		row = row + 8;
	}
	
	vertical_file = fopen("vertical_image.img", "wb");

	if (vertical_file == NULL) {
		printf("save result error");
	}

	fwrite(vertical_image, sizeof(unsigned char), WIDTH * HEIGHT, vertical_file);
	fclose(vertical_file);

	horizontal_file = fopen("horizontal_image.img", "wb");
	
	if (horizontal_file == NULL) {
		printf("save result error");
	}

	fwrite(horizontal_image, sizeof(unsigned char), WIDTH * HEIGHT, horizontal_file);
	fclose(horizontal_file);

	// basis vector ���, ���Ͱ� v��, �� ���ʹ� v�� ���� (v�� DIMENTION ���)
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
			}
			for (int i = 0; i < dimension; i++) {
				for (int j = 0; j < dimension; j++) {// basis�� 4�����̶� ���� ��, ���� ���� ������ �� �ִ� u�� ���ļ��� ������, v�� ���ļ��� ������, i�� �̹��� �������� ������, j�� �̹��� �������� ���� ��
					*(basis_vector + u * dimension * dimension * dimension + v * dimension * dimension + i * dimension + j) = c_v * c_u / 4.0 * cos((2 * i + 1) * u * M_PI / (2.0 * dimension)) * cos((2 * j + 1) * v * M_PI / (2.0 * dimension));
				}
			}
		}
	}

	// vertical DCT
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// ���� 2���� ����? �̹Ƿ� �������� ������ ���� ���ļ��� u, v�� ���� (���ļ� ����, ���ļ� ������)
			for (int u = 0; u < dimension; u++) {
				for (int v = 0; v < dimension; v++) {
					// �ϳ��ϳ� ���ؼ� ������. �����Ϳ� �̹��� ��� �����Ϳ� i, j�� ���� 
					// dot_product�� ��� ����� �ӽ� ����
					for (int i = 0; i < dimension; i++) {
						for (int j = 0; j < dimension; j++) {
							*(dot_product + i * dimension + j) = *(vertical_image + (row + i) * WIDTH + column + j) * *(basis_vector + u * dimension * dimension * dimension + v * dimension * dimension + i * dimension + j);
						}
					}
					// �ӽ� ������ ����� (�ֽ��� ���� ����) ��� ���ؼ� u, v�� �´� ��ġ�� ��� ���� 
					double dot_product_result = 0;
					for (int i = 0; i < dimension * dimension; i++) {
						dot_product_result += *(dot_product + i);
					}
					*(vertical_result_image + (row + u) * WIDTH + column + v) = dot_product_result;
				}
			}
			column = column + dimension;
		}
		row = row + dimension;
	}
	
	// horizontal DCT
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// ���� 2���� ����? �̹Ƿ� �������� ������ ���� ���ļ��� u, v�� ���� (���ļ� ����, ���ļ� ������)
			for (int u = 0; u < dimension; u++) {
				for (int v = 0; v < dimension; v++) {
					// �ϳ��ϳ� ���ؼ� ������. �����Ϳ� �̹��� ��� �����Ϳ� i, j�� ���� 
					// dot_product�� ��� ����� �ӽ� ����
					for (int i = 0; i < dimension; i++) {
						for (int j = 0; j < dimension; j++) {
							*(dot_product + i * dimension + j) = *(horizontal_image + (row + i) * WIDTH + column + j) * *(basis_vector + u * dimension * dimension * dimension + v * dimension * dimension + i * dimension + j);
						}
					}
					double dot_product_result = 0;
					for (int i = 0; i < dimension * dimension; i++) {
						dot_product_result += *(dot_product + i);
					}
					// �ӽ� ������ ����� (�ֽ��� ���� ����) ��� ���ؼ� u, v�� �´� ��ġ�� ��� ���� 
					*(horizontal_result_image + (row + u) * WIDTH + column + v) = dot_product_result;
				}
			}
			column = column + dimension;
		}
		row = row + dimension;
	}

	// vertical ��� ����
	vertical_result_file = fopen("./vertical_DCT.img", "wb");

	if (vertical_result_file == NULL) {
		printf("save result error");
	}

	fwrite(vertical_result_image, sizeof(double), WIDTH * HEIGHT, vertical_result_file);
	fclose(vertical_result_file);

	// vertical view ���� ����
	double max = *(vertical_result_image);
	double min = *(vertical_result_image);

	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(vertical_result_image + i) = *(vertical_result_image + i) * *(vertical_result_image + i) * 30;
		if (max < *(vertical_result_image + i)) {
			max = *(vertical_result_image + i);
		}
		if (min > *(vertical_result_image + i)) {
			min = *(vertical_result_image + i);
		}
	}

	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(vertical_result_image_unsigned_char + i) = round(log(*(vertical_result_image + i) + 1) * 255.0 / log(max + 1));
	}

	vertical_view_file = fopen("./vertical_DCT_view.img", "wb");

	if (vertical_view_file == NULL) {
		printf("save result error");
	}

	fwrite(vertical_result_image_unsigned_char, sizeof(unsigned char), WIDTH* HEIGHT, vertical_view_file);
	fclose(vertical_view_file);

	// horizontal ��� ����
	horizontal_result_file = fopen("./horizontal_DCT.img", "wb");

	if (horizontal_result_file == NULL) {
		printf("save result error");
	}

	fwrite(horizontal_result_image, sizeof(double), WIDTH * HEIGHT, horizontal_result_file);
	fclose(horizontal_result_file);

	// horizontal view ���� ����
	max = *(horizontal_result_image);
	min = *(horizontal_result_image);

	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(horizontal_result_image + i) = *(horizontal_result_image + i) * *(horizontal_result_image + i) * 30;
		if (max < *(horizontal_result_image + i)) {
			max = *(horizontal_result_image + i);
		}
		if (min > *(horizontal_result_image + i)) {
			min = *(horizontal_result_image + i);
		}
	}

	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(horizontal_result_image_unsigned_char + i) = round(log(*(horizontal_result_image + i) + 1) * 255.0 / log(max + 1));
	}

	horizontal_view_file = fopen("./horizontal_DCT_view.img", "wb");

	if (horizontal_view_file == NULL) {
		printf("save result error");
	}

	fwrite(horizontal_result_image_unsigned_char, sizeof(unsigned char), WIDTH * HEIGHT, horizontal_view_file);
	fclose(horizontal_view_file);

	free(vertical_image);
	free(horizontal_image);
	free(vertical_result_image);
	free(horizontal_result_image);
	free(vertical_result_image_unsigned_char);
	free(horizontal_result_image_unsigned_char);
	free(basis_vector);
	free(dot_product);
}

int part_of_IDCT(int dimension, int part, char* input_file_name, char* output_file_name, char* view_file_name) {
	FILE* input_file = NULL;
	FILE* output_file = NULL;
	FILE* output_file_unsigned_char = NULL;

	double* input_image = NULL;
	double* mid_image = NULL;
	double* output_image = NULL;
	unsigned char* output_image_unsigned_char = NULL;

	double* basis_vector = NULL;
	// ���� ��� ����� �ӽ� ������ ������
	double* dot_product = NULL;

	input_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	mid_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	output_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	output_image_unsigned_char = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	basis_vector = (double*)malloc(sizeof(double) * dimension * dimension * dimension * dimension);
	dot_product = (double*)malloc(sizeof(double) * dimension * dimension);

	input_file = fopen(input_file_name, "rb");

	if (input_file == NULL) {
		printf("load img error");
		return -1;
	}

	fread(input_image, sizeof(double), HEIGHT * WIDTH, input_file);
	fclose(input_file);

	// basis vector ���, ���Ͱ� v��, �� ���ʹ� v�� ���� (v�� DIMENTION ���)
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
					}// basis�� 4�����̶� ���� ��, ���� ���� ������ �� �ִ� i�� �̹��� �������� ������, j�� �̹��� �������� ���� ��, u�� ���ļ��� ������, v�� ���ļ��� ������, i�� �̹��� �������� ������, j�� �̹��� �������� ���� ��
					*(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v) = c_v * c_u / (dimension / 2.0) * cos((2 * i + 1) * u * M_PI /(2.0 * dimension)) * cos((2 * j + 1) * v * M_PI / (2.0 * dimension));
				}
			}
		}
	}

	// block IDCT
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// ���� 2���� ����? �̹Ƿ� �������� ������ ���� ��� �����Ϳ� i, j�� ���� (����, ������)
			for (int i = 0; i < dimension; i++) {
				for (int j = 0; j < dimension; j++) {
					// �ϳ��ϳ� ���ؼ� ������. �����Ϳ� ���ļ� �����Ϳ� u, v�� ���� (���ļ� ����, ���ļ� ������)
					// dot_product�� ��� ����� �ӽ� ����
					for (int u = 0; u < dimension; u++) {
						for (int v = 0; v < dimension; v++) {
							// ���⼭ input_image�� DCT�� �̹����� ���´�. (IDCT�̹Ƿ� �翬)
							// �� �̹����� �Ϻθ� ����ؼ� IDCT �غ���.
							if (part == 3) {
								// �� ���� 3���� ���
								if (u + v <= 1) {
									*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								}
								else {
									*(dot_product + u * dimension + v) = 0;
								}
							}
							else if (part == 1) {
								if (u + v <= 0) {
									*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								}
								else {
									*(dot_product + u * dimension + v) = 0;
								}
							}
							else if (part == 6) {
								// ���� ��� 6�� ��� (�ﰢ�� ���)
								if (u + v <= 2) {
									*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								}
								else {
									*(dot_product + u * dimension + v) = 0;
								}
							}
							else if (part == 10) {
								// ���� ��� 10�� ��� (�ﰢ�� ���)
								if (u + v <= 3) {
									*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								}
								else {
									*(dot_product + u * dimension + v) = 0;
								}
							}
							else if (part == 21) {
								// ���� ��� 10�� ��� (�ﰢ�� ���)
								if (u + v <= 5) {
									*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								}
								else {
									*(dot_product + u * dimension + v) = 0;
								}
							}
							else if (part == 36) {
								// ���� ��� �ﰢ�� ���� ��� (36��)
								if (u + v <= 7) {
									*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								}
								else {
									*(dot_product + u * dimension + v) = 0;
								}
							}
						}
					}
					// �ֽ��� ���� ��, (dot_product)�� ��� ���� ���� ����� �Ϸ��Ѵ�.
					// �� ���� ��� ����, i, j�� �����Ѵ�.
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

	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		*(output_image_unsigned_char + i) = round(*(output_image + i));
	}

	// PSNR ���

	FILE* original_file = NULL;
	unsigned char* original_image = NULL;

	original_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	original_file = fopen("./lena.img", "rb");

	fread(original_image, sizeof(unsigned char), WIDTH* HEIGHT, original_file);
	fclose(original_file);

	double PSNR = psnr(original_image, output_image_unsigned_char);

	printf("%d�� ��� �̿� DCT PSNR : %f\n", part, PSNR);


	// ��� ����
	output_file = fopen(output_file_name, "wb");

	if (output_file == NULL) {
		printf("save result error");
	}
	fwrite(output_image, sizeof(double), WIDTH * HEIGHT, output_file);
	fclose(output_file);

	// view ���� ����
	output_file_unsigned_char = fopen(view_file_name, "wb");


	if (output_file_unsigned_char == NULL) {
		printf("save result error");
	}
	fwrite(output_image_unsigned_char, sizeof(unsigned char), WIDTH * HEIGHT, output_file_unsigned_char);
	fclose(output_file_unsigned_char);

	free(input_image);
	free(mid_image);
	free(output_image);
	free(output_image_unsigned_char);
	free(basis_vector);
	free(dot_product);

	return 0;
}

void quantization(int db, double* dct_image, double* output_image) {

	int* coef35 = NULL;
	int* coef58 = NULL;
	int* coef28 = NULL;
	int* coef30 = NULL;

	coef35 = (int*)malloc(sizeof(int) * 8 * 8);
	coef58 = (int*)malloc(sizeof(int) * 8 * 8);
	coef28 = (int*)malloc(sizeof(int) * 8 * 8);
	coef30 = (int*)malloc(sizeof(int) * 8 * 8);

	// ��� ������ ���� �ϴ� ������... �־����ϴ�.
	*(coef35) = 16;
	*(coef35 + 1) = 11;
	*(coef35 + 2) = 10;
	*(coef35 + 3) = 16;
	*(coef35 + 4) = 24;
	*(coef35 + 5) = 40;
	*(coef35 + 6) = 51;
	*(coef35 + 7) = 61;

	*(coef35 + 8) = 12;
	*(coef35 + 9) = 12;
	*(coef35 + 10) = 14;
	*(coef35 + 11) = 19;
	*(coef35 + 12) = 26;
	*(coef35 + 13) = 58;
	*(coef35 + 14) = 60;
	*(coef35 + 15) = 55;

	*(coef35 + 16) = 14;
	*(coef35 + 17) = 13;
	*(coef35 + 18) = 16;
	*(coef35 + 19) = 19;
	*(coef35 + 20) = 26;
	*(coef35 + 21) = 58;
	*(coef35 + 22) = 60;
	*(coef35 + 23) = 55;

	*(coef35 + 24) = 18;
	*(coef35 + 25) = 22;
	*(coef35 + 26) = 37;
	*(coef35 + 27) = 56;
	*(coef35 + 28) = 68;
	*(coef35 + 29) = 109;
	*(coef35 + 30) = 103;
	*(coef35 + 31) = 77;

	*(coef35 + 32) = 24;
	*(coef35 + 33) = 35;
	*(coef35 + 34) = 55;
	*(coef35 + 35) = 64;
	*(coef35 + 36) = 81;
	*(coef35 + 37) = 104;
	*(coef35 + 38) = 113;
	*(coef35 + 39) = 92;

	*(coef35 + 40) = 49;
	*(coef35 + 41) = 64;
	*(coef35 + 42) = 78;
	*(coef35 + 43) = 87;
	*(coef35 + 44) = 103;
	*(coef35 + 45) = 121;
	*(coef35 + 46) = 120;
	*(coef35 + 47) = 101;

	*(coef35 + 48) = 49;
	*(coef35 + 49) = 64;
	*(coef35 + 50) = 78;
	*(coef35 + 51) = 87;
	*(coef35 + 52) = 103;
	*(coef35 + 53) = 121;
	*(coef35 + 54) = 120;
	*(coef35 + 55) = 101;

	*(coef35 + 56) = 72;
	*(coef35 + 57) = 92;
	*(coef35 + 58) = 95;
	*(coef35 + 59) = 98;
	*(coef35 + 60) = 112;
	*(coef35 + 61) = 100;
	*(coef35 + 62) = 130;
	*(coef35 + 63) = 99;

	for (int i = 0; i < 64; i++) {
		*(coef58 + i) = 1;
		*(coef28 + i) = 100;
	}
	// 35db�� 30db�� ���Ϲ����̹Ƿ� �̸� �̿��غ���
	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			*(coef30 + row * 8 + column) = *(coef35 + (7 - row) * 8 + column);
		}
	}

	// ����ȭ
	// row column�� dct �̹����� ��� ��, i, j�� ��� ���� ��� ��
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// ������
			for (int i = 0; i < 8; i++) {
				for (int j = 0; j < 8; j++) {
					if (db == 35) {
						*(output_image + (row + i) * WIDTH + column + j) = (int)(*(dct_image + (row + i) * WIDTH + column + j) / *(coef35 + i * 8 + j));
					}
					else if (db == 30) {
						*(output_image + (row + i) * WIDTH + column + j) = (int)(*(dct_image + (row + i) * WIDTH + column + j) / *(coef30 + i * 8 + j));
					}
					else if (db == 28) {
						*(output_image + (row + i) * WIDTH + column + j) = (int)(*(dct_image + (row + i) * WIDTH + column + j) / *(coef28 + i * 8 + j));
					}
					else if (db == 58) {
						*(output_image + (row + i) * WIDTH + column + j) = (int)(*(dct_image + (row + i) * WIDTH + column + j) / *(coef58 + i * 8 + j));
					}
					if (row < 16 && column < 16) {
						printf("%.1f ", *(output_image + (row + i) * WIDTH + column + j));
					}
				}
				if (row < 16 && column < 16) {
					printf("\n");
				}
			}
			if (row < 16 && column < 16) {
				printf("��� �̵� \n");
			}
			column = column + 8;
		}
		row = row + 8;
	}
}
void quantization_IDCT(int db, double* input_image, unsigned char* output_image_unsigned_char) {

	double* input_image = NULL;
	double* mid_image = NULL;
	double* output_image = NULL;

	double* basis_vector = NULL;
	// ���� ��� ����� �ӽ� ������ ������
	double* dot_product = NULL;

	int* coef35 = NULL;
	int* coef58 = NULL;
	int* coef28 = NULL;
	int* coef30 = NULL;

	coef35 = (int*)malloc(sizeof(int) * 8 * 8);
	coef58 = (int*)malloc(sizeof(int) * 8 * 8);
	coef28 = (int*)malloc(sizeof(int) * 8 * 8);
	coef30 = (int*)malloc(sizeof(int) * 8 * 8);

	mid_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	output_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);


	basis_vector = (double*)malloc(sizeof(double) * 8 * 8 * 8 * 8);
	dot_product = (double*)malloc(sizeof(double) * 8 * 8);

	// ��� ������ ���� �ϴ� ������... �־����ϴ�.
	*(coef35) = 16;
	*(coef35 + 1) = 11;
	*(coef35 + 2) = 10;
	*(coef35 + 3) = 16;
	*(coef35 + 4) = 24;
	*(coef35 + 5) = 40;
	*(coef35 + 6) = 51;
	*(coef35 + 7) = 61;

	*(coef35 + 8) = 12;
	*(coef35 + 9) = 12;
	*(coef35 + 10) = 14;
	*(coef35 + 11) = 19;
	*(coef35 + 12) = 26;
	*(coef35 + 13) = 58;
	*(coef35 + 14) = 60;
	*(coef35 + 15) = 55;

	*(coef35 + 16) = 14;
	*(coef35 + 17) = 13;
	*(coef35 + 18) = 16;
	*(coef35 + 19) = 19;
	*(coef35 + 20) = 26;
	*(coef35 + 21) = 58;
	*(coef35 + 22) = 60;
	*(coef35 + 23) = 55;

	*(coef35 + 24) = 18;
	*(coef35 + 25) = 22;
	*(coef35 + 26) = 37;
	*(coef35 + 27) = 56;
	*(coef35 + 28) = 68;
	*(coef35 + 29) = 109;
	*(coef35 + 30) = 103;
	*(coef35 + 31) = 77;

	*(coef35 + 32) = 24;
	*(coef35 + 33) = 35;
	*(coef35 + 34) = 55;
	*(coef35 + 35) = 64;
	*(coef35 + 36) = 81;
	*(coef35 + 37) = 104;
	*(coef35 + 38) = 113;
	*(coef35 + 39) = 92;

	*(coef35 + 40) = 49;
	*(coef35 + 41) = 64;
	*(coef35 + 42) = 78;
	*(coef35 + 43) = 87;
	*(coef35 + 44) = 103;
	*(coef35 + 45) = 121;
	*(coef35 + 46) = 120;
	*(coef35 + 47) = 101;

	*(coef35 + 48) = 49;
	*(coef35 + 49) = 64;
	*(coef35 + 50) = 78;
	*(coef35 + 51) = 87;
	*(coef35 + 52) = 103;
	*(coef35 + 53) = 121;
	*(coef35 + 54) = 120;
	*(coef35 + 55) = 101;

	*(coef35 + 56) = 72;
	*(coef35 + 57) = 92;
	*(coef35 + 58) = 95;
	*(coef35 + 59) = 98;
	*(coef35 + 60) = 112;
	*(coef35 + 61) = 100;
	*(coef35 + 62) = 130;
	*(coef35 + 63) = 99;

	for (int i = 0; i < 64; i++) {
		*(coef58 + i) = 1;
		*(coef28 + i) = 100;
	}
	// 35db�� 30db�� ���Ϲ����̹Ƿ� �̸� �̿��غ���
	for (int row = 0; row < 8; row++) {
		for (int column = 0; column < 8; column++) {
			*(coef30 + row * 8 + column) = *(coef35 + (7 - row) * 8 + column);
		}
	}


	int dimension = 8;
	// basis vector ���, ���Ͱ� v��, �� ���ʹ� v�� ���� (v�� DIMENTION ���)
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
					}// basis�� 4�����̶� ���� ��, ���� ���� ������ �� �ִ� i�� �̹��� �������� ������, j�� �̹��� �������� ���� ��, u�� ���ļ��� ������, v�� ���ļ��� ������, i�� �̹��� �������� ������, j�� �̹��� �������� ���� ��
					*(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v) = c_v * c_u / (dimension / 2.0) * cos((2 * i + 1) * u * M_PI / 16.0) * cos((2 * j + 1) * v * M_PI / 16.0);
				}
			}
		}
	}
	
	// block IDCT
	for (int row = 0; row < HEIGHT; row) {
		for (int column = 0; column < WIDTH; column) {
			// ���� 2���� ����? �̹Ƿ� �������� ������ ���� ��� �����Ϳ� i, j�� ���� (����, ������)
			for (int i = 0; i < dimension; i++) {
				for (int j = 0; j < dimension; j++) {
					// �ϳ��ϳ� ���ؼ� ������. �����Ϳ� ���ļ� �����Ϳ� u, v�� ���� (���ļ� ����, ���ļ� ������)
					// dot_product�� ��� ����� �ӽ� ����
					for (int u = 0; u < dimension; u++) {
						for (int v = 0; v < dimension; v++) {
							if (db == 35) {
								*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								*(dot_product + u * dimension + v) = *(dot_product + u * dimension + v) * *(coef35 + u * dimension + v);
							}
							else if (db == 30) {
								*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								*(dot_product + u * dimension + v) = *(dot_product + u * dimension + v) * *(coef30 + u * dimension + v);
							}
							else if (db == 28) {
								*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								*(dot_product + u * dimension + v) = *(dot_product + u * dimension + v) * *(coef28 + u * dimension + v);
							}
							else if (db == 58) {
								*(dot_product + u * dimension + v) = *(input_image + (row + u) * WIDTH + column + v) * *(basis_vector + i * dimension * dimension * dimension + j * dimension * dimension + u * dimension + v);
								*(dot_product + u * dimension + v) = *(dot_product + u * dimension + v) * *(coef58 + u * dimension + v);
							}
						}
					}
					// �ֽ��� ���� ��, (dot_product)�� ��� ���� ���� ����� �Ϸ��Ѵ�.
					// �� ���� ��� ����, i, j�� �����Ѵ�.
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

	// ���� & clipping
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		if (*(output_image + i) < 0) {
			*(output_image_unsigned_char + i) = 0;
		}
		else if (*(output_image + i) > 255) {
			*(output_image_unsigned_char + i) = 255;
		}
		else {
			*(output_image_unsigned_char + i) = round(*(output_image + i));
		}
	}

	// PSNR ���

	FILE* original_file = NULL;
	unsigned char* original_image = NULL;

	original_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	original_file = fopen("./lena.img", "rb");

	fread(original_image, sizeof(unsigned char), WIDTH * HEIGHT, original_file);
	fclose(original_file);

	double PSNR = psnr(original_image, output_image_unsigned_char);

	printf("%d db ����ȭ ���� PSNR : %f\n", db, PSNR);

	free(mid_image);
	free(output_image);
	free(basis_vector);
	free(dot_product);
	free(coef35);
	free(coef58);
	free(coef28);
	free(coef30);
	return 0;
}

void show_error_between_DCT_blockDCT(void) {
	FILE* dct = NULL;
	FILE* block_dct = NULL;

	double* dct_image = NULL;
	double* block_dct_image = NULL;

	dct_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);
	block_dct_image = (double*)malloc(sizeof(double) * WIDTH * HEIGHT);

	dct = fopen("./lena_DCT.img", "rb");
	fread(dct_image, sizeof(double), HEIGHT * WIDTH, dct);
	fclose(dct);

	block_dct = fopen("./lena_DCT.img", "rb");
	fread(block_dct_image, sizeof(double), HEIGHT * WIDTH, block_dct);
	fclose(block_dct);

	double TE = total_error_double(dct_image, block_dct_image);
	printf("���μ��� DCT�� block DCT ������ ���� \n");
	printf("TE : %f\n", TE);
	printf("MSE : %f\n", TE / (WIDTH * HEIGHT));

	free(dct_image);
	free(block_dct_image);
}

void show_error_between_original_IDCT(void) {
	FILE* dct = NULL;
	FILE* block_dct = NULL;

	unsigned char* dct_image = NULL;
	unsigned char* block_dct_image = NULL;

	dct_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	block_dct_image = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	dct = fopen("./lena.img", "rb");
	fread(dct_image, sizeof(unsigned char), HEIGHT * WIDTH, dct);
	fclose(dct);

	block_dct = fopen("./recon_lena_view.img", "rb");
	fread(block_dct_image, sizeof(unsigned char), HEIGHT * WIDTH, block_dct);
	fclose(block_dct);

	double TE = total_error_unsigned_char(dct_image, block_dct_image);
	printf("���� �̹����� IDCT�� �̹��� ������ ���� \n");
	printf("TE : %f\n", TE);
	printf("MSE : %f\n", TE / (WIDTH * HEIGHT));

	free(dct_image);
	free(block_dct_image);
}