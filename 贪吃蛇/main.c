#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

int length = 1;
int fx, fy;

//双缓冲区解决闪屏问题
HANDLE houtput, houtbuffer;//创建两个句柄，一个是默认的显示缓冲区，还有一个是新建的输出缓冲区
COORD coord = { 0,0 };//coord是一个结构体，有2个变量x,y
CONSOLE_CURSOR_INFO crr;//设置控制台光标信息
DWORD bytes = 0;//用来表示从缓冲区实际读出或实际写入缓冲区的字节数
char temp[3600];//用来存储从缓冲区读出的字符
//编程方式实现清除屏幕
int cls() 
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	COORD coordScreen = { 0, 0 };    /* here's where we'll home the cursor */
	DWORD cCharsWritten;
	CONSOLE_SCREEN_BUFFER_INFO csbi; /* to get buffer info */
	DWORD dwConSize;                 /* number of character cells in the current buffer */

									 /* get the number of character cells in the current buffer */
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return 0;
	dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

	/* fill the entire screen with blanks */
	if (!FillConsoleOutputCharacter(hConsole, (TCHAR) ' ', dwConSize, coordScreen, &cCharsWritten))
		return 0;

	/* get the current text attribute */
	if (!GetConsoleScreenBufferInfo(hConsole, &csbi))
		return 0;

	/* now set the buffer's attributes accordingly */
	if (!FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten))
		return 0;

	/* put the cursor at (0, 0) */
	if (!SetConsoleCursorPosition(hConsole, coordScreen))
		return 0;
}

//显示
void show(int (*map)[30],int (*snake)[3]) 
{
	int column, line;

	//把snake中的值换到map中
	for (int i = 0; i < length; i++)
	{
		column = snake[899 - i][1];
		line = snake[899 - i][2];
		map[line][column] = 1;
	}

	for (int i = 0; i < 30; i++)
	{
		for (int j = 0; j < 30; j++)
		{
			if (map[i][j] == 1 || map[i][j] == 2)
			{
				printf("▇");
			}
			else
				printf("  ");//再显示屏中▇占两位，所以这里要输出2个空格
		}
		printf("\n");
	}
	//双缓冲区解决闪屏问题
	//本来▇占两个字节，一共900个意味着需要1800个，这里用3600个是因为缓冲区还要存储除了▇以外的空格，以保证控制台每一
	//行能够被填满达到换行的效果。控制台每行120个空格，对应输出缓冲区就是120个字节，一共30行，所以缓冲区需要3600个字节
	ReadConsoleOutputCharacter(houtput,temp,3600,coord,&bytes);
	WriteConsoleOutputCharacter(houtbuffer, temp, 3600, coord, &bytes);

	//消除snake对map的影响，避免下次打印把上一次snake的足迹打印出来
	for (int i = 0; i < length; i++)
	{
		column = snake[899 - i][1];
		line = snake[899 - i][2];
		map[line][column] = 0;
	}

}

//产生食物
void food(int map[][30],int snake[][3]) 
{
	int column, line;
	int flag = 1;
	srand((unsigned)time(NULL));//给srand()提供一个种子，它是一个unsigned int类型，其取值范围从0~65535
	
	while (1)
	{
		//从X到Y，有Y－X＋1个数，所以要产生从X到Y的数，只需要这样写： k=rand()%(Y-X+1)+X. 这里是1~28
		column = rand() % 28 + 1;//调用rand()，它会根据提供给srand()的种子值返回一个随机数(在0到32767之间)
		line = rand() % 28 + 1;

		//检查生成的食物是否和蛇重合
		for (int i = 0; i < length; i++)
		{
			if (snake[899 - i][1] == column && snake[899 - i][2] == line)
			{
				flag = 0;
				break;
			}
		}

		if (flag == 1)
		{
			map[line][column] = 2;
			fx = column;
			fy = line;
			return;
		}
	}
}

//蛇移动
int move(int snake[][3],char direct,int map[][30])
{
	//食物坐标fx, fy；蛇头head，食物和蛇头再X和Y方向的差距dx、dy
	int head,dx,dy;
	int eatflag = 0;

	for (int i = 0; i < 900; i++) 
	{
		if (snake[i][0] == 1)
		{
			head = i;
			break;
		}
	}

	//食物和蛇头再X和Y方向的差距
	dx = fx - snake[head][1];
	dy = fy - snake[head][2];

	//上下移动
	if (direct == 'w' || direct == 's')
	{
		//先判断是否吃到食物
		if ((dy == 1 || dy == -1) && snake[head][1] == fx)
		{
			snake[head][0] = 0;
			head = head - 1;
			snake[head][0] = 1;
			snake[head][1] = fx;
			snake[head][2] = fy;
			length++;
			eatflag = 1;
		}
		else 
		{
			if (direct == 'w')
			{
				//从蛇自身情况考虑
				for (int i = 899; i > head; i--)
				{
					snake[i][1] = snake[i - 1][1];
					snake[i][2] = snake[i - 1][2];
				}
				snake[head][2] -= 1;
			}
			else 
			{
				for (int i = 899; i > head; i--)
				{
					snake[i][1] = snake[i - 1][1];
					snake[i][2] = snake[i - 1][2];
				}
				snake[head][2] += 1;
			}
		}
	}

	//左右移动
	if (direct == 'a' || direct == 'd')
	{
		//先判断是否吃到食物
		if ((dx == 1 || dx == -1) && snake[head][2] == fy)
		{
			snake[head][0] = 0;
			head = head - 1;
			snake[head][0] = 1;
			snake[head][1] = fx;
			snake[head][2] = fy;
			length++;
			eatflag = 1;
		}
		else
		{
			if (direct == 'a')
			{
				for (int i = 899; i > head; i--)
				{
					snake[i][1] = snake[i - 1][1];
					snake[i][2] = snake[i - 1][2];
				}
				snake[head][1] -= 1;
				//printf("snake[head][1]=%d \n", snake[head][1]);
			}
			else
			{
				for (int i = 899; i > head; i--)
				{
					snake[i][1] = snake[i - 1][1];
					snake[i][2] = snake[i - 1][2];
				}
				snake[head][1] += 1;
			}
		}
	}

	return eatflag;
}

