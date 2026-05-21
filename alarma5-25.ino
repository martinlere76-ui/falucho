

/*
#define BOT_TOKEN "6120071508:AAE17Ew3wSBsBZbiPjLH1F_wy1n18JtFGYY"  // grupo alarma
#define CHAT_ID "-644866245"                                        // bot falucho


*/

#define BOT_TOKEN "8820143180:AAGxCk8kju1JF2L7z8UJBIaKxZpbKrgsZjo"  //prueba
#define CHAT_ID "1109752008"
const char* FW_BIN_URL = "https://raw.githubusercontent.com/martinlere76-ui/falucho/main/alarma5-25.ino.bin";




// bot quique
#include <WiFi.h>
#include <ESP32Ping.h>
#include <FastBot2.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>
#include <HTTPUpdate.h>
#include <HTTPClient.h>
#include <cstring>



const char *mqtt_broker = "qe889211.ala.us-east-1.emqxsl.com";
const char *mqtt_topic = "emqx/esp32";
const char *mqtt_username = "martinlere3";  // MQTT username for uthentication
const char *mqtt_password = "melmap2024";   // MQTT
const int mqtt_port = 8883;

// WiFi and MQTT client initialization
WiFiClientSecure esp_client;
PubSubClient mqtt_client(esp_client);

// Root CA Certificate
// Load DigiCert Global Root G2, which is used by EMQX Public Broker: broker.emqx.io
const char *ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh
MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3
d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD
QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT
MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j
b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG
9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB
CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97
nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt
43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P
T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4
gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO
BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR
TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw
DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr
hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg
06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF
PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls
YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk
CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=
-----END CERTIFICATE-----
)EOF";


FastBot2 bot(BOT_TOKEN);
bool botReady = false;
uint32_t fixedMenuMessageId = 0;
String fixedMenuChatId = "";
String currentChatId = CHAT_ID;

void connectToMQTT();
String normalizeTelegramCmd(String in);
void newMsg(fb::Update& u);
void sendBotMessage(const String& text, const String& chatId = CHAT_ID);
void sendMainMenu(const String& chatId = CHAT_ID);
void sendAlarmMenu(const String& chatId = CHAT_ID);
void sendCurtainMenu(const String& chatId = CHAT_ID);
void showOrUpdateMenu(const String& title, fb::InlineMenu& menu, const String& chatId);
void pinFixedMenu(const String& chatId);
bool canUseBot();
void runUpdateFromRepo(const String& chatId);
bool getJsonNumberByKey(const String& json, const char* key, float& outValue);
bool getJsonNumberByKey(const char* json, const char* key, float& outValue);
String getSazmWeatherText();

void mqttCallback(char *topic, byte *payload, unsigned int length);


const int cortinaDownPin = 27;
const int cortinaUpPin = 26;
const int lucesPin = 14;  // D8;
const int sirenaPin = 13;
const int pirPin = 33;      // 2do
const int puertaPin = 25;   // 1ro
const int cortinaPin = 32;  // 3ro
const int magnePin = 35;    // 4to
const int tiempoSubir = 35000;
const int tiempoSubir25 = 10000;
const int tiempoSubir75 = 25000;
unsigned int time15;
unsigned int auxTime;     // cuenta tiempo subiendo cortina
unsigned int auxTimeAla;  // tiempo sonando después de último mov
unsigned int auxTime30;   // 30 segundos para enviar msj de alarma sonando
unsigned int auxTime1;    // 1 segundo parachequear msj
unsigned int auxTimeComms;
unsigned int auxTimeAlarm;
unsigned long previousMillis = 0;
unsigned long interval = 30000;
bool cortinaUpState = 1;
bool cortinaDownState = 1;
bool sonando = 0;
bool alarma = 0;
bool banderaCortina = 0;
bool banderaPuerta = 0;
bool banderaPir = 0;
bool banderaMagne = 0;
bool Subiendo = 0;
bool Subiendo25 = 0;
bool Subiendo75 = 0;
bool Bajando = 0;
bool parcialPir;
bool parcialPuerta;
bool parcialCortina;
bool newmsj = 0;
bool bajarEn15;
bool luz;
bool parcialMagne;
bool conectado = 0;
int bat = 0;
const uint32_t connectTimeoutMs = 10000;
String message = "";
String teleMsj;
bool inicio;
byte conectadoA = 2;
byte intentos = 0;
byte cada10 = 0;
float timePIR2;
byte poneralarmaalbajar = 0;
bool banderaLuz = 0;
float volt;
long millisLuz;
String msj2;
float voltProm;
float voltMin;
bool banderades;
int aux33 = 0;
int aux34 = 0;

TaskHandle_t commsTaskHandle = nullptr;
TaskHandle_t alarmTaskHandle = nullptr;
QueueHandle_t qCmd = nullptr;
QueueHandle_t qNotify = nullptr;

struct CmdMsg {
  char cmd[32];
  char chatId[24];
};

struct NotifyMsg {
  char text[192];
  char chatId[24];
};

