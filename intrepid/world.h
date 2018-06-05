#ifndef WORLD_H
#define WORLD_H

#include <QPainter>
#include <math.h>

using namespace std;

class World
{
public:
    static QPoint viewport[2];
    static QPoint ConvertWorldtoScreen(int x, int y, int ScreenWidth, int ScreenHeight);
    static QPoint ConvertScreentoWorld(int ScreenX, int ScreenY, int ScreenWidth, int ScreenHeight);
};

#endif // WORLD_H
