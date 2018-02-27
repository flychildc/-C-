#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

int length = 1;
int fx, fy;

//˫�����������������
HANDLE houtput, houtbuffer;//�������������һ����Ĭ�ϵ���ʾ������������һ�����½������������
COORD coord = { 0,0 };//coord��һ���ṹ�壬��2������x,y
CONSOLE_CURSOR_INFO crr;//���ÿ���̨�����Ϣ
DWORD bytes = 0;//������ʾ�ӻ�����ʵ�ʶ�����ʵ��д�뻺�������ֽ���
char temp[3600];//�����洢�ӻ������������ַ�
//��̷�ʽʵ�������Ļ
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

//��ʾ
void show(int (*map)[30],int (*snake)[3]) 
{
	int column, line;

	//��snake�е�ֵ����map��
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
				printf("�~");
			}
			else
				printf("  ");//����ʾ���Ш~ռ��λ����������Ҫ���2���ո�
		}
		printf("\n");
	}
	//˫�����������������
	//�����~ռ�����ֽڣ�һ��900����ζ����Ҫ1800����������3600������Ϊ��������Ҫ�洢���˨~����Ŀո��Ա�֤����̨ÿһ
	//���ܹ��������ﵽ���е�Ч��������̨ÿ��120���ո񣬶�Ӧ�������������120���ֽڣ�һ��30�У����Ի�������Ҫ3600���ֽ�
	ReadConsoleOutputCharacter(houtput,temp,3600,coord,&bytes);
	WriteConsoleOutputCharacter(houtbuffer, temp, 3600, coord, &bytes);

	//����snake��map��Ӱ�죬�����´δ�ӡ����һ��snake���㼣��ӡ����
	for (int i = 0; i < length; i++)
	{
		column = snake[899 - i][1];
		line = snake[899 - i][2];
		map[line][column] = 0;
	}

}