void taskComms(void* pvParameters);
void taskAlarm(void* pvParameters);
void loopAlarmStep();
void loopCommsStep();
void enqueueCmd(const String& cmd, const String& chatId);
void enqueueNotify(const String& text, const String& chatId = CHAT_ID);
void handleCommand(const String& cmd, const String& chatId);



void setup() {

  //WiFi.begin(WIFI_SSID, WIFI_PASS);
  Serial.begin(115200);
  delay(500);


 //#define ssid1 "Falucho"
//#define password1 "lere1982"
  #define ssid1 "secta24GHz"
  #define password1 "nomelaacuerdo26"

#define ssid2 "MecanicaCrouzat"
#define password2 "chilli1342"

  conectar();


  bot.setPollMode(fb::Poll::Sync, 1000);

  esp_client.setCACert(ca_cert);
  Serial.println("hola hoola hooola");
  mqtt_client.setServer(mqtt_broker, mqtt_port);
  mqtt_client.setKeepAlive(60);
  mqtt_client.setCallback(mqttCallback);
  connectToMQTT();


  pinMode(cortinaUpPin, OUTPUT);
  pinMode(cortinaDownPin, OUTPUT);
  cortinaDownState = 0;
  cortinaUpState = 0;
  digitalWrite(cortinaUpPin, cortinaUpState);
  digitalWrite(cortinaDownPin, cortinaDownState);

  pinMode(magnePin, INPUT);
  pinMode(pirPin, INPUT);
  pinMode(puertaPin, INPUT);
  pinMode(sirenaPin, OUTPUT);
  digitalWrite(sirenaPin, 0);
  pinMode(cortinaPin, INPUT);
  pinMode(lucesPin, OUTPUT);
  digitalWrite(lucesPin, 0);
  pinMode(34, INPUT);

  inicio = 1;
  bot.attachUpdate(newMsg);
  // Habilitar Telegram solo con WiFi estable para evitar asserts de lwIP.
  if (WiFi.status() == WL_CONNECTED) {
    delay(2000);
    botReady = true;
    sendMainMenu();
    sendBotMessage("Hola!!x2");
  }
  volt = analogRead(34);

  volt = map(volt, 754, 1743, 662, 1350);
  voltProm = volt / 100.00;
  voltMin = voltProm;

  qCmd = xQueueCreate(16, sizeof(CmdMsg));
  qNotify = xQueueCreate(24, sizeof(NotifyMsg));

  xTaskCreatePinnedToCore(taskComms, "taskComms", 16384, nullptr, 1, &commsTaskHandle, 0);
  xTaskCreatePinnedToCore(taskAlarm, "taskAlarm", 12288, nullptr, 1, &alarmTaskHandle, 1);
}


void newMsg(fb::Update& u) {
  if (u.isQuery()) {
    bot.answerCallbackQuery(u.query().id(), "");
    u.query().data().toString(teleMsj);
    u.query().message().chat().id().toString(currentChatId);
  } else if (u.isMessage()) {
    u.message().text().toString(teleMsj);
    u.message().chat().id().toString(currentChatId);
  } else {
    teleMsj = "";
  }
  newmsj = 1;
}

void sendBotMessage(const String& text, const String& chatId) {
  if (!canUseBot()) return;
  bot.sendMessage(fb::Message(text, chatId));
}

void sendMainMenu(const String& chatId) {
  if (!canUseBot()) return;
  fb::Keyboard menu;
  menu.persistent = true;
  menu.resize = true;
  menu.placeholder = "Menu principal";
  menu.addButton("alarma");
  menu.addButton("cortina");
  menu.newRow();
  menu.addButton("clima");
  fb::Message msg("Menu principal", chatId);
  msg.setKeyboard(&menu);
  bot.sendMessage(msg);
}

void sendAlarmMenu(const String& chatId) {
  if (!canUseBot()) return;
  fb::Keyboard menu;
  menu.persistent = true;
  menu.resize = true;
  menu.placeholder = "Elegi una accion";
  menu.addButton("alarma_si");
  menu.addButton("alarma_no");
  menu.newRow();
  menu.addButton("sonar_alarma");
  menu.addButton("luz");
  menu.addButton("s_movimiento");
  menu.newRow();
  menu.addButton("menu");
  fb::Message msg("Menu alarma", chatId);
  msg.setKeyboard(&menu);
  bot.sendMessage(msg);
}

void sendCurtainMenu(const String& chatId) {
  if (!canUseBot()) return;
  fb::Keyboard menu;
  menu.persistent = true;
  menu.resize = true;
  menu.placeholder = "Control de cortina";
  menu.addButton("abrir_cortina");
  menu.addButton("cerrar_cortina");
  menu.newRow();
  menu.addButton("cortina_1mt");
  menu.addButton("cortina_2mts");
  menu.newRow();
  menu.addButton("frenar_cortina");
  menu.addButton("cerrar_alarma");
  menu.newRow();
  menu.addButton("menu");
  fb::Message msg("Menu cortina", chatId);
  msg.setKeyboard(&menu);
  bot.sendMessage(msg);
}

