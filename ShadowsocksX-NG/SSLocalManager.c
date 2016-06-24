//
//  SSLocalManager.c
//  ShadowsocksX-NG
//
//  Created by Lanwind on 6/13/16.
//  Copyright © 2016 qiuyuzhou. All rights reserved.
//

#include "SSLocalManager.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

static pthread_t ss_thread;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

static int isrunning = 0;

/* this function is run by the second thread */
void *do_ss_start(void* profile)
{
    pthread_mutex_lock(&mutex);

    static int i = 0;
    char str [32];
    sprintf(str,"SSLocal %d",++i);
    pthread_setname_np(str);
    
    profile_t * profile1 =  (profile_t *)profile;

    printf("SSLocal: remote_host is %s:%d \n", profile1->remote_host, profile1->remote_port);
    printf("SSLocal: local_addr is %s:%d \n", profile1->local_addr, profile1->local_port);
    printf("SSLocal: method: %s, \n", profile1->method);
    
    start_ss_local_server(*profile1);
    
    // release memory
    free(profile1->remote_host);
    free(profile1->local_addr);
    free(profile1->method);
    free(profile1->password);
    free(profile1->acl);
    free(profile1->log);
    free(profile1);
    
    isrunning = 0;
    printf("SSLocal: Interrupted \n");
    pthread_mutex_unlock(&mutex);
    
    return 0;
}

int sslocal_start(profile_t profile)
{
    pthread_mutex_lock(&mutex);
    if(isrunning == 0){
        isrunning = 1;
        pthread_mutex_unlock(&mutex);
        
        profile_t *cp_profile = malloc(sizeof(profile_t));
        
        // Required
        cp_profile->remote_host = malloc(sizeof(char)*(strlen(profile.remote_host)+1));
        strcpy(cp_profile->remote_host, profile.remote_host);
        
        cp_profile->local_addr = malloc(sizeof(char)*(strlen(profile.local_addr)+1));
        strcpy(cp_profile->local_addr, profile.local_addr);
        
        cp_profile->method = malloc(sizeof(char)*(strlen(profile.method)+1));
        strcpy(cp_profile->method, profile.method);
        
        cp_profile->password = malloc(sizeof(char)*(strlen(profile.password)+1));
        strcpy(cp_profile->password, profile.password);
        
        cp_profile->remote_port = profile.remote_port;
        cp_profile->local_port = profile.local_port;
        cp_profile->timeout = profile.timeout;
        
        // Optional
        char * ac = NULL;
        if(profile.acl != NULL ){
            ac = malloc(sizeof(char)*(strlen(profile.acl)+1));
            strcpy(ac, profile.acl);
            cp_profile->acl = ac;
        }else{
            cp_profile->acl = NULL;
        }
        
        char * lg = NULL;
        if(profile.log != NULL){
            lg = malloc(sizeof(char)*(strlen(profile.log)+1));
            strcpy(lg, profile.log);
            cp_profile->log = lg;
        }else{
            cp_profile->log = NULL;
        }
        
        cp_profile->fast_open = profile.fast_open;
        cp_profile->mode = profile.mode;
        cp_profile->auth = profile.auth;
        cp_profile->verbose = profile.verbose;

        printf("SSLocal: Start\n");

        ss_thread = NULL;
        if(pthread_create( &ss_thread, NULL, do_ss_start, cp_profile)) {
            printf("SSLocal: error creating thread\n");
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
    printf("SSLocal: Stop \n");
    if (isrunning == 1) {
        pthread_t * p = &ss_thread;

        if( 0 == pthread_kill(*p, SIGUSR1) ){
            printf("SSLocal: Sent Signal to %p\n", ss_thread);
            pthread_join(*p, NULL);
            printf("SSLocal: Stopped \n");
        }else{
            printf("SSLocal: Not running (1) \n");
        }
       
        return 0;
    }
    printf("SSLocal: Not running (2) \n");
    
    return -1;
}

int sslocal_isrunning()
{
    return (isrunning == 1);
}
