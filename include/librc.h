/*
 * =====================================================================================
 *
 *       Filename:  librc.h
 *
 *    Description: simple single-header Reference Counter (RC) in C 
 *
 *
 *        Version:  1.0
 *        Created:  25.12.2025 
 *       Revision:  none
 *       Compiler:  clang / gcc (clang first) 
 *
 *         Author:  freemorger (), 
 *         License: MIT License  
 *
 * https://github.com/Freemorger/librc
 * =====================================================================================
 */

#pragma once
#include <stdbool.h>
#include <stdlib.h>

#define Rc(T) struct {RcHeader* head; T* body}
#define RC_NEW(T, destr) {.head = rch_new(destr), .body=(T)}
#define RC_CLONE(other) {.head=(rc_clone((other).head)), .body=(other).body}

// TODO: implement weak refs
typedef struct RcHeader {
    int count, count_weak;
    void (*destructor)(void*);
    bool was_freed;
} RcHeader;

RcHeader* rch_new(void (*destructor)(void*)) {
    RcHeader* rch = (RcHeader*)malloc(sizeof(RcHeader));

    rch->count = 1;
    rch->count_weak = 0;
    rch->destructor = destructor;

    return rch;
}

RcHeader* rc_clone(RcHeader* rch) {
    if (rch == NULL) {
        return NULL;
    }
    
    rch->count += 1;
    return rch;
}

void rc_release(RcHeader* rch, void* body) {
    if (rch == NULL) {
        return;
    }

    rch->count -= 1;

    if (rch->count == 0) {
        if (rch->destructor != NULL) {
            rch->destructor(body);    
        }
        if (body != NULL) {
            free(body);
        }
        rch->was_freed = true;
    }
}
