#ifndef mouse_c
#define mouse_c

#include <dos.h>

union REGS i, o;

int initmouse()
{
   i.x.ax = 0;
   int86(0X33, &i, &o);
   return ( o.x.ax );
}
 
void showmouseptr()
{
   i.x.ax = 1;
   int86(0X33, &i, &o);
}
 
void getmousepos(int *button, int *x, int *y)
{
   i.x.ax = 3;
   int86(0X33, &i, &o);
 
   *button = o.x.bx;
   *x = o.x.cx;
   *y = o.x.dx;
}

void set_mouse_x_boundary(int x1, int x2)
{
   i.x.ax = 7;
   i.x.cx = x1;
   i.x.dx = x2;
   int86(0x33,&i,&o);
}
void set_mouse_y_boundary(int y1, int y2)
{
   i.x.ax = 8;
   i.x.cx = y1;
   i.x.dx = y2;
   int86(0x33,&i,&o);
}

#endif