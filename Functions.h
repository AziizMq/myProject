
void SendMessage(char* msg) {
  for (int d = 0; d < numberOfConcurrentUser; d++) {
    if (ConnectedDevices[d] == 1) {
      
   /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      !! uncomment next line after recoding serial monitor !! 
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

      //Serial.printf("Sending to [%u]: %s\n", ConnectedDevices[d], msg);
      webSocket.sendTXT(d, msg);

    }
  }
}


void Decode_Message(uint8_t client_num, uint8_t* payload) {
  String msg = (const char*)payload;
  for (int i = 0; i < 5; i++) {
    if ( msg.equalsIgnoreCase(devices[i].Name)) {
      devices[i].status = devices[i].status ? 0 : 1;
      /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      !! uncomment next line after recoding serial monitor !! 
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
      //Serial.printf("Toggling %s to %u\n", devices[i].Name, devices[i].status);
      digitalWrite(devices[i].Pin, devices[i].status);
      // Report the state of the devices
    } else if ( msg.equalsIgnoreCase(devices[i].statusStr)) {
      sprintf(msg_buf, "%s%d", devices[i].Name, devices[i].status);
      SendMessage(msg_buf);
    } else if (msg.equalsIgnoreCase("GREEN") ){
      //digitalWrite(devices[2].Pin, 1);
      //digitalWrite(devices[3].Pin, 1);
    } else if (msg.equalsIgnoreCase("RED")){
      //digitalWrite(devices[2].Pin, 0);
      //digitalWrite(devices[3].Pin, 0);
    }
  }

}


// Callback: receiving any WebSocket message
void onWebSocketEvent(uint8_t client_num,
                      WStype_t type,
                      uint8_t * payload,
                      size_t length) {

  // Figure out the type of WebSocket event
  switch (type) {

    // Client has disconnected
    case WStype_DISCONNECTED:
      Serial.printf("[%u] Disconnected!\n", client_num);
      ConnectedDevices[client_num] = 0;
      break;

    // New client has connected
    case WStype_CONNECTED:
      {
        IPAddress ip = webSocket.remoteIP(client_num);
        Serial.printf("[%u] Connection from ", client_num);
        Serial.println(ip.toString());
        ConnectedDevices[client_num] = 1;
        sprintf(msg_buf, "VAL;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f",
                waterflowrate, watertotalliters, airflowrate,
                airtotalvelocity, mixedflowrate, mixedtotalliters, levelPercentage);
        webSocket.sendTXT(client_num, msg_buf);
        for (int i = 0; i < 4; i++) {
          sprintf(msg_buf, "%s%d", devices[i].Name, devices[i].status);
          webSocket.sendTXT(client_num, msg_buf);
        }
      }
      break;

    // Handle text messages from client
    case WStype_TEXT:

      // Print out raw message
       /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
      !! uncomment next line after recoding serial monitor !! 
      !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
      //Serial.printf("[%u] Received text: %s\n", client_num, payload);

      Decode_Message(client_num, payload);
      break;

    // For everything else: do nothing
    case WStype_BIN:
    case WStype_ERROR:
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    default:
      break;
  }
}

// Callback: send homepage
void onIndexRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/index.html", "text/html");
}

// Callback: send style sheet
void onCSSRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/style.css", "text/css");
}

// Callback: send style sheet
void onSCRIPTRequest(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(SPIFFS, "/script.js", "text/js");
}

// Callback: send 404 if requested file does not exist
void onPageNotFound(AsyncWebServerRequest *request) {
  IPAddress remote_ip = request->client()->remoteIP();
  Serial.println("[" + remote_ip.toString() +
                 "] HTTP GET request of " + request->url());
  request->send(404, "text/plain", "Not found");
}
