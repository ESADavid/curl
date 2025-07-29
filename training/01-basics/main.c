/**
 * JPMorgan Chase Validation Services - Basic Training Module
 * Module 01: Foundation Concepts
 * 
 * This module introduces the fundamental concepts of validation services
 * integration with curl/libcurl.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include "../../include/curl/validation.h"

// Training exercise structure
typedef struct {
    char* name;
    char* description;
    int (*execute)(void);
} training_exercise;

// Exercise 1: Basic validation initialization
int exercise_basic_init(void) {
    printf("=== Exercise 1: Basic Validation Initialization ===\n");
    
    validation_context* ctx = validation_create_context();
    if (!ctx) {
        printf("❌ Failed to create validation context\n");
        return 1;
    }
    
    printf("✅ Validation context created successfully\n");
    printf("   Context ID: %p\n", (void*)ctx);
    
    validation_destroy_context(ctx);
    printf("✅ Context cleaned up properly\n");
    
    return 0;
}

// Exercise 2: Simple account validation
int exercise_simple_account_validation(void) {
    printf("\n=== Exercise 2: Simple Account Validation ===\n");
    
    validation_context* ctx = validation_create_context();
    if (!ctx) {
        printf("❌ Failed to create validation context\n");
        return 1;
    }
    
    // Sample account data
    validation_request req = {
        .type = VALIDATION_TYPE_ACCOUNT,
        .data.account = {
            .account_number = "1234567890",
            .routing_number = "021000021",
            .account_type = ACCOUNT_TYPE_CHECKING
        }
    };
    
    validation_result result;
    int ret = validation_validate(ctx, &req, &result);
    
    if (ret == 0) {
        printf("✅ Account validation completed\n");
        printf("   Status: %s\n", result.is_valid ? "VALID" : "INVALID");
        printf("   Response: %s\n", result.response_message);
    } else {
        printf("❌ Validation failed with error: %d\n", ret);
    }
    
    validation_destroy_context(ctx);
    return ret;
}

// Exercise 3: Error handling basics
int exercise_error_handling(void) {
    printf("\n=== Exercise 3: Basic Error Handling ===\n");
    
    validation_context* ctx = validation_create_context();
    if (!ctx) {
        printf("❌ Failed to create validation context\n");
        return 1;
    }
    
    // Test with invalid data
    validation_request req = {
        .type = VALIDATION_TYPE_ACCOUNT,
        .data.account = {
            .account_number = "invalid",
            .routing_number = "000000000",
            .account_type = ACCOUNT_TYPE_UNKNOWN
        }
    };
    
    validation_result result;
    int ret = validation_validate(ctx, &req, &result);
    
    printf("✅ Error handling test completed\n");
    printf("   Return code: %d\n", ret);
    printf("   Error message: %s\n", result.error_message);
    
    validation_destroy_context(ctx);
    return 0;
}

// Main training program
int main(void) {
    printf("🎓 JPMorgan Chase Validation Services - Basic Training\n");
    printf("=====================================================\n");
    
    // Initialize libcurl
    CURLcode res = curl_global_init(CURL_GLOBAL_DEFAULT);
    if (res != CURLE_OK) {
        printf("❌ Failed to initialize libcurl: %s\n", curl_easy_strerror(res));
        return 1;
    }
    
    // Training exercises
    training_exercise exercises[] = {
        {"Basic Initialization", "Learn to create and destroy validation contexts", exercise_basic_init},
        {"Simple Validation", "Perform basic account validation", exercise_simple_account_validation},
        {"Error Handling", "Handle validation errors gracefully", exercise_error_handling}
    };
    
    int total_exercises = sizeof(exercises) / sizeof(exercises[0]);
    int passed = 0;
    
    printf("\n📚 Starting %d training exercises...\n", total_exercises);
    
    for (int i = 0; i < total_exercises; i++) {
        printf("\n[%d/%d] %s\n", i + 1, total_exercises, exercises[i].name);
        printf("   %s\n", exercises[i].description);
        
        if (exercises[i].execute() == 0) {
            passed++;
            printf("✅ PASSED\n");
        } else {
            printf("❌ FAILED\n");
        }
    }
    
    printf("\n🎯 Training Summary:\n");
    printf("   Total exercises: %d\n", total_exercises);
    printf("   Passed: %d\n", passed);
    printf("   Failed: %d\n", total_exercises - passed);
    
    if (passed == total_exercises) {
        printf("🎉 Congratulations! You've completed the basic training!\n");
    } else {
        printf("💪 Keep practicing! Review the failed exercises and try again.\n");
    }
    
    // Cleanup
    curl_global_cleanup();
    
    return passed == total_exercises ? 0 : 1;
}
