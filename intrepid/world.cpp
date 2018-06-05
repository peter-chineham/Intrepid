#include "world.h"

QPoint World::viewport[] = {QPoint(-108000,  108000), QPoint( 108000, -108000)};

QPoint World::ConvertWorldtoScreen(int x, int y, int ScreenWidth, int ScreenHeight)
{
    //cout << "Inputs: " << x << " " << y << " " << ScreenWidth << " " << ScreenHeight << endl;
    QPoint result;
    result.setX(-1);
    result.setY(-1);
    int vx0 = viewport[0].x();
    int vy0 = viewport[0].y();
    int vx1 = viewport[1].x();
    int vy1 = viewport[1].y();

    if ((vx0 != vx1) && (vy0 != vy1)) {
        result.setX(round((float) (x - vx0) * (float) (ScreenWidth / (float) (vx1 - vx0))));
        result.setY(round((float) (vy0 - y) * (float) (ScreenHeight / (float) (vy0 - vy1))));
    }
    return result;
}

QPoint World::ConvertScreentoWorld(int ScreenX, int ScreenY, int ScreenWidth, int ScreenHeight)
{
    QPoint result;
    int vx0 = viewport[0].x();
    int vy0 = viewport[0].y();
    int vx1 = viewport[1].x();
    int vy1 = viewport[1].y();

    result.setX(-1);
    result.setY(-1);
    if (vx0 == vx1) {
        return result;
    }
    if (vy0 == vy1) {
        return result;
    }
    result.setX(round(((float)    ScreenX / ((float) ScreenWidth  / ((float) vx1 - vx0))) + (float) vx0));
    result.setY(round( (float) -((ScreenY / ((float) ScreenHeight / ((float) vy0 - vy1))) - (float) vy0)));
    return result;
}
