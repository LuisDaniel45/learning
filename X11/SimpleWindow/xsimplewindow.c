
//X11 api 
#include <X11/X.h>
#include <X11/Xlib.h>

//standard input and output 
#include <stdio.h>

//catch error to kill app 
#include <err.h>

//-----------------------------

//stablish connection to X server 
static Display *dpy;

//number of the Window created 
static int scr;

//to have a referance point when creating a Window
static Window root;

//--------------------------------

//position of the Window
#define POSX 500
#define POSY 500

//dimension of the Window
#define WIDTH 500
#define HEIGHT 500
#define BORDER 15


//----------------------------

//start of the program 
int main ()
{
    Window win;
    XEvent event;

    //establish connection to X server 
    dpy = XOpenDisplay(NULL);
    if (dpy == NULL)
        errx(1, "Cant open Display");

    // get screen number and root 
    scr = DefaultScreen(dpy);
    root = RootWindow(dpy, scr);

    // create Window
    win = XCreateSimpleWindow(dpy, root, POSX, POSY, WIDTH, HEIGHT, BORDER, BlackPixel(dpy, scr), WhitePixel(dpy, scr));


    // Map our Window to Display server
    XMapWindow(dpy, win);

    // crating a loop to Display Window infinitely
    while (XNextEvent(dpy, &event) == 0)
    {

    }

    //unmap WINDOW
    XUnmapWindow(dpy, scr);

    //free resources 
    XDestroyWindow(dpy, win);


    return 0;
}
