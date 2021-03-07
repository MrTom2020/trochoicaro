// Caro.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Caro.h"
#define MAX_LOADSTRING 100
// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
bool pause = false;//Biến dùng để dừng trò chơi lại
bool user = true;//Ban đầu người chơi sẽ đánh trước,còn về sau ai thắng sẽ là người đánh trước
//Người đánh trước sẽ là X còn người đánh sau là O   
int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_CARO, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CARO));

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CARO));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_CARO);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}



//cấu trúc lưu lại tọa độ 1 điểm trên bàn cờ
struct  point
{
	short x; short y;
	int _danhgia; // = 0;
};
enum Luotdanh{ NGUOI, MAY };
Luotdanh _luot;//lượt đánh của người hoặc máy
int BanCo[30][30];//mảng lưu các vị trí đánh của bàn cờ
int max_dosau =3;//độ sâu kiểm tra trong thuật toán
bool Com_win = false;//kiểm tra xem máy có đi vào nước win hay ko
bool Com_lose = false;//kiểm tra xem máy có đi vào nước lose hay ko
short ix;//tọa độ x nước cờ vừa đánh của máy
short iy;//tọa độ y nước cờ vừa đánh của máy
//mảng điểm tấn công
short DiemTanCong[5] = { 0, 2, 18, 162, 1400 };
//mảng điểm phòng ngự
short DiemPhongNgu[5] = { 0, 1, 9, 81, 729 };
//hàm đánh giá 
point *_NuocWin[11];//đánh giá 3 nước đầu tiên được đánh giá là 3 nước tt61 nhất để chọn đánh
short count;//di kèm với _NuocWin
point *_NuocKT[3];//đánh giá 3 nước đầu tiên được đánh giá là 3 nước tt61 nhất để chọn đánh

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX,
      0, 0, 915, 900, NULL, NULL, hInstance, NULL);
   BanCo[12][12] = 2;
   ix = 12;
   iy = 12;
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}


//kiểm tra máy
bool Com( int i, int j)
{
	if (i >= 0 && i < 30 && j >= 0 && j < 30)
	{
		if (BanCo[i][j] == 2)
		{
			return true;
		}
		return false;
	}return false;
}
//Kiểm tra người
bool User(int i, int j)
{
	if (i >= 0 && i < 30 && j >= 0 && j < 30)
	{
		if (BanCo[i][j] == 1)
		{
			return true;
		}
		return false;
	}return false;
}



//vẽ chữ X
void DrawX(HDC hdc, int iY, int iX)
{
	HPEN hpen;
	HPEN OldPen;
	COLORREF color = RGB(223, 0, 41);
	hpen = CreatePen(PS_SOLID, 4, color);
	OldPen = (HPEN)SelectObject(hdc, hpen);
	//Set cac gia tri
	int X1 = iX * 30 + 4;
	int X2 = (iX + 1) * 30 - 4;
	int Y1 = iY * 30 + 4;
	int Y2 = (iY + 1) * 30 - 4;
	//Vẽ
	MoveToEx(hdc, X1, Y1, NULL);
	LineTo(hdc, X2, Y2);
	MoveToEx(hdc, X2, Y1, NULL);
	LineTo(hdc, X1, Y2);
	///
	SelectObject(hdc, OldPen);
	DeleteObject(hpen);
}
//Vẽ hình tròn == O
void DrawO(HDC hdc, int iY, int iX)
{
	HPEN hpen;
	HPEN OldPen;
	COLORREF color = RGB(0, 178, 191);
	hpen = CreatePen(PS_SOLID, 4, color);
	OldPen = (HPEN)SelectObject(hdc, hpen);
	//set toa do
	int x1 = iX * 30 + 2;
	int x2 = (iX + 1) * 30 - 2;
	int y1 = iY * 30 + 2;
	int y2 = (iY + 1) * 30 - 2;
	Ellipse(hdc, x1, y1, x2, y2);
	SelectObject(hdc, OldPen);
	DeleteObject(hpen);
}
//vẽ màn hình các ô vuông
void DrawBoard(HDC hdc)
{
	HPEN hpen;
	HPEN OldPen;
	COLORREF color = RGB(0, 174, 114);
	hpen = CreatePen(PS_SOLID, 1, color);
	OldPen = (HPEN)SelectObject(hdc, hpen);
	//Draw coloum
	for (int i = 0; i <= 900; i = i + 30)
	{
		MoveToEx(hdc, i, 0, NULL);
		LineTo(hdc, i, 900);
	}
	//Draw row
	for (int i = 0; i <= 900; i = i + 30)
	{
		MoveToEx(hdc, 0, i, NULL);
		LineTo(hdc, 900, i);
	}
	SelectObject(hdc, OldPen);
	DeleteObject(hpen);
}

