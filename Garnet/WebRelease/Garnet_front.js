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

        const requiredFeatures = [];
        if (adapter.features.has("depth32float-stencil8"))
        {
            requiredFeatures.push("depth32float-stencil8");
        }

        const requiredLimits = [];

        const device = await adapter.requestDevice({
            defaultQueue: {
            },
            requiredFeatures,
            requiredLimits
        });
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

addEventListener("keyup", (event) => {
    Module.ccall(
        'OnKeyUp',
        'null',
        ['string'],
        [event.key]
    );
});

addEventListener("contextmenu", (event) => {
    // 右クリック時にメニューが出ないようにする
    event.preventDefault();
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

addEventListener("mousedown", (event) => {
    Module.ccall(
        `OnMouseDown`,
        `null`,
        ['number', 'number', 'number'],
        [event.button, event.clientX, event.clientY]
    );
});

addEventListener("mouseup", (event) => {
    Module.ccall(
        `OnMouseUp`,
        `null`,
        ['number', 'number', 'number'],
        [event.button, event.clientX, event.clientY]
    );
});

addEventListener("mousemove", (event) => {
    Module.ccall(
        `OnMouseMove`,
        `null`,
        ['number', 'number'],
        [event.clientX, event.clientY]
    );
});

addEventListener("wheel", (event) => {
    Module.ccall(
        "OnMouseWheel",
        "null",
        ["number"],
        [event.deltaY]
    );
});