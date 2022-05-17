#pragma warning(disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <windows.h>
#define mapWidth 80 //значение ширины карты
#define mapHeight 25 //значение высоты карты


typedef struct SObject //структура, хранящая координаты персонажа
{
    float x, y;
    float width, height;
    float vertSpeed;
    BOOL IsFLly;
    char cType;//каким символом рисовать объект
    float horizonSpeed;
} MarioObj; //имя для использования в будующем


char map[mapHeight][mapWidth + 1]; //создаем карту
MarioObj mario; //создание переменной персонажа "марио"


MarioObj* brick = NULL;//динамический массив для создания блоков
int brickLength;


MarioObj* enemy = NULL;
int enemyLength;
int level = 1;
int score;
int maxLvl;


void ClearMap()// очистка карты
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


void PlayerDead()//смерть персонажа
{
    system("color 4F");//красный цвет
    Sleep(500);
    CreateLevel(level);
}


BOOL IsCollision(MarioObj o1, MarioObj o2);//описано ниже
MarioObj* GetNewEnemy();


void VertMoveObject(MarioObj* obj)
{
    (*obj).IsFLly = TRUE;
    (*obj).vertSpeed += 0.05f;//0.05 это ускорение
    SetObjectPosition(obj, (*obj).x, (*obj).y + (*obj).vertSpeed);//новая позиция для объекта с изменением координаты у

    for (int i = 0; i < brickLength; i++)
    {
        if (IsCollision(*obj, brick[i]))//если марио после перемещения столкнулся с объектом
        {
            if (obj[0].vertSpeed > 0)  obj[0].IsFLly = FALSE;//приземляемся

            if ((brick[i].cType == '?') && (obj[0].vertSpeed < 0) && (obj == &mario))//если марио столкнулся с кирпичиком при движении снизу вверх
            {
                brick[i].cType = '-';//меняем символ
                InitObject(GetNewEnemy(), brick[i].x + 5, brick[i].y - 2, 3, 2, '$');//спавним ходячий знак вопроса
                enemy[enemyLength - 1].vertSpeed = -0.7f;//задаем ему ускорение
            }

            (*obj).y -= (*obj).vertSpeed;//он вернется назад
            (*obj).vertSpeed = 0;//и прекратит движение

            if (brick[i].cType == '+')//если наступить на такой блок
            {
                level++;//счетчик уровня
                if (level > maxLvl) level = 1;//прошли четвертый - переходим обратно на первый

                system("color 2f");//цвет консоли
                Sleep(500);
                CreateLevel(level);//создаем нужный уровень
            }
            break;
        }
    }
}


void DeleteEnemy(int i)//удаление врага
{
    enemyLength--;
    enemy[i] = enemy[enemyLength];//помещаем последний элемент массива на место удаляемого
    enemy = (MarioObj*)realloc(enemy, sizeof(*enemy) * enemyLength);//усекаем массив на одну ячейку
}


void MarioCollision()//столкновения марио и движущимися объектами
{
    for (int i = 0; i < enemyLength; i++)
    {
        if (IsCollision(mario, enemy[i]))//проверяем на столкновение с врагом
        {
            if (enemy[i].cType == 'o')//в случае столкновения с врагом
            {
                if ((mario.IsFLly == TRUE)//приземляеся сверху если летим
                    && (mario.vertSpeed > 0)//при этом летим вниз
                    && (mario.y + mario.height < enemy[i].y + enemy[i].height * 0.5))//ноги марио должны находится выше половины роста врага
                {
                    score += 50;
                    DeleteEnemy(i);//уничтожаем этот объект
                    i--;
                    continue;
                }
                else PlayerDead();
            }

            if (enemy[i].cType == '$')//в случае столкновения с деньгами
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
    obj[0].x += obj[0].horizonSpeed;//горизонтальное перемещение объекта

    for (int i = 0; i < brickLength; i++)
    {
        if (IsCollision((*obj), brick[i]))//проверка столкновения объекта и блока
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
                obj[0].x -= obj[0].horizonSpeed;//отменяем текущее перемещение
                obj[0].horizonSpeed = -obj[0].horizonSpeed;//разворачиваемся в обратку
            }
        }

        if (obj[0].cType == '$')
        {
            MarioObj tmp = *obj;//копия объекта
            VertMoveObject(&tmp);//подействовали силой тяжести
            if (tmp.IsFLly == TRUE)//если объект оказался в полете
            {
                obj[0].x -= obj[0].horizonSpeed;//отменяем текущее перемещение
                obj[0].horizonSpeed = -obj[0].horizonSpeed;//разворачиваемся в обратку
            }
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
    for (int i = ix; i < (ix + iWidth); i++)//пробегаемся по карте от текущей позиции до тек.поз.+ размер объекта
        for (int j = iy; j < (iy + iHeight); j++)
        {
            if (IsPosInMap(i, j)) map[j][i] = obj.cType;//проверяем на нахождение объекта в пределах карты
        }
}


void setCursor(int x, int y)//установка курсора в определенном месте
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    //функция гет... возвращает адрес передаваемый входным параметром(стандратного устройства вывода)
    //сет... получает адрес окна кнсоли и задает позицию курсору
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
    mario.x += dx;//иначе просто возвращаем в начальную позицию
    for (int i = 0; i < brickLength; i++) brick[i].x += dx;//блоки перемещаются вместе с картой
    for (int i = 0; i < enemyLength; i++) enemy[i].x += dx;//враги перемещаются вместе с картой
}


BOOL IsCollision(MarioObj o1, MarioObj o2)//проверка на столкновение двух объектов
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
    int len = strlen(c);
    for (int i = 0; i < len; i++)
    {
        map[1][i + 5] = c[i];
    }
}


void CreateLevel(int lvl)//создание уровней
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
        ClearMap(); //очищаем карту

        if ((mario.IsFLly == FALSE) && (GetKeyState(VK_SPACE) < 0)) mario.vertSpeed = -1;//если стоим на поверхности и нажимаем пробел - прыгаем
        if (GetKeyState('A') < 0) HorisonMoveMap(1);
        if (GetKeyState('D') < 0) HorisonMoveMap(-1);

        if (mario.y > mapHeight) PlayerDead();//если марио уйдет в закат - уровень начнется заново

        VertMoveObject(&mario);
        MarioCollision();

        for (int i = 0; i < brickLength; i++) PutObjectOnMap(brick[i]);//проходимся по циклу, чтобы разместить все блоки
        for (int i = 0; i < enemyLength; i++)
        {
            VertMoveObject(enemy + i);//применяем к врагам силу тяжести
            HorizonMoveObject(enemy + i);//все враги двигаются относительно карты
            if (enemy[i].y > mapHeight)//уничтожить врага, если он окажется ниже экрана
            {
                DeleteEnemy(i);
                i--;
                continue;
            }
            PutObjectOnMap(enemy[i]);
        }
        PutObjectOnMap(mario); //помещаем персонажа на экран
        PutScoreOnMap();

        setCursor(0, 0);//установка положения курсора
        ShowMap();

        Sleep(1);//замедление программы для лучших FPS
    } while (GetKeyState(VK_ESCAPE) >= 0); //выход из цикла при нажатии ескейп
    return 0;
}