void NewGame()
{
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			BanCo[i][j] = 0;
		}
	}
	_luot = NGUOI;
	BanCo[12][12] = 2;
}
// kiểm tra Com or user Win
int Find_Win(bool(*point)(int, int))
{
	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			//xét hàng ngang
			if (point( i, j) == true && point( i + 1, j) == true && point( i + 2, j) == true && point( i + 3, j) == true && point( i + 4, j) == true)
			{
				return 1;
			}
			//xét hàng dọc
			if (point( i, j) == true && point( i, j + 1) == true && point( i, j + 2) == true && point( i, j + 3) == true && point( i, j + 4) == true)
			{
				return 1;
			}
			//chéo từ trên xuống qua bên phải
			if (point( i, j) == true && point( i + 1, j + 1) == true && point( i + 2, j + 2) == true && point( i + 3, j + 3) == true && point( i + 4, j + 4) == true)
			{
				return 1;
			}
			//chéo từ trên xuống qua bên trái
			if (point( i, j) == true && point( i - 1, j + 1) == true && point( i - 2, j + 2) == true && point( i - 3, j + 3) == true && point( i - 4, j + 4) == true)
			{
				return 1;
			}
		}
	}
	return 0;
}
//**********************TÌM N	ƯỚC ĐI  TỐI ƯU CHO MÁY *******************************//
void hang_ngang(int &i, int &j)
{
	j++;
}
void hang_doc(int &i, int &j)
{
	i++;
}
void cheoquaphai(int &i, int &j)
{
	i++;
	j++;
}
void cheoquatrai(int &i, int &j)
{
	i++;
	j--;
}
//biến caro 
Caro_class caro;

