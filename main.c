#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define mapWidth 80 //�������� ������ �����
#define mapHeight 25 //�������� ������ �����

typedef struct SObject { //���������, �������� ���������� ���������
    float x, y;
}MarioObj;

char map[mapHeight][mapWidth + 1]; //������� �����
MarioObj mario; //�������� ��������� ��������� "�����"

void ClearMap()// ������� �����
{
    for (int i = 0; i < mapWidth; i++) map[0][1] = '.';
    map[0][mapWidth] = '\0';
    for (int j = 1; j < mapHeight; j++) printf(map[j], map[0]);
}

void ShowMap()// ����� ����� �� �����
{
    map[mapHeight - 1][mapWidth - 1] = '\0';
    for (int j = 0; j < mapHeight; j++) printf("%s", map[j]);
}

void SetObjectPosition(MarioObj *obj, float xPos, float yPos)//������� ������� ���������
{
    *(obj)->x = xPos;//���������� ��������� ��� ������ �������� ������� ���������� ����
    *(obj)->y = yPos;
}

void PutObjectOnMap(MarioObj obj)
{
    int ix = (int)round(obj.x); //�����-������� ����������, �� ��� ����� � ���� ������������� �����==�������� � ������ ����
    int iy = (int)round(obj.y);
    map[iy][ix] = '@';
}

int main()
{
    SetObjectPosition(&mario, 20, 10); //������������� ���������
    ClearMap();
    PutObjectOnMap(mario);
    ShowMap();
    return 0;
}
