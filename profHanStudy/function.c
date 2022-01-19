#include "header.h"

void read_image(unsigned char* input, char* file_name) {
	FILE* lena_file = NULL;

	lena_file = fopen(file_name, "rb");

	if (lena_file == NULL) {
		printf("load img error");
	}
	else {
		fread(input, sizeof(unsigned char), HEIGHT * WIDTH, lena_file);
		fclose(lena_file);
	}
}

void save_unsigned_char_to_image(unsigned char* input, char* file_name) {
	FILE* save_file = NULL;

	save_file = fopen(file_name, "wb");

	if (save_file == NULL) {
		printf("save file error");
	}
	else {
		fwrite(input, sizeof(unsigned char), WIDTH * HEIGHT, save_file);
		fclose(save_file);
	}
}

void save_double_to_image(double* input, char* file_name) {
	FILE* save_file = NULL;

	save_file = fopen(file_name, "wb");

	if (save_file == NULL) {
		printf("save file error");
	}
	else {
		fwrite(input, sizeof(double), WIDTH * HEIGHT, save_file);
		fclose(save_file);
	}
}

double MSE(unsigned char* x, unsigned char* y, int size) {

	double error = 0;

	for (int i = 0; i < size * size; i++) {
		error = error + (*(x + i) - *(y + i)) * (*(x + i) - *(y + i));
	}
	return error / (double)(size * size);
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

double total_error_double(double* input, double* output) {
	double result = 0;
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		result += (*(input + i) - *(output + i)) * (*(input + i) - *(output + i));
	}
	return result;
}

double total_error_unsigned_char(unsigned char* input, unsigned char* output) {
	double result = 0;
	for (int i = 0; i < WIDTH * HEIGHT; i++) {
		result += (*(input + i) - *(output + i)) * (*(input + i) - *(output + i));
	}
	return result;
}

double psnr(unsigned char* input, unsigned char* output) {
	double te = total_error_unsigned_char(input, output);
	double mse = te / (WIDTH * HEIGHT);
	return 20 * log10(255) - 10 * log10(mse);
}
