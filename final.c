
window.done = false;

(function() {
    const OrigWS = window.WebSocket;
    window.WebSocket = function(url, protocols) {
        const ws = new OrigWS(url, protocols);
        window.gameWS = ws;

        const origSend = ws.send;
        window.gameWS.send = function(data) {
            const bytes = new Uint8Array(data);

            if (bytes[4] === 129 && bytes[5] === 4) {
                if(!window.done){
                    return; 
                }
            }

            return origSend.apply(this, arguments);
        };
        return ws;
    };
    window.WebSocket.prototype = OrigWS.prototype;
})();



const sleep = (ms) => new Promise(resolve => setTimeout(resolve, ms));

async function vao_pho_ban() {
    // Giai đoạn 1: Yêu cầu danh sách phòng
    window.done = false;
    const goiLayDanhSach = new Uint8Array([20, 0, 0, 0, 167, 39, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 249, 167, 0, 0, 0, 0, 0, 0]);
    window.gameWS.send(goiLayDanhSach);
    await sleep(20); // Chờ Server trả về gói 75 27
    const goiChuyenTrangThai = new Uint8Array([4, 0, 0, 0, 29, 4, 0, 0]);
    window.gameWS.send(goiChuyenTrangThai);
}

async function treo_may() {
    while (1) {
        await vao_pho_ban();
        await sleep(4600);
        window.done = true;
        const thoat = new Uint8Array([4, 0, 0, 0, 129, 4, 0, 0]);
        window.gameWS.send(thoat);
    }
}




(function() {
    // 1. Vô hiệu hóa các lệnh vẽ nặng của WebGL (Tắt render GPU)
    const blockWebGLDraw = (proto) => {
        if (!proto) return;
        const noop = function() {};
        // Chặn các hàm vẽ đa giác và xóa màn hình tốn GPU nhất
        proto.drawElements = noop;
        proto.drawArrays = noop;
        if (proto.drawElementsInstanced) proto.drawElementsInstanced = noop;
        if (proto.drawArraysInstanced) proto.drawArraysInstanced = noop;
    };

    if (window.WebGLRenderingContext) blockWebGLDraw(WebGLRenderingContext.prototype);
    if (window.WebGL2RenderingContext) blockWebGLDraw(WebGL2RenderingContext.prototype);

    // 2. Hạ nhịp vòng lặp logic xuống 5 FPS (thay vì 60 FPS) bằng Web Worker
    // Giúp CPU chỉ tính toán logic 5 lần/giây - vừa đủ mượt cho packet mạng, vừa cực mát máy
    const workerBlob = new Blob([`
        setInterval(() => {
            postMessage('tick');
        }, 200); // 200ms = 5 FPS (Giảm 90% tải CPU so với 60 FPS)
    `], { type: 'application/javascript' });

    const worker = new Worker(URL.createObjectURL(workerBlob));
    const callbacks = new Map();
    let nextId = 1;

    window.requestAnimationFrame = function(cb) {
        const id = nextId++;
        callbacks.set(id, cb);
        return id;
    };

    window.cancelAnimationFrame = function(id) {
        callbacks.delete(id);
    };

    worker.onmessage = function() {
        const currentCallbacks = Array.from(callbacks.values());
        callbacks.clear();
        const now = performance.now();
        for (const cb of currentCallbacks) {
            cb(now);
        }
    };

    console.log("%c[CHẾ ĐỘ TREO MÁY TIẾT KIỆM PIN] Đã tắt Render GPU và hạ xuống 5 FPS!", "color: #00e676; font-weight: bold;");
})();