// libraries 
#include <X11/Xlib.h>
#include <X11/X.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <stdio.h>
#include <err.h>

// position and dimensions of the Window
#define POSX    500
#define POSY    500
#define WIDTH   750 
#define HEIGHT  500
#define BORDER  25
#define LINE    2

// Window
static Display *dpy; 
static int      scr;
static Window   root; 
Window main_win;

// functions 
static Window create_window(int x, int y, int w, int h, int b);
static GC create_gc(int line_width);
static void run(GC gc);


// main program 
int main()
{
    // initialize variables 
    /** Window main_win; */
    GC gc;

    // open Display
    if((dpy = XOpenDisplay(NULL)) == NULL)
        errx(1, "Cant open Display server");

    // get screen and root value for reference
    scr = DefaultScreen(dpy);
    root = RootWindow(dpy, scr);

    // create window and graphic content
    main_win = create_window(POSX, POSY, WIDTH, HEIGHT, BORDER);
    gc = create_gc(LINE);

    // map window to display server
    XMapWindow(dpy, main_win);

    // puts the window name 
    XStoreName(dpy, main_win, "Paint program");
    run(gc);                                        // run the program

    // unmap window
    XUnmapWindow(dpy, main_win);

    // garbage collection 
    XDestroyWindow(dpy, main_win);
    XFreeGC(dpy, gc);
    XCloseDisplay(dpy);

    // exit program 
    return 0;
}


// what the program will run 
static void run(GC gc)
{
    // initialize variables
    XEvent ev;
    int init = 0;
    int prev_x = 0, prev_y = 0;

    // while there are not more XNextEvent run the program
    while (!XNextEvent(dpy, &ev)) {

        // switch base on the even
        switch (ev.type) {

            // if mouse click draw point 
            case ButtonPress:
                if (ev.xbutton.button == Button1)
                    XDrawPoint(dpy, ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
                break;

            // if mouse drag draw line 
            case MotionNotify:
                if (init)
                    XDrawLine(dpy, ev.xbutton.window, gc, prev_x, prev_y, ev.xbutton.x, ev.xbutton.y);
                else
                {
                    XDrawPoint(dpy, ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
                    init = 1;
                }
                prev_x = ev.xbutton.x, prev_y = ev.xbutton.y;
                break;

            // if mouse realese end line 
            case ButtonRelease:
                init = 0;
                break;

            // if button "q" press exit application 
            case KeyPress: 
                if (XkbKeycodeToKeysym(dpy, ev.xkey.keycode, 0, 0) == XK_q)
                        return;
        }
    
    }
}

// creates window
static Window create_window(int x, int y, int w, int h, int b)
{
    // create window variable
    Window win;
    
    // set window atributes
    XSetWindowAttributes xwa = {
        .background_pixel = WhitePixel(dpy, scr),
        .border_pixel = BlackPixel(dpy, scr),
        .event_mask = Button1MotionMask | ButtonPressMask | 
            ButtonReleaseMask | KeyPressMask
    };

    // creates window
    win = XCreateWindow(dpy, root, x, y, w, h, b, DefaultDepth(dpy, scr), 
            InputOutput, DefaultVisual(dpy, scr), 
            CWBackPixel | CWEventMask | CWBorderPixel, &xwa);

    // returns window
    return win; 
}

// creates graphic content 
static GC create_gc(int line_width)
{
    // initialize variables
    GC gc;
    XGCValues xgcv;
    unsigned long valuemask;

    // set graphic content variables
    xgcv.line_style = LineSolid;
    xgcv.line_width = line_width;
    xgcv.cap_style = CapButt;
    xgcv.join_style = JoinMiter;
    xgcv.fill_style = FillSolid;
    xgcv.foreground = BlackPixel(dpy, scr);
    xgcv.background = WhitePixel(dpy, scr);

    // set valuemask
    valuemask = GCForeground | GCBackground | GCFillStyle | 
                GCLineStyle | GCLineWidth | GCCapStyle | GCJoinStyle;

    // creates graphic content
    gc = XCreateGC(dpy, root, valuemask, &xgcv);

    // returns graphic content 
    return gc;
}

