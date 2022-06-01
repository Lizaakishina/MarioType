#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#define mapWidth 80
#define mapHeight 25


typedef struct SObject //структура, хранящая координаты персонажа
{
    float x, y;
    float width, height;
    float vertSpeed;
    BOOL IsFLly;
    char cType;
    float horizonSpeed;
} MarioObj;


char map[mapHeight][mapWidth + 1]; //создаем карту
MarioObj mario; //создание переменной персонажа "марио"


MarioObj* brick = NULL;
int brickLength;

MarioObj* enemy = NULL;
int enemyLength;

int level = 1;
int score;
int maxLvl;


void ClearMap()
{
    for (int i = 0; i < mapWidth; i++)
    {
        map[0][i] = ' ';
        for (int j = 1; j < mapHeight; j++)
        {
            sprintf(map[j], map[0]);
        }
    }
}


void ShowMap()
{
    for (int j = 0; j < mapHeight; j++)
    {
        printf("%s \n", map[j]);
    }
}


void SetObjectPosition(MarioObj* obj, float xPos, float yPos)//задание позиции персонажа
{
    (*obj).x = xPos;
    (*obj).y = yPos;
}


void InitObject(MarioObj* obj, float xPos, float yPos, float oWidth, float oHeight, char inType)//инициализация объекта целиком
{
    SetObjectPosition(obj, xPos, yPos);
    (*obj).width = oWidth;
    (*obj).height = oHeight;
    (*obj).vertSpeed = 0;
    (*obj).cType = inType;
    (*obj).horizonSpeed = 0.2f;
}


void CreateLevel(int lvl);


void PlayerDead()
{
    system("color 4F");
    Sleep(500);
    CreateLevel(level);
}


BOOL IsCollision(MarioObj o1, MarioObj o2);
MarioObj* GetNewEnemy();


void VertMoveObject(MarioObj* obj)
{
    (*obj).IsFLly = TRUE;
    (*obj).vertSpeed += 0.05f;
    SetObjectPosition(obj, (*obj).x, (*obj).y + (*obj).vertSpeed);//новая позиция для объекта с изменением координаты у

    for (int i = 0; i < brickLength; i++)
    {
        if (IsCollision(*obj, brick[i]))//если марио после перемещения столкнулся с объектом
        {
            if (obj[0].vertSpeed > 0)  obj[0].IsFLly = FALSE;//приземляемся

            if ((brick[i].cType == '?') && (obj[0].vertSpeed < 0) && (obj == &mario))//если марио столкнулся с кирпичиком при движении снизу вверх
            {
                brick[i].cType = '-';
                InitObject(GetNewEnemy(), brick[i].x + 5, brick[i].y - 2, 3, 2, '$');
                enemy[enemyLength - 1].vertSpeed = -0.7f;
            }

            (*obj).y -= (*obj).vertSpeed;//он вернется назад
            (*obj).vertSpeed = 0;//и прекратит движение

            if (brick[i].cType == '+')
            {
                level++;
                if (level > maxLvl) level = 1;

                system("color 2f");
                Sleep(500);
                CreateLevel(level);
            }
            break;
        }
    }
}


void DeleteEnemy(int i)
{
    enemyLength--;
    enemy[i] = enemy[enemyLength];
    enemy = (MarioObj*)realloc(enemy, sizeof(*enemy) * enemyLength);
}


void MarioCollision()//столкновения марио с движущимися объектами
{
    for (int i = 0; i < enemyLength; i++)
    {
        if (IsCollision(mario, enemy[i]))
        {
            if (enemy[i].cType == 'o')
            {
                if ((mario.IsFLly == TRUE)//приземляеся сверху если летим
                    && (mario.vertSpeed > 0)//при этом летим вниз
                    && (mario.y + mario.height < enemy[i].y + enemy[i].height * 0.5))//ноги марио должны находится выше половины роста врага
                {
                    score += 50;
                    DeleteEnemy(i);
                    i--;
                    continue;
                }
                else PlayerDead();
            }

            if (enemy[i].cType == '$')
            {
                score += 100;
                DeleteEnemy(i);
                i--;
                continue;
            }
        }
    }
}


