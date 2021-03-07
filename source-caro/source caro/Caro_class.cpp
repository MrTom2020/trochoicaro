#include "stdafx.h"
#include "Caro_class.h"
//New game
int Caro_class::max_value(short& x, short& y)
{
	int i, j;
	int max = 0;
	for (i = 0; i < 30; i++)
	{
		for (j = 0; j < 30; j++)
		{
			if (value_caro[i][j] > max)
			{
				max = value_caro[i][j];
				x = i; y = j;
			}
		}
	}
	return 1;
}
int Caro_class::min_value(short& x, short& y)
{
	int i, j;
	int min = 0;
	for (i = 0; i < 30; i++)
	{
		for (j = 0; j < 30; j++)
		{
			if (value_caro[i][j] < min)
			{
				min = value_caro[i][j];
				x = i; y = j;
			}
		}
	}
	return 1;
}
void Caro_class::Khoitao()
{
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			value_caro[i][j] = 0;
		}
	}
}
int Caro_class::dem()
{
	int _dem = 0;
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			if (value_caro[i][j])
			{
				_dem++;
			}
		}
	}
	return _dem;
}
void Caro_class::set_value(int x, int y,int value)
{
	value_caro[x][y] = value;
}
int Caro_class::get_value(int x, int y)
{
	return value_caro[x][y];
}
//lấy giá trị 1 ô của ma trận
int Caro_class::getMatrix(int i, int j)
{
	return matrix[i][j];
}
//lấy giá trị thuộc tính check 
//tìm xem có ai đã rới vào trường hợp chiến thắng ko
//Phản công với các dường 3 có chặn  1 đầu
Caro_class::Caro_class()
{
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
				matrix[i][j] = 0;
		}
	}
	matrix[12][12] = 2;
}
Caro_class::~Caro_class()
{
	
}