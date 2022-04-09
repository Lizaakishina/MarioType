#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#pragma warning(disable : 4996)
#define mapWidth 80 //значение ширины карты
#define mapHeight 25 //значение высоты карты

typedef struct SObject { //структура, хранящая координаты персонажа
    float x, y;
    float width, height;
    float vertSpeed;
    BOOL IsFLly;
    char cType;//каким символом рисовать объект
}MarioObj; //имя для использования в будующем


char map[mapHeight][mapWidth + 1]; //создаем карту
MarioObj mario; //создание переменной персонажа "марио"
MarioObj* brick = NULL;//динамический массив для создания блоков
int brickLength;
int level = 1;


void ClearMap()// очистка карты
{
    for (int i = 0; i < mapWidth; i++) map[0][i] = ' ';
    for (int j = 1; j < mapHeight; j++)
    {
        sprintf(map[j], map[0]);
    }
}


void ShowMap()// вывод карты на экран
{
    for (int j = 0; j < mapHeight; j++)
    {
        printf("%s \n", map[j]);
    }
}


void SetObjectPosition(MarioObj* obj, float xPos, float yPos)//задание позиции персонажа
{
    (*obj).x = xPos;//используем указатель для замены значения входной переменной обдж
    (*obj).y = yPos;
}


void InitObject(MarioObj* obj, float xPos, float yPos, float oWidth, float oHeight, char inType)//инициализация объекта
{
    SetObjectPosition(obj, xPos, yPos);
    (*obj).width = oWidth;
    (*obj).height = oHeight;
    (*obj).vertSpeed = 0;
    (*obj).cType = inType;
}


BOOL IsCollision(MarioObj o1, MarioObj o2);//описано ниже
void CreateLevel(int lvl);

void VertMoveObject(MarioObj* obj)
{
    (*obj).IsFLly = TRUE;
    (*obj).vertSpeed += 0.05;//005 это ускорение
    SetObjectPosition(obj, (*obj).x, (*obj).y + (*obj).vertSpeed);//новая позиция для объекта с изменением координаты у
    for (int i = 0; i < brickLength; i++)
    {
        if (IsCollision(*obj, brick[i]))//если марио после перемещения столкнулся с объектом
        {
            (*obj).y -= (*obj).vertSpeed;//он вернется назад
            (*obj).vertSpeed = 0;//и прекратит движение
            (*obj).IsFLly = FALSE;
            if (brick[i].cType == '+')//если наступить на такой блок
            {
                level++;//счетчик уровня
                if (level > 2)level = 1;//прошли второй - переходим обратно на первый
                CreateLevel(level);//создаем нужный уровень
                Sleep(1000);
            }
            break;
        }
    }
}


BOOL IsPosInMap(int x, int y)//проверка на нахождение марио в пределах карты
{
    return((x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight));
}


void PutObjectOnMap(MarioObj obj)
{
    int ix = (int)round(obj.x); //раунд-функция округления, но она возвр в виде вещественного числа==приводим к целому типу
    int iy = (int)round(obj.y);
    int iWidth = (int)round(obj.width);
    int iHeight = (int)round(obj.height);
    for (int i = ix; i < (ix + iWidth); i++)//пробегаемся по карте от текущей позиции до тек.поз.+размер объекта
        for (int j = iy; j < (iy + iHeight); j++)
        {
            if (IsPosInMap(i, j)) map[j][i] = obj.cType;
        }
}


void setCursor(int x, int y)//установка курсора в определенном месте
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);//функция гет... возвращает адрес передаваемый входным параметром(стандратного устройства вывода). сет... получает адрес окна кнсоли и задает позицию курсору
}


void HorisonMoveMap(float dx)//горизонтальное перемещение карты
{
    mario.x -= dx;//двигаем в противоположном направлении
    for (int i = 0; i < brickLength; i++)//проверяем на столкновение
        if (IsCollision(mario, brick[i]))//если столкнулся
        {
            mario.x += dx;//возвращаем в начальную позицию
            return;//и выходим
        }
    mario.x += dx;
    for (int i = 0; i < brickLength; i++) brick[i].x += dx;
}


BOOL IsCollision(MarioObj o1, MarioObj o2)//проверка на столкновение
{
    return((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) && ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o1.height));
}


void CreateLevel(int lvl)//создание уровня
{
    InitObject(&mario, 39, 10, 3, 3, '@');
    if (lvl == 1)
    {
        brickLength = 6;//6 блоков
        brick = (MarioObj*)realloc(brick, sizeof(*brick) * brickLength);
        InitObject(brick, 20, 20, 40, 5, '#');
        InitObject(brick + 1, 60, 15, 10, 10, '#');
        InitObject(brick + 2, 80, 20, 20, 5, '#');
        InitObject(brick + 3, 120, 15, 10, 10, '#');
        InitObject(brick + 4, 150, 20, 40, 5, '#');
        InitObject(brick + 5, 210, 15, 10, 10, '+');
    }
    if (lvl == 2)
    {
        brickLength = 4;//4 блока
        brick = (MarioObj*)realloc(brick, sizeof(*brick) * brickLength);
        InitObject(brick, 20, 20, 40, 5, '#');
        InitObject(brick + 1, 80, 20, 15, 5, '#');
        InitObject(brick + 2, 120, 15, 15, 10, '#');
        InitObject(brick + 3, 160, 10, 15, 15, '+');
    }
}


int main()
{
    system("color 9f");//цвет консоли
    CreateLevel(level);
    do {
        ClearMap(); //очищаем карту

        if ((mario.IsFLly == FALSE) && (GetKeyState(VK_SPACE) < 0)) mario.vertSpeed = -1;
        if (GetKeyState('A') < 0) HorisonMoveMap(1);
        if (GetKeyState('D') < 0) HorisonMoveMap(-1);

        if (mario.y > mapHeight) CreateLevel(level);//если марио уйдет в закат-уровень начнется заново

        VertMoveObject(&mario);
        for (int i = 0; i < brickLength; i++) PutObjectOnMap(brick[i]);//проходимся по циклу чтобы разместить все блоки
        PutObjectOnMap(mario); //помещаем персонажа на экран

        setCursor(0, 0);
        ShowMap();

        Sleep(7);
    } while (GetKeyState(VK_ESCAPE) >= 0); //выход из цикла при нажатии ескейп
    return 0;
}