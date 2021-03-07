#pragma once
#include<Windows.h>
#include<ctime>
class Caro_class
{
	int value_caro[30][30];//Mnag3 2 chiều lưu điểm đánh giá của từng ô qua
	int matrix[30][30];//Ma trận quản lí các ô trong màn hình.giá trị tại ô đó trong ma trận là 0 thì ô chưa đánh,nếu giá trị tại ô đó là 1 thì tại đó là X,còn O là 2
	int VT_x;//2 biến vị trí dùng để lưu lại vị trí tốt nhất cần phải đánh vào để chặn hướng tấn công của user
	int VT_y;//2 biến vị trí dùng để lưu lại vị trí tốt nhất cần phải đánh vào để chặn hướng tấn công của user
	int ix;
	int iy;
	//còn kể từ nước thứ 2 trở đi sẽ kiểm tra và chặn các đường đi của user
public:
	int check;
	bool kt;// = true;//kiễm tra xem máy đã đánh chưa nếu chưa thì user ko được đánh
	void set_value(int x, int y,int);//đặt lại value của 1 ô trên bàn cờ
	int get_value(int, int);
	int dem();//đếm số ô có giá trị khác 0
	int max_value(short & ,short& y);//lấy giá trị lớn nhất đã được đánh giá
	int min_value(short&, short& y);//lấy giá trị nhỏ nhất đã được đánh giá
	int getMatrix(int, int);
	void Khoitao();
	Caro_class();
	~Caro_class();
};