//蛇撞墙或撞到自己游戏结束
int gameover(int map[][30],int snake[][3]) 
{
	int gameflag = 0;

	if (snake[900 - length][1] <= 0 || snake[900 - length][1] >= 29 ||
		                  snake[900 - length][2] <= 0 || snake[900 - length][2] >= 29)
	{
		//printf("游戏结束 \n");
		gameflag = 1;
	}

	//撞到自己
	for (int i = 0; i < length-1; i++)
	{
		if (snake[900 - length][1] == snake[899 - i][1] && 
			         snake[900 - length][2] == snake[899 - i][2])
		{
			//printf("游戏结束 \n");
			gameflag = 1;
		}
	}
	return gameflag;
}

int main() 
{
	//map用来显示地图，0表示空格，1表示蛇身或边界，2表示食物
	//snake数组用来描述神，每一行3个元素，第1个元素为1表示蛇头，后面两个元素分别表示坐标x、y
	int map[30][30] = { 0 };
	int snake[900][3] = { 0 };
	char ch1 = 'a',ch2;
	int Gflag = 0,Eflag = 0;

	//printf("%d", sizeof("▇"));//输出3，▇占两个字节

	//绘制贪吃蛇边界,数组为1表示显示▇数组为0显示空格
	for (int index = 0; index < 30; index++)
	{
		map[0][index] = 1;
		map[index][0] = 1;
		map[29][index] = 1;
		map[index][29] = 1;
	}

	//蛇初始再坐标为14*14，运动方向为向左
	snake[899][0] = 1;
	snake[899][1] = 14;
	snake[899][2] = 14;


	//双缓冲区解决闪屏问题
	//HANDLE houtput, houtbuffer;//创建两个句柄，一个是默认的显示缓冲区，还有一个是新建的输出缓冲区
	//COORD coord = { 0,0 };//coord是一个结构体，有2个变量x,y
	houtput = GetStdHandle(STD_OUTPUT_HANDLE);//获取标准输出缓冲区（默认的显示缓冲区）的句柄，只有显示缓冲区能被打印到屏幕中
	houtbuffer = CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_READ,       //创建新的输出缓冲区，第一个参数表示缓冲区
		                                  FILE_SHARE_READ | FILE_SHARE_WRITE,   //可读写
		                                  NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(houtbuffer);//设置新创建的缓冲区为活动缓冲区，意味着这块缓冲区会作为显示缓冲区
	//CONSOLE_CURSOR_INFO crr;//设置控制台光标信息
	crr.bVisible = FALSE;//隐藏控制台光标
	crr.dwSize = 1;//光标大小，取值范围1~100
	SetConsoleCursorInfo(houtput, &crr);
	SetConsoleCursorInfo(houtbuffer,&crr);
	//DWORD bytes = 0;//用来表示从缓冲区实际读出或实际写入缓冲区的字节数
	//char temp[3600];//用来存储从缓冲区读出的字符


	food(map,snake);

	while (1) 
	{
		//检测键盘输入
		//键盘读入事件我们需要用到一个函数_kbhit(); 如果有键盘敲击，这个函数就会返回一个非0的数
		if (_kbhit())
		{
			ch2 = _getch();//getch()函数不用输入回车结束符就可以表示输入结束自动执行下一条语句
			if (ch2 == 'w' || ch2 == 's' || ch2 == 'a' || ch2 == 'd')
			{
				ch1 = ch2;
			}
		}

		show(map,snake);

		//如果撞到边界或者自己游戏结束
		if (Gflag == 1)
		{
			cls();
			
			printf("游戏结束 \n");
			//双缓冲区解决闪屏问题
			ReadConsoleOutputCharacter(houtput, temp, 8, coord, &bytes);//此时调试发现bytes为4，说明实际读入了4个字节，但明显要读8个字节
			coord.X = 0;//X是按控制台空格数计算的
			coord.Y = 30;//注意Y是按行数计算的
			WriteConsoleOutputCharacter(houtbuffer, temp, 4, coord, &bytes);//这里的4要与上面的bytes对应，不然会多打印
			break;
		}

		//初始向左移动
		Eflag = move(snake, ch1, map);
		//如果食物被吃了，重新产生食物
		if (Eflag == 1)
		{
			food(map,snake);
		}

		Gflag = gameover(map, snake);

		Sleep(200);
		//system("cls");//清除屏幕内容重新打印
		cls();
	}

	system("pause");
	return 0;
}

//void main() 
//{
//	HANDLE houtput = GetStdHandle(STD_OUTPUT_HANDLE);
//	int i = 60;
//	COORD coor = {2,2};
//	while(i--)
//	{
//		printf("我");
//	}
//	SetConsoleCursorPosition(houtput, coor);
//	printf("我");
//
//	system("pause");
//}