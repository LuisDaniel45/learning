#include <math.h>
#include <stdint.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <xcb/xcb.h>
#include <xcb/xcb_image.h>
#include <xcb/xproto.h>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_resize.h"

#define BORDERW 2
#define SCROLL_SPEED 25

typedef struct {
    int x;
    int y;
    int width;
    int height;
    char *title;
    xcb_window_t id;
} window_t;

typedef struct {
    int zoom;
    int width;
    int height;
    xcb_image_t *image;
    xcb_gcontext_t gc;
    xcb_pixmap_t   pixmap;
} image_t;

typedef struct {
    xcb_font_t id;
    xcb_gcontext_t gc;
} font_t;


// functions 
void     global_init();
window_t window_init();
void     error(char *s);
font_t   font_init(char *name);
void     render_image(image_t image, int x, int y);
void     resize_image(image_t *image, int width, int height);
int      print_screen(char *text, font_t font, int x, int y); 
image_t  load_image(unsigned char *image_buffer, int image_size);
int      read_file(char *file_name, unsigned char **file_content);
void     resize_pixmap(xcb_pixmap_t *pixmap, int width, int height);
void     run_program(image_t image[], font_t font, int number_images);

// global variables 
xcb_connection_t *connection;
xcb_screen_t     *screen;
window_t          window;

int main(int argc, char *argv[])
{
    global_init();
    font_t font = font_init("fixed");

    int number_images = argc - 1;
    image_t image[number_images];
    for (int i = 0; i < number_images; i++)
    {
        unsigned char *file = NULL;
        int file_size = read_file(argv[i + 1], &file);
        image[i]      = load_image(file, file_size);
    }

    xcb_flush(connection);
    run_program(image, font, number_images);

    // clean program
    for (int i = 0; i < number_images; i++) 
    {
        xcb_image_destroy(image[i].image);
        xcb_free_pixmap(connection, image[i].pixmap);
        xcb_free_gc(connection, image[i].gc);
    }
    xcb_unmap_window(connection, window.id);
    xcb_destroy_window(connection, window.id);
    xcb_close_font(connection, font.id);
    xcb_free_gc(connection, font.gc);
    xcb_disconnect(connection);
    return 0;
}

void render_image(image_t image, int x, int y)
{
    xcb_clear_area(connection, 0, window.id, 0, 0, window.width, window.height);
    xcb_copy_area(connection, image.pixmap, window.id, image.gc, 0, 0, x, y, image.width, image.height);
}

int read_file(char *file_name, unsigned char **file_content)
{
    FILE *file = fopen(file_name, "rb");
    if (file == NULL) 
        error("error opening file");

    fseek(file, 0, SEEK_END);
    int file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    *file_content = malloc(file_size);
    if (fread(*file_content, 1, file_size, file) != file_size) {
        free(*file_content);
        error("Error: reading file");
    }

    fclose(file);
    return file_size;
}

void error(char *s)
{
    perror(s);
    exit(1);
}

void global_init()
{
    connection = xcb_connect(NULL, NULL);
    screen     = xcb_setup_roots_iterator(xcb_get_setup(connection)).data;
    window     = window_init(); 
}

window_t window_init()
{
    window_t window = {
        .x = 0, 
        .y = 0,
        .width  = 200, 
        .height = 200, 
        .title  = "Manga",
        .id     = xcb_generate_id(connection),
    };

    /** // creates pixmap to render stuff  */
    /** xcb_create_pixmap(connection,  */
    /**                   screen->root_depth,  */
    /**                   window.pixmap,  */
    /**                   window.id, window.width, window.height); */
    
    // set window config
    int mask = XCB_CW_BACK_PIXEL | XCB_CW_EVENT_MASK;
    int values[] = {screen->black_pixel,  XCB_EVENT_MASK_KEY_PRESS | XCB_EVENT_MASK_EXPOSURE};

    // create window 
    xcb_create_window(connection,                       // connection  
                      screen->root_depth,               // window depth 
                      window.id,                        // window id 
                      screen->root,                     // screen
                      window.x, window.y,               // cordinates of top left corner of window
                      window.width, window.width,       // size of window
                      BORDERW,                          // border width of window
                      XCB_WINDOW_CLASS_INPUT_OUTPUT,    // what the window receives 
                      screen->root_visual,              // idk  
                      mask, values);                    // the events that program handles 

    // set window title
    xcb_change_property(connection, 
                        XCB_PROP_MODE_REPLACE, 
                        window.id, 
                        XCB_ATOM_WM_NAME, 
                        XCB_ATOM_STRING, 8, 
                        strlen(window.title), window.title);

    // map window to screen to make it visible 
    xcb_map_window(connection, window.id);
    return window;
}

font_t font_init(char *name)
{
    font_t font;
    font.id = xcb_generate_id(connection);

    // open font 
    xcb_open_font(connection, font.id, strlen(name), name);

    // create graphics context for font 
    font.gc = xcb_generate_id(connection);
    int gc_mask = XCB_GC_BACKGROUND | XCB_GC_FOREGROUND | XCB_GC_FONT;
    int gc_values[] = {screen->white_pixel, screen->black_pixel, font.id};

    xcb_create_gc(connection, font.gc, window.id, gc_mask, gc_values);
    return font;
}

int print_screen(char *text, font_t font, int x, int y)
{
    xcb_void_cookie_t testCookie = xcb_image_text_8_checked(connection, strlen(text), 
                                                            window.id, font.gc, x, y, text);
    if (xcb_request_check(connection, testCookie))
        return true;
    return false;
}

