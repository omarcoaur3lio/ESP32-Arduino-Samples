#include <WiFi.h>
#include <FirebaseESP32.h>

#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

#define WIFI_SSID "wifi_name"
#define WIFI_PASSWORD "wifi_password"

#define API_KEY "AIzaSyC--9-VnkB8Mm7kUsn5vKDn9SP8kY1ATgk"
#define DATABASE_URL "https://esp32-arduino-7efe1-default-rtdb.firebaseio.com/"

/* Define credentials to access firebase project */
#define USER_EMAIL "email@project"
#define USER_PASSWORD "your_password"


//Define firebase data objects
FirebaseData fb_receivedData;
FirebaseData fb_sentData;

FirebaseAuth auth;
FirebaseConfig config;

String path = "/device";

#define ledA 32
#define ledV 25
#define ledVm 33

#define btn 34
bool isActive = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 70;
int count = 0;

void setup() {

    Serial.begin(115200);

    pinMode(btn, INPUT);
    pinMode(ledA, OUTPUT);
    pinMode(ledV, OUTPUT);
    pinMode(ledVm, OUTPUT);

    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print("Connecting to Wi-Fi");

    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(300);
    }
    Serial.println();
    Serial.print("Connected with IP: ");
    Serial.println(WiFi.localIP());
    Serial.println();

    config.api_key = API_KEY;
    config.database_url = DATABASE_URL;

    auth.user.email = USER_EMAIL;
    auth.user.password = USER_PASSWORD;

    /* Assign the callback function for the long running token generation task */
    config.token_status_callback = tokenStatusCallback;

    Firebase.begin(&config, &auth);

    Firebase.reconnectWiFi(true);

    if ( !Firebase.beginStream( fb_receivedData, path.c_str() ) ) {
        Serial.println("------------------------------------");
        Serial.println("Can't begin stream connection...");
        Serial.println("REASON: " + fb_receivedData.errorReason());
        Serial.println("------------------------------------");
        Serial.println();
    }
    Firebase.setStreamCallback(fb_receivedData, streamCallback, streamTimeoutCallback);
}

void streamCallback( StreamData data ) {

    Serial.println("Stream Data available...");
    Serial.println("STREAM PATH: " + data.streamPath());
    Serial.println("EVENT PATH: " + data.dataPath());
    Serial.println("DATA TYPE: " + data.dataType());
    Serial.println("EVENT TYPE: " + data.eventType());
    Serial.print("VALUE: ");
    printResult(data); //see addons/RTDBHelper.h

    String ledCommand = data.stringData();

    if (ledCommand == "v") {
        digitalWrite(ledA, false);
        digitalWrite(ledV, true);
        digitalWrite(ledVm, false);
    } else if (ledCommand == "a") {
        digitalWrite(ledA, true);
        digitalWrite(ledV, false);
        digitalWrite(ledVm, false);
    } else if (ledCommand == "vm") {
        digitalWrite(ledA, false);
        digitalWrite(ledV, false);
        digitalWrite(ledVm, true);
    } else {
        digitalWrite(ledA, false);
        digitalWrite(ledV, false);
        digitalWrite(ledVm, false);
    }

    Serial.println();
}

void streamTimeoutCallback( bool timeout ) {
    if ( timeout ) {
        Serial.println();
        Serial.println("Stream timeout, resume streaming...");
        Serial.println();
    }
}

void sendToFirebase() {
    Serial.println("Preparando dados....");
    String name = "LED";
    String command = "";
    FirebaseJson json;
    json.add("name", name).add("Count", count).add("Led Command", command);
    String Path = path + "/"  + name;
    if ( Firebase.setJSONAsync(fb_sentData, Path.c_str(), json) ) {
        Serial.println("Dados enviados.");
    } else {
        Serial.println("Error:  " + fb_sentData.errorReason());
    }
}

void loop() {

    if (digitalRead(btn) && isActive == false) {
        if ((millis() - lastDebounceTime) > debounceDelay) {
            sendToFirebase();
            count++;
        }
        lastDebounceTime = millis();
        isActive = false;
    }
}
