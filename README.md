# IoT Smart Attendance System (ESP32 + Fingerprint + Google Sheets)

A professional Biometric Attendance System built with **ESP32**, utilizing an **Optical Fingerprint Sensor** and integrated with **Google Sheets** as a cloud database via **Google Apps Script**.

## 🚀 Overview
This project provides a seamless way to manage attendance using biometric data. It features real-time cloud synchronization, anti-duplicate check (5-minute buffer), and remote user management (deletion) directly from the spreadsheet.

## ✨ Key Features
*   **Biometric Authentication:** Secure and fast fingerprint matching using the AS608/R503 sensor.
*   **Cloud Integration:** Automatically logs attendance (Name, ID, Date, Time) into Google Sheets.
*   **Anti-Duplicate System:** Prevents accidental double-logging within a 5-minute window.
*   **Bidirectional Sync:** If a user is marked for "Delete" in the spreadsheet, the ESP32 will automatically remove their fingerprint from its local hardware memory.
*   **Visual & Audio Feedback:** Features an I2C LCD for status updates and a Buzzer for distinctive audio alerts (Success, Fail, Alert).
*   **Manual Enrollment:** Dedicated button to enter "Enrollment Mode" for new users.

## 🛠️ Hardware Requirements
*   **ESP32** (NodeMCU or DevKit)
*   **Fingerprint Sensor** (AS608 or R50x series)
*   **LCD Display (16x2)** with I2C Module
*   **Active Buzzer**
*   **Push Button**
*   **Jumper Wires & Breadboard**

## 💻 Tech Stack
*   **Embedded:** C++ (Arduino Framework)
*   **Cloud/Backend:** JavaScript (Google Apps Script)
*   **Database:** Google Sheets

## 📂 Project Structure
*   `Firmware/`: Contains the `.ino` code for the ESP32.
*   `Cloud-Script/`: Contains the `.gs` code for Google Apps Script.
*   `Schematics/`: Circuit diagrams and wiring information.

## 🔧 Setup & Installation
1.  **Google Sheets:** Create a sheet with two tabs: `Users` and `Attendance`.
2.  **Apps Script:** Deploy the provided `.gs` code as a Web App and copy the **Deployment URL**.
3.  **Arduino IDE:** 
    *   Install `Adafruit Fingerprint`, `LiquidCrystal_I2C`, and `WiFi` libraries.
    *   Paste your **SSID**, **Password**, and **Script URL** into the code.
    *   Upload the code to your ESP32.

## 📝 Circuit Wiring
| Component | ESP32 Pin |
| :--- | :--- |
| Fingerprint RX | GPIO 17 (TX2) |
| Fingerprint TX | GPIO 16 (RX2) |
| LCD SDA | GPIO 21 |
| LCD SCL | GPIO 22 |
| Button | GPIO 4 |
| Buzzer | GPIO 12 |

## 🤝 Contributing
Contributions, issues, and feature requests are welcome! Feel free to check the issues page.

---
## Developed by  
* Abdelhalim Mohamed
* Ahmed Fargaly 
* Abdelrahman Esaam
* Abdelrahman El Attar
* Amr Abdelbaki
* Enas El Nassag
* Malak Tamer
* Fatma Helmy
* Salma Amer 

