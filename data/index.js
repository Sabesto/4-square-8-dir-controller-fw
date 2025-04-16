/**
 * ----------------------------------------------------------------------------
 * ESP32 Remote Control with WebSocket
 * ----------------------------------------------------------------------------
 * © 2020 Stéphane Calderoni
 * ----------------------------------------------------------------------------
 */

var gateway = `ws://${window.location.hostname}/ws`;
//var gateway = `ws://10.101.30.1/ws`;
var websocket;
var wdir = 0;

// ----------------------------------------------------------------------------
// Initialization
// ----------------------------------------------------------------------------

window.addEventListener('load', onLoad);

function onLoad(event) {
    initWebSocket();
    initButtons();
}

// ----------------------------------------------------------------------------
// WebSocket handling
// ----------------------------------------------------------------------------

function initWebSocket() {
    console.log('Trying to open a WebSocket connection...');
    websocket = new WebSocket(gateway);
    websocket.onopen    = onOpen;
    websocket.onclose   = onClose;
    websocket.onmessage = onMessage;
}

function onOpen(event) {
    console.log('Connection opened');
}

function onClose(event) {
    console.log('Connection closed');
    setTimeout(initWebSocket, 2000);
}

function onMessage(event) {
    let data = JSON.parse(event.data);
    
    wdir = data.wdir;

    for (let index = 0; index <= 7; index++) {
        if (index == data.adir) {
            document.getElementById('dirbtn_'+index).className = "btn btn-success";
            //document.getElementById('led_dir_'+index).className = "on";
        } else if (index == data.wdir && data.adir != data.wdir) {
            document.getElementById('dirbtn_'+index).className = "btn btn-warning";
            //document.getElementById('led_dir_'+index).className = "blink";
        } else {
            document.getElementById('dirbtn_'+index).className = "btn btn-secondary";
            //document.getElementById('led_dir_'+index).className = "off";
        }
    }
    if (data.ptt == "on") {
        document.getElementById('current-ptt').className = "text-danger";        
    } else {
        document.getElementById('current-ptt').className = "text-success";
    }
    document.getElementById('current-ptt').textContent = " " + data.ptt;
    

    swr_pct = Math.round(data.swr / 4096 * 100);
    //swr_pct = Math.floor(Math.random() * 100);
    document.getElementById('swr-progress').ariaValueNow = swr_pct;
    document.getElementById('swr-progress').style.width = swr_pct+"%";
    if (swr_pct <= 40) {
        document.getElementById('swr-progress').className = "progress-bar bg-success";
    }
    if (swr_pct > 40 && swr_pct <= 70){
        document.getElementById('swr-progress').className = "progress-bar bg-info";
    }
    if (swr_pct > 70 && swr_pct <= 90){
        document.getElementById('swr-progress').className = "progress-bar bg-warning";
    }
    if (swr_pct > 90){
        document.getElementById('swr-progress').className = "progress-bar bg-danger";
    }


    current_ma = Math.round(data.relcurr * 0.33484162895);
    curr_ea_relay = Math.round(current_ma / data.nrelset);
    document.getElementById('relay-current').textContent = " " + current_ma + " mA, " + curr_ea_relay + "mA each relay (" + data.nrelset + " relays active)";

    /*
    if (data.adir != data.wdir) {
        document.getElementById('status-text').textContent = "Changing direction. PTT or high SWR blocking";
    }*/
}

// ----------------------------------------------------------------------------
// Button handling
// ----------------------------------------------------------------------------

function initButtons() {
    document.addEventListener("click", onToggle, false);
}

function onToggle(event) {
    event = event || window.event;
    event.target = event.target || event.srcElement;
    if(event.target.id.startsWith("dirbtn_")) {
        direction = event.target.id[7];
        console.log("Want to change direction to: " + direction);
        websocket.send(JSON.stringify({'action':direction}));
    }
    if(event.target.id.startsWith("rot_cw")) {
        if(wdir<7){ direction = wdir + 1; } else { direction = 0; }
        console.log("Rotate CW ");
        websocket.send(JSON.stringify({'action':""+direction}));
    }
    if(event.target.id.startsWith("rot_ccw")) {
        if(wdir>0){ direction = wdir - 1; } else { direction = 7; }
        console.log("Rotate CCW ");
        websocket.send(JSON.stringify({'action':""+direction}));
    }
}
