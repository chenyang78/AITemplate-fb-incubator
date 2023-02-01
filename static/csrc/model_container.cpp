//  Copyright (c) Meta Platforms, Inc. and affiliates.
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.
//
#include "model_container.h"

#include "device_functions-generated.h"
#include "raii_wrapper.h"

namespace ait {

std::string uuid_to_string(const char bytes[16]) {
  std::vector<std::tuple<int, int>> groups = {
      {0, 4}, {4, 6}, {6, 8}, {8, 10}, {10, 16}};
  char const hex_chars[16] = {
      '0',
      '1',
      '2',
      '3',
      '4',
      '5',
      '6',
      '7',
      '8',
      '9',
      'a',
      'b',
      'c',
      'd',
      'e',
      'f'};

  std::string result;
  bool first = true;
  for (auto g : groups) {
    if (!first) {
      result += "-";
    } else {
      first = false;
    }
    for (size_t i = std::get<0>(g); i < std::get<1>(g); ++i) {
      result += hex_chars[(bytes[i] & 0xF0) >> 4];
      result += hex_chars[(bytes[i] & 0x0F)];
    }
  }
  return result;
}

void print_device_properties(const DevicePropertyType& prop) {
  LOG(INFO)
      << "Hardware accelerator device properties: "
      << "\n  Device: "
      << "\n     ASCII string identifying device: " << prop.name
      << "\n     Major compute capability: " << prop.major
      << "\n     Minor compute capability: " << prop.minor
      << "\n     UUID: " << uuid_to_string(prop.uuid.bytes)
      << "\n     Unique identifier for a group of devices on the same multi-GPU board: "
      << prop.multiGpuBoardGroupID
      << "\n     PCI bus ID of the device: " << prop.pciBusID
      << "\n     PCI device ID of the device: " << prop.pciDeviceID
      << "\n     PCI domain ID of the device: " << prop.pciDomainID

      << "\n  Memory limits: "
      << "\n     Constant memory available on device in bytes: "
      << prop.totalConstMem
      << "\n     Global memory available on device in bytes: "
      << prop.totalGlobalMem
      << "\n     Global memory bus width in bits: " << prop.memoryBusWidth
      << "\n     Size of L2 cache in bytes: " << prop.l2CacheSize
      << "\n     Device's maximum L2 persisting lines capacity in bytes: "
      << prop.persistingL2CacheMaxSize
      << "\n     Shared memory reserved by CUDA driver per block in bytes: "
      << prop.reservedSharedMemPerBlock
      << "\n     Shared memory available per block in bytes: "
      << prop.sharedMemPerBlock
      << "\n     Per device maximum shared memory per block usable by special opt in: "
      << prop.sharedMemPerBlockOptin
      << "\n     Shared memory available per multiprocessor in bytes: "
      << prop.sharedMemPerMultiprocessor
      << "\n     The maximum value of cudaAccessPolicyWindow::num_bytes: "
      << prop.accessPolicyMaxWindowSize

      << "\n  Thread limits: "
      << "\n     Warp size in threads: " << prop.warpSize
      << "\n     Maximum size of each dimension of a grid: "
      << prop.maxGridSize[0] << " " << prop.maxGridSize[1] << " "
      << prop.maxGridSize[2]
      << "\n     Maximum size of each dimension of a block: "
      << prop.maxThreadsDim[0] << " " << prop.maxThreadsDim[1] << " "
      << prop.maxThreadsDim[2]
      << "\n     Number of asynchronous engines: " << prop.asyncEngineCount
      << "\n     Maximum number of resident blocks per multiprocessor: "
      << prop.maxBlocksPerMultiProcessor
      << "\n     Maximum number of threads per block: "
      << prop.maxThreadsPerBlock
      << "\n     Maximum resident threads per multiprocessor: "
      << prop.maxThreadsPerMultiProcessor
      << "\n     Maximum pitch in bytes allowed by memory copies: "
      << prop.memPitch << "\n     Number of multiprocessors on device: "
      << prop.multiProcessorCount
      << "\n     32-bit registers available per block: " << prop.regsPerBlock
      << "\n     32-bit registers available per multiprocessor: "
      << prop.regsPerMultiprocessor

      << "\n  Device features: "
      << "\n     Device has ECC support enabled: "
      << (prop.ECCEnabled ? "yes" : "no")
      << "\n     Device can map host memory with cudaHostAlloc/cudaHostGetDevicePointer: "
      << (prop.canMapHostMemory ? "yes" : "no")
      << "\n     Device can access host registered memory at the same virtual address as the CPU: "
      << (prop.canUseHostPointerForRegisteredMem ? "yes" : "no")
      << "\n     Device supports Compute Preemption: "
      << (prop.computePreemptionSupported ? "yes" : "no")
      << "\n     Device can possibly execute multiple kernels concurrently: "
      << (prop.concurrentKernels ? "yes" : "no")
      << "\n     Device can coherently access managed memory concurrently with the CPU: "
      << (prop.concurrentManagedAccess ? "yes" : "no")
      << "\n     Device supports launching cooperative kernels via cudaLaunchCooperativeKernel: "
      << (prop.cooperativeLaunch ? "yes" : "no")
      << "\n     Host can directly access managed memory on the device without migration: "
      << (prop.directManagedMemAccessFromHost ? "yes" : "no")
      << "\n     Device supports caching globals in L1: "
      << (prop.globalL1CacheSupported ? "yes" : "no")
      << "\n     Link between the device and the host supports native atomic operations: "
      << (prop.hostNativeAtomicSupported ? "yes" : "no")
      << "\n     Device is integrated as opposed to discrete: "
      << (prop.integrated ? "yes" : "no")
      << "\n     Device is on a multi-GPU board: "
      << (prop.isMultiGpuBoard ? "yes" : "no")
      << "\n     Device supports caching locals in L1: "
      << (prop.localL1CacheSupported ? "yes" : "no")
      << "\n     Device supports allocating managed memory on this system: "
      << (prop.managedMemory ? "yes" : "no")
      << "\n     Device supports coherently accessing pageable memory without calling cudaHostRegister on it: "
      << (prop.pageableMemoryAccess ? "yes" : "no")
      << "\n     Device accesses pageable memory via the host's page tables: "
      << (prop.pageableMemoryAccessUsesHostPageTables ? "yes" : "no")
      << "\n     Device supports stream priorities: "
      << (prop.streamPrioritiesSupported ? "yes" : "no")
      << "\n     Device is a Tesla device using TCC driver: "
      << (prop.tccDriver ? "yes" : "no")
      << "\n     Device shares a unified address space with the host: "
      << (prop.unifiedAddressing ? "yes" : "no")

      << "\n  Texture limits: "
      << "\n     Maximum 1D surface size: " << prop.maxSurface1D
      << "\n     Maximum 1D layered surface dimensions: "
      << prop.maxSurface1DLayered[0] << " " << prop.maxSurface1DLayered[1]
      << "\n     Maximum 2D surface dimensions: " << prop.maxSurface2D[0] << " "
      << prop.maxSurface2D[1]
      << "\n     Maximum 2D layered surface dimensions: "
      << prop.maxSurface2DLayered[0] << " " << prop.maxSurface2DLayered[1]
      << " " << prop.maxSurface2DLayered[2]
      << "\n     Maximum 3D surface dimensions: " << prop.maxSurface3D[0] << " "
      << prop.maxSurface3D[1] << " " << prop.maxSurface3D[2]
      << "\n     Maximum Cubemap surface dimensions: " << prop.maxSurfaceCubemap
      << "\n     Maximum Cubemap layered surface dimensions: "
      << prop.maxSurfaceCubemapLayered[0] << " "
      << prop.maxSurfaceCubemapLayered[1]
      << "\n     Maximum 1D texture size: " << prop.maxTexture1D
      << "\n     Maximum 1D layered texture dimensions "
      << prop.maxTexture1DLayered[0] << " " << prop.maxTexture1DLayered[1]
      << "\n     Maximum 1D mipmapped texture size: " << prop.maxTexture1DMipmap
      << "\n     Maximum 2D texture dimensions: " << prop.maxTexture2D[0] << " "
      << prop.maxTexture2D[1]
      << "\n     Maximum 2D texture dimensions if texture gather operations have to be performed: "
      << prop.maxTexture2DGather[0] << " " << prop.maxTexture2DGather[1]
      << "\n     Maximum 2D layered texture dimensions: "
      << prop.maxTexture2DLayered[0] << " " << prop.maxTexture2DLayered[1]
      << " " << prop.maxTexture2DLayered[2]
      << "\n     Maximum dimensions (width, height, pitch) for 2D textures bound to pitched memory: "
      << prop.maxTexture2DLinear[0] << " " << prop.maxTexture2DLinear[1] << " "
      << prop.maxTexture2DLinear[2]
      << "\n     Maximum 2D mipmapped texture dimensions: "
      << prop.maxTexture2DMipmap[0] << " " << prop.maxTexture2DMipmap[1]
      << "\n     Maximum 3D texture dimensions: " << prop.maxTexture3D[0] << " "
      << prop.maxTexture3D[1] << " " << prop.maxTexture3D[2]
      << "\n     Maximum alternate 3D texture dimensions: "
      << prop.maxTexture3DAlt[0] << " " << prop.maxTexture3DAlt[1] << " "
      << prop.maxTexture3DAlt[2]
      << "\n     Maximum Cubemap texture dimensions: " << prop.maxTextureCubemap
      << "\n     Maximum Cubemap layered texture dimensions: "
      << prop.maxTextureCubemapLayered[0] << " "
      << prop.maxTextureCubemapLayered[1]
      << "\n     Alignment requirements for surfaces: " << prop.surfaceAlignment
      << "\n     Alignment requirement for textures: " << prop.textureAlignment
      << "\n     Pitch alignment requirement for texture references bound to pitched memory: "
      << prop.texturePitchAlignment;
}

ModelContainer::ModelContainer(
    size_t num_models,
    size_t num_inputs,
    size_t num_outputs,
    size_t num_unbound_constants,
    size_t params_size,
    AITemplateAllocator& allocator)
    : ModelContainerBase(
          num_inputs,
          num_outputs,
          num_unbound_constants,
          params_size,
          allocator),
      allocator_(allocator),
      num_inputs_(num_inputs),
      num_outputs_(num_outputs) {
  if (num_models == 0) {
    throw std::runtime_error("Number of models must be positive");
  }
  dmlc::InitLogging("aitemplate"); // TODO(xxx): render network name
  int runtime_version;
  int driver_version;
  DEVICE_CHECK(GetDriverVersion(&driver_version));
  DEVICE_CHECK(GetRuntimeVersion(&runtime_version));
  LOG(INFO) << "Device Runtime Version: " << runtime_version
            << "; Driver Version: " << driver_version;

  int dev_id;
  DevicePropertyType prop;
  DEVICE_CHECK(GetDevice(&dev_id));
  DEVICE_CHECK(GetDeviceProperties(&prop, dev_id));

  print_device_properties(prop);

  LOG(INFO) << "Init AITemplate Runtime with " << num_models << " concurrency";
  models_.reserve(num_models);
  available_models_.reserve(num_models);

  for (size_t i = 0; i < num_models; ++i) {
    models_.push_back(
        Model::Create(allocator, static_cast<uint8_t*>(constants_.get())));
    available_models_.push_back(models_.back().get());
  }
}

void ModelContainer::Run(
    const AITData* inputs,
    size_t num_inputs,
    AITData* outputs,
    size_t num_outputs,
    StreamType stream,
    bool sync,
    bool graph_mode,
    int64_t** output_shapes_out) {
  auto* model = GetAvailableModel();
  try {
    PrepareForRun(model, inputs, num_inputs, outputs, num_outputs);
    model->Run(stream, graph_mode);
  } catch (...) {
    std::lock_guard lk(models_mutex_);
    available_models_.push_back(model);
    throw;
  }

  if (output_shapes_out) {
    for (size_t i = 0; i < num_outputs; ++i) {
      auto* out_shape = output_shapes_out[i];
      model->GetOutputShape(i, out_shape);
    }
  }

  {
    std::lock_guard lk(models_mutex_);
    pending_models_.push_back(model);
  }
  pending_models_available_.notify_one();
  if (sync) {
    StreamSynchronize(stream);
  }
}

void ModelContainer::Profile(
    const AITData* inputs,
    size_t num_inputs,
    AITData* outputs,
    size_t num_outputs,
    StreamType stream,
    size_t num_iters,
    const char* filename) {
  auto* model = GetAvailableModel();
  if (filename == nullptr) {
    throw;
  }
  try {
    PrepareForRun(model, inputs, num_inputs, outputs, num_outputs);
    model->Profile(stream, num_iters, filename);
  } catch (...) {
    std::lock_guard lk(models_mutex_);
    available_models_.push_back(model);
    throw;
  }

  {
    std::lock_guard lk(models_mutex_);
    pending_models_.push_back(model);
  }
  pending_models_available_.notify_one();
}

void ModelContainer::RunWithOutputsOnHost(
    const AITData* inputs,
    size_t num_inputs,
    AITData* outputs,
    size_t num_outputs,
    StreamType stream,
    bool graph_mode,
    int64_t** output_shapes_out) {
  std::vector<std::pair<GPUPtr, size_t>> owned_outputs_ptrs;
  std::vector<AITData> owned_outputs;
  owned_outputs_ptrs.reserve(num_outputs);
  owned_outputs.reserve(num_outputs);
  for (size_t i = 0; i < num_outputs; ++i) {
    size_t num_bytes = MaxOutputStorageBytes(i);
    owned_outputs_ptrs.emplace_back(
        RAII_DeviceMalloc(num_bytes, allocator_), num_bytes);
    owned_outputs.emplace_back(
        owned_outputs_ptrs.back().first.get(),
        outputs[i].shape,
        outputs[i].dtype);
  }

  Run(inputs,
      num_inputs,
      owned_outputs.data(),
      num_outputs,
      stream,
      /*sync=*/false,
      graph_mode,
      output_shapes_out);

  for (size_t i = 0; i < num_outputs; ++i) {
    auto& owned_output = owned_outputs_ptrs[i];
    auto& ptr = owned_output.first;
    auto num_bytes = owned_output.second;
    DEVICE_CHECK(CopyToHost(outputs[i].ptr, ptr.get(), num_bytes, stream));
  }

  DEVICE_CHECK(StreamSynchronize(stream));
}

float ModelContainer::Benchmark(
    const AITData* inputs,
    size_t num_inputs,
    AITData* outputs,
    size_t num_outputs,
    StreamType stream,
    bool graph_mode,
    size_t count,
    size_t num_threads,
    bool use_unique_stream_per_thread,
    int64_t** output_shapes_out) {
  if (num_threads == 0) {
    num_threads = std::thread::hardware_concurrency();
  }

  if (num_threads == 1) {
    return BenchmarkImpl(
               inputs,
               num_inputs,
               outputs,
               num_outputs,
               stream,
               graph_mode,
               count,
               output_shapes_out) /
        count;
  }
  // Clone the outputs, each thread needs its own set
  std::vector<std::vector<GPUPtr>> per_thread_outputs_ptrs;
  std::vector<std::vector<AITData>> per_thread_outputs;
  std::vector<StreamPtr> per_thread_streams;
  per_thread_outputs_ptrs.reserve(num_threads - 1);
  per_thread_outputs.reserve(num_threads - 1);

  if (use_unique_stream_per_thread) {
    per_thread_streams.reserve(num_threads);
    for (size_t i = 0; i < num_threads; ++i) {
      per_thread_streams.push_back(RAII_StreamCreate(/*non_blocking=*/true));
    }
  }

  for (size_t i = 1; i < num_threads; ++i) {
    std::vector<GPUPtr> cloned_outputs_ptrs;
    std::vector<AITData> cloned_outputs;

    cloned_outputs_ptrs.reserve(num_outputs);
    cloned_outputs.reserve(num_outputs);

    for (size_t j = 0; j < num_outputs; ++j) {
      size_t num_bytes = MaxOutputStorageBytes(j);
      cloned_outputs_ptrs.emplace_back(
          RAII_DeviceMalloc(num_bytes, allocator_));
      auto* new_pointer = cloned_outputs_ptrs.back().get();
      DEVICE_CHECK(
          DeviceToDeviceCopy(new_pointer, outputs[j].ptr, num_bytes, stream));
      cloned_outputs.emplace_back(
          new_pointer, outputs[j].shape, outputs[j].dtype);
    }
    per_thread_outputs_ptrs.push_back(std::move(cloned_outputs_ptrs));
    per_thread_outputs.push_back(std::move(cloned_outputs));
  }
  DEVICE_CHECK(StreamSynchronize(stream));

  auto get_stream = [stream, use_unique_stream_per_thread, &per_thread_streams](
                        size_t thread_idx) {
    if (!use_unique_stream_per_thread) {
      return stream;
    }
    return per_thread_streams[thread_idx].get();
  };

  auto thread_func = [&](size_t thread_idx) {
    AITData* thread_outputs =
        thread_idx == 0 ? outputs : per_thread_outputs[thread_idx - 1].data();
    StreamType thread_stream = get_stream(thread_idx);
    auto* thread_output_shapes_out =
        thread_idx == 0 ? output_shapes_out : nullptr;
    return BenchmarkImpl(
        inputs,
        num_inputs,
        thread_outputs,
        num_outputs,
        thread_stream,
        graph_mode,
        count,
        thread_output_shapes_out);
  };

  std::vector<std::future<float>> futures;
  futures.reserve(num_threads);
  for (size_t i = 0; i < num_threads; ++i) {
    futures.push_back(std::async(std::launch::async, thread_func, i));
  }

  auto max_time = std::accumulate(
      futures.begin(), futures.end(), 0.f, [](float cur_val, auto& future) {
        return std::max(future.get(), cur_val);
      });

  // Verify that all the outputs are the same
  for (size_t i = 0; i < num_outputs; ++i) {
    auto output_size = MaxOutputStorageBytes(i);
    auto output_host = std::make_unique<uint8_t[]>(output_size);
    // NB: technically, we don't have to copy to host here, but using
    // std::memcmp is easier than writing a kernel that does comparisons
    // for both backends, and performance is not important here.
    DEVICE_CHECK(
        CopyToHost(output_host.get(), outputs[i].ptr, output_size, stream));
    DEVICE_CHECK(StreamSynchronize(stream));

    for (size_t thread_idx = 1; thread_idx < num_threads; ++thread_idx) {
      auto* thread_output = per_thread_outputs[thread_idx - 1][i].ptr;
      auto thread_output_host = std::make_unique<uint8_t[]>(output_size);
      auto thread_stream = get_stream(thread_idx);
      DEVICE_CHECK(CopyToHost(
          thread_output_host.get(), thread_output, output_size, thread_stream));
      DEVICE_CHECK(StreamSynchronize(thread_stream));
      if (std::memcmp(
              output_host.get(), thread_output_host.get(), output_size)) {
        throw std::runtime_error(
            "Output " + std::to_string(i) +
            " did not match for a spawned thread!");
      }
    }
  }
  auto total_num_iters = num_threads * count;
  return max_time / total_num_iters;
}

void ModelContainer::SetConstant(const char* name, const AITData& tensor) {
  auto it = unbound_constant_name_to_idx_.find(name);
  if (it == unbound_constant_name_to_idx_.end()) {
    // TODO make this an exception after we fix the CMF benchmarks
    LOG(ERROR) << "Constant " << name << " not found";
    return;
  }
  auto constant_idx = it->second + num_inputs_ + num_outputs_;
  ValidateDtype(tensor.dtype, constant_idx);

  CHECK_VECTOR_ACCESS(max_param_storage_bytes_, constant_idx)
  auto expected_num_bytes = max_param_storage_bytes_[constant_idx];
  auto actual_num_bytes =
      tensor.shape.Numel() * AITemplateDtypeSizeBytes(tensor.dtype);
  if (expected_num_bytes != actual_num_bytes) {
    throw std::runtime_error(
        std::string(
            "SetConstant did not receive correct number of bytes for constant ") +
        name + ": expected " + std::to_string(expected_num_bytes) +
        " but got " + std::to_string(actual_num_bytes) +
        ". Check that the provided tensor's shape is correct.");
  }

  auto* src = tensor.ptr;
  for (auto& model : models_) {
    model->SetConstant(name, src);
  }
}

size_t ModelContainer::NumInputs() const {
  return num_inputs_;
}

const char* ModelContainer::InputName(size_t input_idx) const {
  CHECK(input_idx < num_inputs_);
  CHECK_VECTOR_ACCESS(param_names_, input_idx)
  return param_names_[input_idx];
}

AITemplateParamShape ModelContainer::MaxInputShape(size_t input_idx) const {
  CHECK(input_idx < num_inputs_);
  CHECK_VECTOR_ACCESS(max_param_shapes_, input_idx)
  auto& input_shape = max_param_shapes_[input_idx];
  return AITemplateParamShape{input_shape.data(), input_shape.size()};
}

AITemplateDtype ModelContainer::InputDtype(size_t input_idx) const {
  CHECK(input_idx < num_inputs_);
  CHECK_VECTOR_ACCESS(param_dtypes_, input_idx)
  return param_dtypes_[input_idx];
}

size_t ModelContainer::NumOutputs() const {
  return num_outputs_;
}

const char* ModelContainer::OutputName(size_t output_idx) const {
  auto idx = output_idx + num_inputs_;
  CHECK_VECTOR_ACCESS(param_names_, idx)
  return param_names_[idx];
}

AITemplateParamShape ModelContainer::MaxOutputShape(size_t output_idx) const {
  auto idx = output_idx + num_inputs_;
  CHECK_VECTOR_ACCESS(max_param_shapes_, idx)
  auto& out_shape = max_param_shapes_[idx];
  return AITemplateParamShape{out_shape.data(), out_shape.size()};
}

AITemplateDtype ModelContainer::OutputDtype(size_t output_idx) const {
  auto idx = output_idx + num_inputs_;
  CHECK_VECTOR_ACCESS(param_dtypes_, idx)
  return param_dtypes_[idx];
}

size_t ModelContainer::MaxOutputStorageBytes(size_t output_idx) const {
  auto idx = output_idx + num_inputs_;
  CHECK_VECTOR_ACCESS(max_param_storage_bytes_, idx)
  return max_param_storage_bytes_[idx];
}

void ModelContainer::PrepareForRun(
    Model* model,
    const AITData* inputs,
    size_t num_inputs,
    AITData* outputs,
    size_t num_outputs) {
  if (num_inputs != num_inputs_) {
    auto msg = "Got wrong number of inputs; expected " +
        std::to_string(num_inputs_) + ", got " + std::to_string(num_inputs);
    throw std::runtime_error(std::move(msg));
  }
  if (num_inputs > 0 && inputs == nullptr) {
    throw std::runtime_error("inputs cannot be null");
  }
  if (num_outputs != num_outputs_) {
    auto msg = "Got wrong number of outputs; expected " +
        std::to_string(num_outputs_) + ", got " + std::to_string(num_outputs);
    throw std::runtime_error(std::move(msg));
  }
  if (num_outputs > 0 && outputs == nullptr) {
    throw std::runtime_error("outputs cannot be null");
  }
  for (size_t i = 0; i < num_inputs_; ++i) {
    auto& input = inputs[i];
    ValidateDtype(input.dtype, i);
    model->SetInput(input.ptr, input.shape, i);
  }

  for (size_t i = 0; i < num_outputs_; ++i) {
    auto& output = outputs[i];
    ValidateDtype(output.dtype, i + num_inputs_);
    model->SetOutput(output.ptr, i);
  }
}

Model* ModelContainer::GetAvailableModel() {
  std::unique_lock lk(models_mutex_);
  if (available_models_.empty()) {
    ReclaimFinishedModels(lk);
  }
  auto* result = available_models_.back();
  available_models_.pop_back();
  return result;
}

void ModelContainer::ReclaimFinishedModels(std::unique_lock<std::mutex>& lk) {
  // Put any complete models at the end
  auto it = std::stable_partition(
      pending_models_.begin(), pending_models_.end(), [](Model* m) {
        return m->IsPending();
      });

  if (it != pending_models_.end()) {
    // Move all available models to the pool.
    available_models_.insert(
        available_models_.end(), it, pending_models_.end());
    pending_models_.erase(it, pending_models_.end());
    return;
  }

  pending_models_available_.wait(
      lk, [this]() { return !pending_models_.empty(); });
  // There are no available workspaces! We have to wait on one.
  auto* model = pending_models_.front();
  pending_models_.pop_front();
  lk.unlock();
  try {
    model->WaitForCompletion();
  } catch (...) {
    lk.lock();
    available_models_.push_back(model);
    throw;
  }
  lk.lock();
  available_models_.push_back(model);
}

void ModelContainer::ValidateDtype(AITemplateDtype dtype, size_t idx) const {
  CHECK_VECTOR_ACCESS(param_dtypes_, idx)
  if (dtype != param_dtypes_[idx]) {
    auto GetEnumString = [](auto dtype) {
      switch (dtype) {
        case AITemplateDtype::kUnset:
          return "kUnset";
        case AITemplateDtype::kHalf:
          return "kHalf";
        case AITemplateDtype::kFloat:
          return "kFloat";
        case AITemplateDtype::kInt:
          return "kInt";
        case AITemplateDtype::kLong:
          return "kLong";
        default:
          return "unknown";
      }
    };
    throw std::runtime_error(
        "Got wrong dtype for param " + std::to_string(idx) + "; expected " +
        GetEnumString(param_dtypes_[idx]) + ", got " + GetEnumString(dtype));
  }
}

float ModelContainer::BenchmarkImpl(
    const AITData* inputs,
    size_t num_inputs,
    AITData* outputs,
    size_t num_outputs,
    StreamType stream,
    bool graph_mode,
    size_t count,
    int64_t** output_shapes_out) {
  auto* model = GetAvailableModel();
  float runtime_ms = 0.;
  auto start_event = RAII_CreateEvent();
  auto end_event = RAII_CreateEvent();
  try {
    PrepareForRun(model, inputs, num_inputs, outputs, num_outputs);
    DEVICE_CHECK(EventRecord(start_event.get(), stream));

    for (size_t i = 0; i < count; ++i) {
      model->Run(stream, graph_mode);
    }
  } catch (...) {
    std::lock_guard lk(models_mutex_);
    available_models_.push_back(model);
    throw;
  }
  if (output_shapes_out) {
    for (size_t i = 0; i < num_outputs; ++i) {
      auto* out_shape = output_shapes_out[i];
      model->GetOutputShape(i, out_shape);
    }
  }
  // Push the model back into the pool before synchronizing the event
  // to exercise the concurrency code
  {
    std::lock_guard lk(models_mutex_);
    pending_models_.push_back(model);
  }
  pending_models_available_.notify_one();

  DEVICE_CHECK(EventRecord(end_event.get(), stream));
  DEVICE_CHECK(EventSynchronize(end_event.get()));
  DEVICE_CHECK(
      EventElapsedTime(&runtime_ms, start_event.get(), end_event.get()));
  LOG(INFO) << "Benchmark runtime ms/iter: " << runtime_ms / count;
  return runtime_ms;
}

} // namespace ait