void Tinh_gia_tri(void(*pointer)(int&, int&),Luotdanh _luot)
{
	int i, j;
	int gh_cot1 = 0;
	int gh_cot2 = 0;
	int gh_dong1 = 0;
	int gh_dong2 = 0;
	if (pointer==hang_ngang)
	{
		gh_cot2 = 4;
	}
	if (pointer == hang_doc)
	{
		gh_dong2 = 4;
	}
	if (pointer == cheoquatrai)
	{
		gh_dong1 = 4;
		gh_cot2 = 4;
	}
	if (pointer == cheoquaphai)
	{
		gh_cot2 = 4;
		gh_dong2 = 4;
	}
	int CountCom, CountUser;
	for (int dong = gh_dong1; dong < 30- gh_dong2; dong++)
		for (int cot = gh_cot1; cot < 30 - gh_cot2; cot++)
		{
			CountCom = 0; CountUser = 0; i = 0; j = 0;
			for (i = 0, j = 0; (i < 5) && (j<5); pointer(i, j))
			{
				//đếm số quên cờ của người và máy trong phạm vi 5 con
				if (BanCo[dong+i][ cot + j] == 1) CountUser++;
				if (BanCo[dong+i][ cot + j] == 2) CountCom++;
			}
			//trong vòng 5 ô thì ko có quân địch
			if (CountUser * CountCom == 0 && CountUser != CountCom)
			{
				for (i = 0, j = 0; (i < 5)&&(j<5); pointer(i, j))
				{
					//tìm ô chưa đánh trông 5 ô đó
					if ((dong+i) == 12 && (cot+j) == 12)
					{
						int a = 2;
					}
					if (BanCo[dong+i][ cot + j] == 0) // Neu o chua duoc danh
					{
						//nếu trong 5 ô ko có quân cờ của người
						if (CountUser == 0)
						{
							//mà lại là lượ đánh của người thì tăng điểm phòng thủ
							if (_luot == NGUOI)
								caro.set_value(dong + i, cot + j, -caro.get_value(dong + i, cot + j) + DiemPhongNgu[CountCom]);
							//ngược lại lượt đánh là 
							else caro.set_value(dong + i, cot + j, caro.get_value(dong + i, cot + j) + DiemTanCong[CountCom]);
						}
						if (CountCom == 0)
						{
							if (_luot == MAY)
								caro.set_value(dong + i, cot + j, caro.get_value(dong + i, cot + j) + DiemPhongNgu[CountUser]);
							//ngược lại lượt đánh là 
							else caro.set_value(dong + i, cot + j, -caro.get_value(dong + i, cot + j) + DiemTanCong[CountUser]);
							if (CountUser == 4 || CountCom == 4)
								caro.set_value(dong + i, cot + j, caro.get_value(dong + i, cot + j) * 2);
						}
						if (_luot==NGUOI)
						{
							if (dong + i == 15)
							{
								int a = 2;
							}
							caro.set_value(dong + i, cot + j, -(caro.get_value(dong + i, cot + j)));
						}
					}
				}

			}
		}
}
//Tính tất cả giá trị của các ô trên bàn cờ theo hế độ Com or User
void danhgia(Luotdanh _luot)
{
	caro.Khoitao();
	Tinh_gia_tri(hang_ngang, _luot);
	Tinh_gia_tri(hang_doc, _luot);
	Tinh_gia_tri(cheoquatrai, _luot);
	Tinh_gia_tri(cheoquaphai, _luot);
}
int temp_user1;
int temp_com1;
//max com
void _min(int _dosau_hientai);
void _max(int _dosau_hientai)
{
	int max = -50000;
	int dem = _dosau_hientai;
	danhgia(MAY);
	point *_Com;////lưu lại 3 vị trí của máy được chọn trong giả lập
	int n;//số phần tử
	//n = caro.dem() ;
	_Com = new point[3];
	//tìm 3 ô trống mà có giá trị được đánh giá là cao nhất
	for (int i = 0; i < 3; i++)
	{
		caro.max_value(_Com[i].x, _Com[i].y);
		caro.set_value(_Com[i].x, _Com[i].y, 0);
	}
	//đánh lần lượt các nước tốt nhất vừa chọn ra
	for (int i = 0; i < 3; i++)
	{
		point temp_Com;//lưu lại point được chọn để đánh thử của Com
		temp_Com = _Com[i];
		BanCo[temp_Com.x][temp_Com.y] = 2;
		//Nếu máy win
		if (Find_Win(Com))
		{
			max = 50000;
			BanCo[temp_Com.x][temp_Com.y] = 0;
			if (_dosau_hientai == 0)
			{
				iy = _Com[i].x;
				ix = _Com[i].y;
			}
			break;
		}
		_min(_dosau_hientai);
		BanCo[temp_Com.x][temp_Com.y] = 0;
		if (max<temp_com1)
		{
			if (_dosau_hientai == 0)
			{
				iy = _Com[i].x;
				ix = _Com[i].y;
			}
			max = temp_com1;
		}
	}
	temp_user1 = max;
}
void _min(int _dosau_hientai)
{
	int min = 50000;
	danhgia(NGUOI);
	point *_User;//lưu lại 3 vị trí của người được chọn trong giả lập
	int n;//số phần tử
	//n = caro.dem() ;
	_User = new point[3];
	//tìm 3 ô trống mà có giá trị được đánh giá là cao nhất
	for (int i = 0; i < 3; i++)
	{
		caro.min_value(_User[i].x, _User[i].y);
		_User[i]._danhgia = caro.get_value(_User[i].x, _User[i].y);
		caro.set_value(_User[i].x, _User[i].y, 0);
	}
	for (int i = 0; i < 3; i++)
	{
		point temp_user;//lưu lại point được chọn để đánh thử của User
		temp_user = _User[i];
		BanCo[temp_user.x][temp_user.y] = 1;
		//Nều người win
		if (Find_Win(User))
		{
			min = -50000;
			BanCo[temp_user.x][temp_user.y] = 0;
			break;
		}
		else
			if (_dosau_hientai < max_dosau)
			{
				_max(_dosau_hientai + 1);
			}
		BanCo[temp_user.x][temp_user.y] = 0;
		if (min > _User[i]._danhgia)
		{
			min = _User[i]._danhgia;
		}

	}
	temp_com1 = min;
}
int Click_chuot_trai(LPARAM lParam)
{
	ix = LOWORD(lParam);
	iy = HIWORD(lParam);
	ix = ix / 30;
	iy = iy / 30;
	if (BanCo[iy][ix] == 0)
	{
		_luot = MAY;
		BanCo[iy][ix] = 1;
		return 1;
	}
	return 0;
}

