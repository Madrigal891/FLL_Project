
#include <WiFiClientSecure.h>

#include <WiFi.h>
#include <Wire.h>

#include "esp_wifi.h"
String kill;
String readString;
const char* ssid = "#######";//wifi name
const char* password = "#######";//wifi password
const char* host = "script.google.com";
const int httpsPort = 443;
WiFiClientSecure client;
const char* fingerprint = "46 B2 C3 44 9C 59 09 8B 01 B6 F8 BD 4C FB 00 74 91 2F EF F6";
String GAS_ID = "##########################";//Google Script ID
String maclist[256][3];
int listcount = 0;


String defaultTTL = "60";
const wifi_promiscuous_filter_t filt = { 
  .filter_mask = WIFI_PROMIS_FILTER_MASK_MGMT | WIFI_PROMIS_FILTER_MASK_DATA
};

typedef struct {
  uint8_t mac[6];
} __attribute__((packed)) MacAddr;

typedef struct {
  int16_t fctl;
  int16_t duration;
  MacAddr da;
  MacAddr sa;
  MacAddr bssid;
  int16_t seqctl;
  unsigned char payload[];
} __attribute__((packed)) WifiMgmtHdr;



#define maxCh 13

int curChannel = 1;


void sniffer(void* buf, wifi_promiscuous_pkt_type_t type) {
  wifi_promiscuous_pkt_t *p = (wifi_promiscuous_pkt_t*)buf;
  int len = p->rx_ctrl.sig_len;
  WifiMgmtHdr *wh = (WifiMgmtHdr*)p->payload;
  len -= sizeof(WifiMgmtHdr);
  if (len < 0) {
    Serial.println("Receuved 0");
    return;
  }
  String packet;
  String mac;
  int fctl = ntohs(wh->fctl);
  for (int i = 8; i <= 8 + 6 + 1; i++) {
    packet += String(p->payload[i], HEX);
  }
  for (int i = 4; i <= 15; i++) {
    mac += packet[i];
  }
  mac.toUpperCase();


  int added = 0;
  for (int i = 0; i <= 255; i++) { // checks if the MAC address has been added before
    if (mac == maclist[i][0]) {
      maclist[i][1] = defaultTTL;
      if (maclist[i][2] == "OFFLINE") {
        maclist[i][2] = "0";
      }
      added = 1;
    }
  }

  if (added == 0) { // If its new. add it to the array.
    maclist[listcount][0] = mac;
    maclist[listcount][1] = defaultTTL;
    //Serial.println(mac);
    listcount ++;
    if (listcount >= 256) {//modifiable
      Serial.println("Too many addresses");
      listcount = 0;
    }
  }
}



//===== SETUP =====//
void setup() {

  Serial.begin(115200);


  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&cfg);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_NULL);
  esp_wifi_start();
  esp_wifi_set_promiscuous(true);
  esp_wifi_set_promiscuous_filter(&filt);
  esp_wifi_set_promiscuous_rx_cb(&sniffer);
  esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);

  Serial.println("starting!");
}

void purge() { // This maanges the TTL
  for (int i = 0; i <= 255; i++) {
    if (!(maclist[i][0] == "")) {
      int ttl = (maclist[i][1].toInt());
      ttl --;
      if (ttl <= 0) {
        //Serial.println("OFFLINE: " + maclist[i][0]);
        maclist[i][2] = "OFFLINE";
        maclist[i][1] = defaultTTL;
      } else {
        maclist[i][1] = String(ttl);
      }
    }
  }
}

void updatetime() { // This updates the time the device has been online for
  for (int i = 0; i <= 255; i++) {
    if (!(maclist[i][0] == "")) {
      if (maclist[i][2] == "")maclist[i][2] = "0";
      if (!(maclist[i][2] == "OFFLINE")) {
        int timehere = (maclist[i][2].toInt());
        timehere ++;
        maclist[i][2] = String(timehere);
      }


    }
  }
}

String people() { // This checks if the MAC is in the  list and then displays it on the OLED and/or prints it to serial.
    String tbp = "";
  for (int i = 0; i <= listcount - 2; i++) {
    tbp += maclist[i][0] + ",";
  }
  tbp += maclist[listcount - 1][0];
return tbp;
}




void loop() {
  kill=people();
  //SerialString .println("Changed channel:" + String(curChannel));
  if (curChannel > maxCh) {
    curChannel = 1;
  }
  esp_wifi_set_channel(curChannel, WIFI_SECOND_CHAN_NONE);
  delay(100);;
  purge();
Serial.println(people());
curChannel++;
  if (millis()>300000||Serial.available()){  sendData(listcount,kill);
  ESP.restart();}
}
void sendData(int x, String y)
{
 WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(500);
    Serial.print(".");
  }       
 Serial.print("connecting to ");
 Serial.println(host);
 if (!client.connect(host, httpsPort)) {
   Serial.println("connection failed");
   return;
 }
 if (client.verify(fingerprint, host)) {
 Serial.println("certificate matches");
 } else {
 Serial.println("certificate doesn't match");
 }
 String string_x     =  String(x, DEC);

 String url = "/macros/s/" + GAS_ID + "/exec?Value1=" + string_x + "&Value2=" +string_x ;
 Serial.print("requesting URL: ");
 Serial.println(url);
 client.print(String("GET ") + url + " HTTP/1.1\r\n" +
        "Host: " + host + "\r\n" +
        "User-Agent: BuildFailureDetectorESP8266\r\n" +
        "Connection: close\r\n\r\n");
 Serial.println("request sent");
 while (client.connected()) {
 String line = client.readStringUntil('\n');
 if (line == "\r") {
   Serial.println("headers received");
   break;
 }
 }
 String line = client.readStringUntil('\n');
 if (line.startsWith("{\"state\":\"success\"")) {
 Serial.println("esp8266/Arduino CI successfull!");
 } else {
 Serial.println("esp8266/Arduino CI has failed");
 }
 Serial.println("reply was:");
 Serial.println("==========");
 Serial.println(line);
 Serial.println("==========");
 Serial.println("closing connection");
}
