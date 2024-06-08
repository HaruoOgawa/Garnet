# GarnetLauncher 
## Garnetライブラリの作り方
1. GarnetのビルドタイプをStatic Linker Libraryにする
2. 基準となるプロジェクト(GarnetFrame)を作成
3. GarnetFrameにGarnetプロジェクトの参照と依存関係を追加する https://itsakura.com/visualstudio-projectref
4. Garnet/Srcをインクルードディレクトリとして追加する
5. Garnet\Src\Library\glmをインクルードディレクトリとして追加する
6. ロード用のMFをResoucesにコピーしておく
7. GraphicsAPIやWindowAPIを変えたいときはConfigrationをDebug, DebugGL, DebugWGなどに切り替えるだけで十分。VSデバッグではチェックできないが、x64のビルド結果ではきちんとAPIがわかれている(それぞれの.libがリンクされる)
