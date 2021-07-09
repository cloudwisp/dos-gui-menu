#ifndef GameApp_H
#define GameApp_H

typedef void (*GameEventCallback)(const char *event, int data1, int data2);

const int MS_PER_UPDATE = 8;

class GameApplication;

#endif
