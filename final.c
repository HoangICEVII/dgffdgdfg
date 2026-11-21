window.ngungTreo = true;

(function() {
    const OrigWS = window.WebSocket;
    window.WebSocket = function(url, protocols) {
        const ws = new OrigWS(url, protocols);
        window.gameWS = ws;

        const origSend = ws.send;
        window.gameWS.send = function(data) {
            const bytes = new Uint8Array(data);

            if(!window.ngungTreo){
                if (bytes[4] === 129 && bytes[5] === 4) {
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

async function treo_may() {
    window.ngungTreo = false;
    while (1) {
        await sleep(800);
        const pass1 = new Uint8Array([20, 0, 0, 0, 167, 39, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 249, 167, 0, 0, 0, 0, 0, 0]);
        window.gameWS.send(pass1);
        if(window.ngungTreo){
            break;   
        }
    }
}