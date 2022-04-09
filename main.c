#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define mapWidth 80 //значение ширины карты
#define mapHeight 25 //значение высоты карты

typedef struct SObject { //структура, хранящая координаты персонажа
    float x, y;
}MarioObj;

char map[mapHeight][mapWidth + 1]; //создаем карту
MarioObj mario; //создание перемнной персонажа "марио"

void ClearMap()// очистка карты
{
    for (int i = 0; i < mapWidth; i++) map[0][1] = '.';
    map[0][mapWidth] = '\0';
    for (int j = 1; j < mapHeight; j++) printf(map[j], map[0]);
}

void ShowMap()// вывод карты на экран
{
    map[mapHeight - 1][mapWidth - 1] = '\0';
    for (int j = 0; j < mapHeight; j++) printf("%s", map[j]);
}

void SetObjectPosition(MarioObj *obj, float xPos, float yPos)//задание позиции персонажа
{
    *(obj)->x = xPos;//используем указатель для замены значения входной переменной обдж
    *(obj)->y = yPos;
}

void PutObjectOnMap(MarioObj obj)
{
    int ix = (int)round(obj.x); //раунд-функция округления, но она возвр в виде вещественного числа==приводим к целому типу
    int iy = (int)round(obj.y);
    map[iy][ix] = '@';
}

int main()
{
    SetObjectPosition(&mario, 20, 10); //инициализация персонажа
    ClearMap();
    PutObjectOnMap(mario);
    ShowMap();
    return 0;
}
