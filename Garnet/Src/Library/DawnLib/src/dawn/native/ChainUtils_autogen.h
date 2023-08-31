// Copyright 2021 The Dawn Authors
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

#ifndef DAWNNATIVE_CHAIN_UTILS_H_
#define DAWNNATIVE_CHAIN_UTILS_H_

#include <tuple>
#include <type_traits>
#include <unordered_set>

#include "absl/strings/str_format.h"
#include "dawn/native/dawn_platform.h"
#include "dawn/native/Error.h"
#include "dawn/native/wgpu_structs_autogen.h"

namespace dawn::native {

namespace detail {

    // SType for implementation details. Kept inside the detail namespace for extensibility.
    template <typename T>
    inline wgpu::SType STypeForImpl;

    // Specialize STypeFor to map from native struct types to their SType.
    template <>
    constexpr inline wgpu::SType STypeForImpl<SurfaceDescriptorFromMetalLayer> = wgpu::SType::SurfaceDescriptorFromMetalLayer;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SurfaceDescriptorFromWindowsHWND> = wgpu::SType::SurfaceDescriptorFromWindowsHWND;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SurfaceDescriptorFromXlibWindow> = wgpu::SType::SurfaceDescriptorFromXlibWindow;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SurfaceDescriptorFromCanvasHTMLSelector> = wgpu::SType::SurfaceDescriptorFromCanvasHTMLSelector;
    template <>
    constexpr inline wgpu::SType STypeForImpl<ShaderModuleSPIRVDescriptor> = wgpu::SType::ShaderModuleSPIRVDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<ShaderModuleWGSLDescriptor> = wgpu::SType::ShaderModuleWGSLDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<PrimitiveDepthClipControl> = wgpu::SType::PrimitiveDepthClipControl;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SurfaceDescriptorFromWaylandSurface> = wgpu::SType::SurfaceDescriptorFromWaylandSurface;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SurfaceDescriptorFromAndroidNativeWindow> = wgpu::SType::SurfaceDescriptorFromAndroidNativeWindow;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SurfaceDescriptorFromWindowsCoreWindow> = wgpu::SType::SurfaceDescriptorFromWindowsCoreWindow;
    template <>
    constexpr inline wgpu::SType STypeForImpl<ExternalTextureBindingEntry> = wgpu::SType::ExternalTextureBindingEntry;
    template <>
    constexpr inline wgpu::SType STypeForImpl<ExternalTextureBindingLayout> = wgpu::SType::ExternalTextureBindingLayout;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SurfaceDescriptorFromWindowsSwapChainPanel> = wgpu::SType::SurfaceDescriptorFromWindowsSwapChainPanel;
    template <>
    constexpr inline wgpu::SType STypeForImpl<RenderPassDescriptorMaxDrawCount> = wgpu::SType::RenderPassDescriptorMaxDrawCount;
    template <>
    constexpr inline wgpu::SType STypeForImpl<DawnTextureInternalUsageDescriptor> = wgpu::SType::DawnTextureInternalUsageDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<DawnEncoderInternalUsageDescriptor> = wgpu::SType::DawnEncoderInternalUsageDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<DawnCacheDeviceDescriptor> = wgpu::SType::DawnCacheDeviceDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<DawnAdapterPropertiesPowerPreference> = wgpu::SType::DawnAdapterPropertiesPowerPreference;
    template <>
    constexpr inline wgpu::SType STypeForImpl<DawnBufferDescriptorErrorInfoFromWireClient> = wgpu::SType::DawnBufferDescriptorErrorInfoFromWireClient;
    template <>
    constexpr inline wgpu::SType STypeForImpl<DawnTogglesDescriptor> = wgpu::SType::DawnTogglesDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<DawnShaderModuleSPIRVOptionsDescriptor> = wgpu::SType::DawnShaderModuleSPIRVOptionsDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<DawnMultisampleStateRenderToSingleSampled> = wgpu::SType::DawnMultisampleStateRenderToSingleSampled;
    template <>
    constexpr inline wgpu::SType STypeForImpl<DawnRenderPassColorAttachmentRenderToSingleSampled> = wgpu::SType::DawnRenderPassColorAttachmentRenderToSingleSampled;
    template <>
    constexpr inline wgpu::SType STypeForImpl<RenderPassPixelLocalStorage> = wgpu::SType::RenderPassPixelLocalStorage;
    template <>
    constexpr inline wgpu::SType STypeForImpl<PipelineLayoutPixelLocalStorage> = wgpu::SType::PipelineLayoutPixelLocalStorage;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryVkImageDescriptor> = wgpu::SType::SharedTextureMemoryVkImageDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryVkDedicatedAllocationDescriptor> = wgpu::SType::SharedTextureMemoryVkDedicatedAllocationDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryAHardwareBufferDescriptor> = wgpu::SType::SharedTextureMemoryAHardwareBufferDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryDmaBufDescriptor> = wgpu::SType::SharedTextureMemoryDmaBufDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryOpaqueFDDescriptor> = wgpu::SType::SharedTextureMemoryOpaqueFDDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryZirconHandleDescriptor> = wgpu::SType::SharedTextureMemoryZirconHandleDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryDXGISharedHandleDescriptor> = wgpu::SType::SharedTextureMemoryDXGISharedHandleDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryIOSurfaceDescriptor> = wgpu::SType::SharedTextureMemoryIOSurfaceDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryEGLImageDescriptor> = wgpu::SType::SharedTextureMemoryEGLImageDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryVkImageLayoutBeginState> = wgpu::SType::SharedTextureMemoryVkImageLayoutBeginState;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedTextureMemoryVkImageLayoutEndState> = wgpu::SType::SharedTextureMemoryVkImageLayoutEndState;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceVkSemaphoreOpaqueFDDescriptor> = wgpu::SType::SharedFenceVkSemaphoreOpaqueFDDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceVkSemaphoreOpaqueFDExportInfo> = wgpu::SType::SharedFenceVkSemaphoreOpaqueFDExportInfo;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceVkSemaphoreSyncFDDescriptor> = wgpu::SType::SharedFenceVkSemaphoreSyncFDDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceVkSemaphoreSyncFDExportInfo> = wgpu::SType::SharedFenceVkSemaphoreSyncFDExportInfo;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceVkSemaphoreZirconHandleDescriptor> = wgpu::SType::SharedFenceVkSemaphoreZirconHandleDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceVkSemaphoreZirconHandleExportInfo> = wgpu::SType::SharedFenceVkSemaphoreZirconHandleExportInfo;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceDXGISharedHandleDescriptor> = wgpu::SType::SharedFenceDXGISharedHandleDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceDXGISharedHandleExportInfo> = wgpu::SType::SharedFenceDXGISharedHandleExportInfo;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceMTLSharedEventDescriptor> = wgpu::SType::SharedFenceMTLSharedEventDescriptor;
    template <>
    constexpr inline wgpu::SType STypeForImpl<SharedFenceMTLSharedEventExportInfo> = wgpu::SType::SharedFenceMTLSharedEventExportInfo;

