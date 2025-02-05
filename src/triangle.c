#include "triangle.h"
#include "display.h"
#include "swap.h"

void fill_flat_bottom_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    float inv_slope_1 = (float)(x1 - x0) / (float)(y1 - y0);
    float inv_slope_2 = (float)(x2 - x0) / (float)(y2 - y0);

    float x_start = x0;
    float x_end = x0;

    for (int y = y0; y <= y2; y++)
    {
        draw_line(x_start, y, x_end, y, color);
        x_start += inv_slope_1;
        x_end += inv_slope_2;
    }
}

void fill_flat_top_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    float inv_slope_1 = (float)(x2 - x0) / (float)(y2 - y0);
    float inv_slope_2 = (float)(x2 - x1) / (float)(y2 - y1);

    float x_start = x2;
    float x_end = x2;

    for (int y = y2; y >= y0; y--)
    {
        draw_line(x_start, y, x_end, y, color);
        x_start -= inv_slope_1;
        x_end -= inv_slope_2;
    }
}

void draw_triangle(const int x0, const int y0, const int x1, const int y1, const int x2, const int y2, const uint32_t color)
{
    draw_line(x0, y0, x1, y1, color);
    draw_line(x1, y1, x2, y2, color);
    draw_line(x2, y2, x0, y0, color);
}

void draw_filled_triangle(int x0, int y0, int x1, int y1, int x2, int y2, uint32_t color)
{
    if (y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 > y2)
    {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
    }

    if (y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
    }

    if (y1 == y2)
    {
        fill_flat_bottom_triangle(x0, y0, x1, y1, x2, y2, color);
    }
    else if (y0 == y1)
    {
        fill_flat_top_triangle(x0, y0, x1, y1, x2, y2, color);
    }
    else
    {
        int m_y = y1;
        int m_x = (int)(((float)((x2 - x0) * (y1 - y0)) / (float)(y2 - y0)) + x0);

        fill_flat_bottom_triangle(x0, y0, x1, y1, m_x, m_y, color);
        fill_flat_top_triangle(x1, y1, m_x, m_y, x2, y2, color);
    }
}

void draw_textured_triangle(
    int x0, int y0, float u0, float v0,
    int x1, int y1, float u1, float v1,
    int x2, int y2, float u2, float v2,
    uint32_t *texture)
{
    if (y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    if (y1 > y2)
    {
        int_swap(&y1, &y2);
        int_swap(&x1, &x2);
        float_swap(&u1, &u2);
        float_swap(&v1, &v2);
    }

    if (y0 > y1)
    {
        int_swap(&y0, &y1);
        int_swap(&x0, &x1);
        float_swap(&u0, &u1);
        float_swap(&v0, &v1);
    }

    float inv_slope_1 = 0.0f;
    float inv_slope_2 = 0.0f;

    if (y1 - y0 != 0.0f)
    {
        inv_slope_1 = (float)(x1 - x0) / abs(y1 - y0);
    }

    if (y2 - y0 != 0.0f)
    {
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y1 - y0 != 0)
    {
        for (int y = y0; y <= y1; y++)
        {
            int x_start = (int)(x1 + ((y - y1) * inv_slope_1));
            int x_end = (int)(x0 + ((y - y0) * inv_slope_2));

            if (x_end < x_start)
            {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++)
            {
                draw_pixel(x, y, (x % 2 == 0 && y % 2 == 0) ? 0xFFFF00FF : 0xFF000000);
            }
        }
    }

    inv_slope_1 = 0.0f;
    inv_slope_2 = 0.0f;

    if (y2 - y1 != 0.0f)
    {
        inv_slope_1 = (float)(x2 - x1) / abs(y2 - y1);
    }

    if (y2 - y0 != 0.0f)
    {
        inv_slope_2 = (float)(x2 - x0) / abs(y2 - y0);
    }

    if (y2 - y1 != 0)
    {
        for (int y = y1; y <= y2; y++)
        {
            int x_start = (int)(x1 + ((y - y1) * inv_slope_1));
            int x_end = (int)(x0 + ((y - y0) * inv_slope_2));

            if (x_end < x_start)
            {
                int_swap(&x_start, &x_end);
            }

            for (int x = x_start; x < x_end; x++)
            {
                draw_pixel(x, y, (x % 2 == 0 && y % 2 == 0) ? 0xFFFF00FF : 0xFF000000);
            }
        }
    }
}
