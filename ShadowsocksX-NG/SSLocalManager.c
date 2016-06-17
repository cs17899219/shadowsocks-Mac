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

static profile_t my_profile;

static int isrunning = 0;

static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

/* this function is run by the second thread */
void *do_ss_start(void* profile)
{
    pthread_mutex_lock(&mutex);

    static int i = 0;
    char str [32];
    sprintf(str,"SSLocal %d",++i);
    pthread_setname_np(str);
    
    profile_t profile1 = {
          .remote_host = "",
          .local_addr = "",
          .method = "",
          .password = "",
          .remote_port = 0,
          .local_port = 0,
          .timeout = 0,
          .acl = NULL,
          .log = NULL,
          .fast_open = 0,
          .mode = 0,
          .auth = 0,
          .verbose = 0
    };
    
    // Required
    char * rh = malloc(sizeof(char)*(strlen(my_profile.remote_host)+1));
    strcpy(rh, my_profile.remote_host);
    profile1.remote_host = rh;
    
    char * la = malloc(sizeof(char)*(strlen(my_profile.local_addr)+1));
    strcpy(la, my_profile.local_addr);
    profile1.local_addr = la;
    
    char * mt = malloc(sizeof(char)*(strlen(my_profile.method)+1));
    strcpy(mt, my_profile.method);
    profile1.method = mt;
    
    char * pwd = malloc(sizeof(char)*(strlen(my_profile.password)+1));
    strcpy(pwd, my_profile.password);
    profile1.password = pwd;
    
    profile1.remote_port = my_profile.remote_port;
    profile1.local_port = my_profile.local_port;
    profile1.timeout = my_profile.timeout;
    
    char * ac = NULL;
    if(my_profile.acl != NULL ){
        ac = malloc(sizeof(char)*(strlen(my_profile.acl)+1));
        strcpy(ac, my_profile.acl);
        profile1.acl = ac;
    }
    
    char * lg = NULL;
    if(my_profile.log != NULL){
        lg = malloc(sizeof(char)*(strlen(my_profile.log)+1));
        strcpy(lg, my_profile.log);
        profile1.log = lg;
    }
    
    profile1.fast_open = my_profile.fast_open;
    profile1.mode = my_profile.mode;
    profile1.auth = my_profile.auth;
    profile1.verbose = my_profile.verbose;
    
    printf("SSLocal: remote_host is %s:%d \n", profile1.remote_host, profile1.remote_port);
    printf("SSLocal: local_addr is %s:%d \n", profile1.local_addr, profile1.local_port);
    printf("SSLocal: method: %s, \n", profile1.method);
    
    start_ss_local_server(profile1);
    
    free(rh); free(la); free(mt); free(pwd);
    if(ac != NULL){
        free(ac);
    }
    if(lg != NULL){
        free(lg);
    }
    
    isrunning = 0;
    pthread_mutex_unlock(&mutex);
    
    printf("SSLocal: Interrupted\n");

    return 0;
}

int sslocal_start(profile_t profile)
{
    pthread_mutex_lock(&mutex);
    if(isrunning == 0){
        isrunning = 1;
        pthread_mutex_unlock(&mutex);
        
        my_profile = profile;
        
        printf("SSLocal: Start\n");

        ss_thread = NULL;
        if(pthread_create( &ss_thread, NULL, do_ss_start, NULL)) {
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

        pthread_kill(ss_thread, SIGUSR1);
        printf("SSLocal: Sent Signal to %p\n", ss_thread);
        
        pthread_join(ss_thread, NULL);
        printf("SSLocal: Stopped \n");
       
        return 0;
    }
    printf("SSLocal: Not running \n");
    
    return -1;
}

int sslocal_isrunning()
{
    return (isrunning == 1);
}
