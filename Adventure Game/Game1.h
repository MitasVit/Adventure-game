#pragma once

#include "resource.h"

#include <d2d1.h>

//Creates simple DirectX 2D rect
//left = x, top = y, right = x+width, down = y+height
D2D1_RECT_F SRect(float left, float top, float right, float down) {
	return D2D1::RectF(left, top, right, down);
}

/*                    string st = "log" + to_string(x)+"x"+to_string(y) + "y.txt";
                    ofstream f(st.c_str());
                    f << x << endl << y << endl;
                    f.close();*/