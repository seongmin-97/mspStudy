#define _CRT_SECURE_NO_WARNINGS
#define WIDTH 512
#define HEIGHT 512

#include <stdio.h>
#include <malloc.h>


int getUserInput(void);
int rotate90(void);
int rotate180(void);
int rotate270(void);
int mirror(void);
int flip(void);

int main(void) {

	getUserInput();

	return 0;
}

int getUserInput(void) {

	int mode;
	int result = 1;

	while (result > 0) {

		printf("���ϴ� �̹����� �����ϼ���.\n");
		printf("0 : ���α׷� ����\n");
		printf("1 : ���������� 90�� ȸ��\n");
		printf("2 : 180�� ȸ��\n");
		printf("3 : �������� 90�� ȸ��\n");
		printf("4 : �¿����\n");
		printf("5 : ���Ϲ���\n");
		printf("��ȣ�� �Է��ϼ��� : ");
		
		scanf_s("%d", &mode);

		if (mode == 1) {
			result = rotate90();
		}
		else if (mode == 2) {
			result = rotate180();
		}
		else if (mode == 3) {
			result = rotate270();
		}
		else if (mode == 4) {
			result = mirror();
		}
		else if (mode == 5) {
			result = flip();
		}
		else if (mode == 0) {
			result = 0;
		}
		else {
			return -1;
		}
	}

	return 0;
}

int rotate90(void) {

	// �ʿ��� ���� ����
	FILE* input_file = NULL;
	FILE* output_file = NULL;

	unsigned char* img = NULL;
	unsigned char* result = NULL;
	img = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	result = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	// error �߻� ��
	if (img == NULL) {
		printf("malloc error");
		return -1;
	}

	// raw ���� �о����
	input_file = fopen("./lena.img", "rb");
	if (input_file == NULL) {
		printf("load img error");
		return -1;
	}
	fread(img, sizeof(unsigned char), HEIGHT * WIDTH, input_file);
	fclose(input_file);


	for (int row = 0; row < WIDTH; row++) {
		for (int column = 0; column < HEIGHT; column++) {
			*(result + row * WIDTH + column) = *(img + (HEIGHT - column) * WIDTH + row);
		}
	}

	output_file = fopen("./lena90.img", "wb");
	fwrite(result, sizeof(unsigned char), HEIGHT * WIDTH, output_file);
	fclose(output_file);

	free(img);
	free(result);

	return 1;
}

int rotate180(void) {

	// �ʿ��� ���� ����
	FILE* input_file = NULL;
	FILE* output_file = NULL;

	unsigned char* img = NULL;
	unsigned char* result = NULL;
	img = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	result = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	// error �߻� ��
	if (img == NULL) {
		printf("malloc error");
		return -1;
	}

	// raw ���� �о����
	input_file = fopen("./lena.img", "rb");
	if (input_file == NULL) {
		printf("load img error");
		return -1;
	}
	fread(img, sizeof(unsigned char), HEIGHT * WIDTH, input_file);
	fclose(input_file);


	for (int row = 0; row < WIDTH; row++) {
		for (int column = 0; column < HEIGHT; column++) {
			*(result + row * WIDTH + column) = *(img + (HEIGHT - row) * WIDTH + (WIDTH - column));
		}
	}

	output_file = fopen("./lena180.img", "wb");
	fwrite(result, sizeof(unsigned char), HEIGHT * WIDTH, output_file);
	fclose(output_file);

	free(img);
	free(result);

	return 1;
}

int rotate270(void) {

	// �ʿ��� ���� ����
	FILE* input_file = NULL;
	FILE* output_file = NULL;

	unsigned char* img = NULL;
	unsigned char* result = NULL;
	img = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	result = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	// error �߻� ��
	if (img == NULL) {
		printf("malloc error");
		return -1;
	}

	// raw ���� �о����
	input_file = fopen("./lena.img", "rb");
	if (input_file == NULL) {
		printf("load img error");
		return -1;
	}
	fread(img, sizeof(unsigned char), HEIGHT * WIDTH, input_file);
	fclose(input_file);


	for (int row = 0; row < WIDTH; row++) {
		for (int column = 0; column < HEIGHT; column++) {
			*(result + row * WIDTH + column) = *(img + column * WIDTH + (WIDTH - row));
		}
	}

	output_file = fopen("./lena270.img", "wb");
	fwrite(result, sizeof(unsigned char), HEIGHT * WIDTH, output_file);
	fclose(output_file);

	free(img);
	free(result);

	return 1;
}

int mirror(void) {

	// �ʿ��� ���� ����
	FILE* input_file = NULL;
	FILE* output_file = NULL;

	unsigned char* img = NULL;
	unsigned char* result = NULL;
	img = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	result = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	// error �߻� ��
	if (img == NULL) {
		printf("malloc error");
		return -1;
	}

	// raw ���� �о����
	input_file = fopen("./lena.img", "rb");
	if (input_file == NULL) {
		printf("load img error");
		return -1;
	}
	fread(img, sizeof(unsigned char), HEIGHT * WIDTH, input_file);
	fclose(input_file);


	for (int row = 0; row < WIDTH; row++) {
		for (int column = 0; column < HEIGHT; column++) {
			*(result + row * WIDTH + column) = *(img + row * WIDTH + (WIDTH - column));
		}
	}

	output_file = fopen("./lenaLeftRight.img", "wb");
	fwrite(result, sizeof(unsigned char), HEIGHT * WIDTH, output_file);
	fclose(output_file);

	free(img);
	free(result);

	return 1;
}

int flip(void) {

	// �ʿ��� ���� ����
	FILE* input_file = NULL;
	FILE* output_file = NULL;

	unsigned char* img = NULL;
	unsigned char* result = NULL;
	img = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);
	result = (unsigned char*)malloc(sizeof(unsigned char) * WIDTH * HEIGHT);

	// error �߻� ��
	if (img == NULL) {
		printf("malloc error");
		return -1;
	}

	// raw ���� �о����
	input_file = fopen("./lena.img", "rb");
	if (input_file == NULL) {
		printf("load img error");
		return -1;
	}
	fread(img, sizeof(unsigned char), HEIGHT * WIDTH, input_file);
	fclose(input_file);


	for (int row = 0; row < WIDTH; row++) {
		for (int column = 0; column < HEIGHT; column++) {
			*(result + row * WIDTH + column) = *(img + (HEIGHT - row) * WIDTH + column);
		}
	}

	output_file = fopen("./lenaUpdown.img", "wb");
	fwrite(result, sizeof(unsigned char), HEIGHT * WIDTH, output_file);
	fclose(output_file);

	free(img);
	free(result);

	return 1;
}