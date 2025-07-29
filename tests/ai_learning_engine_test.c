/***********************************************************************
 *                                   _    _                            *
 *                    ___ ___  _ __ | |_ (_)___ ___                      *
 *                   / __/ _ \| '_ \| __|| / __/ __|                     *
 *                  | (_| (_) | | | | |_ | \__ \__ \                     *
 *                   \___\___/|_| |_|\__||_|___/___/                     *
 *                                                                       *
 *  Test Suite for AI Learning Engine - Comprehensive testing framework  *
 *  Version 1.0.0                                                       *
 *                                                                       *
 *  Copyright (C) 2024 AI-Enhanced Curl Project                        *
 *  Licensed under MIT                                                   *
 *                                                                       *
 ************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

/* Include the AI learning engine header */
#include "../include/curl/ai_learning_engine.h"

/* Test configuration */
#define TEST_MODEL_NAME "test_model"
#define TEST_SAMPLES 1000
#define TEST_FEATURES 10
#define TEST_OUTPUTS 1

/* Forward declarations */
static void test_engine_initialization(void);
static void test_engine_configuration(void);
static void test_engine_training(void);
static void test_engine_feedback(void);
static void test_engine_prediction(void);
static void test_engine_save_load(void);
static void test_engine_metrics(void);
static void test_edge_cases(void);
static void test_memory_management(void);
static void test_performance_benchmarks(void);

/* Test data generation */
static void generate_test_data(double *features, double *labels, size_t samples, size_t features_per_sample)
{
    for(size_t i = 0; i < samples; i++) {
        for(size_t j = 0; j < features_per_sample; j++) {
            features[i * features_per_sample + j] = (double)rand() / RAND_MAX;
        }
        labels[i] = (double)rand() / RAND_MAX; /* Simple regression target */
    }
}

/* Main test runner */
int main(void)
{
    printf("=== AI Learning Engine Test Suite ===\n");
    printf("Running comprehensive tests...\n\n");

    /* Initialize random seed */
    srand((unsigned int)time(NULL));

    /* Run test cases */
    test_engine_initialization();
    test_engine_configuration();
    test_engine_training();
    test_engine_feedback();
    test_engine_prediction();
    test_engine_save_load();
    test_engine_metrics();
    test_edge_cases();
    test_memory_management();
    test_performance_benchmarks();

    printf("\n=== All tests completed successfully! ===\n");
    return 0;
}

/* Test 1: Engine initialization */
static void test_engine_initialization(void)
{
    printf("Test 1: Engine initialization... ");
    
    ai_learning_engine_t *engine = ai_learning_engine_init(TEST_MODEL_NAME);
    assert(engine != NULL);
    
    ai_learning_engine_cleanup(engine);
    printf("PASSED\n");
}

/* Test 2: Engine configuration */
static void test_engine_configuration(void)
{
    printf("Test 2: Engine configuration... ");
    
    ai_learning_engine_t *engine = ai_learning_engine_init(TEST_MODEL_NAME);
    assert(engine != NULL);
    
    learning_config_t config = {
        .initial_learning_rate = 0.01,
        .min_learning_rate = 0.0001,
        .decay_rate = 0.95,
        .decay_strategy = LEARNING_RATE_EXPONENTIAL,
        .batch_size = 64,
        .max_epochs = 500,
        .convergence_threshold = 0.0001,
        .regularization_strength = 0.001,
        .enable_early_stopping = true,
        .patience_epochs = 25
    };
    
    int result = ai_learning_engine_configure(engine, &config);
    assert(result == 0);
    
    ai_learning_engine_cleanup(engine);
    printf("PASSED\n");
}

/* Test 3: Engine training */
static void test_engine_training(void)
{
    printf("Test 3: Engine training... ");
    
    ai_learning_engine_t *engine = ai_learning_engine_init(TEST_MODEL_NAME);
    assert(engine != NULL);
    
    /* Generate test data */
    double features[TEST_SAMPLES * TEST_FEATURES];
    double labels[TEST_SAMPLES];
    generate_test_data(features, labels, TEST_SAMPLES, TEST_FEATURES);
    
    /* Train model */
    int result = ai_learning_engine_train(engine, features, labels, 
                                               TEST_SAMPLES, TEST_FEATURES);
    assert(result == 0);
    
    ai_learning_engine_cleanup(engine);
    printf("PASSED\n");
}

/* Test 4: Reinforcement learning feedback */
static void test_engine_feedback(void)
{
    printf("Test 4: Reinforcement learning feedback... ");
    
    ai_learning_engine_t *engine = ai_learning_engine_init(TEST_MODEL_NAME);
    assert(engine != NULL);
    
    /* Test feedback system */
    int result = ai_learning_engine_feedback(engine, 0, 0, 1.0, 1);
    assert(result == 0);
    
    ai_learning_engine_cleanup(engine);
    printf("PASSED\n");
}

