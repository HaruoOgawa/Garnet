document.body.style.overflow = 'hidden';

const InitWG = async () => {
    // WebGPUのサポート状況をチェック
    if (!navigator.gpu) {
        console.log("WebGPU isn't support your browser");
        return;
    }
    else
    {
        // モジュールの設定
        const canvas = document.getElementById('MainCanvas');
        const width = canvas.clientWidth;
        const height = canvas.clientHeight;

        Module.canvas = canvas;

        // デバイスを事前取得
        const adapter = await navigator.gpu.requestAdapter();
        const device = await adapter.requestDevice();
        Module.preinitializedWebGPUDevice = device;

        // アプリケーション開始 
        Module.ccall(
            'StartApp',
            'null',
            ['number', 'number'],
            [width, height]
        );
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

addEventListener("resize", (event) => {
    const canvas = document.getElementById('MainCanvas');
    const width = canvas.clientWidth;
    const height = canvas.clientHeight;

    Module.ccall(
        'OnResize',
        'null',
        ['number', 'number'],
        [width, height]
    );
});