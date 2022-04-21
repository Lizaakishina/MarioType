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
    float horizonSpeed;
}MarioObj; //имя для использования в будующем


char map[mapHeight][mapWidth + 1]; //создаем карту
MarioObj mario; //создание переменной персонажа "марио"
MarioObj* brick = NULL;//динамический массив для создания блоков
int brickLength;
MarioObj* enemy = NULL;//динамический массив для создания врагов
int enemyLenght;
int level = 1;


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


void InitObject(MarioObj* obj, float xPos, float yPos, float oWidth, float oHeight, char inType)//инициализация объекта
{
    SetObjectPosition(obj, xPos, yPos);
    (*obj).width = oWidth;
    (*obj).height = oHeight;
    (*obj).vertSpeed = 0;
    (*obj).cType = inType;
    (*obj).horizonSpeed = 0.3;
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
                if (level > 3)level = 1;//прошли третий - переходим обратно на первый
                CreateLevel(level);//создаем нужный уровень
                Sleep(1000);
            }
            break;
        }
    }
}

void DeleteEnemy(int i)//убийство врага
{
    enemyLenght--;
    enemy[i] = enemy[enemyLenght];
    enemy = (MarioObj*)realloc(enemy, sizeof(*enemy) * enemyLenght);
}
void MarioCollision()//столкновение с врагом
{
    for (int i = 0; i < enemyLenght; i++)
        if (IsCollision(mario, enemy[i]))
        {
            if ((mario.IsFLly == TRUE) && (mario.vertSpeed > 0) && (mario.y + mario.height < enemy[i].y + enemy[i].height * 0.5))
            {
                DeleteEnemy(i);
                i--;
                continue;
            }
            else
                CreateLevel(level);
        }
}
void HorizonMoveObject(MarioObj* obj) //горизонтальное перемещение объектов
{
    obj[0].x += obj[0].horizonSpeed;

    for (int i = 0; i < brickLength; i++)
        if (IsCollision(obj[0], brick[i]))//проверка на столкновение
        {
            obj[0].x -= obj[0].horizonSpeed;
            obj[0].horizonSpeed = -obj[0].horizonSpeed;
            return;
        }
    MarioObj tmp = *obj;
    VertMoveObject(&tmp);
    if (tmp.IsFLly == TRUE)
    {
        obj[0].x -= obj[0].horizonSpeed;
        obj[0].horizonSpeed = -obj[0].horizonSpeed;

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
    for (int i = 0; i < enemyLenght; i++) enemy[i].x += dx; //перемещение врагов вместе с картой
}


BOOL IsCollision(MarioObj o1, MarioObj o2)//проверка на столкновение
{
    return((o1.x + o1.width) > o2.x) && (o1.x < (o2.x + o2.width)) && ((o1.y + o1.height) > o2.y) && (o1.y < (o2.y + o1.height));
}

MarioObj* GetNewBrick()//функция для создания блоков
{
    brickLength++;
    brick = (MarioObj*)realloc(brick, sizeof(*brick) * brickLength);
    return brick + brickLength - 1;
}
MarioObj* GetNewEnemy()//функция для создания врагов
{
    enemyLenght++;
    enemy = (MarioObj*)realloc(enemy, sizeof(*enemy) * enemyLenght);
    return enemy + enemyLenght - 1;
}

void CreateLevel(int lvl)//создание уровня
{
    InitObject(&mario, 39, 10, 3, 3, '@');
    if (lvl == 1)
    {
        brickLength = 0;
        brick = (MarioObj*)realloc(brick, sizeof(*brick) * brickLength);
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 60, 15, 40, 10, '#');
        InitObject(GetNewBrick(), 100, 20, 20, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 150, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 210, 15, 10, 10, '+');
    }
    if (lvl == 2)
    {
        brickLength = 0;
        brick = (MarioObj*)realloc(brick, sizeof(*brick) * brickLength);
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 60, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 80, 20, 20, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 10, 10, '#');
        InitObject(GetNewBrick(), 150, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 210, 15, 10, 10, '+');
        enemyLenght = 0;
        enemy = (MarioObj*)realloc(enemy, sizeof(*enemy) * enemyLenght);
        InitObject(GetNewEnemy(), 25, 10, 3, 2, 'o');
        InitObject(GetNewEnemy(), 65, 10, 3, 2, 'o');
        InitObject(GetNewEnemy(), 120, 10, 3, 2, 'o');
        InitObject(GetNewEnemy(), 160, 10, 3, 2, 'o');
        InitObject(GetNewEnemy(), 175, 10, 3, 2, 'o');
    }
    if (lvl == 3)
    {
        brickLength = 0;//4 блока
        brick = (MarioObj*)realloc(brick, sizeof(*brick) * brickLength);
        InitObject(GetNewBrick(), 20, 20, 40, 5, '#');
        InitObject(GetNewBrick(), 80, 20, 15, 5, '#');
        InitObject(GetNewBrick(), 120, 15, 15, 10, '#');
        InitObject(GetNewBrick(), 160, 10, 15, 15, '+');
        enemyLenght = 0;
        enemy = (MarioObj*)realloc(enemy, sizeof(*enemy) * enemyLenght);
        InitObject(GetNewEnemy(), 25, 10, 3, 2, 'o');
        InitObject(GetNewEnemy(), 130, 10, 3, 2, 'o');
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
        MarioCollision();
        for (int i = 0; i < brickLength; i++) PutObjectOnMap(brick[i]);//проходимся по циклу чтобы разместить все блоки
        for (int i = 0; i < enemyLenght; i++)
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
        }//проходимся по циклу чтобы разместить врагов&убивать врагов
        PutObjectOnMap(mario); //помещаем персонажа на экран

        setCursor(0, 0);
        ShowMap();

        Sleep(2);
    } while (GetKeyState(VK_ESCAPE) >= 0); //выход из цикла при нажатии ескейп
    return 0;
}