/* Test 5: Model prediction */
static void test_engine_prediction(void)
{
    printf("Test 5: Model prediction... ");
    
    ai_learning_engine_t *engine = ai_learning_engine_init(TEST_MODEL_NAME);
    assert(engine != NULL);
    
    /* Generate and train with test data */
    double features[TEST_SAMPLES * TEST_FEATURES];
    double labels[TEST_SAMPLES];
    generate_test_data(features, labels, TEST_SAMPLES, TEST_FEATURES);
    
    int result = ai_learning_engine_train(engine, features, labels,
                                               TEST_SAMPLES, TEST_FEATURES);
    assert(result == 0);
    
    /* Test prediction */
    double input[TEST_FEATURES];
    double output[TEST_OUTPUTS];
    
    for(size_t i = 0; i < TEST_FEATURES; i++) {
        input[i] = (double)rand() / RAND_MAX;
    }
    
    result = ai_learning_engine_predict(engine, input, TEST_FEATURES,
                                        output, TEST_OUTPUTS);
    assert(result == 0);
    
    ai_learning_engine_cleanup(engine);
    printf("PASSED\n");
}

/* Test 6: Model save/load */
static void test_engine_save_load(void)
{
    printf("Test 6: Model save/load... ");
    
    ai_learning_engine_t *engine = ai_learning_engine_init(TEST_MODEL_NAME);
    assert(engine != NULL);
    
    /* Generate and train with test data */
    double features[TEST_SAMPLES * TEST_FEATURES];
    double labels[TEST_SAMPLES];
    generate_test_data(features, labels, TEST_SAMPLES, TEST_FEATURES);
    
    int result = ai_learning_engine_train(engine, features, labels,
                                               TEST_SAMPLES, TEST_FEATURES);
    assert(result == 0);
    
    /* Save model */
    const char *test_file = "test_model.bin";
    result = ai_learning_engine_save(engine, test_file);
    assert(result == 0);
    
    /* Load model */
    ai_learning_engine_t *loaded_engine = ai_learning_engine_load(test_file);
    assert(loaded_engine != NULL);
    
    /* Cleanup */
    ai_learning_engine_cleanup(engine);
    ai_learning_engine_cleanup(loaded_engine);
    
    /* Remove test file */
    remove(test_file);
    printf("PASSED\n");
}

/* Test 7: Metrics retrieval */
static void test_engine_metrics(void)
{
    printf("Test 7: Metrics retrieval... ");
    
    ai_learning_engine_t *engine = ai_learning_engine_init(TEST_MODEL_NAME);
    assert(engine != NULL);
    
    /* Generate and train with test data */
    double features[TEST_SAMPLES * TEST_FEATURES];
    double labels[TEST_SAMPLES];
    generate_test_data(features, labels, TEST_SAMPLES, TEST_FEATURES);
    
    int result = ai_learning_engine_train(engine, features, labels,
                                               TEST_SAMPLES, TEST_FEATURES);
    assert(result == 0);
    
    /* Get metrics */
    model_metrics_t metrics;
    result = ai_learning_engine_get_metrics(engine, &metrics);
    assert(result == 0);
    assert(metrics.training_samples == TEST_SAMPLES);
    
    ai_learning_engine_cleanup(engine);
    printf("PASSED\n");
}

/* Test 8: Edge cases */
static void test_edge_cases(void)
{
    printf("Test 8: Edge cases... ");
    
    /* Test NULL handling */
    ai_learning_engine_t *engine = ai_learning_engine_init(NULL);
    assert(engine != NULL);
    ai_learning_engine_cleanup(engine);
    
    /* Test empty training data */
    engine = ai_learning_engine_init(TEST_MODEL_NAME);
    assert(engine != NULL);
    
    int result = ai_learning_engine_train(engine, NULL, NULL, 0, 0);
    assert(result == 1); /* Expect failure */
    
    ai_learning_engine_cleanup(engine);
    printf("PASSED\n");
}

/* Test 9: Memory management */
static void test_memory_management(void)
{
    printf("Test 9: Memory management... ");
    
    /* Test multiple create/cleanup cycles */
    for(int i = 0; i < 100; i++) {
        ai_learning_engine_t *engine = ai_learning_engine_init(TEST_MODEL_NAME);
        assert(engine != NULL);
        ai_learning_engine_cleanup(engine);
    }
    
    printf("PASSED\n");
}

/* Test 10: Performance benchmarks */
static void test_performance_benchmarks(void)
{
    printf("Test 10: Performance benchmarks... ");
    
    clock_t start, end;
    double cpu_time_used;
    
    /* Benchmark initialization */
    start = clock();
    ai_learning_engine_t *engine = ai_learning_engine_init(TEST_MODEL_NAME);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\n    Initialization: %.4f seconds", cpu_time_used);
    
    /* Benchmark training */
    double features[TEST_SAMPLES * TEST_FEATURES];
    double labels[TEST_SAMPLES];
    generate_test_data(features, labels, TEST_SAMPLES, TEST_FEATURES);
    
    start = clock();
    int result = ai_learning_engine_train(engine, features, labels,
                                               TEST_SAMPLES, TEST_FEATURES);
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\n    Training: %.4f seconds", cpu_time_used);
    
    /* Benchmark prediction */
    double input[TEST_FEATURES];
    double output[TEST_OUTPUTS];
    for(size_t i = 0; i < TEST_FEATURES; i++) {
        input[i] = (double)rand() / RAND_MAX;
    }
    
    start = clock();
    for(size_t i = 0; i < 1000; i++) {
        ai_learning_engine_predict(engine, input, TEST_FEATURES,
                                  output, TEST_OUTPUTS);
    }
    end = clock();
    cpu_time_used = ((double)(end - start)) / CLOCKS_PER_SEC;
    printf("\n    1000 predictions: %.4f seconds", cpu_time_used);
    
    ai_learning_engine_cleanup(engine);
    printf("\n    Performance tests PASSED\n");
}
