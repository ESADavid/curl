/***********************************************************************
 *                                   _    _                            *
 *                    ___ ___  _ __ | |_ (_)___ ___                      *
 *                   / __/ _ \| '_ \| __|| / __/ __|                     *
 *                  | (_| (_) | | | | |_ | \__ \__ \                     *
 *                   \___\___/|_| |_|\__||_|___/___/                     *
 *                                                                       *
 *  AI Learning Engine Header for curl - Adaptive ML models with feedback *
 *  Version 1.0.0                                                       *
 *                                                                       *
 *  Copyright (C) 2024 AI-Enhanced Curl Project                          *
 *  Licensed under MIT                                                   *
 *                                                                       *
 ************************************************************************/

#ifndef CURL_AI_LEARNING_ENGINE_H
#define CURL_AI_LEARNING_ENGINE_H

#include "curl_setup.h"
#include <stddef.h>
#include <time.h>

#ifdef  __cplusplus
extern "C" {
#endif

/* Learning rate decay strategies */
typedef enum {
    LEARNING_RATE_CONSTANT = 0,
    LEARNING_RATE_EXPONENTIAL,
    LEARNING_RATE_INVERSE_TIME,
    LEARNING_RATE_POLYNOMIAL
} learning_rate_decay_t;

/* Feedback types for reinforcement learning */
typedef enum {
    FEEDBACK_POSITIVE = 1,
    FEEDBACK_NEGATIVE = -1,
    FEEDBACK_NEUTRAL = 0
} feedback_type_t;

/* Model performance metrics */
typedef struct {
    double accuracy;
    double precision;
    double recall;
    double f1_score;
    double auc_roc;
    size_t training_samples;
    size_t validation_samples;
    time_t last_updated;
} model_metrics_t;

/* Adaptive learning configuration */
typedef struct {
    double initial_learning_rate;
    double min_learning_rate;
    double decay_rate;
    learning_rate_decay_t decay_strategy;
    size_t batch_size;
    size_t max_epochs;
    double convergence_threshold;
    double regularization_strength;
    bool enable_early_stopping;
    size_t patience_epochs;
} learning_config_t;

/* Main AI learning engine context */
typedef struct ai_learning_engine ai_learning_engine_t;

/* Function declarations */

/* Initialize new AI learning engine */
ai_learning_engine_t *ai_learning_engine_init(const char *model_name);

/* Configure learning parameters */
CURLcode ai_learning_engine_configure(ai_learning_engine_t *engine, 
                                     const learning_config_t *config);

/* Train model with feedback data */
CURLcode ai_learning_engine_train(ai_learning_engine_t *engine, 
                                 double *features, 
                                 double *labels,
                                 size_t num_samples,
                                 size_t num_features);

/* Reinforcement learning feedback */
CURLcode ai_learning_engine_feedback(ai_learning_engine_t *engine,
                                    size_t state,
                                    size_t action,
                                    double reward,
                                    size_t next_state);

/* Predict using trained model */
CURLcode ai_learning_engine_predict(ai_learning_engine_t *engine,
                                   double *input,
                                   size_t input_size,
                                   double *output,
                                   size_t output_size);

/* Get model performance metrics */
CURLcode ai_learning_engine_get_metrics(ai_learning_engine_t *engine,
                                      model_metrics_t *metrics);

/* Save trained model to file */
CURLcode ai_learning_engine_save(ai_learning_engine_t *engine, const char *filepath);

/* Load trained model from file */
ai_learning_engine_t *ai_learning_engine_load(const char *filepath);

/* Cleanup resources */
void ai_learning_engine_cleanup(ai_learning_engine_t *engine);

#ifdef  __cplusplus
}
#endif

#endif /* CURL_AI_LEARNING_ENGINE_H */