image_t load_image(unsigned char *image_buffer, int image_size)
{
    image_t image;
    int num_channels; 
    unsigned char *data = stbi_load_from_memory(image_buffer, image_size,
                                                 &image.width, &image.height,
                                                 &num_channels, STBI_rgb);
    free(image_buffer);

    image.pixmap = xcb_generate_id(connection);
    xcb_create_pixmap(connection, screen->root_depth, image.pixmap , window.id, image.width, image.height);

    image.gc = xcb_generate_id(connection);
    xcb_create_gc(connection, image.gc, image.pixmap, 0, NULL);

    image.image = xcb_image_create_native(connection,
                                          image.width, image.height,
                                          XCB_IMAGE_FORMAT_Z_PIXMAP,
                                          screen->root_depth, data,
                                          image.width * image.height * 4,
                                          data);

    xcb_image_put(connection, image.pixmap, image.gc, image.image, 0, 0, 0);
    image.zoom = 100;
    return image;
}

void resize_pixmap(xcb_pixmap_t *pixmap, int width, int height)
{
    xcb_free_pixmap(connection, *pixmap);
    *pixmap = xcb_generate_id(connection);
    xcb_create_pixmap(connection, screen->root_depth, *pixmap, window.id, width, height);
}

void resize_image(image_t *image, int width, int height)
{   
    int zoom_width = (image->zoom / 100.0) * width; 

    // resize image based on image proportions 
    int scaled_height = ((float) (width * image->image->height) /
                         (image->image->width * height)) * height;
    
    // zooms image 
    int zoom_height   = (scaled_height * zoom_width) / height; 

    printf("%i %i\n", zoom_width, zoom_height);
    int size = zoom_width * zoom_height * 4;
    unsigned char resized_image[size];
    stbir_filter filter = STBIR_FILTER_BOX;
    stbir_resize(image->image->data,
                 image->image->width, image->image->height, 0,
                 resized_image, zoom_width, zoom_height, 0,
                 STBIR_TYPE_UINT8, 4, 1, 1, STBIR_EDGE_CLAMP, STBIR_EDGE_CLAMP,
                 filter, filter, STBIR_COLORSPACE_LINEAR, NULL);

    if (image->width < zoom_width || image->height < zoom_height) 
        resize_pixmap(&image->pixmap, zoom_width, zoom_height);

    // change values to resize image
    xcb_image_t xcb_image = *image->image;
    xcb_image.data        = resized_image;
    xcb_image.width       = zoom_width;
    xcb_image.height      = zoom_height;
    xcb_image.size        = size;

    xcb_image_put(connection, image->pixmap, image->gc, &xcb_image, 0, 0, 0);

    image->width  = zoom_width;
    image->height = zoom_height;
}

void run_program(image_t image[], font_t font, int number_images)
{
    int cur_image  = 0;
    int image_zoom = 0;
    xcb_generic_event_t *event;
    while ((event = xcb_wait_for_event(connection))) 
    {
        switch (event->response_type) {
            case  XCB_EXPOSE:  
                 xcb_expose_event_t *expose_event = (xcb_expose_event_t *) event;

                // update window values 
                window.width  = expose_event->width;
                window.height = expose_event->height;

                // render image so that there is no blinking 
                render_image(image[cur_image], window.x, window.y);

                // wait for everithing to finish
                xcb_flush(connection);
                break;
          
            case XCB_KEY_PRESS: 
                xcb_key_press_event_t *keypress = (xcb_key_press_event_t*) event;
                switch (keypress->detail) {

                    // if 'k' pressed go down 
                    case 45:
                        window.y += SCROLL_SPEED;
                        break;

                    // if 'j' pressed go up 
                    case  44:
                        window.y -= SCROLL_SPEED;
                        break;

                    // if '-' pressed zoom out 
                    case 61:
                        if (image[cur_image].zoom < 20) 
                            break;

                        image[cur_image].zoom -= 5;
                        resize_image(&image[cur_image], window.width , window.height);
                        break;

                    // if '+' pressed zoom in 
                    case 35:
                        if (image_zoom > 120) 
                            break;

                        image[cur_image].zoom += 5;
                        resize_image(&image[cur_image], window.width , window.height);
                        break;

                    // if 'l' scroll rigth 
                    case 46:
                        window.x -= SCROLL_SPEED;
                        break;

                    // if 'h' scroll left
                    case 43:
                        window.x += SCROLL_SPEED;
                        break;
                        
                    // if 'p' pressed then load previous picture
                    case 33:
                        if (cur_image > 0) 
                            cur_image--;
                        resize_image(&image[cur_image], window.width , window.height);
                        break;
                        
                    // if 'n' pressed load next picture
                    case 57:
                        cur_image++;
                        if (cur_image >= number_images)
                            cur_image--;
                        resize_image(&image[cur_image], window.width , window.height);
                        break;

                    // if 'q' pressed then exit
                    case 24:
                        printf("exit succesfully\n");
                        return;

                    // show pressed keycode 
                    default: 
                        char *layout= "keypressed: %i";
                        char buffer[255];
                        sprintf(buffer, layout, keypress->detail);

                        if (print_screen(buffer, font, 
                            window.width / 2, window.height / 2)) 
                            return;

                        break;
                }
                break;

            default:
                continue;
        
        }
        
        // resize image to fit window 
        /** resize_image(&image[cur_image], window.width , window.height); */
        render_image(image[cur_image], window.x, window.y);
        free(event);
    }
}

