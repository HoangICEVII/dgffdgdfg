
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