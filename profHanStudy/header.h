#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define WIDTH 512
#define HEIGHT 512

#include <stdio.h>
#include <malloc.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>


// function.c ���� ���� ������ ����Ǵ� �Լ���

// �Է°� : ���� �о�� ������, �о�� ����
void read_image(unsigned char* input, char* file_name);
// lena �̹��� ����, �Է°� : �����ϰ� ���� ������ �ִ� ������, ���� �̸�
void save_unsigned_char_to_image(unsigned char* input, char* file_name);
void save_double_to_image(double* input, char* file_name);

// ���� �̹���, ���� �̹���, ���� ������ (�� �̹����� ���簢���� ���� ��� ����)
double MSE(unsigned char*, unsigned char*, int);
// �̹��� ���� �����, MSE ��� ����Ʈ���� �� ����
int calculate_image_MSE(char*, char*);

// ���� �� ��
double total_error_double(double*, double*);
double total_error_unsigned_char(unsigned char*, unsigned char*);
double psnr(unsigned char*, unsigned char*);


// homework 1 �̹��� ȸ��/ �¿����/ ���Ϲ���

// user�� �Է��� �޾� �̹��� ȸ��/�¿� ����/���� ���� ����
int getUserInput(void);

int rotate90(void);
int rotate180(void);
int rotate270(void);
int mirror(void);
int flip(void);

// homework 2 �̹��� �ػ� ����

// Scailing �Ű������� �������
// �Է� ���� ���, ��� ���� ���, �Է� ���� �ȼ� ��, �Է� ���� �ȼ� ��, ��� ���� �ȼ� ��, ��� ���� �ȼ� �� 
// LPF �� ��(0 <= ���x, ��� �Է��� ���), cut_off frequency(�Ҽ��� �Ʒ��� �� �ڸ� ������), mode(1 : S&H, 2 : Bilinear, 3 : Cubic Conv, 4 : cubic b spline)
int Scailing(char*, char*, int, int, int, int, int, int, int);

// LPF �Լ�: �Է� �̹���, ���� ����, ���� ����, LPF �� ��, cut_off frequency(�Ҽ��� �Ʒ��� �� �ڸ� ������) ��� (1 : ���� ����, 2 : ���� ����)
int LPF(float*, int, int, int, int, int);

// �Է� �̹���, ���� ����, ���� ����, tap ��, ��� (1 : ���� ����, 2 : ���� ����)
int pre_emphasis_filter(float*, int, int, int, int);

// mode(1 : S&H, 2 : Bilinear, 3 : Cubic Conv, 4 : cubic b spline)
int lena512to1000to512(int);
int lena512to400to512NoLPF(int);
int lena512to945(int);
int lena512to298NoLPF(int);

// LPF tap ��, cut_off_frequency, mode(1 : S&H, 2 : Bilinear, 3 : Cubic Conv, 4 : cubic b spline)
int lena512to400to512UsingLPF(int, int, int);
int lena512to298UsingLPF(int, int, int);


// homework 3 DCT

// ����, �Է� �̹��� ������, ��� �̹��� ������
int DCT(int, unsigned char*, double*);
// ����, DCT ��� ������ ���� �̸�, DCT����� ���� ���� ���� �̸�
int block_DCT(int, char*, char*);
// ����, IDCT�� DCT�� ��� ������, ��� �̹��� ������
int IDCT(int, double*, unsigned char*);
// �Ϻ� ����� ����� DCT, �Է°� : ����, ����� ����� ��, IDCT�� ���� �̸�, ��� ���� ���� �̸�, ����� ���� ���� ���� �̸�
int part_of_IDCT(int, int, char*, char*, char*);
// ���μ��� DCT
int vertical_horizontal_DCT(void);
// ����ȭ, �Է°� : db, ����ȭ�� DCT ������, ��� ������
void quantization(int, double*, double*);
// ����ȭ DCT, �Է°� : db, IDCT�� ������, ��� ������
void quantization_IDCT(int, double*, unsigned char*);
// DCT ��� ���� ���̰� �ִ��� Ȯ�� & ������ IDCT ������� ���̰� �ִ��� Ȯ��
void show_error_between_DCT_blockDCT(void);
void show_error_between_original_IDCT(void);
// block size�� cpu �ð� Ȯ��
void block_DCT_cpu_time(void);