/*
 * Copyright 2018 Board of Trustees of Stanford University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/*
 * worker.c - Worker core functionality
 *
 * Poll dispatcher CPU to get request to execute. The request is in the form
 * of ucontext_t. If interrupted, swap to main context and poll for next
 * request.
 */

#include <ucontext.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <sched.h>
#include <stdio.h>
#include <time.h>

#include <sys/types.h>
#include <sys/resource.h>

#include <ix/cpu.h>
#include <ix/log.h>
#include <asm/cpu.h>
#include <ix/context.h>
#include <ix/dispatch.h>

__thread ucontext_t uctx_main;
ucontext_t uctx;

//FIXME Remove stack from here when integrated with ffwd
char uctx_stack[16384];

extern int getcontext_fast(ucontext_t *ucp);
extern int swapcontext_fast(ucontext_t *ouctx, ucontext_t *uctx);

static void generic_work(void){
    int ret;

    while(1) {
        //log_info("generic_work: Swapping back to main context\n");
        ret = swapcontext_fast(&uctx, &uctx_main);
        if (ret) {
            //  TODO Free context from mempool or return it to dispatcher
            log_err("do_work: failed to do swapcontext\n");
            exit(-1);
        }
    }
}

static ucontext_t * get_work(ucontext_t * fini_uctx)
{
    // TODO Use ffwd here to talk with dispatcher CPU and get remote context
    // TODO Need to update uc_link to local uctx_main here
    return fini_uctx;
}

void do_work(void)
{
    log_info("do_work: starting...\n");
    // FIXME Remove these after benchmarking finishes
    struct timespec start, end;
    uint64_t start64, end64;
    int i;
    int ret;
        int cpu_nr_ = percpu_get(cpu_nr) - 2;
        log_info("do_work: worker number = %d\n", cpu_nr_);
        worker_responses[cpu_nr_].flag = FINISHED;
    uint64_t foo[100000];

    /*
    if (cpu_nr != 1)
        while(1);
    */

        log_info("do_work: Waiting for dispatcher work\n");
        while (1) {
                while (dispatcher_requests[cpu_nr_].flag == WAITING);
                dispatcher_requests[cpu_nr_].flag = WAITING;
                worker_responses[cpu_nr_].flag = FINISHED;
        }
    /*
    for(i = 0; i < 100000; i++) {
        while (dispatcher_requests[cpu_nr].flag == WAITING);
        //worker_responses[cpu_nr].cont = NULL;
        dispatcher_requests[cpu_nr].flag = WAITING;
        //log_info("do_work: Got dispatcher work\n");
        //dispatcher_requests[cpu_nr].cont->uc_link = &uctx_main;
        //makecontext(dispatcher_requests[cpu_nr].cont, generic_work, 0);

        //log_info("do_work: calling swapcontext_fast()\n");
        //ret = swapcontext_fast(&uctx_main, dispatcher_requests[cpu_nr].cont);
        //if (ret) {
        //   log_err("do_work: failed to swapcontext_fast\n");
        //    exit(-1);
        //}
        //log_info("do_work: swapped back to main context\n");
        //worker_responses[cpu_nr].cont = dispatcher_requests[cpu_nr].cont;
        //worker_responses[cpu_nr].flag = FINISHED;
        //end64 = rdtsc();
        //foo[i] = (end64 - start64) / 2.8;
        //start64 = end64;
    }*/

    /*
    if (cpu_nr == 1) {
        for (i = 0; i < 100000; i++) {
            printf("%lu\n", foo[i]);
        }
    }*/
    /*
    log_info("do_work: starting benchmarking...\n");
    clock_gettime(CLOCK_MONOTONIC, &start);

    for (i = 0; i < 100000; i++) {
        ret = swapcontext_fast(&uctx_main, &uctx);
        if (ret) {
            //  TODO Free context from mempool or return it to dispatcher
            log_err("do_work: failed to do swapcontext_fast\n");
            exit(-1);
        }
    }

    clock_gettime(CLOCK_MONOTONIC, &end);
    start64 = 10e9 * start.tv_sec + start.tv_nsec;
    end64 = 10e9 * end.tv_sec + end.tv_nsec;
    log_info("do_work: average context creation time: %lu ns\n",
             (end64 - start64) / 200000);
    */
    log_info("do_work: finished benchmarking, looping forever\n");
    while (1);
}
