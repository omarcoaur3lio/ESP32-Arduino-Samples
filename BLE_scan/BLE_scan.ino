#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEScan.h>
#include <BLEAdvertisedDevice.h>

int scanTime = 15; // scan duration - in seconds
BLEScan *pBLEScan;

class MyAdvertisedDeviceCallbacks : public BLEAdvertisedDeviceCallbacks {

    String hexToBinaryString(String hexByte) {
        
        unsigned int index = 0;
        String binaryByte = "";
    
        while (hexByte[index]) {
    
            switch (hexByte[index]) {
            case '0':
                binaryByte += "0000";
                break;
            case '1':
                binaryByte += "0001";
                break;
            case '2':
                binaryByte += "0010";
                break;
            case '3':
                binaryByte += "0011";
                break;
            case '4':
                binaryByte += "0100";
                break;
            case '5':
                binaryByte += "0101";
                break;
            case '6':
                binaryByte += "0110";
                break;
            case '7':
                binaryByte += "0111";
                break;
            case '8':
                binaryByte += "1000";
                break;
            case '9':
                binaryByte += "1001";
                break;
            case 'A':
            case 'a':
                binaryByte += "1010";
                break;
            case 'B':
            case 'b':
                binaryByte += "1011";
                break;
            case 'C':
            case 'c':
                binaryByte += "1100";
                break;
            case 'D':
            case 'd':
                binaryByte += "1101";
                break;
            case 'E':
            case 'e':
                binaryByte += "1110";
                break;
            case 'F':
            case 'f':
                binaryByte += "1111";
                break;
            default:
                Serial.printf("\nValor invalido: %c \n", hexByte[index]);
                Serial.printf("binaryByte: %s \n", binaryByte); // for debug
                binaryByte += "";
            }
            index++;
        }
        return binaryByte;
    }

    void onResult(BLEAdvertisedDevice advertisedDevice) {

        String macAddress = advertisedDevice.getAddress().toString().c_str();

        if ( macAddress == "58:7a:62:37:d6:82" ) {

            String manufactureData = advertisedDevice.toString().c_str();

            int RSSI = advertisedDevice.getRSSI();
            String name = advertisedDevice.getName().c_str();
            String payload = manufactureData.substring(manufactureData.length() - 20, manufactureData.length());

            String firstByte = payload.substring(4, 6);
            String bynaryFirstByte = hexToBinaryString(firstByte);
            String secondByte = payload.substring(6, 8);

            Serial.println("****************************************");

            Serial.printf("Name: %s \n", name);
            Serial.print("ADD: "); Serial.println(macAddress);
            Serial.print("ADV: ");Serial.println(payload);
            Serial.printf("RSSI: %d \n", advertisedDevice.getRSSI());
            Serial.printf("First Byte: %s \n", bynaryFirstByte);
        }
    }
};

void setup() {

    Serial.begin(115200);
    Serial.println("Scanning...");

    BLEDevice::init("");
    pBLEScan = BLEDevice::getScan(); //create new scan
    pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
    pBLEScan->setActiveScan(true); //active scan uses more power, but get results faster
    pBLEScan->setInterval(100);
    pBLEScan->setWindow(99); // less or equal setInterval value

}

void loop() {

    pBLEScan->start(scanTime, false);
    pBLEScan->clearResults(); // delete results fromBLEScan buffer to release memory
    delay(2000);

}
