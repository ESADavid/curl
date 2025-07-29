/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 2024, JPMorgan Chase & Co.
 *
 * Performance Benchmark Suite for JPMorgan Validation Services
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include "../include/curl/validation_enhanced.h"

#ifdef _WIN32
#include <windows.h>
#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
int clock_gettime(int clk_id, struct timespec *tp) {
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);
    tp->tv_sec = (time_t)(count.QuadPart / freq.QuadPart);
    tp->tv_nsec = (long)(((count.QuadPart % freq.QuadPart) * 1000000000) / freq.QuadPart);
    return 0;
}
#endif
#endif

#define NUM_REQUESTS 100
#define CONCURRENT_THREADS 5

typedef struct {
    int thread_id;
    int successful_requests;
    int failed_requests;
    double total_time;
    double min_time;
    double max_time;
} benchmark_result_t;

static void run_benchmark_test(benchmark_result_t *result) {
    CURL *curl;
    CURLcode res;
    struct timespec start, end;
    
    curl = curl_easy_init();
    if(curl) {
        for(int i = 0; i < NUM_REQUESTS / CONCURRENT_THREADS; i++) {
            clock_gettime(CLOCK_MONOTONIC, &start);
            
            // Test account validation
            curl_easy_setopt(curl, CURLOPT_URL, "https://api-mock.payments.jpmorgan.com/tsapi/v2/validations/accounts");
            curl_easy_setopt(curl, CURLOPT_POSTFIELDS, 
                "{\"requestId\":\"test-123\",\"account\":{\"accountNumber\":\"123456789\",\"financialInstitutionId\":{\"clearingSystemId\":{\"id\":\"021000021\",\"idType\":\"ABA\"}}}}");
            
            struct curl_slist *headers = NULL;
            headers = curl_slist_append(headers, "Content-Type: application/json");
            headers = curl_slist_append(headers, "x-client-id: CLIENTID");
            headers = curl_slist_append(headers, "x-program-id: PROGRAMID");
            curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
            
            res = curl_easy_perform(curl);
            
            clock_gettime(CLOCK_MONOTONIC, &end);
            
            double elapsed = (end.tv_sec - start.tv_sec) * 1000.0 + 
                           (end.tv_nsec - start.tv_nsec) / 1000000.0;
            
            if(res == CURLE_OK) {
                result->successful_requests++;
            } else {
                result->failed_requests++;
            }
            
            result->total_time += elapsed;
            if(elapsed < result->min_time || result->min_time == 0) result->min_time = elapsed;
            if(elapsed > result->max_time) result->max_time = elapsed;
            
            curl_slist_free_all(headers);
        }
        
        curl_easy_cleanup(curl);
    }
}

int main() {
    printf("🚀 JPMorgan Validation Services Performance Benchmark\n");
    printf("=====================================================\n\n");
    
    printf("Configuration:\n");
    printf("- Total Requests: %d\n", NUM_REQUESTS);
    printf("- Concurrent Threads: %d\n", CONCURRENT_THREADS);
    printf("- Requests per Thread: %d\n", NUM_REQUESTS / CONCURRENT_THREADS);
    
    benchmark_result_t results[CONCURRENT_THREADS];
    
    // Initialize results
    for(int i = 0; i < CONCURRENT_THREADS; i++) {
        results[i].thread_id = i;
        results[i].successful_requests = 0;
        results[i].failed_requests = 0;
        results[i].total_time = 0;
        results[i].min_time = 0;
        results[i].max_time = 0;
    }
    
    printf("\n📊 Running Performance Tests...\n");
    
    // Run benchmark tests
    for(int i = 0; i < CONCURRENT_THREADS; i++) {
        run_benchmark_test(&results[i]);
    }
    
    // Calculate totals
    int total_success = 0, total_fail = 0;
    double total_time = 0, min_time = 0, max_time = 0;
    
    for(int i = 0; i < CONCURRENT_THREADS; i++) {
        total_success += results[i].successful_requests;
        total_fail += results[i].failed_requests;
        total_time += results[i].total_time;
        
        if(min_time == 0 || results[i].min_time < min_time) min_time = results[i].min_time;
        if(results[i].max_time > max_time) max_time = results[i].max_time;
    }
    
    printf("\n📈 Results Summary:\n");
    printf("==================\n");
    printf("✅ Successful Requests: %d\n", total_success);
    printf("❌ Failed Requests: %d\n", total_fail);
    printf("⚡ Average Response Time: %.2f ms\n", total_time / total_success);
    printf("🔥 Fastest Response: %.2f ms\n", min_time);
    printf("🐌 Slowest Response: %.2f ms\n", max_time);
    printf("📊 Success Rate: %.2f%%\n", (double)total_success / NUM_REQUESTS * 100);
    
    return 0;
}