void showOrUpdateMenu(const String& title, fb::InlineMenu& menu, const String& chatId) {
  (void)title;
  (void)menu;
  sendMainMenu(chatId);
}

void pinFixedMenu(const String& chatId) {
  (void)chatId;
}

bool canUseBot() {
  return botReady && WiFi.status() == WL_CONNECTED;
}

void runUpdateFromRepo(const String& chatId) {
  if (WiFi.status() != WL_CONNECTED) {
    sendBotMessage("Update cancelado: sin WiFi.", chatId);
    return;
  }

  sendBotMessage("Iniciando update...", chatId);
  sendBotMessage(String("URL: ") + FW_BIN_URL, chatId);

  WiFiClientSecure otaClient;
  otaClient.setInsecure();
  httpUpdate.rebootOnUpdate(false);
  t_httpUpdate_return ret = httpUpdate.update(otaClient, FW_BIN_URL);

  if (ret == HTTP_UPDATE_FAILED) {
    sendBotMessage("Update fallido. Error " + String(httpUpdate.getLastError()) + ": " + httpUpdate.getLastErrorString(), chatId);
    return;
  }

  if (ret == HTTP_UPDATE_NO_UPDATES) {
    sendBotMessage("No hay actualizaciones.", chatId);
    return;
  }

  sendBotMessage("Update OK. Reiniciando...", chatId);
  delay(1000);
  ESP.restart();
}

bool getJsonNumberByKey(const String& json, const char* key, float& outValue) {
  String pattern = "\"";
  pattern += key;
  pattern += "\":";

  int idx = json.indexOf(pattern);
  if (idx < 0) return false;
  idx += pattern.length();

  while (idx < (int)json.length() && (json[idx] == ' ' || json[idx] == '\t' || json[idx] == '"')) idx++;
  if (idx >= (int)json.length()) return false;

  int end = idx;
  while (end < (int)json.length()) {
    char c = json[end];
    if ((c >= '0' && c <= '9') || c == '-' || c == '+' || c == '.') {
      end++;
      continue;
    }
    break;
  }
  if (end <= idx) return false;

  outValue = json.substring(idx, end).toFloat();
  return true;
}

bool getJsonNumberByKey(const char* json, const char* key, float& outValue) {
  if (!json || !key) return false;

  char pattern[48];
  snprintf(pattern, sizeof(pattern), "\"%s\":", key);

  const char* p = strstr(json, pattern);
  if (!p) return false;
  p += strlen(pattern);

  while (*p == ' ' || *p == '\t' || *p == '"') p++;
  if (*p == '\0') return false;

  char* endPtr = nullptr;
  outValue = strtof(p, &endPtr);
  if (endPtr == p) return false;
  return true;
}

String getSazmWeatherText() {
  if (WiFi.status() != WL_CONNECTED) {
    return "Clima SAZM: sin WiFi.";
  }

  const char* url = "https://api.open-meteo.com/v1/forecast?latitude=-37.934&longitude=-57.573&current=temperature_2m,relative_humidity_2m,surface_pressure,wind_speed_10m&timezone=America%2FArgentina%2FBuenos_Aires";
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient https;

  if (!https.begin(client, url)) {
    return "Clima SAZM: no pude iniciar consulta.";
  }

  https.setTimeout(6000);
  int httpCode = https.GET();
  if (httpCode <= 0) {
    String err = "Clima SAZM: error HTTP ";
    err += https.errorToString(httpCode);
    https.end();
    return err;
  }

  if (httpCode != HTTP_CODE_OK) {
    String err = "Clima SAZM: respuesta HTTP ";
    err += httpCode;
    https.end();
    return err;
  }

  static char body[3072];
  int len = https.getSize();
  if (len <= 0 || len >= (int)sizeof(body)) {
    https.end();
    return "Clima SAZM: respuesta demasiado grande.";
  }

  WiFiClient* stream = https.getStreamPtr();
  int readLen = stream->readBytes(body, len);
  body[readLen] = '\0';
  https.end();

  float temp = 0.0f, hum = 0.0f, pres = 0.0f, wind = 0.0f;
  bool okT = getJsonNumberByKey(body, "temperature_2m", temp);
  bool okH = getJsonNumberByKey(body, "relative_humidity_2m", hum);
  bool okP = getJsonNumberByKey(body, "surface_pressure", pres);
  bool okW = getJsonNumberByKey(body, "wind_speed_10m", wind);

  if (!(okT && okH && okP && okW)) {
    return "Clima SAZM: respuesta incompleta.";
  }

  String out = "SAZM Mar del Plata\n";
  out += "Temperatura: ";
  out += String(temp, 1);
  out += " C\n";
  out += "Presion: ";
  out += String(pres, 1);
  out += " hPa\n";
  out += "Viento: ";
  out += String(wind, 1);
  out += " km/h\n";
  out += "Humedad: ";
  out += String(hum, 0);
  out += " %";
  return out;
}

