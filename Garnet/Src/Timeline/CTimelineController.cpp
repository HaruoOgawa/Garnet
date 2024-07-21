#include "CTimelineController.h"
#ifdef USE_BINARY_WRITE
#include "CTimelineExporter.h"
#endif // USE_BINARY_WRITE
#include "../Input/CInputState.h"
#include "../Scene/CSceneController.h"

namespace timeline
{
	CTimelineController::CTimelineController(float CurrentTime, const std::shared_ptr<CTimelineClip>& Clip, bool PlayFlag) :
		m_PlayBackTime(CurrentTime),
		m_Clip(Clip),
		m_Play(PlayFlag)
	{
	}

	CTimelineController::CTimelineController():
		CTimelineController(0.0f, std::make_shared<timeline::CTimelineClip>(), false)
	{
	}

	CTimelineController::~CTimelineController()
	{
	}

	bool CTimelineController::Initialize(const std::shared_ptr<app::IApp>& App)
	{
		if (m_Clip && App)
		{
			// タイムラインにオブジェクトリストを割り当てる
			m_Clip->AssignObjectResourceToTrack(App->GetObjectList());

			// ファイル名が空ならシーンファイル名の拡張子をtlに変えて割り当てる
			const std::string FileName = m_Clip->GetFileName();

			if (FileName.empty())
			{
				const auto& SceneController = App->GetSceneController();

				if (SceneController)
				{
					std::string SceneFileName = SceneController->GetFileName();

					size_t Index = SceneFileName.find(".");
					if (Index != -1)
					{
						SceneFileName = SceneFileName.substr(0, Index) + ".tl";

						m_Clip->SetFileName(SceneFileName);
					}
				}
			}
		}

		return true;
	}

	void CTimelineController::SetPlayBackTime(float Time)
	{
		m_PlayBackTime = Time;
	}

	float CTimelineController::GetPlayBackTime() const
	{
		return m_PlayBackTime;
	}

	void CTimelineController::SetMaxTime(float Time)
	{
		if (m_Clip) m_Clip->SetMaxTime(Time);
	}

	float CTimelineController::GetMaxTime() const
	{
		if (!m_Clip) return 0.0f;

		return m_Clip->GetMaxTime();
	}

	const std::shared_ptr<CTimelineClip>& CTimelineController::GetClip() const
	{
		return m_Clip;
	}

	void CTimelineController::SetPlay(bool Flag)
	{
		m_Play = Flag;
	}

	bool CTimelineController::IsPlay() const
	{
		return m_Play;
	}

	bool CTimelineController::Update(float DeltaSecondsTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (m_PlayBackTime < 0.0f || m_PlayBackTime > GetMaxTime()) return true;

		if (m_Play)
		{
			m_PlayBackTime += DeltaSecondsTime;
		}

		if (!UpdateClip(m_PlayBackTime, InputState)) return false;

		return true;
	}

	bool CTimelineController::UpdateClip(float CurrentTime, const std::shared_ptr<input::CInputState>& InputState)
	{
		if (!m_Clip) return true;

		if (!m_Clip->Update(CurrentTime)) return false;

#ifdef USE_BINARY_WRITE
		// タイムラインクリップの書き出し
		if (InputState->IsKeyDown(input::EKeyType::KEY_TYPE_CONTROL) && InputState->IsKeyUp(input::EKeyType::KEY_TYPE_S))
		{
			if (!CTimelineExporter::Export(m_Clip->GetFileName(), m_Clip)) return false;
			return true;
		}
#endif // USE_BINARY_WRITE

		return true;
	}
}