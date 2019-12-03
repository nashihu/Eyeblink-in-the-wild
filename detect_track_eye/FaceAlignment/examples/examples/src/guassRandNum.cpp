#include"guassRandNum.h"
#include <iostream>  
#include <time.h>  
#include <iomanip>  
#include <math.h>  
#include <fstream>  
#ifndef PI
#define PI 3.14159  
#endif
void UNIFORM(double *);  //UINFORM��������  

int guassRandNum(double E, double D, double result[][3], const int GuassNum, const int AffineNum)
{
	int i, j;
	double A, B, C, r;
	double uni[2];
	double *p;
	srand((unsigned)time(NULL));  //��������Ӳ���ϵͳʱ��  
	//std::ofstream outfile("Gauss.txt", std::ios::out);  //�����ļ�����  
	std::cout << "�����ͷ���:" << E <<" "<< D << std::endl;
	for (j = 0; j<GuassNum; j++)
	{
		for (i = 0; i < AffineNum; i++)
		{
			UNIFORM(&uni[0]);  //����UNIFORM��������2�����ȷֲ������������������nui[2]  
			A = sqrt((-2)*log(uni[0]));
			B = 2 * PI*uni[1];
			C = A*cos(B);
			r = E + C*D;    //E,D�ֱ��������ͷ���  
			result[j][i] = r;
			//outfile << r << "   ";  //������C�������������ļ���  
		}
	}
	return 0;
}
void UNIFORM(double *p)
{
	static int x = 0;
	int i, a;
	double f;
	for (i = 0; i<2; i++, x = x + 689)
	{
		a = rand() + x;  //����689����Ϊϵͳ����������ĸ���Ƶ��ԶԶ����������ú�����ʱ��  
		a = a % 1000;
		f = (double)a;
		f = f / 1000.0;
		*p = f;
		p++;
	}
}