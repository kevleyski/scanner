/* Copyright 2016 Carnegie Mellon University
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include "scanner/eval/evaluator.h"
#include "scanner/eval/evaluator_factory.h"
#include "scanner/evaluators/caffe/net_descriptor.h"
#include "scanner/util/opencv.h"

//#define HAVE_CUDA

#ifdef HAVE_CUDA
#include <opencv2/core/cuda_stream_accessor.hpp>
#include <opencv2/cudaarithm.hpp>
#include "scanner/util/cuda.h"
#endif

namespace scanner {

void cpm2_net_config(f32 scale, const BatchConfig &config,
                     caffe::Net<float> *net);

class CPM2InputEvaluator : public Evaluator {
 public:
  CPM2InputEvaluator(DeviceType device_type, i32 device_id,
                     const NetDescriptor& descriptor, i32 batch_size,
                     f32 scale);

  void configure(const BatchConfig& config) override;

  void evaluate(const BatchedColumns& input_columns,
                BatchedColumns& output_columns) override;

 private:
  DeviceType device_type_;
  i32 device_id_;
  NetDescriptor descriptor_;
  i32 batch_size_;
  f32 scale_;

  i32 frame_width_;
  i32 frame_height_;
  i32 resize_width_;
  i32 resize_height_;
  i32 width_padding_;
  i32 height_padding_;
  i32 net_input_width_;
  i32 net_input_height_;

#ifdef HAVE_CUDA
  i32 num_cuda_streams_;
  std::vector<cv::cuda::Stream> streams_;
  std::vector<cv::cuda::GpuMat> frame_input_g_;
  std::vector<cv::cuda::GpuMat> bgr_input_g_;
  std::vector<cv::cuda::GpuMat> resized_input_g_;
  std::vector<cv::cuda::GpuMat> padded_input_g_;
  std::vector<cv::cuda::GpuMat> float_input_g_;
  std::vector<std::vector<cv::cuda::GpuMat>> input_planes_g_;
  std::vector<std::vector<cv::cuda::GpuMat>> flipped_planes_g_;
  std::vector<cv::cuda::GpuMat> planar_input_g_;
#endif
  cv::Mat mean_mat_c_;
  cv::Mat float_input_c_;
  cv::Mat normalized_input_c_;
  cv::Mat flipped_input_c_;
  std::vector<cv::Mat> input_planes_c_;
  cv::Mat planar_input_c_;
};

class CPM2InputEvaluatorFactory : public EvaluatorFactory {
 public:
  CPM2InputEvaluatorFactory(DeviceType device_type,
                            const NetDescriptor& descriptor,
                            i32 batch_size,
                            f32 scale);

  EvaluatorCapabilities get_capabilities() override;

  std::vector<std::string> get_output_columns(
      const std::vector<std::string>& input_columns) override;

  Evaluator* new_evaluator(const EvaluatorConfig& config) override;

 private:
  DeviceType device_type_;
  NetDescriptor net_descriptor_;
  i32 batch_size_;
  f32 scale_;
};
}
