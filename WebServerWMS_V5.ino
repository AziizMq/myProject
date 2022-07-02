#include <WiFi.h>
#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>
#include <WebSocketsServer.h>


TaskHandle_t MeteringLoop;
volatile float waterflowrate, airflowrate, mixedflowrate, watertotalliters, water_perc;
volatile float airtotalvelocity, mixedtotalliters, watertotallitersold;
volatile float airtotalvelocityold, mixedtotallitersold, waterLevel = 0.0;
volatile float levelPercentage = 0.0;
volatile byte waterpulsecount = 0, airpulsecount = 0, mixedpulsecount = 0;
volatile unsigned long watertotalMM, airtotalms, mixedtotalMM, wateroldtime;
volatile unsigned long  airoldtime, mixedoldtime;
unsigned int waterflowMM, airflowms, mixedflowMM;
unsigned long lastUpdateTime = 0;

const char *ssid = "CEN493 V5";
const char *password =  "123456789";
const int water_sensor = 2 ;
const int air_sensor = 21;
const int mixed_sensor = 15;
const int Level_Sensor = A0;
const int dns_port = 53;
const int http_port = 80;
const int ws_port = 1337;
const int numberOfConcurrentUser = 5;
const float calfac = 4.5;


#define state_init 1
#define state_closed_period 2
#define state_wait_sensor_stable  3
int co_secs , num , delay_pump_co = 0;
int state = state_init;
int las_water_pump;

int ConnectedDevices[numberOfConcurrentUser];
char msg_buf[170], color;
int led_state = 0, ARCount = 0, ARArray[100];

AsyncWebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(1337);

struct device {
  String Name;
  const int Pin;
  int status;
  String statusStr;
};

//devices
device pump = {"Pump", 19, 0, "pumpState"};
device Fan = {"Fan", 18, 0, "FanState"};
device Air_Valve = {"Air_Valve", 5, 0, "AirValveState"};
device Water_Valve = {"W_Valve", 17, 0, "WValveState"};
device LPF = {"LPF", 16, 0, "LPFState"};

device devices[6] = {pump, Fan, Air_Valve, Water_Valve, LPF};

void IRAM_ATTR waterpulsecounter() {
  waterpulsecount++;
}

void IRAM_ATTR airpulsecounter() {
  airpulsecount++;
}

void IRAM_ATTR mixedpulsecounter() {
  mixedpulsecount++;
}

#include "Functions.h"

void CalculateFlowRate() {
  noInterrupts();



  //                  # CHECK WATER FLOW #
  //Serial.println(waterpulsecount);
  //Serial.println((millis() - wateroldtime));
  //waterflowrate = (((waterpulsecount*1000) / (millis() - wateroldtime) )) / calfac;

  // waterflowrate = (((waterpulsecount * 1000.0) / (millis() - wateroldtime) ));
  waterflowrate = waterpulsecount;
  wateroldtime = millis();
  waterflowMM = (waterflowrate / 60) * 1000;
  watertotalMM += waterflowMM;
  watertotalliters = watertotalMM * 0.001;
  watertotalliters = watertotallitersold - (watertotalMM * 0.001);

  //                  # CHECK AIR FLOW #
  //airflowrate = ((1000 / (millis() - airoldtime) * airpulsecount));
  airflowrate = airpulsecount;
  airoldtime = millis();
  airflowms = (airflowrate / 60) * 1000;
  airtotalms = airflowms;
  airtotalvelocity  = (airtotalvelocityold + airtotalms) / 2;

  //                  # CHECK ACTUAL WATER PERCANTAGE #
  //mixedflowrate = ((1000.0 / (millis() - mixedoldtime) * mixedpulsecount));
  mixedflowrate = mixedpulsecount;
  mixedoldtime = millis();
  mixedflowMM = (mixedflowrate / 60) * 1000;
  mixedtotalMM = mixedtotalMM + mixedflowMM;
  mixedtotalliters = mixedtotalMM * 0.001;
  mixedtotalliters = mixedtotallitersold - (mixedtotalMM * 0.001);

  //                  # CHECK SENSOR WATER PERCANTAGE #
  int AR = analogRead(Level_Sensor);
  //Base on data analysis -> max of s is 2351
  levelPercentage = AR / 23.51;
  // check if more than %100 set it 100
  if (AR > 2351) {
    levelPercentage = 100 ;
  }

  //                  # OUTPOT VALIDATION #
  if ( (waterflowrate + airflowrate) > 0 ) {
    water_perc = 100 * waterflowrate / (waterflowrate + airflowrate);
  } else {
    water_perc = 0;
  }


  //                  # lowPassFilter #
  if (devices[4].status != 0) {
    lowPassFilter();
    Serial.println("LPF code has not been weitten yet !");
  }

  //                  #   state   #

 
  
  switch (state) {
    case state_init:
      digitalWrite(devices[2].Pin, 1);
      digitalWrite(devices[3].Pin, 1);
      if (mixedflowrate <= 0) color = 'W';
      else {state = state_wait_sensor_stable;co_secs=0;color = 'A';}  
      break;

    case state_wait_sensor_stable:
       if (co_secs<200) co_secs++;
       if (mixedflowrate <= 0) state=state_init;
       else if (co_secs>4){
         if ((AR > 1200))color = 'G';
         else {
            state = state_closed_period;
            digitalWrite(devices[2].Pin, 0);
            digitalWrite(devices[3].Pin, 0);
            // red
            color = 'R';
            co_secs = 0;
          }
       }        
        break;  
        
    case state_closed_period:
      co_secs++;
      if (co_secs > 15) state = state_init;
      
      break;

  }
  if (state == state_closed_period){
    num = 15 - co_secs;
  } else if (state == state_wait_sensor_stable) {
    num = 4 - co_secs;
    }

  sprintf(msg_buf, "VAL;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%u;%c;%u",
          waterflowrate, watertotalliters, airflowrate,
          airtotalvelocity, mixedflowrate , mixedtotalliters, water_perc, levelPercentage , AR , color , num);
  Serial.println(msg_buf);
  SendMessage(msg_buf);


  /*
    // open w/a val

    delay(8000);
    if (mixedflowrate > 1) {
      if (AR < 700) {
        // close w/a val
        digitalWrite(devices[2].Pin, 0);
        digitalWrite(devices[3].Pin, 0);
        // red
        color = 'R';
        // send c data
        sprintf(msg_buf, "VAL;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%u;%c",
            waterflowrate, watertotalliters, airflowrate,
            airtotalvelocity, mixedflowrate , mixedtotalliters, water_perc, levelPercentage , AR , color );
        Serial.println(msg_buf);
        SendMessage(msg_buf);
        // delay 15s
        delay(15000);
       } else color = 'G'; // green
     } else color = 'G'; //green

      sprintf(msg_buf, "VAL;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%u;%c",
            waterflowrate, watertotalliters, airflowrate,
            airtotalvelocity, mixedflowrate , mixedtotalliters, water_perc, levelPercentage , AR , color );
      Serial.println(msg_buf);

    if ((millis() - lastUpdateTime) >= 500) {
      SendMessage(msg_buf);
      lastUpdateTime = millis();
    }
  */
  mixedpulsecount = 0;
  airpulsecount = 0;
  waterpulsecount = 0;
  interrupts();
}

