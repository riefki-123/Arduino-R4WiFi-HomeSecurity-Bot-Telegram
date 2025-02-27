#include <WiFi.h>
#include <WiFiSSLClient.h>

// WiFi Config
const char* ssid = "YOUR WIFI";
const char* password = "YOUR PASS";

// Token Bot Di Telegram
const char* botToken = "xxx";

// Chat ID Telegram Kamu
const char* chatID = "xxx";

// Endpoint Telegram API
String telegramAPI = "/bot" + String(botToken) + "/sendMessage";
const char* host = "api.telegram.org";
const int httpsPort = 443;

// Pin sensor PIR dan buzzer
const int pirPin = 2;
const int buzzerPin = 3;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(buzzerPin, OUTPUT);
  
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.print("Menghubungkan ke WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nTerhubung ke WiFi");
}

void loop() {
  int motionDetected = digitalRead(pirPin);

  if (motionDetected == HIGH) {
    digitalWrite(buzzerPin, HIGH);
    Serial.println("Gerakan terdeteksi! Mengirim notifikasi...");

    if(WiFi.status() == WL_CONNECTED) {
      WiFiSSLClient client;
      Serial.println("Menghubungkan ke Telegram API...");
      
      if (!client.connect(host, httpsPort)) {
        Serial.println("Koneksi ke Telegram API gagal.");
        return;
      }

      String message = "Penyusup terdeteksi di rumah Anda!";
      String jsonData = "{\"chat_id\":\"" + String(chatID) + "\",\"text\":\"" + message + "\"}";

      String request = String("POST ") + telegramAPI + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "Content-Type: application/json\r\n" +
                       "Content-Length: " + jsonData.length() + "\r\n\r\n" +
                       jsonData;

      client.print(request);

      while (client.connected()) {
        String response = client.readString();
        if (response.length() > 0) {
          Serial.println("Response dari Telegram API:");
          Serial.println(response);
        }
      }
    }
    else {
      Serial.println("WiFi tidak terhubung.");
    }

    // Setting untuk Suara Buzzer
    for (int i = 1000; i <= 2000; i += 50) {
      tone(buzzerPin, i);
      delay(10);
    }
    for (int i = 2000; i >= 1000; i -= 50) {
      tone(buzzerPin, i);
      delay(10);
    }
  }
  else {
    noTone(buzzerPin);
  }

  delay(500);
}
