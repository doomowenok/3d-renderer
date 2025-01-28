#include "triangle.h"

void int_swap(int* a, int* b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{

}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    if(y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if(y1 > y2)
    {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }

    if(y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    int m_y = y1;
    int m_x = (int)(((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0);

    fill_flat_bottom_triangle(x0, y0, x1, y1, m_x, m_y, color);
    fill_flat_top_triangle(x1, y1, m_x, m_y, x2, y2, color);
}