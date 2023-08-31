// Copyright 2023 The Dawn Authors
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

namespace dawn::native {

wgpu::FeatureName ToAPI(Feature feature) {
  switch (feature) {
  case Feature::DepthClipControl:
    return wgpu::FeatureName::DepthClipControl;
  case Feature::Depth32FloatStencil8:
    return wgpu::FeatureName::Depth32FloatStencil8;
  case Feature::TimestampQuery:
    return wgpu::FeatureName::TimestampQuery;
  case Feature::PipelineStatisticsQuery:
    return wgpu::FeatureName::PipelineStatisticsQuery;
  case Feature::TextureCompressionBC:
    return wgpu::FeatureName::TextureCompressionBC;
  case Feature::TextureCompressionETC2:
    return wgpu::FeatureName::TextureCompressionETC2;
  case Feature::TextureCompressionASTC:
    return wgpu::FeatureName::TextureCompressionASTC;
  case Feature::IndirectFirstInstance:
    return wgpu::FeatureName::IndirectFirstInstance;
  case Feature::ShaderF16:
    return wgpu::FeatureName::ShaderF16;
  case Feature::RG11B10UfloatRenderable:
    return wgpu::FeatureName::RG11B10UfloatRenderable;
  case Feature::BGRA8UnormStorage:
    return wgpu::FeatureName::BGRA8UnormStorage;
  case Feature::Float32Filterable:
    return wgpu::FeatureName::Float32Filterable;
  case Feature::DawnInternalUsages:
    return wgpu::FeatureName::DawnInternalUsages;
  case Feature::DawnMultiPlanarFormats:
    return wgpu::FeatureName::DawnMultiPlanarFormats;
  case Feature::DawnNative:
    return wgpu::FeatureName::DawnNative;
  case Feature::ChromiumExperimentalDp4a:
    return wgpu::FeatureName::ChromiumExperimentalDp4a;
  case Feature::TimestampQueryInsidePasses:
    return wgpu::FeatureName::TimestampQueryInsidePasses;
  case Feature::ImplicitDeviceSynchronization:
    return wgpu::FeatureName::ImplicitDeviceSynchronization;
  case Feature::SurfaceCapabilities:
    return wgpu::FeatureName::SurfaceCapabilities;
  case Feature::TransientAttachments:
    return wgpu::FeatureName::TransientAttachments;
  case Feature::MSAARenderToSingleSampled:
    return wgpu::FeatureName::MSAARenderToSingleSampled;
  case Feature::DualSourceBlending:
    return wgpu::FeatureName::DualSourceBlending;
  case Feature::D3D11MultithreadProtected:
    return wgpu::FeatureName::D3D11MultithreadProtected;
  case Feature::ANGLETextureSharing:
    return wgpu::FeatureName::ANGLETextureSharing;
  case Feature::ChromiumExperimentalSubgroups:
    return wgpu::FeatureName::ChromiumExperimentalSubgroups;
  case Feature::ChromiumExperimentalSubgroupUniformControlFlow:
    return wgpu::FeatureName::ChromiumExperimentalSubgroupUniformControlFlow;
  case Feature::ChromiumExperimentalReadWriteStorageTexture:
    return wgpu::FeatureName::ChromiumExperimentalReadWriteStorageTexture;
  case Feature::PixelLocalStorageCoherent:
    return wgpu::FeatureName::PixelLocalStorageCoherent;
  case Feature::PixelLocalStorageNonCoherent:
    return wgpu::FeatureName::PixelLocalStorageNonCoherent;
  case Feature::Norm16TextureFormats:
    return wgpu::FeatureName::Norm16TextureFormats;
  case Feature::MultiPlanarFormatExtendedUsages:
    return wgpu::FeatureName::MultiPlanarFormatExtendedUsages;
  case Feature::MultiPlanarFormatP010:
    return wgpu::FeatureName::MultiPlanarFormatP010;
  case Feature::SharedTextureMemoryVkDedicatedAllocation:
    return wgpu::FeatureName::SharedTextureMemoryVkDedicatedAllocation;
  case Feature::SharedTextureMemoryAHardwareBuffer:
    return wgpu::FeatureName::SharedTextureMemoryAHardwareBuffer;
  case Feature::SharedTextureMemoryDmaBuf:
    return wgpu::FeatureName::SharedTextureMemoryDmaBuf;
  case Feature::SharedTextureMemoryOpaqueFD:
    return wgpu::FeatureName::SharedTextureMemoryOpaqueFD;
  case Feature::SharedTextureMemoryZirconHandle:
    return wgpu::FeatureName::SharedTextureMemoryZirconHandle;
  case Feature::SharedTextureMemoryDXGISharedHandle:
    return wgpu::FeatureName::SharedTextureMemoryDXGISharedHandle;
  case Feature::SharedTextureMemoryD3D11Texture2D:
    return wgpu::FeatureName::SharedTextureMemoryD3D11Texture2D;
  case Feature::SharedTextureMemoryIOSurface:
    return wgpu::FeatureName::SharedTextureMemoryIOSurface;
  case Feature::SharedTextureMemoryEGLImage:
    return wgpu::FeatureName::SharedTextureMemoryEGLImage;
  case Feature::SharedFenceVkSemaphoreOpaqueFD:
    return wgpu::FeatureName::SharedFenceVkSemaphoreOpaqueFD;
  case Feature::SharedFenceVkSemaphoreSyncFD:
    return wgpu::FeatureName::SharedFenceVkSemaphoreSyncFD;
  case Feature::SharedFenceVkSemaphoreZirconHandle:
    return wgpu::FeatureName::SharedFenceVkSemaphoreZirconHandle;
  case Feature::SharedFenceDXGISharedHandle:
    return wgpu::FeatureName::SharedFenceDXGISharedHandle;
  case Feature::SharedFenceMTLSharedEvent:
    return wgpu::FeatureName::SharedFenceMTLSharedEvent;
    case Feature::InvalidEnum:
      UNREACHABLE();
  }
}

Feature FromAPI(wgpu::FeatureName feature) {
  switch (feature) {
  case wgpu::FeatureName::Undefined:
  return Feature::InvalidEnum;
  case wgpu::FeatureName::DepthClipControl:
  return Feature::DepthClipControl;
  case wgpu::FeatureName::Depth32FloatStencil8:
  return Feature::Depth32FloatStencil8;
  case wgpu::FeatureName::TimestampQuery:
  return Feature::TimestampQuery;
  case wgpu::FeatureName::PipelineStatisticsQuery:
  return Feature::PipelineStatisticsQuery;
  case wgpu::FeatureName::TextureCompressionBC:
  return Feature::TextureCompressionBC;
  case wgpu::FeatureName::TextureCompressionETC2:
  return Feature::TextureCompressionETC2;
  case wgpu::FeatureName::TextureCompressionASTC:
  return Feature::TextureCompressionASTC;
  case wgpu::FeatureName::IndirectFirstInstance:
  return Feature::IndirectFirstInstance;
  case wgpu::FeatureName::ShaderF16:
  return Feature::ShaderF16;
  case wgpu::FeatureName::RG11B10UfloatRenderable:
  return Feature::RG11B10UfloatRenderable;
  case wgpu::FeatureName::BGRA8UnormStorage:
  return Feature::BGRA8UnormStorage;
  case wgpu::FeatureName::Float32Filterable:
  return Feature::Float32Filterable;
  case wgpu::FeatureName::DawnInternalUsages:
  return Feature::DawnInternalUsages;
  case wgpu::FeatureName::DawnMultiPlanarFormats:
  return Feature::DawnMultiPlanarFormats;
  case wgpu::FeatureName::DawnNative:
  return Feature::DawnNative;
  case wgpu::FeatureName::ChromiumExperimentalDp4a:
  return Feature::ChromiumExperimentalDp4a;
  case wgpu::FeatureName::TimestampQueryInsidePasses:
  return Feature::TimestampQueryInsidePasses;
  case wgpu::FeatureName::ImplicitDeviceSynchronization:
  return Feature::ImplicitDeviceSynchronization;
  case wgpu::FeatureName::SurfaceCapabilities:
  return Feature::SurfaceCapabilities;
  case wgpu::FeatureName::TransientAttachments:
  return Feature::TransientAttachments;
  case wgpu::FeatureName::MSAARenderToSingleSampled:
  return Feature::MSAARenderToSingleSampled;
  case wgpu::FeatureName::DualSourceBlending:
  return Feature::DualSourceBlending;
  case wgpu::FeatureName::D3D11MultithreadProtected:
  return Feature::D3D11MultithreadProtected;
  case wgpu::FeatureName::ANGLETextureSharing:
  return Feature::ANGLETextureSharing;
  case wgpu::FeatureName::ChromiumExperimentalSubgroups:
  return Feature::ChromiumExperimentalSubgroups;
  case wgpu::FeatureName::ChromiumExperimentalSubgroupUniformControlFlow:
  return Feature::ChromiumExperimentalSubgroupUniformControlFlow;
  case wgpu::FeatureName::ChromiumExperimentalReadWriteStorageTexture:
  return Feature::ChromiumExperimentalReadWriteStorageTexture;
  case wgpu::FeatureName::PixelLocalStorageCoherent:
  return Feature::PixelLocalStorageCoherent;
  case wgpu::FeatureName::PixelLocalStorageNonCoherent:
  return Feature::PixelLocalStorageNonCoherent;
  case wgpu::FeatureName::Norm16TextureFormats:
  return Feature::Norm16TextureFormats;
  case wgpu::FeatureName::MultiPlanarFormatExtendedUsages:
  return Feature::MultiPlanarFormatExtendedUsages;
  case wgpu::FeatureName::MultiPlanarFormatP010:
  return Feature::MultiPlanarFormatP010;
  case wgpu::FeatureName::SharedTextureMemoryVkDedicatedAllocation:
  return Feature::SharedTextureMemoryVkDedicatedAllocation;
  case wgpu::FeatureName::SharedTextureMemoryAHardwareBuffer:
  return Feature::SharedTextureMemoryAHardwareBuffer;
  case wgpu::FeatureName::SharedTextureMemoryDmaBuf:
  return Feature::SharedTextureMemoryDmaBuf;
  case wgpu::FeatureName::SharedTextureMemoryOpaqueFD:
  return Feature::SharedTextureMemoryOpaqueFD;
  case wgpu::FeatureName::SharedTextureMemoryZirconHandle:
  return Feature::SharedTextureMemoryZirconHandle;
  case wgpu::FeatureName::SharedTextureMemoryDXGISharedHandle:
  return Feature::SharedTextureMemoryDXGISharedHandle;
  case wgpu::FeatureName::SharedTextureMemoryD3D11Texture2D:
  return Feature::SharedTextureMemoryD3D11Texture2D;
  case wgpu::FeatureName::SharedTextureMemoryIOSurface:
  return Feature::SharedTextureMemoryIOSurface;
  case wgpu::FeatureName::SharedTextureMemoryEGLImage:
  return Feature::SharedTextureMemoryEGLImage;
  case wgpu::FeatureName::SharedFenceVkSemaphoreOpaqueFD:
  return Feature::SharedFenceVkSemaphoreOpaqueFD;
  case wgpu::FeatureName::SharedFenceVkSemaphoreSyncFD:
  return Feature::SharedFenceVkSemaphoreSyncFD;
  case wgpu::FeatureName::SharedFenceVkSemaphoreZirconHandle:
  return Feature::SharedFenceVkSemaphoreZirconHandle;
  case wgpu::FeatureName::SharedFenceDXGISharedHandle:
  return Feature::SharedFenceDXGISharedHandle;
  case wgpu::FeatureName::SharedFenceMTLSharedEvent:
  return Feature::SharedFenceMTLSharedEvent;
    default:
      return Feature::InvalidEnum;
  }
}

static constexpr bool FeatureInfoIsDefined(Feature feature) {
  for (const auto& info : kFeatureInfo) {
    if (info.feature == feature) {
      return true;
    }
  }
  return false;
}

static constexpr ityp::array<Feature, FeatureInfo, kEnumCount<Feature>> InitializeFeatureEnumAndInfoList() {
  constexpr size_t kInfoCount = sizeof(kFeatureInfo) / sizeof(kFeatureInfo[0]);
  ityp::array<Feature, FeatureInfo, kEnumCount<Feature>> list{};
{
  static_assert(FeatureInfoIsDefined(Feature::DepthClipControl),
                "Please define feature info for DepthClipControl in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::DepthClipControl) {
      list[Feature::DepthClipControl] = {
        "depth_clip_control",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::Depth32FloatStencil8),
                "Please define feature info for Depth32FloatStencil8 in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::Depth32FloatStencil8) {
      list[Feature::Depth32FloatStencil8] = {
        "depth32_float_stencil8",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::TimestampQuery),
                "Please define feature info for TimestampQuery in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::TimestampQuery) {
      list[Feature::TimestampQuery] = {
        "timestamp_query",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::PipelineStatisticsQuery),
                "Please define feature info for PipelineStatisticsQuery in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::PipelineStatisticsQuery) {
      list[Feature::PipelineStatisticsQuery] = {
        "pipeline_statistics_query",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::TextureCompressionBC),
                "Please define feature info for TextureCompressionBC in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::TextureCompressionBC) {
      list[Feature::TextureCompressionBC] = {
        "texture_compression_BC",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::TextureCompressionETC2),
                "Please define feature info for TextureCompressionETC2 in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::TextureCompressionETC2) {
      list[Feature::TextureCompressionETC2] = {
        "texture_compression_ETC2",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::TextureCompressionASTC),
                "Please define feature info for TextureCompressionASTC in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::TextureCompressionASTC) {
      list[Feature::TextureCompressionASTC] = {
        "texture_compression_ASTC",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::IndirectFirstInstance),
                "Please define feature info for IndirectFirstInstance in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::IndirectFirstInstance) {
      list[Feature::IndirectFirstInstance] = {
        "indirect_first_instance",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::ShaderF16),
                "Please define feature info for ShaderF16 in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::ShaderF16) {
      list[Feature::ShaderF16] = {
        "shader_f16",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::RG11B10UfloatRenderable),
                "Please define feature info for RG11B10UfloatRenderable in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::RG11B10UfloatRenderable) {
      list[Feature::RG11B10UfloatRenderable] = {
        "RG11B10_ufloat_renderable",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::BGRA8UnormStorage),
                "Please define feature info for BGRA8UnormStorage in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::BGRA8UnormStorage) {
      list[Feature::BGRA8UnormStorage] = {
        "BGRA8_unorm_storage",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::Float32Filterable),
                "Please define feature info for Float32Filterable in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::Float32Filterable) {
      list[Feature::Float32Filterable] = {
        "float32_filterable",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::DawnInternalUsages),
                "Please define feature info for DawnInternalUsages in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::DawnInternalUsages) {
      list[Feature::DawnInternalUsages] = {
        "dawn_internal_usages",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::DawnMultiPlanarFormats),
                "Please define feature info for DawnMultiPlanarFormats in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::DawnMultiPlanarFormats) {
      list[Feature::DawnMultiPlanarFormats] = {
        "dawn_multi_planar_formats",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::DawnNative),
                "Please define feature info for DawnNative in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::DawnNative) {
      list[Feature::DawnNative] = {
        "dawn_native",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::ChromiumExperimentalDp4a),
                "Please define feature info for ChromiumExperimentalDp4a in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::ChromiumExperimentalDp4a) {
      list[Feature::ChromiumExperimentalDp4a] = {
        "chromium_experimental_dp4a",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::TimestampQueryInsidePasses),
                "Please define feature info for TimestampQueryInsidePasses in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::TimestampQueryInsidePasses) {
      list[Feature::TimestampQueryInsidePasses] = {
        "timestamp_query_inside_passes",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::ImplicitDeviceSynchronization),
                "Please define feature info for ImplicitDeviceSynchronization in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::ImplicitDeviceSynchronization) {
      list[Feature::ImplicitDeviceSynchronization] = {
        "implicit_device_synchronization",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SurfaceCapabilities),
                "Please define feature info for SurfaceCapabilities in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SurfaceCapabilities) {
      list[Feature::SurfaceCapabilities] = {
        "surface_capabilities",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::TransientAttachments),
                "Please define feature info for TransientAttachments in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::TransientAttachments) {
      list[Feature::TransientAttachments] = {
        "transient_attachments",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::MSAARenderToSingleSampled),
                "Please define feature info for MSAARenderToSingleSampled in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::MSAARenderToSingleSampled) {
      list[Feature::MSAARenderToSingleSampled] = {
        "MSAA_render_to_single_sampled",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::DualSourceBlending),
                "Please define feature info for DualSourceBlending in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::DualSourceBlending) {
      list[Feature::DualSourceBlending] = {
        "dual_source_blending",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::D3D11MultithreadProtected),
                "Please define feature info for D3D11MultithreadProtected in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::D3D11MultithreadProtected) {
      list[Feature::D3D11MultithreadProtected] = {
        "D3D11_multithread_protected",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::ANGLETextureSharing),
                "Please define feature info for ANGLETextureSharing in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::ANGLETextureSharing) {
      list[Feature::ANGLETextureSharing] = {
        "ANGLE_texture_sharing",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::ChromiumExperimentalSubgroups),
                "Please define feature info for ChromiumExperimentalSubgroups in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::ChromiumExperimentalSubgroups) {
      list[Feature::ChromiumExperimentalSubgroups] = {
        "chromium_experimental_subgroups",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::ChromiumExperimentalSubgroupUniformControlFlow),
                "Please define feature info for ChromiumExperimentalSubgroupUniformControlFlow in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::ChromiumExperimentalSubgroupUniformControlFlow) {
      list[Feature::ChromiumExperimentalSubgroupUniformControlFlow] = {
        "chromium_experimental_subgroup_uniform_control_flow",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::ChromiumExperimentalReadWriteStorageTexture),
                "Please define feature info for ChromiumExperimentalReadWriteStorageTexture in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::ChromiumExperimentalReadWriteStorageTexture) {
      list[Feature::ChromiumExperimentalReadWriteStorageTexture] = {
        "chromium_experimental_read_write_storage_texture",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::PixelLocalStorageCoherent),
                "Please define feature info for PixelLocalStorageCoherent in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::PixelLocalStorageCoherent) {
      list[Feature::PixelLocalStorageCoherent] = {
        "pixel_local_storage_coherent",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::PixelLocalStorageNonCoherent),
                "Please define feature info for PixelLocalStorageNonCoherent in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::PixelLocalStorageNonCoherent) {
      list[Feature::PixelLocalStorageNonCoherent] = {
        "pixel_local_storage_non_coherent",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::Norm16TextureFormats),
                "Please define feature info for Norm16TextureFormats in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::Norm16TextureFormats) {
      list[Feature::Norm16TextureFormats] = {
        "norm16_texture_formats",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::MultiPlanarFormatExtendedUsages),
                "Please define feature info for MultiPlanarFormatExtendedUsages in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::MultiPlanarFormatExtendedUsages) {
      list[Feature::MultiPlanarFormatExtendedUsages] = {
        "multi_planar_format_extended_usages",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::MultiPlanarFormatP010),
                "Please define feature info for MultiPlanarFormatP010 in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::MultiPlanarFormatP010) {
      list[Feature::MultiPlanarFormatP010] = {
        "multi_planar_format_p010",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedTextureMemoryVkDedicatedAllocation),
                "Please define feature info for SharedTextureMemoryVkDedicatedAllocation in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedTextureMemoryVkDedicatedAllocation) {
      list[Feature::SharedTextureMemoryVkDedicatedAllocation] = {
        "shared_texture_memory_vk_dedicated_allocation",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedTextureMemoryAHardwareBuffer),
                "Please define feature info for SharedTextureMemoryAHardwareBuffer in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedTextureMemoryAHardwareBuffer) {
      list[Feature::SharedTextureMemoryAHardwareBuffer] = {
        "shared_texture_memory_a_hardware_buffer",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedTextureMemoryDmaBuf),
                "Please define feature info for SharedTextureMemoryDmaBuf in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedTextureMemoryDmaBuf) {
      list[Feature::SharedTextureMemoryDmaBuf] = {
        "shared_texture_memory_dma_buf",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedTextureMemoryOpaqueFD),
                "Please define feature info for SharedTextureMemoryOpaqueFD in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedTextureMemoryOpaqueFD) {
      list[Feature::SharedTextureMemoryOpaqueFD] = {
        "shared_texture_memory_opaque_FD",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedTextureMemoryZirconHandle),
                "Please define feature info for SharedTextureMemoryZirconHandle in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedTextureMemoryZirconHandle) {
      list[Feature::SharedTextureMemoryZirconHandle] = {
        "shared_texture_memory_zircon_handle",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedTextureMemoryDXGISharedHandle),
                "Please define feature info for SharedTextureMemoryDXGISharedHandle in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedTextureMemoryDXGISharedHandle) {
      list[Feature::SharedTextureMemoryDXGISharedHandle] = {
        "shared_texture_memory_DXGI_shared_handle",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedTextureMemoryD3D11Texture2D),
                "Please define feature info for SharedTextureMemoryD3D11Texture2D in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedTextureMemoryD3D11Texture2D) {
      list[Feature::SharedTextureMemoryD3D11Texture2D] = {
        "shared_texture_memory_D3D11_texture_2D",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedTextureMemoryIOSurface),
                "Please define feature info for SharedTextureMemoryIOSurface in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedTextureMemoryIOSurface) {
      list[Feature::SharedTextureMemoryIOSurface] = {
        "shared_texture_memory_IO_surface",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedTextureMemoryEGLImage),
                "Please define feature info for SharedTextureMemoryEGLImage in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedTextureMemoryEGLImage) {
      list[Feature::SharedTextureMemoryEGLImage] = {
        "shared_texture_memory_EGL_image",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedFenceVkSemaphoreOpaqueFD),
                "Please define feature info for SharedFenceVkSemaphoreOpaqueFD in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedFenceVkSemaphoreOpaqueFD) {
      list[Feature::SharedFenceVkSemaphoreOpaqueFD] = {
        "shared_fence_vk_semaphore_opaque_FD",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedFenceVkSemaphoreSyncFD),
                "Please define feature info for SharedFenceVkSemaphoreSyncFD in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedFenceVkSemaphoreSyncFD) {
      list[Feature::SharedFenceVkSemaphoreSyncFD] = {
        "shared_fence_vk_semaphore_sync_FD",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedFenceVkSemaphoreZirconHandle),
                "Please define feature info for SharedFenceVkSemaphoreZirconHandle in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedFenceVkSemaphoreZirconHandle) {
      list[Feature::SharedFenceVkSemaphoreZirconHandle] = {
        "shared_fence_vk_semaphore_zircon_handle",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedFenceDXGISharedHandle),
                "Please define feature info for SharedFenceDXGISharedHandle in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedFenceDXGISharedHandle) {
      list[Feature::SharedFenceDXGISharedHandle] = {
        "shared_fence_DXGI_shared_handle",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
{
  static_assert(FeatureInfoIsDefined(Feature::SharedFenceMTLSharedEvent),
                "Please define feature info for SharedFenceMTLSharedEvent in Features.cpp");
  for (size_t i = 0; i < kInfoCount; ++i) {
    if (kFeatureInfo[i].feature == Feature::SharedFenceMTLSharedEvent) {
      list[Feature::SharedFenceMTLSharedEvent] = {
        "shared_fence_MTL_shared_event",
        kFeatureInfo[i].info.description,
        kFeatureInfo[i].info.url,
        kFeatureInfo[i].info.featureState,
      };
    }
  }
}
  return list;
}

const ityp::array<Feature, FeatureInfo, kEnumCount<Feature>> kFeatureNameAndInfoList = InitializeFeatureEnumAndInfoList();

}