    template <typename Arg, typename... Rest>
    std::string STypesToString() {
        if constexpr (sizeof...(Rest)) {
            return absl::StrFormat("%s, ", STypeForImpl<Arg>) + STypesToString<Rest...>();
        } else {
            return absl::StrFormat("%s", STypeForImpl<Arg>);
        }
    }

    //
    // Unpacked chain types structs and helpers.
    //   Note that unpacked types are tuples to enable further templating extensions based on
    //   typing via something like std::get<const Extension*> in templated functions.
    //

    // Typelist type used to further add extensions to chain roots when they are not in the json.
    template <typename... Exts>
    struct AdditionalExtensionsList;

    // Root specializations for adding additional extensions.
    template <typename Root>
    struct AdditionalExtensions {
        using List = AdditionalExtensionsList<>;
    };

    // Template structs to get the typing for the unpacked chains.
    template <typename...>
    struct UnpackedChain;
    template <typename... Additionals, typename... Ts>
    struct UnpackedChain<AdditionalExtensionsList<Additionals...>, Ts...> {
        using Type = std::tuple<Ts..., Additionals...>;
    };

    // Template function that returns a string of the non-nullptr STypes from an unpacked chain.
    template <typename Unpacked>
    std::string UnpackedChainToString(const Unpacked& unpacked) {
        std::string result = "( ";
        std::apply(
            [&](const auto*... args) {
                (([&](const auto* arg) {
                    if (arg != nullptr) {
                        result += absl::StrFormat("%s, ", arg->sType);
                    }
                }(args)), ...);}, unpacked);
        result += " )";
        return result;
    }

}  // namespace detail

