#pragma once

namespace resource
{
	enum class ELoadStatus
	{
		None,
		Loading,
		Loaded,

		AssertError, // エラー通知状態(強制終了するようなエラーではない時用)
	};
}