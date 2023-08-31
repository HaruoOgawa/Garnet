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

#include "dawn/native/ChainUtils_autogen.h"

#include <tuple>
#include <unordered_set>
#include <utility>

namespace dawn::native {

MaybeError ValidateSTypes(const ChainedStruct* chain,
                          std::vector<std::vector<wgpu::SType>> oneOfConstraints) {
    std::unordered_set<wgpu::SType> allSTypes;
    for (; chain; chain = chain->nextInChain) {
        DAWN_INVALID_IF(allSTypes.find(chain->sType) != allSTypes.end(),
            "Extension chain has duplicate sType %s.", chain->sType);
        allSTypes.insert(chain->sType);
    }

    for (const auto& oneOfConstraint : oneOfConstraints) {
        bool satisfied = false;
        for (wgpu::SType oneOfSType : oneOfConstraint) {
            if (allSTypes.find(oneOfSType) != allSTypes.end()) {
                DAWN_INVALID_IF(satisfied,
                    "sType %s is part of a group of exclusive sTypes that is already present.",
                    oneOfSType);
                satisfied = true;
                allSTypes.erase(oneOfSType);
            }
        }
    }

    DAWN_INVALID_IF(!allSTypes.empty(), "Unsupported sType %s.", *allSTypes.begin());
    return {};
}

MaybeError ValidateSTypes(const ChainedStructOut* chain,
                          std::vector<std::vector<wgpu::SType>> oneOfConstraints) {
    std::unordered_set<wgpu::SType> allSTypes;
    for (; chain; chain = chain->nextInChain) {
        DAWN_INVALID_IF(allSTypes.find(chain->sType) != allSTypes.end(),
            "Extension chain has duplicate sType %s.", chain->sType);
        allSTypes.insert(chain->sType);
    }

    for (const auto& oneOfConstraint : oneOfConstraints) {
        bool satisfied = false;
        for (wgpu::SType oneOfSType : oneOfConstraint) {
            if (allSTypes.find(oneOfSType) != allSTypes.end()) {
                DAWN_INVALID_IF(satisfied,
                    "sType %s is part of a group of exclusive sTypes that is already present.",
                    oneOfSType);
                satisfied = true;
                allSTypes.erase(oneOfSType);
            }
        }
    }

    DAWN_INVALID_IF(!allSTypes.empty(), "Unsupported sType %s.", *allSTypes.begin());
    return {};
}

// Returns true iff the chain's SType matches the extension, false otherwise. If the SType was
// not already matched, sets the unpacked result accordingly. Otherwise, stores the duplicated
// SType in 'duplicate'.
template <typename Root, typename Unpacked, typename Ext>
bool UnpackExtension(Unpacked& unpacked, const ChainedStruct* chain, bool& duplicate) {
    ASSERT(chain != nullptr);
    if (chain->sType == STypeFor<Ext>) {
        auto& member = std::get<Ext>(unpacked);
        if (member != nullptr) {
            duplicate = true;
        } else {
            member = reinterpret_cast<Ext>(chain);
        }
        return true;
    }
    return false;
}

// Tries to match all possible extensions, returning true iff one of the allowed extensions were
// matched, false otherwise. If the SType was not already matched, sets the unpacked result
// accordingly. Otherwise, stores the diplicated SType in 'duplicate'.
template <typename Root, typename Unpacked, typename AdditionalExts>
struct AdditionalExtensionUnpacker;
template <typename Root, typename Unpacked, typename... Exts>
struct AdditionalExtensionUnpacker<Root, Unpacked, detail::AdditionalExtensionsList<Exts...>> {
    static bool Unpack(Unpacked& unpacked, const ChainedStruct* chain, bool& duplicate) {
        return ((UnpackExtension<Root, Unpacked, Exts>(unpacked, chain, duplicate)) || ...);
    }
};

//
// Unpacked chain helpers.
//
ResultOrError<UnpackedBindGroupEntryChain> ValidateAndUnpackChain(const BindGroupEntry* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedBindGroupEntryChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<ExternalTextureBindingEntry>: {
                auto& member = std::get<const ExternalTextureBindingEntry*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const ExternalTextureBindingEntry*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        BindGroupEntry,
                        UnpackedBindGroupEntryChain,
                        detail::AdditionalExtensions<BindGroupEntry>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "BindGroupEntry"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "BindGroupEntry"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedBufferBindingLayoutChain> ValidateAndUnpackChain(const BufferBindingLayout* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedBufferBindingLayoutChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        BufferBindingLayout,
                        UnpackedBufferBindingLayoutChain,
                        detail::AdditionalExtensions<BufferBindingLayout>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "BufferBindingLayout"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "BufferBindingLayout"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedBufferDescriptorChain> ValidateAndUnpackChain(const BufferDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedBufferDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<DawnBufferDescriptorErrorInfoFromWireClient>: {
                auto& member = std::get<const DawnBufferDescriptorErrorInfoFromWireClient*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnBufferDescriptorErrorInfoFromWireClient*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        BufferDescriptor,
                        UnpackedBufferDescriptorChain,
                        detail::AdditionalExtensions<BufferDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "BufferDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "BufferDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedCommandBufferDescriptorChain> ValidateAndUnpackChain(const CommandBufferDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedCommandBufferDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        CommandBufferDescriptor,
                        UnpackedCommandBufferDescriptorChain,
                        detail::AdditionalExtensions<CommandBufferDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "CommandBufferDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "CommandBufferDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedCommandEncoderDescriptorChain> ValidateAndUnpackChain(const CommandEncoderDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedCommandEncoderDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<DawnEncoderInternalUsageDescriptor>: {
                auto& member = std::get<const DawnEncoderInternalUsageDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnEncoderInternalUsageDescriptor*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        CommandEncoderDescriptor,
                        UnpackedCommandEncoderDescriptorChain,
                        detail::AdditionalExtensions<CommandEncoderDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "CommandEncoderDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "CommandEncoderDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedCompilationMessageChain> ValidateAndUnpackChain(const CompilationMessage* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedCompilationMessageChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        CompilationMessage,
                        UnpackedCompilationMessageChain,
                        detail::AdditionalExtensions<CompilationMessage>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "CompilationMessage"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "CompilationMessage"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedConstantEntryChain> ValidateAndUnpackChain(const ConstantEntry* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedConstantEntryChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ConstantEntry,
                        UnpackedConstantEntryChain,
                        detail::AdditionalExtensions<ConstantEntry>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ConstantEntry"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ConstantEntry"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedCopyTextureForBrowserOptionsChain> ValidateAndUnpackChain(const CopyTextureForBrowserOptions* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedCopyTextureForBrowserOptionsChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        CopyTextureForBrowserOptions,
                        UnpackedCopyTextureForBrowserOptionsChain,
                        detail::AdditionalExtensions<CopyTextureForBrowserOptions>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "CopyTextureForBrowserOptions"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "CopyTextureForBrowserOptions"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedInstanceDescriptorChain> ValidateAndUnpackChain(const InstanceDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedInstanceDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<DawnTogglesDescriptor>: {
                auto& member = std::get<const DawnTogglesDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnTogglesDescriptor*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        InstanceDescriptor,
                        UnpackedInstanceDescriptorChain,
                        detail::AdditionalExtensions<InstanceDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "InstanceDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "InstanceDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedMultisampleStateChain> ValidateAndUnpackChain(const MultisampleState* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedMultisampleStateChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<DawnMultisampleStateRenderToSingleSampled>: {
                auto& member = std::get<const DawnMultisampleStateRenderToSingleSampled*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnMultisampleStateRenderToSingleSampled*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        MultisampleState,
                        UnpackedMultisampleStateChain,
                        detail::AdditionalExtensions<MultisampleState>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "MultisampleState"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "MultisampleState"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedPipelineLayoutDescriptorChain> ValidateAndUnpackChain(const PipelineLayoutDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedPipelineLayoutDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<PipelineLayoutPixelLocalStorage>: {
                auto& member = std::get<const PipelineLayoutPixelLocalStorage*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const PipelineLayoutPixelLocalStorage*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        PipelineLayoutDescriptor,
                        UnpackedPipelineLayoutDescriptorChain,
                        detail::AdditionalExtensions<PipelineLayoutDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "PipelineLayoutDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "PipelineLayoutDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedPipelineLayoutStorageAttachmentChain> ValidateAndUnpackChain(const PipelineLayoutStorageAttachment* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedPipelineLayoutStorageAttachmentChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        PipelineLayoutStorageAttachment,
                        UnpackedPipelineLayoutStorageAttachmentChain,
                        detail::AdditionalExtensions<PipelineLayoutStorageAttachment>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "PipelineLayoutStorageAttachment"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "PipelineLayoutStorageAttachment"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedPrimitiveStateChain> ValidateAndUnpackChain(const PrimitiveState* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedPrimitiveStateChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<PrimitiveDepthClipControl>: {
                auto& member = std::get<const PrimitiveDepthClipControl*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const PrimitiveDepthClipControl*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        PrimitiveState,
                        UnpackedPrimitiveStateChain,
                        detail::AdditionalExtensions<PrimitiveState>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "PrimitiveState"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "PrimitiveState"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedQuerySetDescriptorChain> ValidateAndUnpackChain(const QuerySetDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedQuerySetDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        QuerySetDescriptor,
                        UnpackedQuerySetDescriptorChain,
                        detail::AdditionalExtensions<QuerySetDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "QuerySetDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "QuerySetDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedQueueDescriptorChain> ValidateAndUnpackChain(const QueueDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedQueueDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        QueueDescriptor,
                        UnpackedQueueDescriptorChain,
                        detail::AdditionalExtensions<QueueDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "QueueDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "QueueDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedRenderBundleDescriptorChain> ValidateAndUnpackChain(const RenderBundleDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedRenderBundleDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        RenderBundleDescriptor,
                        UnpackedRenderBundleDescriptorChain,
                        detail::AdditionalExtensions<RenderBundleDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "RenderBundleDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "RenderBundleDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedRenderBundleEncoderDescriptorChain> ValidateAndUnpackChain(const RenderBundleEncoderDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedRenderBundleEncoderDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        RenderBundleEncoderDescriptor,
                        UnpackedRenderBundleEncoderDescriptorChain,
                        detail::AdditionalExtensions<RenderBundleEncoderDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "RenderBundleEncoderDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "RenderBundleEncoderDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedRequestAdapterOptionsChain> ValidateAndUnpackChain(const RequestAdapterOptions* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedRequestAdapterOptionsChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<DawnTogglesDescriptor>: {
                auto& member = std::get<const DawnTogglesDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnTogglesDescriptor*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        RequestAdapterOptions,
                        UnpackedRequestAdapterOptionsChain,
                        detail::AdditionalExtensions<RequestAdapterOptions>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "RequestAdapterOptions"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "RequestAdapterOptions"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedSamplerBindingLayoutChain> ValidateAndUnpackChain(const SamplerBindingLayout* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedSamplerBindingLayoutChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        SamplerBindingLayout,
                        UnpackedSamplerBindingLayoutChain,
                        detail::AdditionalExtensions<SamplerBindingLayout>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "SamplerBindingLayout"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "SamplerBindingLayout"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedSamplerDescriptorChain> ValidateAndUnpackChain(const SamplerDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedSamplerDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        SamplerDescriptor,
                        UnpackedSamplerDescriptorChain,
                        detail::AdditionalExtensions<SamplerDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "SamplerDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "SamplerDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedShaderModuleDescriptorChain> ValidateAndUnpackChain(const ShaderModuleDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedShaderModuleDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<ShaderModuleSPIRVDescriptor>: {
                auto& member = std::get<const ShaderModuleSPIRVDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const ShaderModuleSPIRVDescriptor*>(next);
                }
                break;
            }
            case STypeFor<ShaderModuleWGSLDescriptor>: {
                auto& member = std::get<const ShaderModuleWGSLDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const ShaderModuleWGSLDescriptor*>(next);
                }
                break;
            }
            case STypeFor<DawnShaderModuleSPIRVOptionsDescriptor>: {
                auto& member = std::get<const DawnShaderModuleSPIRVOptionsDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnShaderModuleSPIRVOptionsDescriptor*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ShaderModuleDescriptor,
                        UnpackedShaderModuleDescriptorChain,
                        detail::AdditionalExtensions<ShaderModuleDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ShaderModuleDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ShaderModuleDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedSharedFenceDescriptorChain> ValidateAndUnpackChain(const SharedFenceDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedSharedFenceDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<SharedFenceVkSemaphoreOpaqueFDDescriptor>: {
                auto& member = std::get<const SharedFenceVkSemaphoreOpaqueFDDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedFenceVkSemaphoreOpaqueFDDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedFenceVkSemaphoreSyncFDDescriptor>: {
                auto& member = std::get<const SharedFenceVkSemaphoreSyncFDDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedFenceVkSemaphoreSyncFDDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedFenceVkSemaphoreZirconHandleDescriptor>: {
                auto& member = std::get<const SharedFenceVkSemaphoreZirconHandleDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedFenceVkSemaphoreZirconHandleDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedFenceDXGISharedHandleDescriptor>: {
                auto& member = std::get<const SharedFenceDXGISharedHandleDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedFenceDXGISharedHandleDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedFenceMTLSharedEventDescriptor>: {
                auto& member = std::get<const SharedFenceMTLSharedEventDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedFenceMTLSharedEventDescriptor*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        SharedFenceDescriptor,
                        UnpackedSharedFenceDescriptorChain,
                        detail::AdditionalExtensions<SharedFenceDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "SharedFenceDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "SharedFenceDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedSharedTextureMemoryBeginAccessDescriptorChain> ValidateAndUnpackChain(const SharedTextureMemoryBeginAccessDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedSharedTextureMemoryBeginAccessDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<SharedTextureMemoryVkImageLayoutBeginState>: {
                auto& member = std::get<const SharedTextureMemoryVkImageLayoutBeginState*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryVkImageLayoutBeginState*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        SharedTextureMemoryBeginAccessDescriptor,
                        UnpackedSharedTextureMemoryBeginAccessDescriptorChain,
                        detail::AdditionalExtensions<SharedTextureMemoryBeginAccessDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "SharedTextureMemoryBeginAccessDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "SharedTextureMemoryBeginAccessDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedSharedTextureMemoryDescriptorChain> ValidateAndUnpackChain(const SharedTextureMemoryDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedSharedTextureMemoryDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<SharedTextureMemoryVkImageDescriptor>: {
                auto& member = std::get<const SharedTextureMemoryVkImageDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryVkImageDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedTextureMemoryVkDedicatedAllocationDescriptor>: {
                auto& member = std::get<const SharedTextureMemoryVkDedicatedAllocationDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryVkDedicatedAllocationDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedTextureMemoryAHardwareBufferDescriptor>: {
                auto& member = std::get<const SharedTextureMemoryAHardwareBufferDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryAHardwareBufferDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedTextureMemoryDmaBufDescriptor>: {
                auto& member = std::get<const SharedTextureMemoryDmaBufDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryDmaBufDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedTextureMemoryOpaqueFDDescriptor>: {
                auto& member = std::get<const SharedTextureMemoryOpaqueFDDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryOpaqueFDDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedTextureMemoryZirconHandleDescriptor>: {
                auto& member = std::get<const SharedTextureMemoryZirconHandleDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryZirconHandleDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedTextureMemoryDXGISharedHandleDescriptor>: {
                auto& member = std::get<const SharedTextureMemoryDXGISharedHandleDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryDXGISharedHandleDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedTextureMemoryIOSurfaceDescriptor>: {
                auto& member = std::get<const SharedTextureMemoryIOSurfaceDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryIOSurfaceDescriptor*>(next);
                }
                break;
            }
            case STypeFor<SharedTextureMemoryEGLImageDescriptor>: {
                auto& member = std::get<const SharedTextureMemoryEGLImageDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SharedTextureMemoryEGLImageDescriptor*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        SharedTextureMemoryDescriptor,
                        UnpackedSharedTextureMemoryDescriptorChain,
                        detail::AdditionalExtensions<SharedTextureMemoryDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "SharedTextureMemoryDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "SharedTextureMemoryDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedStorageTextureBindingLayoutChain> ValidateAndUnpackChain(const StorageTextureBindingLayout* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedStorageTextureBindingLayoutChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        StorageTextureBindingLayout,
                        UnpackedStorageTextureBindingLayoutChain,
                        detail::AdditionalExtensions<StorageTextureBindingLayout>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "StorageTextureBindingLayout"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "StorageTextureBindingLayout"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedSurfaceDescriptorChain> ValidateAndUnpackChain(const SurfaceDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedSurfaceDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<SurfaceDescriptorFromAndroidNativeWindow>: {
                auto& member = std::get<const SurfaceDescriptorFromAndroidNativeWindow*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SurfaceDescriptorFromAndroidNativeWindow*>(next);
                }
                break;
            }
            case STypeFor<SurfaceDescriptorFromCanvasHTMLSelector>: {
                auto& member = std::get<const SurfaceDescriptorFromCanvasHTMLSelector*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SurfaceDescriptorFromCanvasHTMLSelector*>(next);
                }
                break;
            }
            case STypeFor<SurfaceDescriptorFromMetalLayer>: {
                auto& member = std::get<const SurfaceDescriptorFromMetalLayer*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SurfaceDescriptorFromMetalLayer*>(next);
                }
                break;
            }
            case STypeFor<SurfaceDescriptorFromWindowsHWND>: {
                auto& member = std::get<const SurfaceDescriptorFromWindowsHWND*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SurfaceDescriptorFromWindowsHWND*>(next);
                }
                break;
            }
            case STypeFor<SurfaceDescriptorFromXlibWindow>: {
                auto& member = std::get<const SurfaceDescriptorFromXlibWindow*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SurfaceDescriptorFromXlibWindow*>(next);
                }
                break;
            }
            case STypeFor<SurfaceDescriptorFromWaylandSurface>: {
                auto& member = std::get<const SurfaceDescriptorFromWaylandSurface*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SurfaceDescriptorFromWaylandSurface*>(next);
                }
                break;
            }
            case STypeFor<SurfaceDescriptorFromWindowsCoreWindow>: {
                auto& member = std::get<const SurfaceDescriptorFromWindowsCoreWindow*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SurfaceDescriptorFromWindowsCoreWindow*>(next);
                }
                break;
            }
            case STypeFor<SurfaceDescriptorFromWindowsSwapChainPanel>: {
                auto& member = std::get<const SurfaceDescriptorFromWindowsSwapChainPanel*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const SurfaceDescriptorFromWindowsSwapChainPanel*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        SurfaceDescriptor,
                        UnpackedSurfaceDescriptorChain,
                        detail::AdditionalExtensions<SurfaceDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "SurfaceDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "SurfaceDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedSwapChainDescriptorChain> ValidateAndUnpackChain(const SwapChainDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedSwapChainDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        SwapChainDescriptor,
                        UnpackedSwapChainDescriptorChain,
                        detail::AdditionalExtensions<SwapChainDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "SwapChainDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "SwapChainDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedTextureBindingLayoutChain> ValidateAndUnpackChain(const TextureBindingLayout* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedTextureBindingLayoutChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        TextureBindingLayout,
                        UnpackedTextureBindingLayoutChain,
                        detail::AdditionalExtensions<TextureBindingLayout>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "TextureBindingLayout"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "TextureBindingLayout"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedTextureDataLayoutChain> ValidateAndUnpackChain(const TextureDataLayout* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedTextureDataLayoutChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        TextureDataLayout,
                        UnpackedTextureDataLayoutChain,
                        detail::AdditionalExtensions<TextureDataLayout>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "TextureDataLayout"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "TextureDataLayout"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedTextureViewDescriptorChain> ValidateAndUnpackChain(const TextureViewDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedTextureViewDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        TextureViewDescriptor,
                        UnpackedTextureViewDescriptorChain,
                        detail::AdditionalExtensions<TextureViewDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "TextureViewDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "TextureViewDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedBindGroupDescriptorChain> ValidateAndUnpackChain(const BindGroupDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedBindGroupDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        BindGroupDescriptor,
                        UnpackedBindGroupDescriptorChain,
                        detail::AdditionalExtensions<BindGroupDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "BindGroupDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "BindGroupDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedBindGroupLayoutEntryChain> ValidateAndUnpackChain(const BindGroupLayoutEntry* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedBindGroupLayoutEntryChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<ExternalTextureBindingLayout>: {
                auto& member = std::get<const ExternalTextureBindingLayout*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const ExternalTextureBindingLayout*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        BindGroupLayoutEntry,
                        UnpackedBindGroupLayoutEntryChain,
                        detail::AdditionalExtensions<BindGroupLayoutEntry>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "BindGroupLayoutEntry"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "BindGroupLayoutEntry"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedCompilationInfoChain> ValidateAndUnpackChain(const CompilationInfo* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedCompilationInfoChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        CompilationInfo,
                        UnpackedCompilationInfoChain,
                        detail::AdditionalExtensions<CompilationInfo>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "CompilationInfo"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "CompilationInfo"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedComputePassDescriptorChain> ValidateAndUnpackChain(const ComputePassDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedComputePassDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ComputePassDescriptor,
                        UnpackedComputePassDescriptorChain,
                        detail::AdditionalExtensions<ComputePassDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ComputePassDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ComputePassDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedDepthStencilStateChain> ValidateAndUnpackChain(const DepthStencilState* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedDepthStencilStateChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        DepthStencilState,
                        UnpackedDepthStencilStateChain,
                        detail::AdditionalExtensions<DepthStencilState>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "DepthStencilState"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "DepthStencilState"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedExternalTextureDescriptorChain> ValidateAndUnpackChain(const ExternalTextureDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedExternalTextureDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ExternalTextureDescriptor,
                        UnpackedExternalTextureDescriptorChain,
                        detail::AdditionalExtensions<ExternalTextureDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ExternalTextureDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ExternalTextureDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedImageCopyBufferChain> ValidateAndUnpackChain(const ImageCopyBuffer* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedImageCopyBufferChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ImageCopyBuffer,
                        UnpackedImageCopyBufferChain,
                        detail::AdditionalExtensions<ImageCopyBuffer>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ImageCopyBuffer"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ImageCopyBuffer"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedImageCopyExternalTextureChain> ValidateAndUnpackChain(const ImageCopyExternalTexture* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedImageCopyExternalTextureChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ImageCopyExternalTexture,
                        UnpackedImageCopyExternalTextureChain,
                        detail::AdditionalExtensions<ImageCopyExternalTexture>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ImageCopyExternalTexture"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ImageCopyExternalTexture"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedImageCopyTextureChain> ValidateAndUnpackChain(const ImageCopyTexture* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedImageCopyTextureChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ImageCopyTexture,
                        UnpackedImageCopyTextureChain,
                        detail::AdditionalExtensions<ImageCopyTexture>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ImageCopyTexture"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ImageCopyTexture"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedProgrammableStageDescriptorChain> ValidateAndUnpackChain(const ProgrammableStageDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedProgrammableStageDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ProgrammableStageDescriptor,
                        UnpackedProgrammableStageDescriptorChain,
                        detail::AdditionalExtensions<ProgrammableStageDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ProgrammableStageDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ProgrammableStageDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedRenderPassColorAttachmentChain> ValidateAndUnpackChain(const RenderPassColorAttachment* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedRenderPassColorAttachmentChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<DawnRenderPassColorAttachmentRenderToSingleSampled>: {
                auto& member = std::get<const DawnRenderPassColorAttachmentRenderToSingleSampled*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnRenderPassColorAttachmentRenderToSingleSampled*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        RenderPassColorAttachment,
                        UnpackedRenderPassColorAttachmentChain,
                        detail::AdditionalExtensions<RenderPassColorAttachment>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "RenderPassColorAttachment"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "RenderPassColorAttachment"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedRenderPassStorageAttachmentChain> ValidateAndUnpackChain(const RenderPassStorageAttachment* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedRenderPassStorageAttachmentChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        RenderPassStorageAttachment,
                        UnpackedRenderPassStorageAttachmentChain,
                        detail::AdditionalExtensions<RenderPassStorageAttachment>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "RenderPassStorageAttachment"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "RenderPassStorageAttachment"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedRequiredLimitsChain> ValidateAndUnpackChain(const RequiredLimits* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedRequiredLimitsChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        RequiredLimits,
                        UnpackedRequiredLimitsChain,
                        detail::AdditionalExtensions<RequiredLimits>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "RequiredLimits"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "RequiredLimits"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedTextureDescriptorChain> ValidateAndUnpackChain(const TextureDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedTextureDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<DawnTextureInternalUsageDescriptor>: {
                auto& member = std::get<const DawnTextureInternalUsageDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnTextureInternalUsageDescriptor*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        TextureDescriptor,
                        UnpackedTextureDescriptorChain,
                        detail::AdditionalExtensions<TextureDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "TextureDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "TextureDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedBindGroupLayoutDescriptorChain> ValidateAndUnpackChain(const BindGroupLayoutDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedBindGroupLayoutDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        BindGroupLayoutDescriptor,
                        UnpackedBindGroupLayoutDescriptorChain,
                        detail::AdditionalExtensions<BindGroupLayoutDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "BindGroupLayoutDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "BindGroupLayoutDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedColorTargetStateChain> ValidateAndUnpackChain(const ColorTargetState* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedColorTargetStateChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ColorTargetState,
                        UnpackedColorTargetStateChain,
                        detail::AdditionalExtensions<ColorTargetState>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ColorTargetState"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ColorTargetState"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedComputePipelineDescriptorChain> ValidateAndUnpackChain(const ComputePipelineDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedComputePipelineDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        ComputePipelineDescriptor,
                        UnpackedComputePipelineDescriptorChain,
                        detail::AdditionalExtensions<ComputePipelineDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "ComputePipelineDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "ComputePipelineDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedDeviceDescriptorChain> ValidateAndUnpackChain(const DeviceDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedDeviceDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<DawnTogglesDescriptor>: {
                auto& member = std::get<const DawnTogglesDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnTogglesDescriptor*>(next);
                }
                break;
            }
            case STypeFor<DawnCacheDeviceDescriptor>: {
                auto& member = std::get<const DawnCacheDeviceDescriptor*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const DawnCacheDeviceDescriptor*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        DeviceDescriptor,
                        UnpackedDeviceDescriptorChain,
                        detail::AdditionalExtensions<DeviceDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "DeviceDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "DeviceDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedRenderPassDescriptorChain> ValidateAndUnpackChain(const RenderPassDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedRenderPassDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            case STypeFor<RenderPassDescriptorMaxDrawCount>: {
                auto& member = std::get<const RenderPassDescriptorMaxDrawCount*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const RenderPassDescriptorMaxDrawCount*>(next);
                }
                break;
            }
            case STypeFor<RenderPassPixelLocalStorage>: {
                auto& member = std::get<const RenderPassPixelLocalStorage*>(result);
                if (member != nullptr) {
                    duplicate = true;
                } else {
                    member = static_cast<const RenderPassPixelLocalStorage*>(next);
                }
                break;
            }
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        RenderPassDescriptor,
                        UnpackedRenderPassDescriptorChain,
                        detail::AdditionalExtensions<RenderPassDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "RenderPassDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "RenderPassDescriptor"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedVertexStateChain> ValidateAndUnpackChain(const VertexState* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedVertexStateChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        VertexState,
                        UnpackedVertexStateChain,
                        detail::AdditionalExtensions<VertexState>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "VertexState"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "VertexState"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedFragmentStateChain> ValidateAndUnpackChain(const FragmentState* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedFragmentStateChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        FragmentState,
                        UnpackedFragmentStateChain,
                        detail::AdditionalExtensions<FragmentState>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "FragmentState"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "FragmentState"
            );
        }
    }
    return result;
}

ResultOrError<UnpackedRenderPipelineDescriptorChain> ValidateAndUnpackChain(const RenderPipelineDescriptor* chain) {
    const ChainedStruct* next = chain->nextInChain;
    UnpackedRenderPipelineDescriptorChain result;

    for (; next != nullptr; next = next->nextInChain) {
        bool duplicate = false;
        switch (next->sType) {
            default: {
                using Unpacker =
                    AdditionalExtensionUnpacker<
                        RenderPipelineDescriptor,
                        UnpackedRenderPipelineDescriptorChain,
                        detail::AdditionalExtensions<RenderPipelineDescriptor>::List>;
                if (!Unpacker::Unpack(result, next, duplicate)) {
                    return DAWN_VALIDATION_ERROR(
                        "Unexpected chained struct of type %s found on %s chain.",
                        next->sType, "RenderPipelineDescriptor"
                    );
                }
                break;
            }
        }
        if (duplicate) {
            return DAWN_VALIDATION_ERROR(
                "Duplicate chained struct of type %s found on %s chain.",
                next->sType, "RenderPipelineDescriptor"
            );
        }
    }
    return result;
}


}  // namespace dawn::native
