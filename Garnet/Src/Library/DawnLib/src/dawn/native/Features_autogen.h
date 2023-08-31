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

#ifndef DAWNNATIVE_FEATURES_AUTOGEN_H_
#define DAWNNATIVE_FEATURES_AUTOGEN_H_

#include "dawn/native/dawn_platform.h"
#include "dawn/native/DawnNative.h"
#include "dawn/common/ityp_array.h"

namespace dawn::native {

enum class Feature {
DepthClipControl,
Depth32FloatStencil8,
TimestampQuery,
PipelineStatisticsQuery,
TextureCompressionBC,
TextureCompressionETC2,
TextureCompressionASTC,
IndirectFirstInstance,
ShaderF16,
RG11B10UfloatRenderable,
BGRA8UnormStorage,
Float32Filterable,
DawnInternalUsages,
DawnMultiPlanarFormats,
DawnNative,
ChromiumExperimentalDp4a,
TimestampQueryInsidePasses,
ImplicitDeviceSynchronization,
SurfaceCapabilities,
TransientAttachments,
MSAARenderToSingleSampled,
DualSourceBlending,
D3D11MultithreadProtected,
ANGLETextureSharing,
ChromiumExperimentalSubgroups,
ChromiumExperimentalSubgroupUniformControlFlow,
ChromiumExperimentalReadWriteStorageTexture,
PixelLocalStorageCoherent,
PixelLocalStorageNonCoherent,
Norm16TextureFormats,
MultiPlanarFormatExtendedUsages,
MultiPlanarFormatP010,
SharedTextureMemoryVkDedicatedAllocation,
SharedTextureMemoryAHardwareBuffer,
SharedTextureMemoryDmaBuf,
SharedTextureMemoryOpaqueFD,
SharedTextureMemoryZirconHandle,
SharedTextureMemoryDXGISharedHandle,
SharedTextureMemoryD3D11Texture2D,
SharedTextureMemoryIOSurface,
SharedTextureMemoryEGLImage,
SharedFenceVkSemaphoreOpaqueFD,
SharedFenceVkSemaphoreSyncFD,
SharedFenceVkSemaphoreZirconHandle,
SharedFenceDXGISharedHandle,
SharedFenceMTLSharedEvent,
  InvalidEnum,
};

template<>
struct EnumCount<Feature> {
    static constexpr uint32_t value = 46;
};

}  // namespace dawn::native

#endif  // DAWNNATIVE_FEATURES_AUTOGEN_H_
