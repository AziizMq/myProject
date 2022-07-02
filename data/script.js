var url = "ws://192.168.4.1:1337/";


var button1;
var button2;
var button3;
var button4;
//var button5;

var Water_Level=0.0; 
var Water_Flow=0.0;
var Air_Flow=0.0;
var Mixed_Flow=0.0;

var old_s=0;
// This is called when the page finishes loading
function init() {

    // Assign page elements to variables
    button1 = document.getElementById("Pump");
    button2 = document.getElementById("Fan");
    button3 = document.getElementById("Air_Valve");
    button4 = document.getElementById("Water_Valve");
    //button5 = document.getElementById("LPF");

    Water_Flow = document.getElementById("water-flow");
    Air_Flow = document.getElementById("air-flow");
    Mixed_Flow = document.getElementById("mixed-flow");
    Main_Flow = document.getElementById("main-flow");
    Water_Level = document.getElementById("myBar");

    // Connect to WebSocket server
    wsConnect(url);
}

// Call this to connect to the WebSocket server
function wsConnect(url) {
    
    // Connect to WebSocket server
    websocket = new WebSocket(url);
    
    // Assign callbacks
    websocket.onopen = function(evt) { onOpen(evt) };
    websocket.onclose = function(evt) { onClose(evt) };
    websocket.onmessage = function(evt) { onMessage(evt) };
    websocket.onerror = function(evt) { onError(evt) };

}

// Called when a WebSocket connection is established with the server
function onOpen(evt) {

    // Log connection state
    console.log("Connected");
    
    // Enable button
    button1.disabled = false;
    button2.disabled = false;
    button3.disabled = false;
    button4.disabled = false;
    //button5.disabled = false;

    // Get the current state of the LED
    //doSend("pumpState");
    
   // doSend("FanState");
   // doSend("AirValveState");
    //doSend("WaterValveState");
  
}

// Called when the WebSocket connection is closed
function onClose(evt) {

    // Log disconnection state
    console.log("Disconnected");
    
    // Disable button
    //button.disabled = true;
    
    // Try to reconnect after a few seconds
    setTimeout(function() { wsConnect(url) }, 2000);
}

