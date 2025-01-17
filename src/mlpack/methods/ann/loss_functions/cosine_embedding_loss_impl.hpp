/**
 * @file methods/ann/loss_functions/cosine_embedding_loss_impl.hpp
 * @author Kartik Dutt
 *
 * Implementation of the Cosine Embedding loss function.
 *
 * mlpack is free software; you may redistribute it and/or modify it under the
 * terms of the 3-clause BSD license.  You should have received a copy of the
 * 3-clause BSD license along with mlpack.  If not, see
 * http://www.opensource.org/licenses/BSD-3-Clause for more information.
 */
#ifndef MLPACK_METHODS_ANN_LOSS_FUNCTION_COSINE_EMBEDDING_IMPL_HPP
#define MLPACK_METHODS_ANN_LOSS_FUNCTION_COSINE_EMBEDDING_IMPL_HPP

// In case it hasn't yet been included.
#include "cosine_embedding_loss.hpp"

namespace mlpack {
namespace ann /** Artificial Neural Network. */ {

template<typename InputDataType, typename OutputDataType>
CosineEmbeddingLoss<InputDataType, OutputDataType>::CosineEmbeddingLoss(
    const double margin, const bool similarity, const bool reduction):
    margin(margin), similarity(similarity), reduction(reduction)
{
  // Nothing to do here.
}

template<typename InputDataType, typename OutputDataType>
template<typename PredictionType, typename TargetType>
typename PredictionType::elem_type
CosineEmbeddingLoss<InputDataType, OutputDataType>::Forward(
    const PredictionType& prediction,
    const TargetType& target)
{
  typedef typename PredictionType::elem_type ElemType;

  const size_t cols = prediction.n_cols;
  const size_t batchSize = prediction.n_elem / cols;
  if (arma::size(prediction) != arma::size(target))
    Log::Fatal << "Input Tensors must have same dimensions." << std::endl;

  arma::colvec inputTemp1 = arma::vectorise(prediction);
  arma::colvec inputTemp2 = arma::vectorise(target);
  ElemType lossSum = 0.0;

  for (size_t i = 0; i < inputTemp1.n_elem; i += cols)
  {
    const ElemType cosDist = kernel::CosineDistance::Evaluate(
        inputTemp1(arma::span(i, i + cols - 1)), inputTemp2(arma::span(i,
        i + cols - 1)));
    if (similarity)
      lossSum += 1 - cosDist;
    else
    {
      const ElemType currentLoss = cosDist - margin;
      lossSum += currentLoss > 0 ? currentLoss : 0;
    }
  }

  if (reduction)
    return lossSum;

  return (ElemType) lossSum / batchSize;
}

template<typename InputDataType, typename OutputDataType>
template<typename PredictionType, typename TargetType, typename LossType>
void CosineEmbeddingLoss<InputDataType, OutputDataType>::Backward(
    const PredictionType& prediction,
    const TargetType& target,
    LossType& loss)
{
  typedef typename PredictionType::elem_type ElemType;

  const size_t cols = prediction.n_cols;
  const size_t batchSize = prediction.n_elem / cols;
  if (arma::size(prediction) != arma::size(target))
    Log::Fatal << "Input Tensors must have same dimensions." << std::endl;

  arma::colvec inputTemp1 = arma::vectorise(prediction);
  arma::colvec inputTemp2 = arma::vectorise(target);
  loss.set_size(arma::size(inputTemp1));

  arma::colvec outputTemp(loss.memptr(), inputTemp1.n_elem,
      false, false);
  for (size_t i = 0; i < inputTemp1.n_elem; i += cols)
  {
    const ElemType cosDist = kernel::CosineDistance::Evaluate(inputTemp1(
        arma::span(i, i + cols -1)), inputTemp2(arma::span(i, i + cols -1)));

    if (cosDist < margin && !similarity)
      outputTemp(arma::span(i, i + cols - 1)).zeros();
    else
    {
      const int multiplier = similarity ? 1 : -1;
      outputTemp(arma::span(i, i + cols -1)) = -1 * multiplier *
          (arma::normalise(inputTemp2(arma::span(i, i + cols - 1))) -
          cosDist * arma::normalise(inputTemp1(arma::span(i, i + cols -
          1)))) / std::sqrt(arma::accu(arma::pow(inputTemp1(arma::span(i, i +
          cols - 1)), 2)));
    }

    if (!reduction)
      outputTemp = outputTemp / batchSize;
  }
}

template<typename InputDataType, typename OutputDataType>
template<typename Archive>
void CosineEmbeddingLoss<InputDataType, OutputDataType>::serialize(
    Archive& ar, const uint32_t /* version */)
{
  ar(CEREAL_NVP(margin));
  ar(CEREAL_NVP(similarity));
  ar(CEREAL_NVP(reduction));
}

} // namespace ann
} // namespace mlpack

#endif
