#pragma once

namespace descapp
{
	class CDescAppManager
	{
		// インプットプールを作成する
		// 1フレームに1つしか受け取れなかったり、途中で処理を挟まれたりするのもよくないので、1フレームの間のインプットをプールに溜めて
		// まとめて実行する

		// 使用するGraphicsAPIのクラスもここでインスタンス化する
		// Desctopなら

		// アプリケーションタイプはメインからもらう
	private:
		bool Release();
	public:
		CDescAppManager();
		virtual ~CDescAppManager();
		bool Initialize();
		bool RunLopp();


		// インプットイベント
	};
}