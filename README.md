# Garnet 3D Engine
## Dependencies
* nuget packages
  * [tbb](https://github.com/oneapi-src/oneTBB) 
* [naga](https://github.com/gfx-rs/naga)
  * Convert SPIR-V to WGSL
* [VSYASM](https://github.com/ShiftMediaProject/VSYASM)
  * compile 4klang
* emsdk 3.1.44
* webgpu
  * Google Dawn [chromium/5904](https://dawn.googlesource.com/dawn/+/refs/heads/chromium/5904)
  * [ninja 1.11.1](https://github.com/ninja-build/ninja)
## Overview
Garnet is Computer Graphics Engine running on Vulkan and WebGPU.
https://scrapbox.io/Garnet/
![image](https://user-images.githubusercontent.com/55416754/215251947-5618f9cd-3a50-4127-9ff0-dde9790b004b.png)

## 開発ルール
* プリプロセッサの使用はライブラリと紐づくクラスに対してのみ使用可能
  * そのライブラリを使用しない時はダミー継承クラスで代用する
  * 関数の引数がプリプロセッサで無限に汚くなるのを防ぐため
  
## 注意事項
* bulletはReleaseビルドを使わないと重い(Debug: 10 ~ 20FPS, Release: 60FPS)