    template <typename T>
    constexpr inline wgpu::SType STypeFor = detail::STypeForImpl<T>;
    template <typename T>
    constexpr inline wgpu::SType STypeFor<const T*> = detail::STypeForImpl<T>;

    template <typename T>
    void FindInChain(const ChainedStruct* chain, const T** out) {
        for (; chain; chain = chain->nextInChain) {
            if (chain->sType == STypeFor<T>) {
                *out = static_cast<const T*>(chain);
                break;
            }
        }
    }
    template <typename T>
    void FindInChain(ChainedStructOut* chain, T** out) {
        for (; chain; chain = chain->nextInChain) {
            if (chain->sType == STypeFor<T>) {
                *out = static_cast<T*>(chain);
                break;
            }
        }
    }

    // Verifies that |chain| only contains ChainedStructs of types enumerated in
    // |oneOfConstraints| and contains no duplicate sTypes. Each vector in
    // |oneOfConstraints| defines a set of sTypes that cannot coexist in the same chain.
    // For example:
    //   ValidateSTypes(chain, { { ShaderModuleSPIRVDescriptor, ShaderModuleWGSLDescriptor } }))
    //   ValidateSTypes(chain, { { Extension1 }, { Extension2 } })
    MaybeError ValidateSTypes(const ChainedStruct* chain,
                              std::vector<std::vector<wgpu::SType>> oneOfConstraints);
    MaybeError ValidateSTypes(const ChainedStructOut* chain,
                              std::vector<std::vector<wgpu::SType>> oneOfConstraints);

    template <typename T>
    MaybeError ValidateSingleSTypeInner(const ChainedStruct* chain, T sType) {
        DAWN_INVALID_IF(chain->sType != sType,
            "Unsupported sType (%s). Expected (%s)", chain->sType, sType);
        return {};
    }
    template <typename T>
    MaybeError ValidateSingleSTypeInner(const ChainedStructOut* chain, T sType) {
        DAWN_INVALID_IF(chain->sType != sType,
            "Unsupported sType (%s). Expected (%s)", chain->sType, sType);
        return {};
    }

    template <typename T, typename... Args>
    MaybeError ValidateSingleSTypeInner(const ChainedStruct* chain, T sType, Args... sTypes) {
        if (chain->sType == sType) {
            return {};
        }
        return ValidateSingleSTypeInner(chain, sTypes...);
    }
    template <typename T, typename... Args>
    MaybeError ValidateSingleSTypeInner(const ChainedStructOut* chain, T sType, Args... sTypes) {
        if (chain->sType == sType) {
            return {};
        }
        return ValidateSingleSTypeInner(chain, sTypes...);
    }

    // Verifies that |chain| contains a single ChainedStruct of type |sType| or no ChainedStructs
    // at all.
    template <typename T>
    MaybeError ValidateSingleSType(const ChainedStruct* chain, T sType) {
        if (chain == nullptr) {
            return {};
        }
        DAWN_INVALID_IF(chain->nextInChain != nullptr,
            "Chain can only contain a single chained struct.");
        return ValidateSingleSTypeInner(chain, sType);
    }
    template <typename T>
    MaybeError ValidateSingleSType(const ChainedStructOut* chain, T sType) {
        if (chain == nullptr) {
            return {};
        }
        DAWN_INVALID_IF(chain->nextInChain != nullptr,
            "Chain can only contain a single chained struct.");
        return ValidateSingleSTypeInner(chain, sType);
    }

    // Verifies that |chain| contains a single ChainedStruct with a type enumerated in the
    // parameter pack or no ChainedStructs at all.
    template <typename T, typename... Args>
    MaybeError ValidateSingleSType(const ChainedStruct* chain, T sType, Args... sTypes) {
        if (chain == nullptr) {
            return {};
        }
        DAWN_INVALID_IF(chain->nextInChain != nullptr,
            "Chain can only contain a single chained struct.");
        return ValidateSingleSTypeInner(chain, sType, sTypes...);
    }
    template <typename T, typename... Args>
    MaybeError ValidateSingleSType(const ChainedStructOut* chain, T sType, Args... sTypes) {
        if (chain == nullptr) {
            return {};
        }
        DAWN_INVALID_IF(chain->nextInChain != nullptr,
            "Chain can only contain a single chained struct.");
        return ValidateSingleSTypeInner(chain, sType, sTypes...);
    }

