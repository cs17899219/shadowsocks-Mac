//
//  SSLocalManager.c
//  ShadowsocksX-NG
//
//  Created by Lanwind on 6/13/16.
//  Copyright © 2016 qiuyuzhou. All rights reserved.
//

#include "SSLocalManager.h"

#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>

pthread_t inc_x_thread;

/* this function is run by the second thread */
void *do_ss_start(void *profile)
{
    pthread_setname_np( "SSLocal");
    
    profile_t pp = *(profile_t*) profile;
    
    if (NULL != pp.remote_host){
        start_ss_local_server(pp);
    }
    
    printf("sslocal started\n");
    
    return NULL;
}

int sslocal_start(profile_t profile)
{
    /* create a second thread which executes inc_x(&x) */
    inc_x_thread = NULL;
    if(pthread_create(&inc_x_thread, NULL, do_ss_start, (void *) &profile)) {
        fprintf(stderr, "Error creating thread\n");
        return 1;
    }
    return 0;
}

int sslocal_stop()
{
    if (0 != inc_x_thread) {
        pthread_kill(inc_x_thread, SIGTERM);
    }
    return 0;
}