void lowPassFilter() {
  /*
     !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

     sprintf(msg_buf, "VAL;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%.2f;%u",
          waterflowrate, watertotalliters, airflowrate,
          airtotalvelocity, mixedflowrate , mixedtotalliters, water_perc, levelPercentage , AR );
  */
}

void AddOnArray() {
  int AR = analogRead(Level_Sensor);
  //levelPercentage = AR/23.51;
  ARArray[ARCount] = AR;
}

void setup() {
  // put your setup code here, to run once:
  pinMode(pump.Pin, OUTPUT);
  pinMode(Fan.Pin, OUTPUT);
  pinMode(Air_Valve.Pin, OUTPUT);
  pinMode(Water_Valve.Pin, OUTPUT);
  pinMode(LPF.Pin, OUTPUT);

  Serial.begin(115200);
  co_secs = 0;


  pinMode(water_sensor, INPUT_PULLUP);
  pinMode(air_sensor, INPUT_PULLUP);
  pinMode(mixed_sensor, INPUT_PULLUP);
  pinMode(water_sensor, INPUT_PULLUP);
  pinMode(Level_Sensor, INPUT);

  //  pinMode(PIN_FLOAT, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(water_sensor), waterpulsecounter, FALLING);
  attachInterrupt(digitalPinToInterrupt(air_sensor), airpulsecounter, FALLING);
  attachInterrupt(digitalPinToInterrupt(mixed_sensor), mixedpulsecounter, FALLING);

  // Make sure we can read the file system
  if ( !SPIFFS.begin()) {
    Serial.println("Error mounting SPIFFS");
    while (1);
  }

  // Start access point
  WiFi.softAP(ssid, password);

  // Print our IP address
  Serial.println();
  Serial.println("AP running");
  Serial.print("My IP address: ");
  Serial.println(WiFi.softAPIP());

  // On HTTP request for root, provide index.html file
  server.on("/", HTTP_GET, onIndexRequest);

  // On HTTP request for style sheet, provide style.css
  server.on("/style.css", HTTP_GET, onCSSRequest);

  // On HTTP request for style sheet, provide style.css
  server.on("/script.js", HTTP_GET, onSCRIPTRequest);

  // Handle requests for pages that do not exist
  server.onNotFound(onPageNotFound);

  // Start web server
  server.begin();

  // Start WebSocket server and assign callback
  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  //create a task that will be executed in the Task2code() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(
    Metering_Loop,   /* Task function. */
    "Metering",     /* name of task. */
    10000,       /* Stack size of task */
    NULL,        /* parameter of the task */
    1,           /* priority of the task */
    &MeteringLoop,      /* Task handle to keep track of created task */
    1);          /* pin task to core 1 */
  delay(500);

  las_water_pump = devices[1].status;
}


void Metering_Loop(void* pvParameters ) {
  for (;;) {
    if (ARCount >= 100) {
      Serial.println(" ");
      CalculateFlowRate();
      ARCount = 0;
    } else {
      AddOnArray();
      ARCount = ARCount + 1;
      Serial.printf("%u, ", ARArray[ARCount - 1]);
    }
    delay(10);
  }
}

void loop() {
  webSocket.loop();
}
