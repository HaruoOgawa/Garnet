#if defined(USE_RTXGI) && defined(USE_VULKAN)
#include "CVulkanRTXGIController.h"

using namespace rtxgi;
using namespace rtxgi::vulkan;

namespace graphics
{
	CVulkanRTXGIController::CVulkanRTXGIController():
		CRTXGIController()
	{
	}

	CVulkanRTXGIController::~CVulkanRTXGIController()
	{
	}

	bool CVulkanRTXGIController::Initialize(api::IGraphicsAPI* pGraphicsAPI)
	{
		if (!CreateDDGIVolume(pGraphicsAPI)) return false;

		return true;
	}

	bool CVulkanRTXGIController::Update(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CVulkanRTXGIController::Draw(api::IGraphicsAPI* pGraphicsAPI)
	{
		return true;
	}

	bool CVulkanRTXGIController::CreateDDGIVolume(api::IGraphicsAPI* pGraphicsAPI)
	{
		// ボリュームの設定 //////////////////////////////////////////////////////////////////////////////////////////////
		std::string VolumeName = "TestVolume";
		size_t size = VolumeName.size();

		DDGIVolumeDesc volumeDesc;
		volumeDesc.name = new char[size + 1];
		std::memset(volumeDesc.name, 0, size + 1);
		std::memcpy(volumeDesc.name, VolumeName.c_str(), size);

		volumeDesc.index = 0; // Shaderのコンスタントバッファでのインデックス
		volumeDesc.rngSeed = 0; // 乱数生成のシード。たしかフレームごとにプローブから出るレイの方向をランダムに決めていたはず
		volumeDesc.origin = { 0.0f, 1.0f, 0.0f };
		volumeDesc.eulerAngles = { 0.0f, 0.0f, 0.0f };
		volumeDesc.probeSpacing = { 0.3f, 0.3f, 0.3f }; // プローブの配置間隔
		volumeDesc.probeCounts = { 9, 9, 9 }; // グリッド内のプローブ数
		volumeDesc.probeNumRays = 256; // 1フレームの間に1つのプローブから出るレイの数
		volumeDesc.probeNumIrradianceTexels = 8; // 放射照度テクスチャの1つの次元(RGBAチャンネルのそれぞれ)で使われるテクセルの数。外枠あり
		// 放射照度テクスチャの1つの次元(RGBAチャンネルのそれぞれ)で使われるテクセルの数。外枠を除くので2を引く
		volumeDesc.probeNumIrradianceInteriorTexels = volumeDesc.probeNumIrradianceTexels - 2; 
		volumeDesc.probeNumDistanceTexels = 16; // 距離テクスチャの1つの次元(RGBAチャンネルのそれぞれ)で使われるテクセルの数。外枠あり
		// 距離テクスチャの1つの次元(RGBAチャンネルのそれぞれ)で使われるテクセルの数。外枠を除くので2を引く
		volumeDesc.probeNumDistanceInteriorTexels = volumeDesc.probeNumDistanceTexels - 2;
		// プローブの位置などが更新されたときにどれぐらい影響を受けるかの割合。1に近いほどゆっくり変化する
		volumeDesc.probeHysteresis = 0.97f; 
		volumeDesc.probeMaxRayDistance = 10.0f; // ワールドスペースにてプローブから出たレイが進む最大距離
		volumeDesc.probeNormalBias = 0.02f; // 計算誤差を防ぐための法線方向の小さなオフセット
		volumeDesc.probeViewBias = 0.1f; // 計算誤差を防ぐためのカメラビューベクトル方向の小さなオフセット
		volumeDesc.probeIrradianceThreshold = 0.2f; // 放射照度がこれよりも大きかったらprobeHysteresisが減少する？
		volumeDesc.probeBrightnessThreshold = 1.0f;

		volumeDesc.showProbes = false; // プローブを可視化するときに必要なフラグ(たぶんデバッグ情報の付与)
		volumeDesc.probeVisType = EDDGIVolumeProbeVisType::Default;

		volumeDesc.probeRayDataFormat = EDDGIVolumeTextureFormat::F32x2; // レイデータを格納するテクスチャのテクセルフォーマット
		volumeDesc.probeIrradianceFormat = EDDGIVolumeTextureFormat::U32; // 放射照度データを格納するテクスチャのテクセルフォーマット
		volumeDesc.probeDistanceFormat = EDDGIVolumeTextureFormat::F16x2; // 距離データを格納するテクスチャのテクセルフォーマット
		volumeDesc.probeDataFormat = EDDGIVolumeTextureFormat::F16x4; // プローブデータを格納するテクスチャのテクセルフォーマット
		volumeDesc.probeVariabilityFormat = EDDGIVolumeTextureFormat::F16; // プローブの変動率データを格納するテクスチャのテクセルフォーマット

		volumeDesc.probeRelocationEnabled = true; // プローブが再配置可能か
		volumeDesc.probeMinFrontfaceDistance = 0.1f; // 再配置時の最小移動距離？(最適化のために決まった範囲内のスピードでしか移動しないみたいな)
		volumeDesc.probeClassificationEnabled = true; // ブレンディングの作業負荷を軽減するために、プローブに状態をマーク
		volumeDesc.probeVariabilityEnabled = false; // 更新間のプローブの変化を追跡

		// ボリュームが動くかどうかの設定
		bool Move = false;
		if (Move) volumeDesc.movementType = EDDGIVolumeMovementType::Scrolling;
		else volumeDesc.movementType = EDDGIVolumeMovementType::Default;

		// ボリュームリソースの設定 //////////////////////////////////////////////////////////////////////////////////////////////
		DDGIVolumeResources volumeResources;

		// ボリューム生成
		DDGIVolume* volume = new DDGIVolume();
		ERTXGIStatus status = volume->Create(volumeDesc, volumeResources);

		return true;
	}
}
#endif // USE_RTXGI && USE_VULKAN