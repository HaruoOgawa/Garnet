#if defined(USE_GPGPU) && defined(USE_WEBGPU)
#include "CWebGPUGPGPUHandler.h"
#include "CWebGPUMaterial.h"

namespace api
{
	CWebGPUGPGPUHandler::CWebGPUGPGPUHandler(api::CWebGPUAPI* pGraphicsAPI, const std::shared_ptr<graphics::CMaterial>& ComputeMaterial) :
		m_pGraphicsAPI(pGraphicsAPI),
		m_ComputeMaterial(ComputeMaterial),
		m_ComputePipelineLayout(nullptr),
		m_ComputePipeline(nullptr),
		m_ComputePass(nullptr)
	{
	}

	CWebGPUGPGPUHandler::~CWebGPUGPGPUHandler()
	{
	}

	const std::shared_ptr<graphics::CMaterial>& CWebGPUGPGPUHandler::GetComputeMaterial()
	{
		return m_ComputeMaterial;
	}

	bool CWebGPUGPGPUHandler::Create()
	{
		if (!m_ComputeMaterial) return false;

		if (!m_ComputeMaterial->Create(std::vector<std::shared_ptr<graphics::CTexture>>(), std::vector<std::shared_ptr<graphics::CTexture>>())) return false;
		if (!CreateComputePipeline()) return false; // Compute Pipelineを作成

		return true;
	}

	bool CWebGPUGPGPUHandler::CreateComputePipeline()
	{
		CWebGPUMaterial* pWebGPUMat = static_cast<CWebGPUMaterial*>(m_ComputeMaterial.get());

		// Layoutの作成
		WGPUPipelineLayoutDescriptor pipelineLayoutInfo{};
		pipelineLayoutInfo.bindGroupLayoutCount = 1;
		pipelineLayoutInfo.bindGroupLayouts = &pWebGPUMat->GetBindGroupLayout();

		m_ComputePipelineLayout = wgpuDeviceCreatePipelineLayout(m_pGraphicsAPI->GetLogicalDevice(), &pipelineLayoutInfo);

		if (m_ComputePipelineLayout == nullptr)
		{
			return false;
		}

		// Pipelineの作成
		WGPUProgrammableStageDescriptor ComputeShaderDescriptor{};
		ComputeShaderDescriptor.module = pWebGPUMat->GetComputeShaderModele();
		ComputeShaderDescriptor.entryPoint = "main";
		ComputeShaderDescriptor.constantCount = 0;
		ComputeShaderDescriptor.constants = nullptr;

		WGPUComputePipelineDescriptor pipelineInfo{};
		pipelineInfo.layout = m_ComputePipelineLayout;
		pipelineInfo.compute = ComputeShaderDescriptor;

		m_ComputePipeline = wgpuDeviceCreateComputePipeline(m_pGraphicsAPI->GetLogicalDevice(), &pipelineInfo);

		if (m_ComputePipeline == nullptr)
		{
			return false;
		}

		return true;
	}

	bool CWebGPUGPGPUHandler::Dispatch(const glm::ivec3& GroupCount, float SecondsTime, const std::shared_ptr<camera::CCamera>& Camera, const std::shared_ptr<projection::CProjection>& Projection,
		const std::shared_ptr<graphics::CDrawInfo>& DrawInfo)
	{
		CWebGPUMaterial* pWebGPUMat = static_cast<CWebGPUMaterial*>(m_ComputeMaterial.get());

		// データの更新
		if (!pWebGPUMat->SetCommonUniform(SecondsTime, Camera, Projection, DrawInfo)) return false;
		if (!pWebGPUMat->BuildDrawBuffer(0)) return false;

		// コマンドバッファの記録開始
		if (!BeginRecordCommandBuffer()) return false;

		// Cmd
		wgpuComputePassEncoderSetPipeline(m_ComputePass, m_ComputePipeline);
		wgpuComputePassEncoderSetBindGroup(m_ComputePass, 0, pWebGPUMat->GetBindGroup(), 0, NULL);
		wgpuComputePassEncoderDispatchWorkgroups(m_ComputePass, GroupCount.x, GroupCount.y, GroupCount.z);

		// コマンドバッファの記録終了
		if (!EndRecordCommandBuffer()) return false;

		return true;
	}

	bool CWebGPUGPGPUHandler::BeginRecordCommandBuffer()
	{
		m_ComputePass = wgpuCommandEncoderBeginComputePass(m_pGraphicsAPI->GetEncoder(), NULL);

		return true;
	}

	bool CWebGPUGPGPUHandler::EndRecordCommandBuffer()
	{
		wgpuComputePassEncoderEnd(m_ComputePass);

		return true;
	}
}
#endif // USE_GPGPU