String normalizeTelegramCmd(String in) {
  in.trim();
  in.toLowerCase();
  in.replace("\r", "");
  in.replace("\n", "");

  int spAny = in.indexOf(' ');
  if (spAny > 0) in = in.substring(0, spAny);

  if (in.startsWith("/")) {
    in.remove(0, 1);
  }

  int at = in.indexOf('@');
  if (at > 0) in = in.substring(0, at);

  in.replace(" ", "");
  in.replace(".", "");
  in.replace(",", "");
  in.replace(";", "");
  in.replace(":", "");
  in.replace("!", "");
  in.replace("?", "");

  if (in.startsWith("menu")) in = "menu";
  if (in == "tiempo") in = "clima";
  if (in == "weather") in = "clima";
  if (in == "smovimiento") in = "s_movimiento";
  if (in == "s/movimiento") in = "s_movimiento";

  return in;
}

void enqueueCmd(const String& cmd, const String& chatId) {
  if (!qCmd || cmd.length() == 0) return;
  CmdMsg m = {};
  cmd.substring(0, sizeof(m.cmd) - 1).toCharArray(m.cmd, sizeof(m.cmd));
  chatId.substring(0, sizeof(m.chatId) - 1).toCharArray(m.chatId, sizeof(m.chatId));
  xQueueSend(qCmd, &m, 0);
}

void enqueueNotify(const String& text, const String& chatId) {
  if (!qNotify || text.length() == 0) return;
  NotifyMsg m = {};
  text.substring(0, sizeof(m.text) - 1).toCharArray(m.text, sizeof(m.text));
  chatId.substring(0, sizeof(m.chatId) - 1).toCharArray(m.chatId, sizeof(m.chatId));
  xQueueSend(qNotify, &m, 0);
}

void connectToMQTT() {
  if (!mqtt_client.connected()) {
    String client_id = "esp32-client-" + String(WiFi.macAddress());
    Serial.printf("Connecting to MQTT Broker as %s...\n", client_id.c_str());
    if (mqtt_client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Connected to MQTT broker");
      mqtt_client.subscribe(mqtt_topic);
      mqtt_client.subscribe("reset");
      //mqtt_client.publish(mqtt_topic, "Hi EMQX I'm ESP32 ^^");  // Publish message upon connection
    } else {
      Serial.print("Failed to connect to MQTT broker, rc=");
      Serial.print(mqtt_client.state());
      Serial.println(" Retrying in 5 seconds.");
      //delay(5000);
    }
  }
}
void mqttCallback(char *topic, byte *payload, unsigned int length) {
  Serial.print("Message received on topic: ");
  Serial.println(topic);
  Serial.print("Message: ");

  String payloadText = "";
  for (unsigned int i = 0; i < length; i++) {
    payloadText += ((char)payload[i]);
  }
  payloadText.trim();

  if (payloadText == "reset") {
    Serial.print("nos vemos");
    esp_restart();
  }
  if (strcmp(topic, "reset") == 0) {
    String cmd = normalizeTelegramCmd(payloadText);
    enqueueCmd(cmd, CHAT_ID);
  } else {
    enqueueNotify(payloadText, CHAT_ID);
  }

  Serial.println("\n-----------------------");
}
//                                             ***************LOOP***************
void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void taskComms(void* pvParameters) {
  (void)pvParameters;
  for (;;) {
    loopCommsStep();
    vTaskDelay(pdMS_TO_TICKS(20));
  }
}

void taskAlarm(void* pvParameters) {
  (void)pvParameters;
  for (;;) {
    loopAlarmStep();
    vTaskDelay(pdMS_TO_TICKS(5));
  }
}

void loopCommsStep() {
  // Keep Telegram polling responsive so menu button presses are processed immediately.
  if (canUseBot()) bot.tick();

  if (newmsj == 1) {
    newmsj = 0;
    String cmd = normalizeTelegramCmd(teleMsj);
    enqueueCmd(cmd, currentChatId);
    teleMsj = "";
  }

  NotifyMsg n = {};
  while (qNotify && xQueueReceive(qNotify, &n, 0) == pdTRUE) {
    sendBotMessage(String(n.text), String(n.chatId));
  }

  if (millis() > auxTimeComms + 1000) {
    Serial.println((millis() - auxTimeComms) / 100);
    auxTimeComms = millis();
    Serial.print(" aux33 ");
    Serial.print(aux33);
    Serial.print(" aux34 ");
    Serial.println(aux34);

    Serial.println("tick");
    Serial.println("mqtt_client.loop();");
    mqtt_client.loop();
    Serial.println("mqtt_client.loop(fin);");

    if (cada10 > 10) {
      chequearInternet();
      if (!mqtt_client.connected()) {
        connectToMQTT();
      }
      Serial.print("fui yo");
      cada10 = 0;
      int luzSen = analogRead(39);
      if (luzSen > 3000 && digitalRead(lucesPin) == 0) {
        digitalWrite(lucesPin, 1);
        enqueueNotify("Luz si", CHAT_ID);
      }
      if (luzSen < 2000 && digitalRead(lucesPin) == 1) {
        digitalWrite(lucesPin, 0);
        enqueueNotify("Luz no", CHAT_ID);
      }
    } else {
      cada10++;
      cada10 = constrain(cada10, 0, 11);
    }
  }
}

