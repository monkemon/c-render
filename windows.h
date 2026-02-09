#ifndef WINDOW_MANAGER
#define WINDOW_MANAGER

#include <stdlib.h>

#include <GLFW/glfw3.h>

typedef struct sWindowNode
{
    GLFWwindow* w;
    struct sWindowNode* next;
} sWindowNode;


typedef struct {
    sWindowNode* base;
    uint8_t     count;
} sWindowManager;


void WManagerAddWindow(sWindowManager* wm, GLFWwindow* w)
{
    sWindowNode* n = (sWindowNode*)malloc(sizeof(sWindowNode));
    n->w = w;
    n->next = NULL;

    // we're creating first window
    if (wm->base == NULL)
    {
        wm->base = n;
        wm->count = 1;
    }
    // there is alrdy some window inside the manager
    else
    {
        sWindowNode* tmp = wm->base;
        do
        {
            tmp = tmp->next;
        } 
        while (tmp->next != NULL);
        tmp->w = w;
    }
    wm->count += 1;
}

GLFWwindow* WManagerGetWindow(sWindowManager* wm, uint8_t idx)
{
    sWindowNode* tmp = wm->base;
    while (idx > 0)
    {
        tmp = tmp->next;
        idx --;
    }
    return tmp->w;
}

#endif // WINDOW_MANAGER