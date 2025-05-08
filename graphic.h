// mobile.cc
// Auteur : theo brochier
// Version : 1.0
#ifndef GRAPHIC_H
#define GRAPHIC_H

enum Color
{
    WHITE,
    GREY,
    BLACK,
    RED,
    GREEN,
    BLUE,
    ORANGE,
    PURPLE,
    CYAN
};
void draw_circle(double x,double y, double r,Color color, Color color2);
void draw_point(double x, double y, Color color);
void draw_line(double depart_x, double depart_y, double fin_x, double fin_y, Color color);

#endif