    // Template type to get root type from the unpacked chain and vice-versa.
    template <typename Unpacked>
    struct RootTypeFor;
    template <typename Root>
    struct UnpackedTypeFor;

}  // namespace dawn::native

// Include specializations before declaring types for ordering purposes.
#include "dawn/native/ChainUtilsImpl.h"

namespace dawn::native {

    using UnpackedBindGroupEntryChain = detail::UnpackedChain<
        detail::AdditionalExtensions<BindGroupEntry>::List,
        const ExternalTextureBindingEntry*
    >::Type;
    template <>
    struct UnpackedTypeFor<BindGroupEntry> {
        using Type = UnpackedBindGroupEntryChain;
    };
    ResultOrError<UnpackedBindGroupEntryChain> ValidateAndUnpackChain(const BindGroupEntry* chain);

    using UnpackedBufferBindingLayoutChain = detail::UnpackedChain<
        detail::AdditionalExtensions<BufferBindingLayout>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<BufferBindingLayout> {
        using Type = UnpackedBufferBindingLayoutChain;
    };
    ResultOrError<UnpackedBufferBindingLayoutChain> ValidateAndUnpackChain(const BufferBindingLayout* chain);

    using UnpackedBufferDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<BufferDescriptor>::List,
        const DawnBufferDescriptorErrorInfoFromWireClient*
    >::Type;
    template <>
    struct UnpackedTypeFor<BufferDescriptor> {
        using Type = UnpackedBufferDescriptorChain;
    };
    ResultOrError<UnpackedBufferDescriptorChain> ValidateAndUnpackChain(const BufferDescriptor* chain);

    using UnpackedCommandBufferDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<CommandBufferDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<CommandBufferDescriptor> {
        using Type = UnpackedCommandBufferDescriptorChain;
    };
    ResultOrError<UnpackedCommandBufferDescriptorChain> ValidateAndUnpackChain(const CommandBufferDescriptor* chain);

    using UnpackedCommandEncoderDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<CommandEncoderDescriptor>::List,
        const DawnEncoderInternalUsageDescriptor*
    >::Type;
    template <>
    struct UnpackedTypeFor<CommandEncoderDescriptor> {
        using Type = UnpackedCommandEncoderDescriptorChain;
    };
    ResultOrError<UnpackedCommandEncoderDescriptorChain> ValidateAndUnpackChain(const CommandEncoderDescriptor* chain);

    using UnpackedCompilationMessageChain = detail::UnpackedChain<
        detail::AdditionalExtensions<CompilationMessage>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<CompilationMessage> {
        using Type = UnpackedCompilationMessageChain;
    };
    ResultOrError<UnpackedCompilationMessageChain> ValidateAndUnpackChain(const CompilationMessage* chain);

    using UnpackedConstantEntryChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ConstantEntry>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<ConstantEntry> {
        using Type = UnpackedConstantEntryChain;
    };
    ResultOrError<UnpackedConstantEntryChain> ValidateAndUnpackChain(const ConstantEntry* chain);

    using UnpackedCopyTextureForBrowserOptionsChain = detail::UnpackedChain<
        detail::AdditionalExtensions<CopyTextureForBrowserOptions>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<CopyTextureForBrowserOptions> {
        using Type = UnpackedCopyTextureForBrowserOptionsChain;
    };
    ResultOrError<UnpackedCopyTextureForBrowserOptionsChain> ValidateAndUnpackChain(const CopyTextureForBrowserOptions* chain);

    using UnpackedInstanceDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<InstanceDescriptor>::List,
        const DawnTogglesDescriptor*
    >::Type;
    template <>
    struct UnpackedTypeFor<InstanceDescriptor> {
        using Type = UnpackedInstanceDescriptorChain;
    };
    ResultOrError<UnpackedInstanceDescriptorChain> ValidateAndUnpackChain(const InstanceDescriptor* chain);

    using UnpackedMultisampleStateChain = detail::UnpackedChain<
        detail::AdditionalExtensions<MultisampleState>::List,
        const DawnMultisampleStateRenderToSingleSampled*
    >::Type;
    template <>
    struct UnpackedTypeFor<MultisampleState> {
        using Type = UnpackedMultisampleStateChain;
    };
    ResultOrError<UnpackedMultisampleStateChain> ValidateAndUnpackChain(const MultisampleState* chain);

    using UnpackedPipelineLayoutDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<PipelineLayoutDescriptor>::List,
        const PipelineLayoutPixelLocalStorage*
    >::Type;
    template <>
    struct UnpackedTypeFor<PipelineLayoutDescriptor> {
        using Type = UnpackedPipelineLayoutDescriptorChain;
    };
    ResultOrError<UnpackedPipelineLayoutDescriptorChain> ValidateAndUnpackChain(const PipelineLayoutDescriptor* chain);

    using UnpackedPipelineLayoutStorageAttachmentChain = detail::UnpackedChain<
        detail::AdditionalExtensions<PipelineLayoutStorageAttachment>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<PipelineLayoutStorageAttachment> {
        using Type = UnpackedPipelineLayoutStorageAttachmentChain;
    };
    ResultOrError<UnpackedPipelineLayoutStorageAttachmentChain> ValidateAndUnpackChain(const PipelineLayoutStorageAttachment* chain);

    using UnpackedPrimitiveStateChain = detail::UnpackedChain<
        detail::AdditionalExtensions<PrimitiveState>::List,
        const PrimitiveDepthClipControl*
    >::Type;
    template <>
    struct UnpackedTypeFor<PrimitiveState> {
        using Type = UnpackedPrimitiveStateChain;
    };
    ResultOrError<UnpackedPrimitiveStateChain> ValidateAndUnpackChain(const PrimitiveState* chain);

    using UnpackedQuerySetDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<QuerySetDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<QuerySetDescriptor> {
        using Type = UnpackedQuerySetDescriptorChain;
    };
    ResultOrError<UnpackedQuerySetDescriptorChain> ValidateAndUnpackChain(const QuerySetDescriptor* chain);

    using UnpackedQueueDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<QueueDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<QueueDescriptor> {
        using Type = UnpackedQueueDescriptorChain;
    };
    ResultOrError<UnpackedQueueDescriptorChain> ValidateAndUnpackChain(const QueueDescriptor* chain);

    using UnpackedRenderBundleDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<RenderBundleDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<RenderBundleDescriptor> {
        using Type = UnpackedRenderBundleDescriptorChain;
    };
    ResultOrError<UnpackedRenderBundleDescriptorChain> ValidateAndUnpackChain(const RenderBundleDescriptor* chain);

    using UnpackedRenderBundleEncoderDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<RenderBundleEncoderDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<RenderBundleEncoderDescriptor> {
        using Type = UnpackedRenderBundleEncoderDescriptorChain;
    };
    ResultOrError<UnpackedRenderBundleEncoderDescriptorChain> ValidateAndUnpackChain(const RenderBundleEncoderDescriptor* chain);

    using UnpackedRequestAdapterOptionsChain = detail::UnpackedChain<
        detail::AdditionalExtensions<RequestAdapterOptions>::List,
        const DawnTogglesDescriptor*
    >::Type;
    template <>
    struct UnpackedTypeFor<RequestAdapterOptions> {
        using Type = UnpackedRequestAdapterOptionsChain;
    };
    ResultOrError<UnpackedRequestAdapterOptionsChain> ValidateAndUnpackChain(const RequestAdapterOptions* chain);

    using UnpackedSamplerBindingLayoutChain = detail::UnpackedChain<
        detail::AdditionalExtensions<SamplerBindingLayout>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<SamplerBindingLayout> {
        using Type = UnpackedSamplerBindingLayoutChain;
    };
    ResultOrError<UnpackedSamplerBindingLayoutChain> ValidateAndUnpackChain(const SamplerBindingLayout* chain);

    using UnpackedSamplerDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<SamplerDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<SamplerDescriptor> {
        using Type = UnpackedSamplerDescriptorChain;
    };
    ResultOrError<UnpackedSamplerDescriptorChain> ValidateAndUnpackChain(const SamplerDescriptor* chain);

    using UnpackedShaderModuleDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ShaderModuleDescriptor>::List,
        const ShaderModuleSPIRVDescriptor*,
        const ShaderModuleWGSLDescriptor*,
        const DawnShaderModuleSPIRVOptionsDescriptor*
    >::Type;
    template <>
    struct UnpackedTypeFor<ShaderModuleDescriptor> {
        using Type = UnpackedShaderModuleDescriptorChain;
    };
    ResultOrError<UnpackedShaderModuleDescriptorChain> ValidateAndUnpackChain(const ShaderModuleDescriptor* chain);

