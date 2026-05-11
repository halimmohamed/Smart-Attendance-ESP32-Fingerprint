#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

// --- إعدادات الشبكة والجوجل شيت ---
const char* ssid = "Halim";
const char* password = "00000000";
String googleScriptURL = "https://script.google.com/macros/s/AKfycbwefDKS80oP5ktOnrM3dg7kCR91NbMKUNSg7HU8BIR6-wEbuPNWoB2cXLlAz3qU8ak9/exec";

// --- تعريف البنات (Pins) ---
#define RXD2 16
#define TXD2 17
#define BUTTON_PIN 4
#define BUZZER_PIN 12

LiquidCrystal_I2C lcd(0x27, 16, 2);
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&Serial2);

// --- مكتبة الأصوات ---
void soundSuccess() { tone(BUZZER_PIN, 2000, 200); }
void soundFail() { tone(BUZZER_PIN, 400, 150); delay(100); tone(BUZZER_PIN, 400, 150); }
void soundAlert() { tone(BUZZER_PIN, 1200, 100); }
void soundDelete() { for(int i=0; i<3; i++) { tone(BUZZER_PIN, 800, 100); delay(150); } }
void soundSaveMelody() { 
  tone(BUZZER_PIN, 1500, 100); delay(100);
  tone(BUZZER_PIN, 2000, 100); delay(100);
  tone(BUZZER_PIN, 2500, 300);
}

void setup() {
  Serial.begin(115200);
  Serial2.begin(57600, SERIAL_8N1, RXD2, TXD2);
  
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  
  lcd.init();
  lcd.backlight();
  
  // الاتصال بالواي فاي
  lcd.setCursor(0, 0);
  lcd.print("WiFi: Halim");
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    lcd.setCursor(0, 1);
    lcd.print("Connecting...");
  }
  
  lcd.clear();
  lcd.print("WiFi Connected!");
  soundSuccess();
  
  if (finger.verifyPassword()) {
    lcd.setCursor(0, 1);
    lcd.print("Sensor Ready!");
  } else {
    lcd.clear();
    lcd.print("Sensor Error!");
    while (1) { tone(BUZZER_PIN, 200, 500); delay(1000); }
  }
  delay(1500);
  lcd.clear();
}

void loop() {
  lcd.setCursor(0, 0);
  lcd.print("Scan Finger...  ");
  lcd.setCursor(0, 1);
  lcd.print("Ready to Work   ");

  // حالة الضغط على زر تسجيل بصمة جديدة
  if (digitalRead(BUTTON_PIN) == LOW) {
    soundAlert();
    delay(500);
    finger.getTemplateCount();
    enrollFinger(finger.templateCount + 1);
  }

  // محاولة قراءة البصمة
  int fingerID = getFingerprintID();
  if (fingerID >= 0) {
    processAttendance(fingerID);
  }
}

// دالة التواصل مع جوجل شيت ومعالجة الردود
void processAttendance(int id) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = googleScriptURL + "?action=login&id=" + String(id);
    
    lcd.clear();
    lcd.print("Syncing Cloud..");
    
    http.begin(url);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    int httpCode = http.GET();
    
    if (httpCode > 0) {
      String response = http.getString();
      
      // 1. حالة مسح البصمة
      if (response == "COMMAND:DELETE") {
        if (finger.deleteModel(id) == FINGERPRINT_OK) {
          lcd.clear();
          lcd.print("ID "); lcd.print(id);
          lcd.setCursor(0, 1);
          lcd.print("Deleted (Cloud)");
          soundDelete();
        }
      } 
      // 2. حالة منع التدبيل (أقل من 5 دقائق)
      else if (response.startsWith("Already In")) {
        int split = response.indexOf('|');
        String name = response.substring(split + 1);
        lcd.clear();
        lcd.print("Wait 5 Mins!");
        lcd.setCursor(0, 1);
        lcd.print(name);
        soundFail();
      }
      // 3. حالة النجاح (حضور أو انصراف)
      else {
        int split = response.indexOf('|');
        String msg = response.substring(0, split);
        String grp = response.substring(split + 1);
        
        lcd.clear();
        lcd.print(msg); // يعرض Welcome الاسم أو Goodbye الاسم
        lcd.setCursor(0, 1);
        lcd.print(grp); // يعرض رقم الجروب
        soundSuccess();
      }
    } else {
      lcd.clear();
      lcd.print("HTTP Error");
      soundFail();
    }
    http.end();
    delay(3000);
    lcd.clear();
  }
}

int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;
  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) return finger.fingerID;
  if (p == FINGERPRINT_NOTFOUND) {
    lcd.clear(); lcd.print("Unknown Finger");
    soundFail();
    delay(2000);
    return -1;
  }
  return -1;
}

void enrollFinger(int id) {
  int p = -1;
  lcd.clear();
  lcd.print("New User ID:"); lcd.print(id);
  
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    lcd.setCursor(0, 1);
    lcd.print("Place Finger...");
  }
  soundAlert();
  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return;

  lcd.clear();
  lcd.print("Remove Finger");
  delay(1000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) { p = finger.getImage(); }
  
  p = -1;
  lcd.clear();
  lcd.print("Place Again...");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
  }

  soundAlert();
  p = finger.image2Tz(2);
  if (finger.createModel() == FINGERPRINT_OK) {
    finger.storeModel(id);
    lcd.clear();
    lcd.print("Saved to HW!");
    soundSaveMelody();
    // إخطار جوجل بإضافة ID جديد
    sendDataToGoogleSimple(id);
  } else {
    lcd.clear();
    lcd.print("Enroll Failed");
    soundFail();
  }
  delay(2000);
}

void sendDataToGoogleSimple(int id) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = googleScriptURL + "?action=login&id=" + String(id);
    http.begin(url);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.GET();
    http.end();
  }
}