void loopAlarmStep() {
  if (millis() > auxTimeAlarm + 1000) {
    auxTimeAlarm = millis();
    if (message != 0) {
      volt = analogRead(34);
      //int luz = analogRead(39);
      //message += "\n";
      Serial.println(volt);
      volt = map(volt, 754, 1743, 662, 1350);
      volt = volt / 100.00;
      if (volt > 13.7 || volt < 13.3) {
        message += volt;
        message += " V \n";
      }
      //   message += luz;
      // message += "  \n";
      message += WiFi.SSID();

      enqueueNotify(message, currentChatId);
      //Serial.print("send");  // sendmsj = 0;
      message = "";
    }


    //                         ************************if there is an incoming message...

    if (inicio == 1) {
      inicio = 0;
      enqueueCmd("menu", currentChatId);
    }
    CmdMsg cmdMsg = {};
    while (qCmd && xQueueReceive(qCmd, &cmdMsg, 0) == pdTRUE) {
      handleCommand(String(cmdMsg.cmd), String(cmdMsg.chatId));
    }

    if (false) {
      newmsj = 0;
      String cmd = normalizeTelegramCmd(teleMsj);
      if (cmd == "update") {
        runUpdateFromRepo(currentChatId);
        teleMsj = "";
      }

      if (cmd == "alarma") {
        sendAlarmMenu(currentChatId);
      }

      if (cmd == "cortina") {
        sendCurtainMenu(currentChatId);
      }
      if (cmd == "menu" || inicio == 1) {
        inicio = 0;
        sendMainMenu(currentChatId);
      }

      if (cmd == "cerrar_cortina") {

        if (digitalRead(cortinaUpPin) == 1) {
          digitalWrite(cortinaUpPin, 0);
          delay(2000);
        }
        digitalWrite(cortinaDownPin, 1);
        auxTime = millis();
        Bajando = 1;
        message += "Bajando Cortina\n";
        bajarEn15 = 0;
      }


      if (cmd == "cerrar_alarma") {
        poneralarmaalbajar = 1;
        if (digitalRead(cortinaUpPin) == 1) {
          digitalWrite(cortinaUpPin, 0);
          delay(2000);
        }
        digitalWrite(cortinaDownPin, 1);
        auxTime = millis();
        Bajando = 1;
        message += "Bajando Cortina\n";
        bajarEn15 = 0;
      }

      if (cmd == "reset") { esp_restart(); }



      if (cmd == "abrir_cortina") {

        if (digitalRead(cortinaDownPin) == 1) {
          digitalWrite(cortinaDownPin, 0);
          delay(2000);
        }
        if (alarma == 1) {
          message += " Primero desactive la alarma\n";

        } else {
          digitalWrite(cortinaUpPin, 1);
          auxTime = millis();
          message += "Subiendo Cortina\n";
          Subiendo = 1;
        }
      }
      if (cmd == "cortina_1mt") {

        if (digitalRead(cortinaDownPin) == 1) {
          digitalWrite(cortinaDownPin, 0);
          delay(2000);
        }
        if (alarma == 1) {
          message += " Primero desactive la alarma\n";
        } else {
          digitalWrite(cortinaUpPin, 1);
          auxTime = millis();
          //opciones();
          message += "Subiendo Cortina 1mt\n";

          Subiendo25 = 1;
        }
      }

      if (cmd == "cortina_2mts") {

        if (digitalRead(cortinaDownPin) == 1) {
          digitalWrite(cortinaDownPin, 0);
          delay(2000);
        }
        if (alarma == 1) {
          message += " Primero desactive la alarma\n";
        } else {
          digitalWrite(cortinaUpPin, 1);
          auxTime = millis();
          //opciones();
          message += "Subiendo Cortina 2mt\n";
          //sendmsj = 1;
          Subiendo75 = 1;
        }
      }


      if (cmd == "frenar_cortina") {

        digitalWrite(cortinaUpPin, 0);
        digitalWrite(cortinaDownPin, 0);
        //   opciones();
        message += "Cortina frenada\n";
        //sendmsj = 1;
        //auxTime = millis();
        Subiendo = 0;
        Bajando = 0;
        poneralarmaalbajar = 0;
      }

      if (cmd == "alarma_si" || cmd == "s_movimiento") {

        alarma = 1;
        sonando = 0;
        banderaPir = 0;
        banderaPuerta = 0;
        banderaCortina = 0;
        banderaMagne = 0;
        digitalWrite(sirenaPin, 1);
        delay(100);
        digitalWrite(sirenaPin, 0);
        if (digitalRead(cortinaPin) == 1) {
          message += "Activación parcial, Cortina  \n";
          parcialCortina = 1;
        } else {
          parcialCortina = 0;
        }
        if (digitalRead(puertaPin) == 1) {
          message += "Activación parcial, Puerta \n";
          parcialPuerta = 1;
        } else {
          parcialPuerta = 0;
        }

        if (digitalRead(magnePin) == 1) {
          message += "Activación parcial, Magnéticos\n";
          parcialMagne = 1;
        } else {
          parcialMagne = 0;
        }


        if (digitalRead(pirPin) == 1 || cmd == "s_movimiento") {
          message += "Activación parcial, Sensor de movimiento \n";
          parcialPir = 1;
        } else {
          parcialPir = 0;
          auxTimeAla = millis();

          message += "Alarma ARMADA\n";
        }
      }

      if (cmd == "alarma_no") {
        alarma = 0;
        digitalWrite(sirenaPin, 0);
        delay(200);
        digitalWrite(sirenaPin, 1);
        delay(100);
        digitalWrite(sirenaPin, 0);
        delay(200);
        digitalWrite(sirenaPin, 1);
        delay(100);
        digitalWrite(sirenaPin, 0);
        timePIR2 = millis();
        message += "Alarma DESarmada\n";

        sonando = 0;
      }


      if (cmd == "sonar_alarma") {
        alarma = 1;
        digitalWrite(sirenaPin, 1);
        auxTimeAla = millis();
        message += "Alarma sonando, manualmente\n";
        sonando = 1;
      }
      if (cmd == "luz") {
        // String sticker = F("AAMCBQADGQEAARNXJ2JmLCEf98yfbH4IrxllIeSUBERZAAIeAAP2groPvWHPcxamOuYBAAdtAAMkBA");
        // bot.sendSticker(sticker);
        //bot.sendMessage("Luz \n" ,"1109752008");
        if (luz) {
          digitalWrite(lucesPin, 0);
          message += "Luz apagada\n";
          luz = 0;
          mqtt_client.publish(mqtt_topic, "luz no");  //
        } else {
          digitalWrite(lucesPin, 1);
          message += "Luz prendida\n";
          luz = 1;
          mqtt_client.publish(mqtt_topic, "luz si");  //
        }
      }
    }
  }
  if (digitalRead(cortinaPin) == 0 && Bajando == 1) {
    delay(500);
    if (digitalRead(cortinaPin) == 0) {
      delay(1500);
      digitalWrite(cortinaDownPin, 0);
      Bajando = 0;
      message += "Cortina Cerrada\n";
      if (poneralarmaalbajar == 1) {
        poneralarmaalbajar = 0;
        message += "Activa Alarma auto\n";
        alarma = 1;
        sonando = 0;
        banderaPir = 0;
        banderaPuerta = 0;
        banderaCortina = 0;
        banderaMagne = 0;
        digitalWrite(sirenaPin, 1);
        delay(100);
        digitalWrite(sirenaPin, 0);
        if (digitalRead(cortinaPin) == 1) {
          message += "Activación parcial, Cortina  \n";
          parcialCortina = 1;
        } else {
          parcialCortina = 0;
        }
        if (digitalRead(puertaPin) == 1) {
          message += "Activación parcial, Puerta \n";
          parcialPuerta = 1;
        } else {
          parcialPuerta = 0;
        }

        if (digitalRead(magnePin) == 1) {
          message += "Activación parcial, Magnéticos\n";
          parcialMagne = 1;
        } else {
          parcialMagne = 0;
        }


        if (digitalRead(pirPin) == 1) {
          message += "Activación parcial, Sensor de movimiento \n";
          parcialPir = 1;
        } else {
          parcialPir = 0;
          auxTimeAla = millis();

          message += "Alarma ARMADA\n";
        }
      }
    }
  }

  if (Subiendo25 == 1 && millis() > auxTime + tiempoSubir25) {

    digitalWrite(cortinaUpPin, 0);
    Subiendo25 = 0;
    message += "Cortina arriba 1mt\n";
  }


  if (Subiendo75 == 1 && millis() > auxTime + tiempoSubir75) {

    digitalWrite(cortinaUpPin, 0);
    Subiendo75 = 0;

    message += "Cortina arriba 2mts\n";
  }

  if (Subiendo == 1 && millis() > auxTime + tiempoSubir) {

    digitalWrite(cortinaUpPin, 0);
    Subiendo = 0;

    message += "Cortina arriba\n ";
  }

  //*************************************************************************************************
  if (digitalRead(pirPin) == 1 || digitalRead(magnePin) == 1) { timePIR2 = millis(); }


  if (alarma == 0 && millis() - timePIR2 > 1200000) {
    message += "Alarma desarmada y no hay movimiento\n";
    timePIR2 = millis();
  }


  //*******************************************************************************************************

  if (alarma == 1 && digitalRead(puertaPin) == 1 && parcialPuerta == 0 && banderaPuerta == 0) {
    delay(500);
    if (digitalRead(puertaPin) == 1) {
      banderaPuerta = 1;
      message += "Alarma sonando, apertura de puerta\n";
      digitalWrite(sirenaPin, 1);
      auxTimeAla = millis();
      sonando = 1;
    }
  }

  if (alarma == 0 && digitalRead(puertaPin) == 1 && banderaPuerta == 0) {
    delay(500);
    if (digitalRead(puertaPin) == 1) {
      banderaPuerta = 1;
      digitalWrite(sirenaPin, 1);
      delay(40);
      digitalWrite(sirenaPin, 0);
      delay(200);
      digitalWrite(sirenaPin, 1);
      delay(40);
      digitalWrite(sirenaPin, 0);
      delay(200);
      digitalWrite(sirenaPin, 1);
      delay(40);
      digitalWrite(sirenaPin, 0);
      message += "Puerta abierta\n";
      digitalWrite(sirenaPin, 0);
    }
  }

  if (alarma == 0 && digitalRead(puertaPin) == 0 && banderaPuerta == 1) {
    delay(500);
    if (digitalRead(puertaPin) == 0) {
      banderaPuerta = 1;
      message += "Puerta cerrada\n";
    }
  }

  if (alarma == 1 && digitalRead(pirPin) == 1 && parcialPir == 0 && banderaPir == 0) {
    delay(500);
    if (digitalRead(pirPin) == 1) {
      banderaPir = 1;
      message += "Alarma sonando, sensor de movimiento activado\n";
      //sendmsj = 1;
      digitalWrite(sirenaPin, 1);
      auxTimeAla = millis();
      sonando = 1;
    }
  }

  if (alarma == 1 && digitalRead(cortinaPin) == 1 && parcialCortina == 0 && banderaCortina == 0) {

    delay(500);
    if (digitalRead(cortinaPin) == 1) {
      banderaCortina = 1;
      message += "Alarma sonando, cortina abierta\n";

      digitalWrite(sirenaPin, 1);
      auxTimeAla = millis();
      sonando = 1;
    }
  }


  //******************************************** alarma luz cortada

  if (millis() > millisLuz) {
    millisLuz = millis() + 10000;
    voltProm = voltProm * 0.8;
    volt = analogRead(34);

    volt = map(volt, 754, 1743, 662, 1350);
    volt = volt / 500.00;
    voltProm = voltProm + volt;

    if (banderades == 1 && banderaLuz == 0) {
      message += "Sin energía eléctrica \n";
      banderaLuz = 1;
      message += voltProm;
      message += " V \n";
    }
    if (banderades == 0 && banderaLuz == 1) {
      message += "Volvió la energia eléctrica \n";
      banderaLuz = 0;
      message += voltProm;
      message += " V \n";
    }

    if (voltProm < voltMin - 0.15) {
      voltMin = voltProm + 0.15;
      banderades = 1;
    }
    if (voltProm > voltMin + 0.10) {
      voltMin = voltProm - 0.10;
      banderades = 0;
    }

  }


  //*****************************************************************
  if (alarma == 1 && digitalRead(magnePin) == 1 && parcialMagne == 0 && banderaMagne == 0) {

    delay(500);
    if (digitalRead(magnePin) == 1) {
      banderaMagne = 1;
      message += "Alarma sonando, Magnéticos\n";
      digitalWrite(sirenaPin, 1);
      auxTimeAla = millis();
      sonando = 1;
    }
  }

  if (digitalRead(cortinaPin) == 0) {
    banderaCortina = 0;
  }


  if (digitalRead(pirPin) == 0) {
    banderaPir = 0;
  }


  if (digitalRead(puertaPin) == 0) {
    banderaPuerta = 0;
  }

  if (digitalRead(magnePin) == 0) {
    banderaMagne = 0;
  }

  if (sonando == 1 && millis() > auxTime30 + 30000) {
    auxTime30 = millis();
    message += "Alarma sonando !!!\n";
    //sendmsj = 1;
  }

  if (sonando == 1 && millis() > auxTimeAla + 600000) {
    message += "Alarma silenciada automaticamente, pasaron 10 minutos del último movimiento\n";
    //sendmsj = 1;
    //alarma = 0;
    digitalWrite(sirenaPin, 0);
    sonando = 0;
  }
}

