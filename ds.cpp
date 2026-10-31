#include <iostream>
using namespace std;

const int LEFT = 1, RIGHT = 2;
const int BOTTOM = 4, TOP = 8;

int xmin = 10, ymin = 10;
int xmax = 100, ymax = 100;

// Function to calculate region code
int getCode(double x, double y)
{
    int code = 0;

    if (x < xmin)
        code |= LEFT;

    if (x > xmax)
        code |= RIGHT;

    if (y < ymin)
        code |= BOTTOM;

    if (y > ymax)
        code |= TOP;

    return code;
}

int main()
{
    double x1 = 5, y1 = 50;
    double x2 = 120, y2 = 80;

    int c1 = getCode(x1, y1);
    int c2 = getCode(x2, y2);

    bool accept = false;

    while (true)
    {
        if ((c1 | c2) == 0)
        {
            accept = true;
            break;
        }
        else if (c1 & c2)
        {
            break;
        }
        else
        {
            int code = c1 ? c1 : c2;

            double x = 0, y = 0;

            if (code & TOP)
            {
                y = ymax;
                x = x1 + (x2 - x1) * (ymax - y1) / (y2 - y1);
            }
            else if (code & BOTTOM)
            {
                y = ymin;
                x = x1 + (x2 - x1) * (ymin - y1) / (y2 - y1);
            }
            else if (code & RIGHT)
            {
                x = xmax;
                y = y1 + (y2 - y1) * (xmax - x1) / (x2 - x1);
            }
            else if (code & LEFT)
            {
                x = xmin;
                y = y1 + (y2 - y1) * (xmin - x1) / (x2 - x1);
            }

            if (code == c1)
            {
                x1 = x;
                y1 = y;
                c1 = getCode(x1, y1);
            }
            else
            {
                x2 = x;
                y2 = y;
                c2 = getCode(x2, y2);
            }
        }
    }

    if (accept)
    {
        cout << "Line Accepted\n";

        cout << "Clipped Line: ("
             << x1 << "," << y1 << ") to ("
             << x2 << "," << y2 << ")";
    }
    else
    {
        cout << "Line Rejected";
    }

    return 0;
}