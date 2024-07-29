/*
  This project after uploading to UNIT CAM S3 (with connected G0 and GND for upload mode)
  creates a wifi hotspot after connecting to whom you can access http://192.168.4.1/capture.jpg
  to get one current photo in the browser (get request and the image is as response)

  The esp32cam does not have configuration for M5_CamS3 (so I created one myslef from schematics and reference configuration)
  constexpr Pins UNIT_CAM_S3{
    D0: 6,
    D1: 15,
    D2: 16,
    D3: 7,
    D4: 5,
    D5: 10,
    D6: 4,
    D7: 13,
    XCLK: 11,
    PCLK: 12,
    VSYNC: 42,
    HREF: 18,
    SDA: 17,
    SCL: 41,
    RESET: 21,
    PWDN: -1,
  };
*/

//setup_board_select(on project creation): AI Thinker ESP32-CAM
/*
  //platformio setup from M5 recovery project
  [env:esp32cam]
  platform = espressif32@6.4.0
  board = esp32s3box
  framework = arduino
  monitor_speed = 115200
*/

//https://youtu.be/Sb08leLWOgA
//https://github.com/yoursunny/esp32cam
//http://192.168.4.1/capture.jpg

//esp32-ST1-IMG.local/image

//#include <Arduino.h>
#include <esp32cam.h>
#include <WebServer.h>
#include <WiFi.h>

#define AP_SSID "PhotoBooth"
#define AP_PASS "wifi_booth"

WebServer server(80);

void handleCapture(){
  auto img = esp32cam::capture();
  if(img==nullptr){
    server.send(500,"","");
    return;
  }
  server.setContentLength(img->size());
  server.send(200,"image/jpeg");
  WiFiClient client = server.client();
  img->writeTo(client);
}

void setup() {
  auto res = esp32cam::Resolution::find(1600,1200);//640,480
  esp32cam::Config cfg;
  cfg.setPins(esp32cam::pins::UNIT_CAM_S3);
  cfg.setResolution(res);
  cfg.setJpeg(80);
  esp32cam::Camera.begin(cfg);
  WiFi.softAP(AP_SSID,AP_PASS);
  server.on("/capture.jpg",handleCapture);
  server.begin();
}

void loop() {
  server.handleClient();
}
