// libraries 
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <err.h>

// position and dimensions of the Window
#define POSX 500
#define POSY 500
#define WIDTH 750 
#define HEIGHT 500
#define BORDER 25

// Window
static Display *dpy; 
static int scr;
static Window root; 

// functions 
static Window create_window(int x, int y, int w, int h, int b);
static void run();

// main program 
int main ()
{
    // stablish connection to server and check for errors 
    if ((dpy = XOpenDisplay(NULL)) == NULL)
        errx(1, "Cant open display ");

    // initialize Window
    scr = DefaultScreen(dpy);
    root = RootWindow(dpy, scr);

    // creates Window
    Window win = create_window(POSX, POSY, WIDTH, HEIGHT, BORDER);

    // maps window to screen (i.e draws window)
    XMapWindow(dpy, win);

    // runs the program 
    run();

    // clean window
    XUnmapWindow(dpy, win);
    XDestroyWindow(dpy, win);
    XCloseDisplay(dpy);

    // exit program
    return 0;
}


// make the program run 
static void run()
{
    // initialize event variable 
    XEvent ev;

    // wait for next event and returns event in variable ev 
    XNextEvent(dpy, &ev);
    
    // if button press then exit program 
    if (ev.type == ButtonPress)
        return;
    
}

// creates Window
static Window create_window(int x, int y, int w, int h, int b)
{
    // initialize variable
    Window win;
    XSetWindowAttributes xwa;

    // window atributes 
    xwa.background_pixel = WhitePixel(dpy, scr);
    xwa.border_pixel = BlackPixel(dpy, scr);
    xwa.event_mask = ButtonPress;

    // creates window
    win = XCreateWindow(dpy, root, x, y, w, h, b,  DefaultDepth(dpy, scr), 
            InputOutput, DefaultVisual(dpy, scr), 
            CWBackPixel | CWBorderPixel | CWEventMask, &xwa);

    // return window
    return win;
}

    
