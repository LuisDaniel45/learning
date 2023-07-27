#include <Imlib2.h>
#include <X11/X.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../structures.h"
#include "../request.h" 
#include "../choose.h" 
#include "../get_data.h" 
#include "../manga_reader.h"

int read_file(char *file_name, unsigned char **file_content);

int test_search_html();
int test_chapters_html();
int test_pages_html();

int test_pages_json();

int test_request();
int test_reader ();

static void error(char *msg) 
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    /** int status = test_search_html(); */
    /** int status = test_chapters_html(); */
    /** int status = test_pages_html(); */
    /** int status = test_request(); */
    int status = test_pages_json();
    /** int status = test_reader(); */
    printf("number of results: %i\n", status);
    return 0;
}

int test_request()
{
    /** char *ip = "142.250.217.164"; */
    /** char *host = "www.google.com"; */
    /** char *path = "/"; */
    /** char *ip = manga_sources[ASURASCANS].ip; */
    /** char *host = manga_sources[ASURASCANS].host; */
    /** char *path = "/warrior-high-school-dungeon-raid-department-chapter-43/"; */
    /** int port = 443; */
    /** int compress = 1; */
    /**  */
    /** int response_size = 0; */
    /** char *response = request(ip, host, path, port, compress, &response_size); */
    /** printf("%s\n", response); */
    /** free(response); */

    /** return response_size; */
    return 0;
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

int test_search_html()
{
    char *response = NULL;
    int response_size = read_file("test/search.html", &response);

    int number_of_results = 0; 
    object *mangas = get_manga_html(response, response_size, &number_of_results);

    for (int i = 0; i < number_of_results; i++) {
        printf("path: %s\ntitle: %s\n\n", mangas[i].id, mangas[i].title);

        free(mangas[i].id);
        free(mangas[i].title);
    }
    
    free(mangas);
    return number_of_results;
}

int test_chapters_html()
{
    char *response = NULL;
    int response_size = read_file("test/chapters.html", &response);

    int number_of_results = 0; 
    object *chapters = get_chapters_html(response, response_size, &number_of_results);

    for (int i = 0; i < number_of_results; i++) 
        printf("path: %s\ntitle: %s\n\n", chapters[i].id, chapters[i].title);

    free(chapters);
    return number_of_results;
}

int test_pages_html()
{
    char *response = NULL;
    int response_size = read_file("test/pages.html", &response);
    get_pages_html(response, response_size);

    return 0;
}

int test_reader ()
{
    int number_pages =  42;
    images_t images[number_pages];
    char file_name[255];

    for (int i = 0; i < number_pages; i++) 
    {
        sprintf(file_name, "test/pages/%i.jpg", i);
        images[i].file_name = file_name;

        images[i].image_size = read_file(file_name, &images[i].image_buffer);
        if (!images[i].image_buffer) 
            error("Error reading image");
    }

    manga_reader(images, number_pages);
    return 0;
}

int test_pages_json()
{
    char *response;
    int response_size = read_file("test/pages.json", &response);

    char **images;
    char *host, *ip;
    int number_pages = get_pages_json(response, &images, &host, &ip);

    printf("number_pages: %i\n", number_pages);

    images_t images_buffers[number_pages];
    int results_len = request_img(images, number_pages, &images_buffers);

    char buffer[255];
    for (int i = 0; i < results_len; i++) 
    {
        sprintf(buffer, "test_pages/%i", i);
        FILE *fp = fopen(buffer, "w");
        fwrite(images_buffers[i].image_buffer, sizeof(uint8_t), 1, fp);
        fclose(fp);
    }

    manga_reader(images_buffers, number_pages);
    return 0;
}
    