    using UnpackedSharedFenceDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<SharedFenceDescriptor>::List,
        const SharedFenceVkSemaphoreOpaqueFDDescriptor*,
        const SharedFenceVkSemaphoreSyncFDDescriptor*,
        const SharedFenceVkSemaphoreZirconHandleDescriptor*,
        const SharedFenceDXGISharedHandleDescriptor*,
        const SharedFenceMTLSharedEventDescriptor*
    >::Type;
    template <>
    struct UnpackedTypeFor<SharedFenceDescriptor> {
        using Type = UnpackedSharedFenceDescriptorChain;
    };
    ResultOrError<UnpackedSharedFenceDescriptorChain> ValidateAndUnpackChain(const SharedFenceDescriptor* chain);

    using UnpackedSharedTextureMemoryBeginAccessDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<SharedTextureMemoryBeginAccessDescriptor>::List,
        const SharedTextureMemoryVkImageLayoutBeginState*
    >::Type;
    template <>
    struct UnpackedTypeFor<SharedTextureMemoryBeginAccessDescriptor> {
        using Type = UnpackedSharedTextureMemoryBeginAccessDescriptorChain;
    };
    ResultOrError<UnpackedSharedTextureMemoryBeginAccessDescriptorChain> ValidateAndUnpackChain(const SharedTextureMemoryBeginAccessDescriptor* chain);

    using UnpackedSharedTextureMemoryDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<SharedTextureMemoryDescriptor>::List,
        const SharedTextureMemoryVkImageDescriptor*,
        const SharedTextureMemoryVkDedicatedAllocationDescriptor*,
        const SharedTextureMemoryAHardwareBufferDescriptor*,
        const SharedTextureMemoryDmaBufDescriptor*,
        const SharedTextureMemoryOpaqueFDDescriptor*,
        const SharedTextureMemoryZirconHandleDescriptor*,
        const SharedTextureMemoryDXGISharedHandleDescriptor*,
        const SharedTextureMemoryIOSurfaceDescriptor*,
        const SharedTextureMemoryEGLImageDescriptor*
    >::Type;
    template <>
    struct UnpackedTypeFor<SharedTextureMemoryDescriptor> {
        using Type = UnpackedSharedTextureMemoryDescriptorChain;
    };
    ResultOrError<UnpackedSharedTextureMemoryDescriptorChain> ValidateAndUnpackChain(const SharedTextureMemoryDescriptor* chain);

    using UnpackedStorageTextureBindingLayoutChain = detail::UnpackedChain<
        detail::AdditionalExtensions<StorageTextureBindingLayout>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<StorageTextureBindingLayout> {
        using Type = UnpackedStorageTextureBindingLayoutChain;
    };
    ResultOrError<UnpackedStorageTextureBindingLayoutChain> ValidateAndUnpackChain(const StorageTextureBindingLayout* chain);

    using UnpackedSurfaceDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<SurfaceDescriptor>::List,
        const SurfaceDescriptorFromAndroidNativeWindow*,
        const SurfaceDescriptorFromCanvasHTMLSelector*,
        const SurfaceDescriptorFromMetalLayer*,
        const SurfaceDescriptorFromWindowsHWND*,
        const SurfaceDescriptorFromXlibWindow*,
        const SurfaceDescriptorFromWaylandSurface*,
        const SurfaceDescriptorFromWindowsCoreWindow*,
        const SurfaceDescriptorFromWindowsSwapChainPanel*
    >::Type;
    template <>
    struct UnpackedTypeFor<SurfaceDescriptor> {
        using Type = UnpackedSurfaceDescriptorChain;
    };
    ResultOrError<UnpackedSurfaceDescriptorChain> ValidateAndUnpackChain(const SurfaceDescriptor* chain);

    using UnpackedSwapChainDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<SwapChainDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<SwapChainDescriptor> {
        using Type = UnpackedSwapChainDescriptorChain;
    };
    ResultOrError<UnpackedSwapChainDescriptorChain> ValidateAndUnpackChain(const SwapChainDescriptor* chain);

    using UnpackedTextureBindingLayoutChain = detail::UnpackedChain<
        detail::AdditionalExtensions<TextureBindingLayout>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<TextureBindingLayout> {
        using Type = UnpackedTextureBindingLayoutChain;
    };
    ResultOrError<UnpackedTextureBindingLayoutChain> ValidateAndUnpackChain(const TextureBindingLayout* chain);

