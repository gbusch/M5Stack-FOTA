#include <M5Stack.h>
#include <M5ez.h>
#include <HTTPClient.h>
#include <Arduino_JSON.h>

#define VERSION 1

void setup() {

  ez.begin();
  Serial.begin(115200);
  
  ezMenu OTA_menu ("OTA via https demo");
  OTA_menu.addItem("Settings", ez.wifi.menu);
  OTA_menu.addItem("Start", loop);
  OTA_menu.run();
}

void loop() {
  while (true) {
    M5.Lcd.fillScreen(BLACK);
    M5.Lcd.setCursor(10, 10);
    M5.Lcd.setTextColor(WHITE);
    M5.Lcd.setTextSize(1);
    M5.Lcd.printf("Display Test!");
    M5.Lcd.fillTriangle(random(M5.Lcd.width()-1), random(M5.Lcd.height()-1), random(M5.Lcd.width()-1), random(M5.Lcd.height()-1), random(M5.Lcd.width()-1), random(M5.Lcd.height()-1), random(0xfffe));
    M5.update();


    HTTPClient http;
    http.begin("https://raw.githubusercontent.com/gbusch/M5Stack-FOTA-artifacts/master/version.json");
    int httpResponseCode = http.GET();

    String payload = "{}"; 

    if (httpResponseCode == 200) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      
      payload = http.getString();
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();

    M5.Lcd.println(payload);

    JSONVar myObject = JSON.parse(payload);
  
    // JSON.typeof(jsonVar) can be used to get the type of the var
    if (JSON.typeof(myObject) == "undefined") {
      Serial.println("Parsing input failed!");
      return;
    }

    int online_version = (int)myObject["version"];

    M5.Lcd.println("This is version:");
    M5.Lcd.println(VERSION);

    if (online_version > VERSION) {
      M5.Lcd.println("update required");
      ezProgressBar pb("OTA update in progress", "Downloading ...", "Abort");
      #include "raw_githubusercontent_com.h"
      if (ez.wifi.update("https://raw.githubusercontent.com/gbusch/M5Stack-FOTA-artifacts/master/firmware.bin", root_cert, &pb)) {
          ez.msgBox("Over The Air updater", "OTA download successful. Reboot to new firmware", "Reboot");
          ESP.restart();
      } else {
          ez.msgBox("OTA error", ez.wifi.updateError(), "OK");
      }
    } else {
      M5.Lcd.println("software up-to-date");
    }

    delay(5000);
  }
}
