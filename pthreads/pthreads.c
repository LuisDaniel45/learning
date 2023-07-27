#include <stdio.h>
#include <unistd.h>
#include <pthread.h>

void *myturn(void *arg)
{
    for (int i = 0; i < 8; i++) 
    {
        sleep(1);
        printf("My Turn %d\n", i);
    }
    return NULL;
}
void *yourturn(void *arg)
{
    for (int i = 0; i < 3; i++) 
    {
        sleep(2);
        printf("your Turn %d\n", i);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    // thread id 
    pthread_t thread;

    // creates and runs thread 
    pthread_create(&thread, NULL, myturn, NULL);
    yourturn(NULL);

    // waits for thread to finish 
    pthread_join(thread, NULL);
    return 0;
}