    using UnpackedTextureDataLayoutChain = detail::UnpackedChain<
        detail::AdditionalExtensions<TextureDataLayout>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<TextureDataLayout> {
        using Type = UnpackedTextureDataLayoutChain;
    };
    ResultOrError<UnpackedTextureDataLayoutChain> ValidateAndUnpackChain(const TextureDataLayout* chain);

    using UnpackedTextureViewDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<TextureViewDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<TextureViewDescriptor> {
        using Type = UnpackedTextureViewDescriptorChain;
    };
    ResultOrError<UnpackedTextureViewDescriptorChain> ValidateAndUnpackChain(const TextureViewDescriptor* chain);

    using UnpackedBindGroupDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<BindGroupDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<BindGroupDescriptor> {
        using Type = UnpackedBindGroupDescriptorChain;
    };
    ResultOrError<UnpackedBindGroupDescriptorChain> ValidateAndUnpackChain(const BindGroupDescriptor* chain);

    using UnpackedBindGroupLayoutEntryChain = detail::UnpackedChain<
        detail::AdditionalExtensions<BindGroupLayoutEntry>::List,
        const ExternalTextureBindingLayout*
    >::Type;
    template <>
    struct UnpackedTypeFor<BindGroupLayoutEntry> {
        using Type = UnpackedBindGroupLayoutEntryChain;
    };
    ResultOrError<UnpackedBindGroupLayoutEntryChain> ValidateAndUnpackChain(const BindGroupLayoutEntry* chain);

    using UnpackedCompilationInfoChain = detail::UnpackedChain<
        detail::AdditionalExtensions<CompilationInfo>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<CompilationInfo> {
        using Type = UnpackedCompilationInfoChain;
    };
    ResultOrError<UnpackedCompilationInfoChain> ValidateAndUnpackChain(const CompilationInfo* chain);

    using UnpackedComputePassDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ComputePassDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<ComputePassDescriptor> {
        using Type = UnpackedComputePassDescriptorChain;
    };
    ResultOrError<UnpackedComputePassDescriptorChain> ValidateAndUnpackChain(const ComputePassDescriptor* chain);

    using UnpackedDepthStencilStateChain = detail::UnpackedChain<
        detail::AdditionalExtensions<DepthStencilState>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<DepthStencilState> {
        using Type = UnpackedDepthStencilStateChain;
    };
    ResultOrError<UnpackedDepthStencilStateChain> ValidateAndUnpackChain(const DepthStencilState* chain);

    using UnpackedExternalTextureDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ExternalTextureDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<ExternalTextureDescriptor> {
        using Type = UnpackedExternalTextureDescriptorChain;
    };
    ResultOrError<UnpackedExternalTextureDescriptorChain> ValidateAndUnpackChain(const ExternalTextureDescriptor* chain);

    using UnpackedImageCopyBufferChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ImageCopyBuffer>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<ImageCopyBuffer> {
        using Type = UnpackedImageCopyBufferChain;
    };
    ResultOrError<UnpackedImageCopyBufferChain> ValidateAndUnpackChain(const ImageCopyBuffer* chain);

    using UnpackedImageCopyExternalTextureChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ImageCopyExternalTexture>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<ImageCopyExternalTexture> {
        using Type = UnpackedImageCopyExternalTextureChain;
    };
    ResultOrError<UnpackedImageCopyExternalTextureChain> ValidateAndUnpackChain(const ImageCopyExternalTexture* chain);

    using UnpackedImageCopyTextureChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ImageCopyTexture>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<ImageCopyTexture> {
        using Type = UnpackedImageCopyTextureChain;
    };
    ResultOrError<UnpackedImageCopyTextureChain> ValidateAndUnpackChain(const ImageCopyTexture* chain);

    using UnpackedProgrammableStageDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ProgrammableStageDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<ProgrammableStageDescriptor> {
        using Type = UnpackedProgrammableStageDescriptorChain;
    };
    ResultOrError<UnpackedProgrammableStageDescriptorChain> ValidateAndUnpackChain(const ProgrammableStageDescriptor* chain);

    using UnpackedRenderPassColorAttachmentChain = detail::UnpackedChain<
        detail::AdditionalExtensions<RenderPassColorAttachment>::List,
        const DawnRenderPassColorAttachmentRenderToSingleSampled*
    >::Type;
    template <>
    struct UnpackedTypeFor<RenderPassColorAttachment> {
        using Type = UnpackedRenderPassColorAttachmentChain;
    };
    ResultOrError<UnpackedRenderPassColorAttachmentChain> ValidateAndUnpackChain(const RenderPassColorAttachment* chain);

