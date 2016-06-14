//
//  SSLocalManager.c
//  ShadowsocksX-NG
//
//  Created by Lanwind on 6/13/16.
//  Copyright © 2016 qiuyuzhou. All rights reserved.
//

#include "SSLocalManager.h"

#include <string.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

static pthread_t ss_thread;

static profile_t my_profile;

static int isrunning = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* this function is run by the second thread */
void *do_ss_start(void* profile)
{
    pthread_mutex_lock(&mutex);

    static int i;
    char str [32];
    snprintf(str,32,"SSLocal %d",++i);
    pthread_setname_np(str);
    
    profile_t pp = * (profile_t *) profile;
    
    start_ss_local_server(pp);
    
    isrunning = 0;
    pthread_mutex_unlock(&mutex);
    
    printf("SSLocal Interrupted\n");

    return 0;
}

int sslocal_start(profile_t profile)
{
    pthread_mutex_lock(&mutex);
    if(isrunning == 0){
        isrunning = 1;
        pthread_mutex_unlock(&mutex);
        
        my_profile = profile;
        
        printf("SSLocal Start\n");
        /* create a second thread which executes inc_x(&x) */
        if(pthread_create( &ss_thread, NULL, do_ss_start, (void *) &my_profile)) {
            printf("SSLocal error creating thread\n");
            
            return -1;
        }
        
        return 0;
    }else{
        pthread_mutex_unlock(&mutex);
        
        return -1;
    }
}

int sslocal_stop()
{
    printf("SSLocal Stop \n");
    if (isrunning == 1) {

        pthread_kill(ss_thread, SIGUSR1);
        printf("Sent Signal to %p\n", ss_thread);
        
        pthread_join(ss_thread, NULL);
        printf("SSLocal Stopped \n");
       
        return 0;
    }
    printf("SSLocal is not running \n");
    
    return -1;
}

int sslocal_isrunning()
{
    return (isrunning == 1);
}