void HorizonMoveObject(MarioObj* obj)
{
    obj[0].x += obj[0].horizonSpeed;

    for (int i = 0; i < brickLength; i++)
    {
        if (IsCollision((*obj), brick[i]))
        {
            obj[0].x -= obj[0].horizonSpeed;//отменяем текущее перемещение
            obj[0].horizonSpeed = -obj[0].horizonSpeed;//разворачиваемся в обратку
            return;
        }

        if (obj[0].cType == 'o')
        {
            MarioObj tmp = *obj;//копия объекта
            VertMoveObject(&tmp);//подействовали силой тяжести
            if (tmp.IsFLly == TRUE)//если объект оказался в полете
            {
                obj[0].x -= obj[0].horizonSpeed;
                obj[0].horizonSpeed = -obj[0].horizonSpeed;
            }
        }

        if (obj[0].cType == '$')
        {
            MarioObj tmp = *obj;
            VertMoveObject(&tmp);
            if (tmp.IsFLly == TRUE)
            {
                obj[0].x -= obj[0].horizonSpeed;
                obj[0].horizonSpeed = -obj[0].horizonSpeed;
            }
        }
    }
}


BOOL IsPosInMap(int x, int y)
{
    return((x >= 0) && (x < mapWidth) && (y >= 0) && (y < mapHeight));
}


void PutObjectOnMap(MarioObj obj)
{
    int ix = (int)round(obj.x);
    int iy = (int)round(obj.y);
    int iWidth = (int)round(obj.width);
    int iHeight = (int)round(obj.height);
    for (int i = ix; i < (ix + iWidth); i++)
        for (int j = iy; j < (iy + iHeight); j++)
        {
            if (IsPosInMap(i, j)) map[j][i] = obj.cType;
        }
}


void setCursor(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}


void HorisonMoveMap(float dx)
{
    mario.x -= dx;
    for (int i = 0; i < brickLength; i++)
        if (IsCollision(mario, brick[i]))
        {
            mario.x += dx;
            return;
        }
    mario.x += dx;
    for (int i = 0; i < brickLength; i++) brick[i].x += dx;
    for (int i = 0; i < enemyLength; i++) enemy[i].x += dx;
}


BOOL IsCollision(MarioObj o1, MarioObj o2)
{
    return ((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) &&
        ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o2.height));
}


MarioObj* GetNewBrick()
{
    brickLength++;
    brick = (MarioObj*)realloc(brick, sizeof(*brick) * brickLength);
    return brick + brickLength - 1;
}


MarioObj* GetNewEnemy()
{
    enemyLength++;
    enemy = (MarioObj*)realloc(enemy, sizeof(*enemy) * enemyLength);
    return enemy + enemyLength - 1;
}


void PutScoreOnMap()
{
    char c[30];
    sprintf(c, "Score: %d", score);
    int len = strlen(c);//длина строки
    for (int i = 0; i < len; i++)
    {
        map[1][i + 5] = c[i];
    }
}