bool huongdi = false;
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc,mHdc;
	RECT rct,rct1;
	HBRUSH brush;
	GetClientRect(hWnd, &rct);
	HBITMAP bitmap1, oldBitmap;
	switch (message)
	{
	case WM_LBUTTONDOWN:
		if (_luot==NGUOI)
		{
			if (Click_chuot_trai(lParam) == 1 && pause == false)
			{//cập nhật giá trị Banco trước rồi xuống WM_PAINT mới vẽ
				InvalidateRect(hWnd, 0, FALSE);//Cập nhật mới khung hình>Lý do là khi nhấn chuột trái khung hình sẽ được cập nhật mới
				//vùng sẽ di tới msg WM_PAINT
			}
		}
		else
		{
			if (Find_Win(User) == 0 &&Find_Win(Com) == 0)
			{
				_max(0);
				BanCo[iy][ix] = 2;
				_luot = NGUOI;
			}
			InvalidateRect(hWnd, 0, FALSE);
		}
		break;
	case WM_COMMAND:
		switch (LOWORD(wParam))
		{
		case IDM_NEWGAME:
			NewGame();
			pause =false;
			Com_win = false;
			ix = 12;
			iy = 12;
			InvalidateRect(hWnd, NULL, FALSE);
			break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			if (MessageBox(hWnd, TEXT("Ban co muon thoat khong?"), TEXT("Exit"),MB_OKCANCEL)==IDOK)
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT://Mỗi lần đánh 1 ước cớ ,sẽ vẽ lại toàn bộ màn hình cộng với tất cả các nước cờ đã đi
		hdc = BeginPaint(hWnd, &ps);
		mHdc = CreateCompatibleDC(hdc);
		bitmap1 = CreateCompatibleBitmap(hdc, rct.right - rct.left, rct.bottom - rct.top);
		oldBitmap = (HBITMAP)SelectObject(mHdc, (HBITMAP)bitmap1);
		FillRect(mHdc, &rct, HBRUSH(RGB(255, 255, 255)));
		DrawBoard(mHdc);
		if (ix != -1 && iy != -1)
		{
			rct1.top = iy * 30 + 2;
			rct1.bottom = rct1.top + 28;
			rct1.left = ix * 30 + 2;
			rct1.right = rct1.left + 28;
			brush = CreateSolidBrush(RGB(254, 248, 134));
			FillRect(mHdc, &rct1, brush);
		}
		for (int i = 0; i < 30; i++)
		{
			for (int j = 0; j < 30; j++)
			{
				if (BanCo[i][j]==1)
				{
					DrawX(mHdc, i, j);
				}
				else
					if (BanCo[i][j] == 2)
				{
					DrawO(mHdc, i, j);
				}
			}
		}
		BitBlt(hdc, 0, 0, 1000, 1000, mHdc, 0, 0, SRCCOPY);
		if (Find_Win(User) == 1)
		{
			if (MessageBox(hWnd, TEXT("Ban da thang \nBan co muon choi man moi khong?"), TEXT("New game"), MB_OKCANCEL) == IDOK)
			{
				NewGame();
				pause = false;
				Com_win = false;
				ix=12;
				iy = 12;
				InvalidateRect(hWnd, NULL, FALSE);
			}
			else
			{
				pause = true;
			}
		}
		else
		{
			if (Find_Win(Com) == 1)
			{
				if (MessageBox(hWnd, TEXT("Ban da thua!\nBan co muon choi man moi khong?"), TEXT("New game"), MB_OKCANCEL) == IDOK)
				{
					ix = 12;
					iy = 12;
					NewGame();
					Com_win = false;
					pause = false;
					InvalidateRect(hWnd, NULL, FALSE);
				}
				else
				{
					pause = true;
				}
			}
			else
			{
				if (_luot == MAY)
				{
					SendMessage(hWnd, WM_LBUTTONDOWN, 0, MAKELPARAM(84, 138));
				}
			}
		}
		SelectObject(mHdc, oldBitmap);
		//Xóa memory DC
		DeleteObject(mHdc);
		DeleteObject(bitmap1);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;
	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}