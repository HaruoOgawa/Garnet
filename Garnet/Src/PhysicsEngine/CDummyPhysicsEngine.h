#pragma once

// USE_PHYSICSがない時に使うダミーの空物理エンジン
// 物理エンジンをバイナリに含めるかどうか引数を変えるのはめんどくさいのでこれを使う
// GUIEngineや他の部分にもDummyの考え方は流用する
class CDummyPhysicsEngine
{
};