void handleCommand(const String& cmd, const String& chatId) {
  if (cmd.length() == 0) return;

  if (cmd == "update") {
    runUpdateFromRepo(chatId);
    return;
  }
  if (cmd == "alarma") {
    sendAlarmMenu(chatId);
    return;
  }
  if (cmd == "cortina") {
    sendCurtainMenu(chatId);
    return;
  }
  if (cmd == "menu") {
    sendMainMenu(chatId);
    return;
  }
  if (cmd == "reset") {
    esp_restart();
    return;
  }
  if (cmd == "clima") {
    sendBotMessage(getSazmWeatherText(), chatId);
    return;
  }

  if (cmd == "cerrar_cortina" || cmd == "cerrar_alarma") {
    if (cmd == "cerrar_alarma") poneralarmaalbajar = 1;
    if (digitalRead(cortinaUpPin) == 1) {
      digitalWrite(cortinaUpPin, 0);
      delay(2000);
    }
    digitalWrite(cortinaDownPin, 1);
    auxTime = millis();
    Bajando = 1;
    message += "Bajando Cortina\n";
    bajarEn15 = 0;
    return;
  }

  if (cmd == "abrir_cortina" || cmd == "cortina_1mt" || cmd == "cortina_2mts") {
    if (digitalRead(cortinaDownPin) == 1) {
      digitalWrite(cortinaDownPin, 0);
      delay(2000);
    }
    if (alarma == 1) {
      message += " Primero desactive la alarma\n";
      return;
    }
    digitalWrite(cortinaUpPin, 1);
    auxTime = millis();
    if (cmd == "abrir_cortina") {
      message += "Subiendo Cortina\n";
      Subiendo = 1;
    } else if (cmd == "cortina_1mt") {
      message += "Subiendo Cortina 1mt\n";
      Subiendo25 = 1;
    } else {
      message += "Subiendo Cortina 2mt\n";
      Subiendo75 = 1;
    }
    return;
  }

  if (cmd == "frenar_cortina") {
    digitalWrite(cortinaUpPin, 0);
    digitalWrite(cortinaDownPin, 0);
    message += "Cortina frenada\n";
    Subiendo = 0;
    Bajando = 0;
    poneralarmaalbajar = 0;
    return;
  }

  if (cmd == "alarma_si" || cmd == "s_movimiento") {
    alarma = 1;
    sonando = 0;
    banderaPir = 0;
    banderaPuerta = 0;
    banderaCortina = 0;
    banderaMagne = 0;
    digitalWrite(sirenaPin, 1);
    delay(100);
    digitalWrite(sirenaPin, 0);
    parcialCortina = (digitalRead(cortinaPin) == 1);
    parcialPuerta = (digitalRead(puertaPin) == 1);
    parcialMagne = (digitalRead(magnePin) == 1);
    parcialPir = (digitalRead(pirPin) == 1 || cmd == "s_movimiento");
    if (parcialCortina) message += "Activacion parcial, Cortina\n";
    if (parcialPuerta) message += "Activacion parcial, Puerta\n";
    if (parcialMagne) message += "Activacion parcial, Magneticos\n";
    if (parcialPir) {
      message += "Activacion parcial, Sensor de movimiento\n";
    } else {
      auxTimeAla = millis();
      message += "Alarma ARMADA\n";
    }
    return;
  }

  if (cmd == "alarma_no") {
    alarma = 0;
    digitalWrite(sirenaPin, 0);
    delay(200);
    digitalWrite(sirenaPin, 1);
    delay(100);
    digitalWrite(sirenaPin, 0);
    delay(200);
    digitalWrite(sirenaPin, 1);
    delay(100);
    digitalWrite(sirenaPin, 0);
    timePIR2 = millis();
    message += "Alarma DESarmada\n";
    sonando = 0;
    return;
  }

  if (cmd == "sonar_alarma") {
    alarma = 1;
    digitalWrite(sirenaPin, 1);
    auxTimeAla = millis();
    message += "Alarma sonando, manualmente\n";
    sonando = 1;
    return;
  }

  if (cmd == "luz") {
    if (luz) {
      digitalWrite(lucesPin, 0);
      message += "Luz apagada\n";
      luz = 0;
    } else {
      digitalWrite(lucesPin, 1);
      message += "Luz prendida\n";
      luz = 1;
    }
    return;
  }

  sendBotMessage("Comando no reconocido: " + cmd + "\nEscribi 'menu' para ver opciones.", chatId);
}

