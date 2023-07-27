#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Struct to store response data
struct ResponseData {
    char *data;
    size_t size;
};

// Callback function to handle the response
size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata) {
    struct ResponseData *response = (struct ResponseData *)userdata;

    size_t total_size = size * nmemb;
    response->data = realloc(response->data, response->size + total_size + 1);
    if (response->data == NULL) {
        fprintf(stderr, "Failed to allocate memory.\n");
        return 0;
    }

    memcpy(response->data + response->size, ptr, total_size);
    response->size += total_size;
    response->data[response->size] = '\0';

    return total_size;
}

int request_img(const char *host, const char *paths[], const int size, char ***results)
{
    CURL *curl;
    CURLcode res;

    // Initialize CURL
    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    int results_len = 0;
    *results = malloc(sizeof(char*) * size);
    if (curl) {
        // Create response data struct
        struct ResponseData response;
        response.data = NULL;
        response.size = 0;

        // Set the callback function to handle the response
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);

        // Set HTTP/2 specific options
        curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_2_0);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

        for (int i = 0; i < size; i++) 
        {
            // Set the specific path in the URL
            int url_size = strlen(host) + strlen(paths[i]);
            char url[url_size];
            snprintf(url, url_size, "%s%s", host, paths[i]);
            curl_easy_setopt(curl, CURLOPT_URL, url);

            // Reset response data
            response.size = 0;

            // Make the request
            res = curl_easy_perform(curl);
            if (res != CURLE_OK) 
            {
                fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
                continue;
            } 

            (*results)[results_len] = response.data; 
            results_len++;
        }

        // Clean up CURL handle
        curl_easy_cleanup(curl);
    }

    // Clean up global CURL resources
    curl_global_cleanup();

    return results_len;
}

int main(int argc, char *argv[])
{

    return EXIT_SUCCESS;
}
