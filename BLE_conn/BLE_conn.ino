#include "BLEDevice.h"
#include "./Cipher.h"       
  
Cipher * cipher = new Cipher();

static BLEUUID serviceUUID("6E400001-B5A3-F393-E0A9-E50E24DCCA9E");
static BLEUUID charUUID("6E400002-B5A3-F393-E0A9-E50E24DCCA9E");

static BLERemoteCharacteristic *pRemoteCharacteristic;

#define btn 34
#define redLED 25
#define greenLED 33

int count = 0;
bool isActive = false;
bool isClosed = false;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 70;

void setup() {

    Serial.begin(115200);

    pinMode(btn, INPUT);
    pinMode(redLED, OUTPUT);
    pinMode(greenLED, OUTPUT);

    BLEDevice::init("");

}

void doorOpen() {

    Serial.println("Opening the door...");
    
    BLEClient *pClient = BLEDevice::createClient();

    delay(200);

    pClient->connect(BLEAddress("XX:XX:XX:XX:XX:XX")); 
    Serial.println(" - Connected to server");

    BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
        Serial.print("UUID not found: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
    } else {
        Serial.println(" - UUID found");
    }

    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
        Serial.print("Characteristic not found: ");
        Serial.println(charUUID.toString().c_str());
        pClient->disconnect();
    } else {
        Serial.println(" - Characteristic found");
    }

    if (pRemoteCharacteristic->canRead()) {
        std::string crypted_private_key = pRemoteCharacteristic->readValue();

        delay(200);

        String at_door_open = "AT+DOOR=1\r\n\0\0\0\0\0";

        /* For Cipher */  
        char *serial_number = "xxxxxxxxxx";
        cipher->setKey(serial_number);

        String private_key = cipher->decryptString(crypted_private_key.c_str());
 
        char *key2 = (char*) private_key.c_str();
        cipher->setKey(key2);

        String crypted_at_cmd = cipher->encryptString(at_door_open);

        pRemoteCharacteristic->writeValue(crypted_at_cmd.c_str(), crypted_at_cmd.length());

        delay(200);

        pClient->disconnect();

        digitalWrite(greenLED, true);
        digitalWrite(redLED, false);

        Serial.println("\n***** DOOR OPEN *****\n");
 
    }
}

void doorClose() {
    Serial.println("Closing the door...");

    BLEClient *pClient = BLEDevice::createClient();

    delay(200);

    pClient->connect(BLEAddress("XX:XX:XX:XX:XX:XX")); //58:7A:62:37:D6:82
    Serial.println(" - Connected to server");

    BLERemoteService *pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
        Serial.print("UUID not found: ");
        Serial.println(serviceUUID.toString().c_str());
        pClient->disconnect();
    } else {
        Serial.println(" - UUID found");
    }

    pRemoteCharacteristic = pRemoteService->getCharacteristic(charUUID);
    if (pRemoteCharacteristic == nullptr) {
        Serial.print("Characteristic not found: ");
        Serial.println(charUUID.toString().c_str());
        pClient->disconnect();
    } else {
        Serial.println(" - Characteristic found");
    }

    if (pRemoteCharacteristic->canRead()) {
        std::string crypted_private_key = pRemoteCharacteristic->readValue();

        delay(200);

        String at_door_open = "AT+DOOR=0\r\n\0\0\0\0\0";

        /* For Cipher */  
        char *serial_number = "xxxxxxxxxx"; 
        cipher->setKey(serial_number);

        String private_key = cipher->decryptString(crypted_private_key.c_str());
 
        //char *key2 = (char*) private_key.c_str();
        cipher->setKey((char*) private_key.c_str());

        String crypted_at_cmd = cipher->encryptString(at_door_open);

        pRemoteCharacteristic->writeValue(crypted_at_cmd.c_str(), crypted_at_cmd.length());

        delay(200);

        pClient->disconnect();

        digitalWrite(greenLED, false);
        digitalWrite(redLED, true);

        Serial.println("\n***** CLOSED DOOR *****\n");
 
    }

}

void loop() {
    if(digitalRead(btn) && isActive == false){
        if ((millis() - lastDebounceTime) > debounceDelay) {
            if (isClosed) {
                doorOpen();
                isClosed = false;
            } else {
                doorClose();
                isClosed = true;
            }
        }
        lastDebounceTime = millis();
        isActive = false;
    }
}