void conectar() {
  aux34++;
alla:
  intentos = 0;
  WiFi.disconnect(true);
  delay(500);
  if (conectadoA == 2) {
    conectadoA = 1;
    WiFi.begin(ssid1, password1);

  } else {

    conectadoA = 2;
    WiFi.begin(ssid2, password2);
  }
  delay(500);
  intentos = 0;

  Serial.println("");
  Serial.print(conectadoA);
  while (WiFi.status() != WL_CONNECTED && intentos < 20) {
    delay(500);
    Serial.print(".");
    intentos++;
  }
  if (intentos > 18) { goto alla; }
  //chequearInternet();
  aux34--;
}

void chequearInternet() {
  aux33++;
  if (intentos > 18) {

    intentos = 0;
    conectar();
  }
  Serial.println("checking");
  if (Ping.ping("www.google.com", 1) == 0) {
    conectado = 1;
    Serial.println("no ping, probando otra red");
    enqueueNotify("no ping, probando otra red", CHAT_ID);
    conectar();
    intentos = 0;
    while (Ping.ping("www.google.com", 1) == 0 && intentos < 20) {
      intentos++;
      Serial.print(intentos);
      Serial.println(" waiting ping");
      delay(500);
    }
    if (intentos > 18) {
      if (canUseBot()) bot.tick();
      intentos = 0;
      conectar();
    }
  }
  if (conectado == 1) {
    String netMsg = "\nConectado a ";
    netMsg += WiFi.SSID();
    netMsg += "\n";
    conectado = 0;
    enqueueNotify(netMsg, CHAT_ID);
  }
  aux33--;
}