void CreateLevel(int lvl)
{
    system("color 9f");//цвет консоли небесно голубого оттенка с примесью лазурного ночного неба

    brickLength = 0;
    brick = (MarioObj*)realloc(brick, 0);
    enemyLength = 0;
    enemy = (MarioObj*)realloc(enemy, 0);

    InitObject(&mario, 39, 10, 3, 3, '@');
    score = 0;

    if (lvl == 1)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
            InitObject(GetNewBrick(), 30, 10, 5, 3, '?');
        InitObject(GetNewBrick(), 60, 15, 40, 10, '#');
            InitObject(GetNewBrick(), 70, 5, 5, 3, '?');
            InitObject(GetNewBrick(), 90, 5, 5, 3, '?');
        InitObject(GetNewBrick(), 100, 20, 20, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 140, 20, 50, 5, '#');
            InitObject(GetNewBrick(), 170, 10, 5, 3, '?');
        InitObject(GetNewBrick(), 200, 15, 15, 10, '+');

        InitObject(GetNewEnemy(), 25, 10, 3, 2, 'o');
        InitObject(GetNewEnemy(), 80, 10, 3, 2, 'o');
        InitObject(GetNewEnemy(), 150, 10, 3, 2, 'o');
    }
    if (lvl == 2)
    {
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
            InitObject(GetNewBrick(), 30, 10, 5, 3, '?');
            InitObject(GetNewBrick(), 50, 10, 5, 3, '?');
        InitObject(GetNewBrick(), 60, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 80, 20, 20, 5, '#');
        InitObject(GetNewBrick(), 110, 15, 25, 10, '#');
            InitObject(GetNewBrick(), 115, 6, 5, 3, '?');
        InitObject(GetNewBrick(), 150, 20, 50, 5, '#');
            InitObject(GetNewBrick(), 170, 10, 5, 3, '?');
        InitObject(GetNewBrick(), 205, 15, 15, 10, '+');

        InitObject(GetNewEnemy(), 25, 15, 3, 2, 'o');
        InitObject(GetNewEnemy(), 80, 15, 3, 2, 'o');
        InitObject(GetNewEnemy(), 160, 10, 3, 2, 'o');
    }
    if (lvl == 3)
    {
        InitObject(GetNewBrick(), 20, 20, 50, 5, '#');
            InitObject(GetNewBrick(), 30, 10, 5, 3, '?');
        InitObject(GetNewBrick(), 80, 20, 30, 5, '#');
        InitObject(GetNewBrick(), 110, 17, 30, 10, '#');
        InitObject(GetNewBrick(), 120, 10, 10, 2, '#');
            InitObject(GetNewBrick(), 145, 5, 5, 3, '?');
        InitObject(GetNewBrick(), 140, 15, 20, 20, '#');
        InitObject(GetNewBrick(), 175, 10, 15, 15, '+');

        InitObject(GetNewEnemy(), 25, 15, 3, 2, 'o');
        InitObject(GetNewEnemy(), 80, 15, 3, 2, 'o');
        InitObject(GetNewEnemy(), 130, 10, 3, 2, 'o');
    }
    if (lvl == 4)
    {
        InitObject(GetNewBrick(), 20, 20, 50, 5, '#');
        InitObject(GetNewBrick(), 10, 15, 20, 2, '#');
        InitObject(GetNewBrick(), 50, 10, 50, 2, '#');
            InitObject(GetNewBrick(), 65, 3, 5, 3, '?');
            InitObject(GetNewBrick(), 90, 3, 5, 3, '?');
        InitObject(GetNewBrick(), 80, 20, 30, 5, '#');
        InitObject(GetNewBrick(), 110, 17, 30, 10, '#');
        InitObject(GetNewBrick(), 150, 20, 50, 5, '#');
            InitObject(GetNewBrick(), 170, 10, 5, 3, '?');
        InitObject(GetNewBrick(), 210, 15, 15, 10, '+');

        InitObject(GetNewEnemy(), 85, 15, 3, 2, 'o');
        InitObject(GetNewEnemy(), 160, 15, 3, 2, 'o');
    }
    maxLvl = 4;
}


int main()
{
    CreateLevel(level);
    do {
        ClearMap();

        if ((mario.IsFLly == FALSE) && (GetKeyState(VK_SPACE) < 0)) mario.vertSpeed = -1;
        if (GetKeyState('A') < 0) HorisonMoveMap(1);
        if (GetKeyState('D') < 0) HorisonMoveMap(-1);

        if (mario.y > mapHeight) PlayerDead();

        VertMoveObject(&mario);
        MarioCollision();

        for (int i = 0; i < brickLength; i++) PutObjectOnMap(brick[i]);
        for (int i = 0; i < enemyLength; i++)
        {
            VertMoveObject(enemy + i);
            HorizonMoveObject(enemy + i);
            if (enemy[i].y > mapHeight)
            {
                DeleteEnemy(i);
                i--;
                continue;
            }
            PutObjectOnMap(enemy[i]);
        }
        PutObjectOnMap(mario);
        PutScoreOnMap();

        setCursor(0, 0);
        ShowMap();

        Sleep(1);
    } while (GetKeyState(VK_ESCAPE) >= 0);
    return 0;
}