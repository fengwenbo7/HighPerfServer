#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <unistd.h>
#include <assert.h>
#include <wait.h>

#define CUSTOM_COUNT 2
#define PRODUCT_COUNT 3

pthread_mutex_t mutex_=PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_ =PTHREAD_COND_INITIALIZER;

int g_count_=0;

//custmor thread
void* custom_handle(void* arg){
    while(1){
        pthread_mutex_lock(&mutex_);
        while(g_count_==0){
            printf("cosume is %lu,produce count is 0\n",pthread_self());
            pthread_cond_wait(&cond_,&mutex_);
        }
        printf("cosume is %lu,g_counter is %d\n",pthread_self(),g_count_);
        g_count_--;
        pthread_mutex_unlock(&mutex_);
        sleep(1);
    }
    pthread_exit(NULL);
}

//product thread
void* product_handle(void* arg){
    while(1){
        pthread_mutex_lock(&mutex_);
        if(g_count_>=10){
            printf("too many products,wait...\n");
            pthread_mutex_unlock(&mutex_);
            sleep(10);
            continue;
        }
        printf("start produce the product\n");
        g_count_++;
        printf("product is %lu,produce count is %d\n",pthread_self(), g_count_);
        pthread_cond_signal(&cond_);
        pthread_mutex_unlock(&mutex_);
        sleep(1);
    }
    pthread_exit(NULL);
}

int main(){
    pthread_t customs[CUSTOM_COUNT];
    pthread_t products[PRODUCT_COUNT];
    for(int i=0;i<CUSTOM_COUNT;i++){
        pthread_create(&customs[i],NULL,custom_handle,NULL);
    }
    sleep(3);
    for(int i=0;i<PRODUCT_COUNT;i++){
        pthread_create(&products[i],NULL,product_handle,NULL);
    }

    //wait for custom threads
    for(int i=0;i<CUSTOM_COUNT;i++){
        pthread_join(customs[i],NULL);
    }
    //wait for product threads
    for(int i=0;i<PRODUCT_COUNT;i++){
        pthread_join(products[i],NULL);
    }

    printf("parent exit.\n");
    exit(0);
}