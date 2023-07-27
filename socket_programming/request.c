#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <arpa/inet.h>
#include <sys/socket.h>

#include <openssl/ssl.h>
#include <zlib.h>

#include "request.h"

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

char *request(char *ip, char *host, char *path, const int portno, const int compress, int *size)
{
    // initializes the socket
    int socketfd = socket(AF_INET, SOCK_STREAM, 0);
    if ( socketfd < 0 )
        error("cannot open socket");

    // config of the socket
    struct sockaddr_in serv_addr = {
            .sin_family = AF_INET,
            .sin_port = htons(portno)
    };
    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0)
        error("error invalid ip");

    // connect to socket
    if (connect(socketfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("connection failed");
    
    // initializes ssl protocol
    SSL_library_init();
    SSL_CTX *ssl_ctx = SSL_CTX_new(TLS_client_method());
    if (ssl_ctx == NULL )
        error("error on ssl init");

    // configure ssl 
    SSL_CTX_set_alpn_protos(ssl_ctx, (const unsigned char *)"\x02h2\x08http/1.1", 10);
    SSL_CTX_set_ciphersuites(ssl_ctx, "TLS_AES_256_GCM_SHA384");
    SSL *ssl = SSL_new(ssl_ctx);
    SSL_set_tlsext_host_name(ssl, host);

    // stablish ssl communication
    SSL_set_fd(ssl, socketfd);
    if (SSL_connect(ssl) < 1)
        error("error on stablishing ssl connection");

    // layout of header for request 
    char *header_layout = "GET %s HTTP/1.1\r\n"
                          "Host: %s\r\n"
                          "Accept: */*\r\n" 
                          "User-Agent: Mozilla/5.0 "
                              "(Windows NT 10.0; Win64; x64) "
                              "AppleWebKit/537.36 (KHTML, like Gecko) "
                              "Chrome/111.0.0.0 Safari/537.36 \r\n" 
                          "Accept-Language: en-US\r\n"
                          "%s"
                          "Connection: Keep-Alive\r\n\r\n";

    // wether it is compress or not
    char *header_encoding = "";
    if (compress) 
        header_encoding = "Accept-Encoding: gzip, deflate\r\n";


    // initializes the header string
    int header_lenght = strlen(header_layout) + strlen(header_encoding) + strlen(host) + strlen(path);
    char header[header_lenght];

    // makes the header for request
    snprintf(header, header_lenght, header_layout, path, host, header_encoding);

    
    // send https request 
    int n = SSL_write(ssl, header, strlen(header));
    if ( n < 0 )
        error("error on write");

    // initializes string to read response header
    char *response_header = malloc(sizeof(char));
    int counter = 0;
    int content_length = 0;


    // read bytes of socket infinitely
    while (1) 
    {
        // reads byte
        if (SSL_read(ssl, &response_header[counter], 1) <= 0)
            error("error on reading response header");

        // to get the keys of the values from the response
        if ( response_header[counter] == ':' )
        {
            // just makes sure that it is treated as string
            response_header[counter] = '\0';
            // if the key id content_length read the value 
            if (!strcmp(response_header, "Content-Length") || !strcmp(response_header, "content-length"))
            {
                char number[255];
                int i = 0;
                for ( char byte; byte != '\r'; SSL_read(ssl, &byte, 1))
                {
                    if ( byte > 47 && byte < 58)
                        number[i] = byte;
                    else
                         i--;
                    i++;
                }
                content_length = atoi(number);
                response_header[counter] = '\r';
            }
        }
        
        // checks wether this is the last byte of the response 
        if (response_header[counter] == '\r') 
        {
            // clean response header
            free(response_header);
            counter = 0;

            // if next tree bytes are '\n\r\n' then that is the end of header
            char end_of_chunk[3];
            if (SSL_read(ssl, end_of_chunk, 3) < 0)
                error("error on reading end chunk");

            else if (end_of_chunk[0] == '\n' && end_of_chunk[1] == '\r' &&
                    end_of_chunk[2] == '\n')
                break;

            // allocates space for next bytes
            response_header = malloc(sizeof(char) * 2);

            // write the last three bytes to response since is not the end 
            for (int i = 1; i < 3; i++ )
                response_header[i - 1] = end_of_chunk[i];
            counter++;
        }

        // allocates more space for the response_header
        response_header = realloc(response_header, counter + 2);
        counter++;
    }

    // body of the response
    char *output;
    int output_size = 0;

    // if content_length = 0 assume that it is written in chunks
    if (!content_length)
    {
        // allocates scpace for the body 
        output = malloc(sizeof(char));

        // allocates scpace for the chunk of body 
        char *data_of_chunk = malloc(sizeof(char));
        counter = 0;

        // read response until no more data 
        while (1) 
        {
            // read bytes of response 
            if (SSL_read(ssl, &data_of_chunk[counter], 1) <= 0)
                error("error reading chunk of body");

            // checks for end of chunk 
            if ( data_of_chunk[counter] == '\r' )
            {
                // makes sure that chunk is treated as string
                data_of_chunk[counter] = '\0';

                // gets the lenght of chunks and trasnforms it from hex to decimal
                int length_of_chunk;
                sscanf(data_of_chunk, "%x", &length_of_chunk);

                // clears chunk of body 
                free(data_of_chunk);
                counter = -1;
                

                // if the lenght of the chunk is 0 then it is the end of response 
                if ( length_of_chunk == 0)
                    break;

                // moves read pointer to start in next chunk
                char byte;
                if (SSL_read(ssl, &byte, 1) < 0 )
                    error("error moving read pointer");

                // reads the chunk and stored it in output
                output = realloc(output, output_size + length_of_chunk);
                for (int i = 0; i < length_of_chunk; i++)
                {
                    if (SSL_read(ssl, &byte, 1) < 0)
                        error("error on reading chunk");

                    output[output_size + i] = byte;
                }
                output_size += length_of_chunk;

                // move the pointer to data of next chunk
                char bytes[2];
                if (SSL_read(ssl, bytes, 2) < 0 )
                    error("error on read");

                // allocates space start reading the data of next chunk
                data_of_chunk = malloc(sizeof(char));
            }
            // allocates more space for the next byte 
            counter++;
            data_of_chunk = realloc(data_of_chunk, counter + 2);
        }
    }
    else 
    {
        // reads the content of the response with fixed size 
        output = malloc(sizeof(char) * content_length);
        output_size += content_length;
        for (int i = 0; i < content_length; i++) 
            if (SSL_read(ssl, &output[i], 1) < 0)
                error("error on reading body with content_length");

    }


    // shutdown ssl communication
    SSL_shutdown(ssl);
    SSL_free(ssl);
    SSL_CTX_free(ssl_ctx);

    // closes socket
    close(socketfd);

    // decompress_data if it was compress
    if (compress)
        output = decompress_response(output, output_size);

    *size = output_size;

    // returns the body of the response 
    return output;
}


char *decompress_response(char *compress_data, int compressed_size)
{
    const int CHUNK = 4096;  
    int ret;
    unsigned int have;
    z_stream strm;
    unsigned char out[CHUNK];
    int out_size = 0;
    int out_capacity = CHUNK;
     
    char *decompress_data = malloc(CHUNK);

    memset(&strm, 0, sizeof(z_stream));
    ret = inflateInit2(&strm, MAX_WBITS + 16);
    if (ret != Z_OK)
        error("error compress");

    strm.avail_in = compressed_size;
    strm.next_in = (Bytef*) compress_data;

    do 
    {
        strm.avail_out = CHUNK;
        strm.next_out = out;
        ret = inflate(&strm, Z_NO_FLUSH);
        switch (ret) {
            case Z_NEED_DICT:
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                inflateEnd(&strm);
                error("error compression");
        }
    

        have = CHUNK - strm.avail_out;
        if ( out_size + have > out_capacity) 
        {
            out_capacity *= 2;
            decompress_data = (char *) realloc(decompress_data, out_capacity);
        }
        memcpy(decompress_data + out_size, out, have);

        out_size += have;
    }
    while (strm.avail_out == 0);
    inflateEnd(&strm);
    decompress_data = (char *) realloc(decompress_data, out_size + 1);
    decompress_data[out_size] = '\0';

    return decompress_data;
}

