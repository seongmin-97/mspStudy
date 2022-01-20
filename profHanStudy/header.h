#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define WIDTH 512
#define HEIGHT 512

#include <stdio.h>
#include <malloc.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>


// function.c 자주 쓰일 것으로 예상되는 함수들

// 입력값 : 값을 읽어올 포인터, 읽어올 파일
void read_image(unsigned char*, char*);
// lena 이미지 저장, 입력값 : 저장하고 싶은 정보가 있는 포인터, 파일 이름
void save_unsigned_char_to_image(unsigned char*, char*);
void save_double_to_image(double*, char*);

// 원본 이미지, 예측 이미지, 가로 사이즈 (두 이미지가 정사각형일 때만 사용 가능)
double MSE(unsigned char*, unsigned char*, int);
// 이미지 파일 입출력, MSE 결과 프린트까지 한 번에
int calculate_image_MSE(char*, char*);

// 비교할 두 값
double total_error_double(double*, double*);
double total_error_unsigned_char(unsigned char*, unsigned char*);
double psnr(unsigned char*, unsigned char*);

// 블록 프린트
// 입력 이미지, row, column (단, block을 하나의 데이터라 생각하고 위치 입력)
void print_block_unsigned_char(unsigned char*, int, int);
void print_block_char(char*, int, int);
void print_block_double(double*, int, int);
void print_block_int(int*, int, int);

// homework 1 이미지 회전/ 좌우반전/ 상하반전

// user의 입력을 받아 이미지 회전/좌우 반전/상하 반전 시행
int getUserInput(void);

int rotate90(void);
int rotate180(void);
int rotate270(void);
int mirror(void);
int flip(void);

// homework 2 이미지 해상도 변경

// Scailing 매개변수는 순서대로
// 입력 파일 경로, 출력 파일 경로, 입력 가로 픽셀 수, 입력 세로 픽셀 수, 출력 가로 픽셀 수, 출력 세로 픽셀 수 
// LPF 탭 수(0 <= 사용x, 양수 입력은 사용), cut_off frequency(소숫점 아래를 두 자리 정수로), mode(1 : S&H, 2 : Bilinear, 3 : Cubic Conv, 4 : cubic b spline)
int Scailing(char*, char*, int, int, int, int, int, int, int);

// LPF 함수: 입력 이미지, 가로 길이, 세로 길이, LPF 탭 수, cut_off frequency(소숫점 아래를 두 자리 정수로) 모드 (1 : 가로 방향, 2 : 세로 방향)
int LPF(float*, int, int, int, int, int);

// 입력 이미지, 가로 길이, 세로 길이, tap 수, 모드 (1 : 가로 방향, 2 : 세로 방향)
int pre_emphasis_filter(float*, int, int, int, int);

// mode(1 : S&H, 2 : Bilinear, 3 : Cubic Conv, 4 : cubic b spline)
int lena512to1000to512(int);
int lena512to400to512NoLPF(int);
int lena512to945(int);
int lena512to298NoLPF(int);

// LPF tap 수, cut_off_frequency, mode(1 : S&H, 2 : Bilinear, 3 : Cubic Conv, 4 : cubic b spline)
int lena512to400to512UsingLPF(int, int, int);
int lena512to298UsingLPF(int, int, int);


// homework 3 DCT

// 차원, 입력 이미지 포인터, 결과 이미지 포인터
int DCT(int, unsigned char*, double*);
// 차원, DCT 결과 저장할 파일 이름, DCT결과를 보기 위한 파일 이름
int block_DCT(int, char*, char*);
// 차원, IDCT할 DCT가 담긴 포인터, 결과 이미지 포인터
int IDCT(int, double*, unsigned char*);
// 일부 계수만 사용한 DCT, 입력값 : 차원, 사용할 계수의 수, IDCT할 파일 이름, 결과 저장 파일 이름, 결과를 보기 위한 파일 이름
int part_of_IDCT(int, int, char*, char*, char*);
// 가로세로 DCT
int vertical_horizontal_DCT(void);
// 양자화, 입력값 : db, 양자화할 DCT 포인터, 결과 포인터
void quantization(int, double*, double*);
// 양자화 DCT, 입력값 : db, IDCT할 포인터, 결과 포인터
void quantization_IDCT(int, double*, unsigned char*);
// DCT 방법 간에 차이가 있는지 확인 & 원본과 IDCT 결과물에 차이가 있는지 확인
void show_error_between_DCT_blockDCT(void);
void show_error_between_original_IDCT(void);
// block size별 cpu 시간 확인
void block_DCT_cpu_time(void);

// homework 5 JPEG

// jpeg 언코더 함수, 입력값 : 읽어올 파일 이름, dpcm, zigzag scan 받을 포인터들
void jpeg_encoder(char*, char*, int*);
// 차원, 입력 이미지 포인터, 결과 이미지 포인터
void DCT_char(int, char*, double*);
// 양자화, 입력값 : 양자화할 DCT 포인터, 결과 포인터
void quantization_jpeg(double*, int*);
// 레벨 쉬프트, 입력 이미지, 결과 이미지
void level_shifting_128_encoder(unsigned char*, char*);
// dpcm, 양자화된 이미지 포인터, dc계수를 저장할 포인터
void DPCM(int*, char*);
// zigzag scan, 스캔할 이미지, 스캔 결과
void zigzag_scan(int*, int*);
// RLC, 부호화할 포인터, 결과, 반환값은 사용한 튜플 수
int run_length_code(int*, char*);



// jpeg 디코더 함수, 결과를 저장할 파일 이름, dpcm포인터, zigzag scan 포인터
void jpeg_decoder(char*, char*, int*);
// dc값을 원상복귀, dpcm 포인터, 결과를 담을 포인터
void decode_DPCM(char*, int*);
// zigzag scan과 dc 계수를 이용해 양자화 행렬 복원, 입력값 : dc계수를 담은 포인터, zigzag_scan 포인터, 결과 담을 포인터
void decode_zigzag_scan(int*, int*, int*);
// 양자화 IDCT, 입력값 : IDCT할 포인터, 결과 포인터
void quantization_IDCT_jpeg(int*, char*);
// 레벨 쉬프트, 입력 이미지, 결과 이미지
void level_shifting_128_decoder(char*, unsigned char*);