    using UnpackedRenderPassStorageAttachmentChain = detail::UnpackedChain<
        detail::AdditionalExtensions<RenderPassStorageAttachment>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<RenderPassStorageAttachment> {
        using Type = UnpackedRenderPassStorageAttachmentChain;
    };
    ResultOrError<UnpackedRenderPassStorageAttachmentChain> ValidateAndUnpackChain(const RenderPassStorageAttachment* chain);

    using UnpackedRequiredLimitsChain = detail::UnpackedChain<
        detail::AdditionalExtensions<RequiredLimits>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<RequiredLimits> {
        using Type = UnpackedRequiredLimitsChain;
    };
    ResultOrError<UnpackedRequiredLimitsChain> ValidateAndUnpackChain(const RequiredLimits* chain);

    using UnpackedTextureDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<TextureDescriptor>::List,
        const DawnTextureInternalUsageDescriptor*
    >::Type;
    template <>
    struct UnpackedTypeFor<TextureDescriptor> {
        using Type = UnpackedTextureDescriptorChain;
    };
    ResultOrError<UnpackedTextureDescriptorChain> ValidateAndUnpackChain(const TextureDescriptor* chain);

    using UnpackedBindGroupLayoutDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<BindGroupLayoutDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<BindGroupLayoutDescriptor> {
        using Type = UnpackedBindGroupLayoutDescriptorChain;
    };
    ResultOrError<UnpackedBindGroupLayoutDescriptorChain> ValidateAndUnpackChain(const BindGroupLayoutDescriptor* chain);

    using UnpackedColorTargetStateChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ColorTargetState>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<ColorTargetState> {
        using Type = UnpackedColorTargetStateChain;
    };
    ResultOrError<UnpackedColorTargetStateChain> ValidateAndUnpackChain(const ColorTargetState* chain);

    using UnpackedComputePipelineDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<ComputePipelineDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<ComputePipelineDescriptor> {
        using Type = UnpackedComputePipelineDescriptorChain;
    };
    ResultOrError<UnpackedComputePipelineDescriptorChain> ValidateAndUnpackChain(const ComputePipelineDescriptor* chain);

    using UnpackedDeviceDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<DeviceDescriptor>::List,
        const DawnTogglesDescriptor*,
        const DawnCacheDeviceDescriptor*
    >::Type;
    template <>
    struct UnpackedTypeFor<DeviceDescriptor> {
        using Type = UnpackedDeviceDescriptorChain;
    };
    ResultOrError<UnpackedDeviceDescriptorChain> ValidateAndUnpackChain(const DeviceDescriptor* chain);

    using UnpackedRenderPassDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<RenderPassDescriptor>::List,
        const RenderPassDescriptorMaxDrawCount*,
        const RenderPassPixelLocalStorage*
    >::Type;
    template <>
    struct UnpackedTypeFor<RenderPassDescriptor> {
        using Type = UnpackedRenderPassDescriptorChain;
    };
    ResultOrError<UnpackedRenderPassDescriptorChain> ValidateAndUnpackChain(const RenderPassDescriptor* chain);

    using UnpackedVertexStateChain = detail::UnpackedChain<
        detail::AdditionalExtensions<VertexState>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<VertexState> {
        using Type = UnpackedVertexStateChain;
    };
    ResultOrError<UnpackedVertexStateChain> ValidateAndUnpackChain(const VertexState* chain);

    using UnpackedFragmentStateChain = detail::UnpackedChain<
        detail::AdditionalExtensions<FragmentState>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<FragmentState> {
        using Type = UnpackedFragmentStateChain;
    };
    ResultOrError<UnpackedFragmentStateChain> ValidateAndUnpackChain(const FragmentState* chain);

    using UnpackedRenderPipelineDescriptorChain = detail::UnpackedChain<
        detail::AdditionalExtensions<RenderPipelineDescriptor>::List
    >::Type;
    template <>
    struct UnpackedTypeFor<RenderPipelineDescriptor> {
        using Type = UnpackedRenderPipelineDescriptorChain;
    };
    ResultOrError<UnpackedRenderPipelineDescriptorChain> ValidateAndUnpackChain(const RenderPipelineDescriptor* chain);


}  // namespace dawn::native

#endif  // DAWNNATIVE_CHAIN_UTILS_H_
