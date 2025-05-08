// mobile.cc
// Auteur : theo brochier
// Version : 4.0
#include <cmath>
#include <iostream>
#include "graphic_gui.h"

using namespace std;

static const Cairo::RefPtr<Cairo::Context> *ptcr(nullptr);

// local prototypes
static void set_color(Color color);
// graphic_gui.h
void graphic_set_context(const Cairo::RefPtr<Cairo::Context> &cr)
{
    ptcr = &cr;
}
// à compléter pour effectuer des dessins élémentaires: trait, cercle, ...
void draw_circle(double x , double y , double r,Color color){
    set_line_width(r_viz);
    set_color(color);
    (*ptcr)->arc(x, y, r, 0.0, 2 * M_PI);
    (*ptcr)->stroke();
}
void draw_point(double x, double y, Color color){
    set_line_width(r_viz);
    set_color(color);
    (*ptcr)->arc(x, y, 1.0, 0.0, 2 * M_PI);
    (*ptcr)->stroke();
}
void draw_line(double depart_x, double depart_y, double fin_x, double fin_y, Color color){
    set_line_width(r_viz);
    set_color(color);
    (*ptcr)->move_to(depart_x,depart_y);
    (*ptcr)->line_to(fin_x,fin_y);
    (*ptcr)->stroke();
}

// local function
static void set_color(Color color)
{
    double r(0.), g(0.), b(0.);
    switch (color)
    {
    case WHITE:
        r = g = b = 1;
        break;
    case GREY:
        r = g = b = 0.5;
        break;
    case BLACK:
        r = g = b = 0.;
        break;
    case RED:
        r = 1;
        break;
    case GREEN:
        g = 0.65;
        break;
    case BLUE:
        r = g = 0.65;
        b = 1;
        break;
    case ORANGE:
        r = 1;
        g = 0.65;
        break;
    case PURPLE:
        r = b = 0.65;
        break;
    case CYAN:
        g = b = 1;
        break;
    }
    (*ptcr)->set_source_rgb(r, g, b);
}