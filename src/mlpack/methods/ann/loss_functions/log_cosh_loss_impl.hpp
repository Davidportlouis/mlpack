/**
 * @file methods/ann/loss_functions/log_cosh_loss_impl.hpp
 * @author Kartik Dutt
 *
 * Implementation of the Log-Hyperbolic-Cosine loss function.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */

#ifndef MLPACK_METHODS_ANN_LOSS_FUNCTION_LOG_COSH_LOSS_IMPL_HPP
#define MLPACK_METHODS_ANN_LOSS_FUNCTION_LOG_COSH_LOSS_IMPL_HPP

// In case it hasn't yet been included.
#include "log_cosh_loss.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

template<typename InputDataType, typename OutputDataType>
LogCoshLoss<InputDataType, OutputDataType>::LogCoshLoss(
    const double a, const bool reduction) :
    a(a) , reduction(reduction)
{
  Log::Assert(a > 0, "Hyper-Parameter \'a\' must be positive");
}

template<typename InputDataType, typename OutputDataType>
template<typename PredictionType, typename TargetType>
typename PredictionType::elem_type
LogCoshLoss<InputDataType, OutputDataType>::Forward(
    const PredictionType& prediction,
    const TargetType& target)
{
  typename PredictionType::elem_type lossSum = 
      arma::accu(arma::log(arma::cosh(a * (target - prediction)))) / a;

  if (reduction)
    return lossSum;

  return lossSum / target.n_elem;
}

template<typename InputDataType, typename OutputDataType>
template<typename PredictionType, typename TargetType, typename LossType>
void LogCoshLoss<InputDataType, OutputDataType>::Backward(
    const PredictionType& prediction,
    const TargetType& target,
    LossType& loss)
{
  loss = arma::tanh(a * (target - prediction));

  if (!reduction)
    loss = loss / target.n_elem;
}

template<typename InputDataType, typename OutputDataType>
template<typename Archive>
void LogCoshLoss<InputDataType, OutputDataType>::serialize(
    Archive& ar,
    const uint32_t /* version */)
{
  ar(CEREAL_NVP(a));
  ar(CEREAL_NVP(reduction));
}

} // namespace ann
} // namespace mlpack

#endif
