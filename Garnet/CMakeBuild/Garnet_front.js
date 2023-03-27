const InitWG = async () => {
    // WebGPUのサポート状況をチェック
    if (!navigator.gpu) {
        console.log("WebGPU isn't support your browser");
        return;
    }
    else
    {
        // モジュールの設定
        Module.canvas = document.getElementById('MainCanvas');

        // デバイスを事前取得
        const adapter = await navigator.gpu.requestAdapter();
        const device = await adapter.requestDevice();
        Module.preinitializedWebGPUDevice = device;
        console.log("WebGPUDevice is pre initialized!!");

        // アプリケーション開始 
        Module.ccall('StartApp', 'null', [], []);
    }
};

addEventListener("load", (event) => {
    InitWG();
});

addEventListener("keydown", (event) => {
    Module.ccall(
        'OnKeyDown',
        'null',
        ['string'],
        [event.key]
    );
});