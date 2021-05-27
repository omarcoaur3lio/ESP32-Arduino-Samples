## BLE Scann
With this code it is possible to scan [BLE (Bluetooth Low Energy)](https://en.wikipedia.org/wiki/Bluetooth_Low_Energy) devices. 


> 👉  These codes were implemented for an ESP model ESP32-WROOM-32E

<br>

<p align="center">
   <img src="../.github/esp.png" alt="ESP32" width="40%"/>  
</p>


### ✨ Features 
- Scanning of BLE devices 
- MAC address filter 
- Advertising capture 
- Separation of the following data: `RSSI`, `device name`, `manufacture data`, `payload`.

---

The code also performs the payload interpretation by converting it to `hexToBinaryString` (this part is specific to a particular project 😅) 

The `hexToBinaryString` function takes as a parameter a hexadecimal byte, captured in the pyload, and transforms this information into its binary representation. 