/**
 * @file methods/ann/loss_functions/hinge_embedding_loss_impl.hpp
 * @author Lakshya Ojha
 *
 * Implementation of the Hinge Embedding loss function.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */

#ifndef MLPACK_METHODS_ANN_LOSS_FUNCTION_HINGE_EMBEDDING_LOSS_IMPL_HPP
#define MLPACK_METHODS_ANN_LOSS_FUNCTION_HINGE_EMBEDDING_LOSS_IMPL_HPP

// In case it hasn't yet been included.
#include "hinge_embedding_loss.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

template<typename InputDataType, typename OutputDataType>
HingeEmbeddingLoss<InputDataType, OutputDataType>
  ::HingeEmbeddingLoss(const bool reduction) : reduction(reduction)
{
  // Nothing to do here.
}

template<typename InputDataType, typename OutputDataType>
template<typename PredictionType, typename TargetType>
typename PredictionType::elem_type
HingeEmbeddingLoss<InputDataType, OutputDataType>::Forward(
    const PredictionType& prediction,
    const TargetType& target)
{
  PredictionType loss = (1 - target) / 2 + prediction % (target);
  typename PredictionType::elem_type lossSum = arma::accu(loss);

  if (reduction)
    return lossSum;

  return lossSum / target.n_elem;
}

template<typename InputDataType, typename OutputDataType>
template<typename PredictionType, typename TargetType, typename LossType>
void HingeEmbeddingLoss<InputDataType, OutputDataType>::Backward(
    const PredictionType& prediction,
    const TargetType& target,
    LossType& loss)
{
  loss = target;

  if (!reduction)
    loss = loss / target.n_elem;
}

template<typename InputDataType, typename OutputDataType>
template<typename Archive>
void HingeEmbeddingLoss<InputDataType, OutputDataType>::serialize(
    Archive& ar,
    const uint32_t /* version */)
{
  ar(CEREAL_NVP(reduction));
}

} // namespace ann
} // namespace mlpack

#endif
