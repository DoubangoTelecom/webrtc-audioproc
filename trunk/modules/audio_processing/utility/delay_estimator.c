/*
 *  Copyright (c) 2012 The WebRTC project authors. All Rights Reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#include "delay_estimator.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

// Number of right shifts for scaling is linearly depending on number of bits in
// the far-end binary spectrum.
static const int kShiftsAtZero = 13;  // Right shifts at zero binary spectrum.
static const int kShiftsLinearSlope = 3;

static const int32_t kProbabilityOffset = 1024;  // 2 in Q9.
static const int32_t kProbabilityLowerLimit = 8704;  // 17 in Q9.
static const int32_t kProbabilityMinSpread = 2816;  // 5.5 in Q9.

// Counts and returns number of bits of a 32-bit word.
static int BitCount(uint32_t u32) {
  uint32_t tmp = u32 - ((u32 >> 1) & 033333333333) -
      ((u32 >> 2) & 011111111111);
  tmp = ((tmp + (tmp >> 3)) & 030707070707);
  tmp = (tmp + (tmp >> 6));
  tmp = (tmp + (tmp >> 12) + (tmp >> 24)) & 077;

  return ((int) tmp);
}

// Compares the |binary_vector| with all rows of the |binary_matrix| and counts
// per row the number of times they have the same value.
//
// Inputs:
//      - binary_vector     : binary "vector" stored in a long
//      - binary_matrix     : binary "matrix" stored as a vector of long
//      - matrix_size       : size of binary "matrix"
//
// Output:
//      - bit_counts        : "Vector" stored as a long, containing for each
//                            row the number of times the matrix row and the
//                            input vector have the same value
//
static void BitCountComparison(uint32_t binary_vector,
                               const uint32_t* binary_matrix,
                               int matrix_size,
                               int32_t* bit_counts) {
  int n = 0;

  // Compare |binary_vector| with all rows of the |binary_matrix|
  for (; n < matrix_size; n++) {
    bit_counts[n] = (int32_t) BitCount(binary_vector ^ binary_matrix[n]);
  }
}

void WebRtc_FreeBinaryDelayEstimator(BinaryDelayEstimator* handle) {

  if (handle == NULL) {
    return;
  }

  free(handle->mean_bit_counts);
  handle->mean_bit_counts = NULL;

  free(handle->bit_counts);
  handle->bit_counts = NULL;

  free(handle->binary_far_history);
  handle->binary_far_history = NULL;

  free(handle->binary_near_history);
  handle->binary_near_history = NULL;

  free(handle->far_bit_counts);
  handle->far_bit_counts = NULL;

  free(handle);
}

BinaryDelayEstimator* WebRtc_CreateBinaryDelayEstimator(int max_delay,
                                                        int lookahead) {
  BinaryDelayEstimator* self = NULL;
  int history_size = max_delay + lookahead;  // Must be > 1 for buffer shifting.

  if ((max_delay >= 0) && (lookahead >= 0) && (history_size > 1)) {
    // Sanity conditions fulfilled.
    self = malloc(sizeof(BinaryDelayEstimator));
  }

  if (self != NULL) {
    int malloc_fail = 0;

    self->mean_bit_counts = NULL;
    self->bit_counts = NULL;
    self->binary_far_history = NULL;
    self->far_bit_counts = NULL;
    self->binary_near_history = NULL;

    self->history_size = history_size;
    self->near_history_size = lookahead + 1;

    // Allocate memory for spectrum buffers.
    self->mean_bit_counts = malloc(history_size * sizeof(int32_t));
    malloc_fail |= (self->mean_bit_counts == NULL);

    self->bit_counts = malloc(history_size * sizeof(int32_t));
    malloc_fail |= (self->bit_counts == NULL);

    // Allocate memory for history buffers.
    self->binary_far_history = malloc(history_size * sizeof(uint32_t));
    malloc_fail |= (self->binary_far_history == NULL);

    self->binary_near_history = malloc((lookahead + 1) * sizeof(uint32_t));
    malloc_fail |= (self->binary_near_history == NULL);

    self->far_bit_counts = malloc(history_size * sizeof(int));
    malloc_fail |= (self->far_bit_counts == NULL);

    if (malloc_fail) {
      WebRtc_FreeBinaryDelayEstimator(self);
      self = NULL;
    }
  }

  return self;
}

void WebRtc_InitBinaryDelayEstimator(BinaryDelayEstimator* handle) {
  int i = 0;
  assert(handle != NULL);

  memset(handle->bit_counts, 0, sizeof(int32_t) * handle->history_size);
  memset(handle->binary_far_history, 0,
         sizeof(uint32_t) * handle->history_size);
  memset(handle->binary_near_history, 0,
         sizeof(uint32_t) * handle->near_history_size);
  memset(handle->far_bit_counts, 0, sizeof(int) * handle->history_size);
  for (i = 0; i < handle->history_size; ++i) {
    handle->mean_bit_counts[i] = (20 << 9);  // 20 in Q9.
  }
  handle->minimum_probability = (32 << 9);  // 32 in Q9.
  handle->last_delay_probability = (32 << 9);  // 32 in Q9.

  // Default return value if we're unable to estimate. -1 is used for errors.
  handle->last_delay = -2;
}

int WebRtc_ProcessBinarySpectrum(BinaryDelayEstimator* handle,
                                 uint32_t binary_far_spectrum,
                                 uint32_t binary_near_spectrum) {
  int i = 0;
  int candidate_delay = -1;

  int32_t value_best_candidate = 16384;  // 32 in Q9, (max |mean_bit_counts|).
  int32_t value_worst_candidate = 0;

  assert(handle != NULL);
  // Shift binary spectrum history and insert current |binary_far_spectrum|.
  memmove(&(handle->binary_far_history[1]), &(handle->binary_far_history[0]),
          (handle->history_size - 1) * sizeof(uint32_t));
  handle->binary_far_history[0] = binary_far_spectrum;

  // Shift history of far-end binary spectrum bit counts and insert bit count
  // of current |binary_far_spectrum|.
  memmove(&(handle->far_bit_counts[1]), &(handle->far_bit_counts[0]),
          (handle->history_size - 1) * sizeof(int));
  handle->far_bit_counts[0] = BitCount(binary_far_spectrum);

  if (handle->near_history_size > 1) {
    // If we apply lookahead, shift near-end binary spectrum history. Insert
    // current |binary_near_spectrum| and pull out the delayed one.
    memmove(&(handle->binary_near_history[1]),
            &(handle->binary_near_history[0]),
            (handle->near_history_size - 1) * sizeof(uint32_t));
    handle->binary_near_history[0] = binary_near_spectrum;
    binary_near_spectrum =
        handle->binary_near_history[handle->near_history_size - 1];
  }

  // Compare with delayed spectra and store the |bit_counts| for each delay.
  BitCountComparison(binary_near_spectrum,
                     handle->binary_far_history,
                     handle->history_size,
                     handle->bit_counts);

  // Update |mean_bit_counts|, which is the smoothed version of |bit_counts|.
  for (i = 0; i < handle->history_size; i++) {
    // |bit_counts| is constrained to [0, 32], meaning we can smooth with a
    // factor up to 2^26. We use Q9.
    int32_t bit_count = (handle->bit_counts[i] << 9);  // Q9.

    // Update |mean_bit_counts| only when far-end signal has something to
    // contribute. If |far_bit_counts| is zero the far-end signal is weak and
    // we likely have a poor echo condition, hence don't update.
    if (handle->far_bit_counts[i] > 0) {
      // Make number of right shifts piecewise linear w.r.t. |far_bit_counts|.
      int shifts = kShiftsAtZero;
      shifts -= (kShiftsLinearSlope * handle->far_bit_counts[i]) >> 4;
      WebRtc_MeanEstimatorFix(bit_count, shifts, &(handle->mean_bit_counts[i]));
    }
  }

  // Find |candidate_delay|, |value_best_candidate| and |value_worst_candidate|
  // of |mean_bit_counts|.
  for (i = 0; i < handle->history_size; i++) {
    if (handle->mean_bit_counts[i] < value_best_candidate) {
      value_best_candidate = handle->mean_bit_counts[i];
      candidate_delay = i;
    }
    if (handle->mean_bit_counts[i] > value_worst_candidate) {
      value_worst_candidate = handle->mean_bit_counts[i];
    }
  }

  // The |value_best_candidate| is a good indicator on the probability of
  // |candidate_delay| being an accurate delay (a small |value_best_candidate|
  // means a good binary match). In the following sections we make a decision
  // whether to update |last_delay| or not.
  // 1) If the difference bit counts between the best and the worst delay
  //    candidates is too small we consider the situation to be unreliable and
  //    don't update |last_delay|.
  // 2) If the situation is reliable we update |last_delay| if the value of the
  //    best candidate delay has a value less than
  //     i) an adaptive threshold |minimum_probability|, or
  //    ii) this corresponding value |last_delay_probability|, but updated at
  //        this time instant.

  // Update |minimum_probability|.
  if ((handle->minimum_probability > kProbabilityLowerLimit) &&
      (value_worst_candidate - value_best_candidate > kProbabilityMinSpread)) {
    // The "hard" threshold can't be lower than 17 (in Q9).
    // The valley in the curve also has to be distinct, i.e., the
    // difference between |value_worst_candidate| and |value_best_candidate| has
    // to be large enough.
    int32_t threshold = value_best_candidate + kProbabilityOffset;
    if (threshold < kProbabilityLowerLimit) {
      threshold = kProbabilityLowerLimit;
    }
    if (handle->minimum_probability > threshold) {
      handle->minimum_probability = threshold;
    }
  }
  // Update |last_delay_probability|.
  // We use a Markov type model, i.e., a slowly increasing level over time.
  handle->last_delay_probability++;
  if (value_worst_candidate > value_best_candidate + kProbabilityOffset) {
    // Reliable delay value for usage.
    if (value_best_candidate < handle->minimum_probability) {
      handle->last_delay = candidate_delay;
    }
    if (value_best_candidate < handle->last_delay_probability) {
      handle->last_delay = candidate_delay;
      // Reset |last_delay_probability|.
      handle->last_delay_probability = value_best_candidate;
    }
  }

  return handle->last_delay;
}

int WebRtc_binary_last_delay(BinaryDelayEstimator* handle) {
  assert(handle != NULL);
  return handle->last_delay;
}

void WebRtc_MeanEstimatorFix(int32_t new_value,
                             int factor,
                             int32_t* mean_value) {
  int32_t diff = new_value - *mean_value;

  // mean_new = mean_value + ((new_value - mean_value) >> factor);
  if (diff < 0) {
    diff = -((-diff) >> factor);
  } else {
    diff = (diff >> factor);
  }
  *mean_value += diff;
}