// Called when a message is received from the server
function onMessage(evt) {

    // Print out our received message
    console.log("Received: " + evt.data);
    
    // Update circle graphic with LED state
    switch(evt.data) {
        case "Pump1":
            console.log("Pump is off");
            document.getElementById("Pump").innerText = "ON";
            document.getElementById("Pump").className = "button_N";
            break;
        case "Pump0":
            console.log("Pump is on");
            document.getElementById("Pump").innerText = "OFF";
            document.getElementById("Pump").className = "button_F";
            break;
        case "Fan1":
            console.log("Fan is off");
            document.getElementById("Fan").innerText = "ON";
            document.getElementById("Fan").className = "button_N";
            break;
        case "Fan0":
            console.log("Fan is on");
            document.getElementById("Fan").innerText = "OFF";
            document.getElementById("Fan").className = "button_F";
            break;
        case "Air_Valve1":
            console.log("Air_Valve is off");
            document.getElementById("Air_Valve").innerText = "ON";
            document.getElementById("Air_Valve").className = "button_N";
            break;
        case "Air_Valve0":
            console.log("Air_Valve is on");
            document.getElementById("Air_Valve").innerText = "OFF";
            document.getElementById("Air_Valve").className = "button_F";
            break;
        case "W_Valve1":
            console.log("Water_Valve is off");
            document.getElementById("Water_Valve").innerText = "ON";
            document.getElementById("Water_Valve").className = "button_N";
            break;
        case "W_Valve0":
            console.log("Water_Valve is on");
            document.getElementById("Water_Valve").innerText = "OFF";
            document.getElementById("Water_Valve").className = "button_F";
            break;
        case "LPF1":
            console.log("LPF is off");
            document.getElementById("LPF").innerText = "ON";
            document.getElementById("LPF").className = "button_N";
            break;
        case "LPF0":
            console.log("LPF is on");
            document.getElementById("LPF").innerText = "OFF";
            document.getElementById("LPF").className = "button_F";
            break;         
        default:
            break;
    }
    let msg = evt.data;

    if (msg.includes("VAL;")) {
        var values = msg.split(";");
        document.getElementById("water-flow").textContent = values[1]+" c/s";
        document.getElementById("water-total").textContent = "";
        document.getElementById("air-flow").textContent = values[3]+" c/s";
        document.getElementById("air-velocity").textContent = "";
        document.getElementById("mixed-flow").textContent = values[5]+" c/s";
        document.getElementById("mixed-total").textContent = "";
        document.getElementById("main-flow").textContent = values[7]+" %";
        document.getElementById("myBar").style.width = values[8]+ '%';
        document.getElementById("SWP").innerHTML = values[9];

        var num = values[9];
        if (0 <= num && num < 600){
            document.getElementById("myBar").className = "z3-red w3-container w3-center";} 
            else if (600 <= num && num < 1100){
            document.getElementById("myBar").className = "z3-green w3-container w3-center";} 
            else {
            document.getElementById("myBar").className = "z3-blue w3-container w3-center";}
        
        
        // values[10].somthing
        if (values[10] == 'G' || values[10] == 'W' || values[10] == 'A') {
                doSend("GREEN");
                if (values[10] == 'G'){ 
                //console.log("GREEN is on");
                document.getElementById("hdr").className ="topnavG";
                document.getElementById("Stt").innerText = "Water flow - Valves is open";
                } else if (values[10] == 'W'){
                //console.log("Whate is on");
                document.getElementById("hdr").className ="topnavW";
                document.getElementById("Stt").innerText = "No flow - Valves is open";
                } else {
                //console.log("Whate is on");
                document.getElementById("hdr").className ="topnavW";
                document.getElementById("Stt").innerText = "Flow - Wating for [" + values[11] +"]";    
                }
                //console.log("Water_Valve is off");
                document.getElementById("Water_Valve").innerText = "ON";
                document.getElementById("Water_Valve").className = "button_N";
                //console.log("Air_Valve is off");
                document.getElementById("Air_Valve").innerText = "ON";
                document.getElementById("Air_Valve").className = "button_N";
            } else if (values[10] == 'R'){
                doSend("RED");
                //console.log("RED is on");
                document.getElementById("hdr").className ="topnavR";
                document.getElementById("Stt").innerText = "Air flow detected! - Wating for [" + values[11] +"]"; 
                //console.log("Water_Valve is on");
                document.getElementById("Water_Valve").innerText = "OFF";
                document.getElementById("Water_Valve").className = "button_F";
                //console.log("Air_Valve is on");
                document.getElementById("Air_Valve").innerText = "OFF";
                document.getElementById("Air_Valve").className = "button_F";
            }
    }        
}

// Called when a WebSocket error occurs
function onError(evt) {
    console.log("ERROR: " + evt.data);
}

// Sends a message to the server (and prints it to the console)
function doSend(message) {
    console.log("Sending: " + message);
    websocket.send(message);
}

// Called whenever the HTML button is pressed
function Pump() {
    doSend("Pump");
    doSend("pumpState");
}
function Fan() {
    doSend("Fan");
    doSend("FanState");
    
}
function Air_Valve() {
    doSend("Air_Valve");
    doSend("AirValveState");
}
function Water_Valve() {
    doSend("W_Valve");
    doSend("WValveState");
}
/*
function StartTiming(){
  const today = new Date();
  let s = today.getSeconds();
  if((s-old_s)>0 || (s-old_s)<0){
      doSend("update");
  }
}
*/
function LPF() {
    doSend("LPF");
    doSend("LPFState");
}
// Call the init function as soon as the page loads
window.addEventListener("load", init, false);