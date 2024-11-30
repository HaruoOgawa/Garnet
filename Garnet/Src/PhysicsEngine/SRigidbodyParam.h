#pragma once

#include "EPhysicsType.h"
#include <string>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace physics
{
	struct SRigidbodyParam
	{
		std::pair<std::string, std::wstring> RigidbodyName = { "", L"" };
		EPhysicsType PhysicsType = EPhysicsType::STATIC;

		float Mass = 1.0f;

		bool UseCollideMask = false;

		// CollideMaskは0以外である必要がある
		// 0は何にも衝突しない判定になる(?)
		//  32768, 16384, 8192, 4096, 2048, 1024, 512, 256, 128, 64, 32, 16, 8, 4, 2, 1の16パターン
		// (Pmxの仕様書でunsigned charになっているのだが、PmxEditorのボタンだとグループ16まであるのはなぜ？？？)
		unsigned short group = 1; // 0000000000000001b

		// こっちは0の時は全てに衝突する
		// フラグが立つと当たらない → あとでNot演算をして反転する
		unsigned short NoneCollideGroupFlag = 0; // 0000000000000000b => (Bit Not) => 1111111111111111b => 全て衝突する
		
		/// <summary> 衝突例 1
		/// 0000000000000001b
		/// 1111111111111111b
		/// and -----------(両方のビットが1なら1)
		/// 0000000000000001b(true)
		/// </summary>
		/// 
		/// <summary> 衝突例 2
		/// 0000000000000001b
		/// 1111111111111110b
		/// and -----------(両方のビットが1なら1)
		/// 0000000000000000b(false)
		/// </summary>

		float TransDamping = 0.0f;
		float RotateDamping = 0.0f;
		float Repulsion = 0.0f;
		float Friction = 0.5f;
	};
}
