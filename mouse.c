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

#endif