//����ʳ��
void food(int map[][30],int snake[][3]) 
{
	int column, line;
	int flag = 1;
	srand((unsigned)time(NULL));//��srand()�ṩһ�����ӣ�����һ��unsigned int���ͣ���ȡֵ��Χ��0~65535
	
	while (1)
	{
		//��X��Y����Y��X��1����������Ҫ������X��Y������ֻ��Ҫ����д�� k=rand()%(Y-X+1)+X. ������1~28
		column = rand() % 28 + 1;//����rand()����������ṩ��srand()������ֵ����һ�������(��0��32767֮��)
		line = rand() % 28 + 1;

		//������ɵ�ʳ���Ƿ�����غ�
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

//���ƶ�
int move(int snake[][3],char direct,int map[][30])
{
	//ʳ������fx, fy����ͷhead��ʳ�����ͷ��X��Y����Ĳ��dx��dy
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

	//ʳ�����ͷ��X��Y����Ĳ��
	dx = fx - snake[head][1];
	dy = fy - snake[head][2];

	//�����ƶ�
	if (direct == 'w' || direct == 's')
	{
		//���ж��Ƿ�Ե�ʳ��
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
				//���������������
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

	//�����ƶ�
	if (direct == 'a' || direct == 'd')
	{
		//���ж��Ƿ�Ե�ʳ��
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

//��ײǽ��ײ���Լ���Ϸ����
int gameover(int map[][30],int snake[][3]) 
{
	int gameflag = 0;

	if (snake[900 - length][1] <= 0 || snake[900 - length][1] >= 29 ||
		                  snake[900 - length][2] <= 0 || snake[900 - length][2] >= 29)
	{
		//printf("��Ϸ���� \n");
		gameflag = 1;
	}

	//ײ���Լ�
	for (int i = 0; i < length-1; i++)
	{
		if (snake[900 - length][1] == snake[899 - i][1] && 
			         snake[900 - length][2] == snake[899 - i][2])
		{
			//printf("��Ϸ���� \n");
			gameflag = 1;
		}
	}
	return gameflag;
}

int main() 
{
	//map������ʾ��ͼ��0��ʾ�ո�1��ʾ�����߽磬2��ʾʳ��
	//snake��������������ÿһ��3��Ԫ�أ���1��Ԫ��Ϊ1��ʾ��ͷ����������Ԫ�طֱ��ʾ����x��y
	int map[30][30] = { 0 };
	int snake[900][3] = { 0 };
	char ch1 = 'a',ch2;
	int Gflag = 0,Eflag = 0;

	//printf("%d", sizeof("�~"));//���3���~ռ�����ֽ�

	//����̰���߽߱�,����Ϊ1��ʾ��ʾ�~����Ϊ0��ʾ�ո�
	for (int index = 0; index < 30; index++)
	{
		map[0][index] = 1;
		map[index][0] = 1;
		map[29][index] = 1;
		map[index][29] = 1;
	}

	//�߳�ʼ������Ϊ14*14���˶�����Ϊ����
	snake[899][0] = 1;
	snake[899][1] = 14;
	snake[899][2] = 14;


	//˫�����������������
	//HANDLE houtput, houtbuffer;//�������������һ����Ĭ�ϵ���ʾ������������һ�����½������������
	//COORD coord = { 0,0 };//coord��һ���ṹ�壬��2������x,y
	houtput = GetStdHandle(STD_OUTPUT_HANDLE);//��ȡ��׼�����������Ĭ�ϵ���ʾ���������ľ����ֻ����ʾ�������ܱ���ӡ����Ļ��
	houtbuffer = CreateConsoleScreenBuffer(GENERIC_WRITE | GENERIC_READ,       //�����µ��������������һ��������ʾ������
		                                  FILE_SHARE_READ | FILE_SHARE_WRITE,   //�ɶ�д
		                                  NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(houtbuffer);//�����´����Ļ�����Ϊ�����������ζ����黺��������Ϊ��ʾ������
	//CONSOLE_CURSOR_INFO crr;//���ÿ���̨�����Ϣ
	crr.bVisible = FALSE;//���ؿ���̨���
	crr.dwSize = 1;//����С��ȡֵ��Χ1~100
	SetConsoleCursorInfo(houtput, &crr);
	SetConsoleCursorInfo(houtbuffer,&crr);
	//DWORD bytes = 0;//������ʾ�ӻ�����ʵ�ʶ�����ʵ��д�뻺�������ֽ���
	//char temp[3600];//�����洢�ӻ������������ַ�


	food(map,snake);

	while (1) 
	{
		//����������
		//���̶����¼�������Ҫ�õ�һ������_kbhit(); ����м����û�����������ͻ᷵��һ����0����
		if (_kbhit())
		{
			ch2 = _getch();//getch()������������س��������Ϳ��Ա�ʾ��������Զ�ִ����һ�����
			if (ch2 == 'w' || ch2 == 's' || ch2 == 'a' || ch2 == 'd')
			{
				ch1 = ch2;
			}
		}

		show(map,snake);

		//���ײ���߽�����Լ���Ϸ����
		if (Gflag == 1)
		{
			cls();
			
			printf("��Ϸ���� \n");
			//˫�����������������
			ReadConsoleOutputCharacter(houtput, temp, 8, coord, &bytes);//��ʱ���Է���bytesΪ4��˵��ʵ�ʶ�����4���ֽڣ�������Ҫ��8���ֽ�
			coord.X = 0;//X�ǰ�����̨�ո��������
			coord.Y = 30;//ע��Y�ǰ����������
			WriteConsoleOutputCharacter(houtbuffer, temp, 4, coord, &bytes);//�����4Ҫ�������bytes��Ӧ����Ȼ����ӡ
			break;
		}

		//��ʼ�����ƶ�
		Eflag = move(snake, ch1, map);
		//���ʳ�ﱻ���ˣ����²���ʳ��
		if (Eflag == 1)
		{
			food(map,snake);
		}

		Gflag = gameover(map, snake);

		Sleep(200);
		//system("cls");//�����Ļ�������´�ӡ
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
//		printf("��");
//	}
//	SetConsoleCursorPosition(houtput, coor);
//	printf("��");
//
//	system("pause");
//}