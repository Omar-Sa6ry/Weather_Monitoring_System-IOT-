#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

// Sensor and LCD setup
#define DHTPIN 5
#define DHTTYPE DHT11
#define MQ2_PIN 4
#define RAIN_SENSOR_PIN 18
#define SOIL_MOISTURE_PIN 19 
#define BUZZER_PIN 23

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

WebServer server(80);

// Keypad setup
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {12, 14, 27, 26}; // R1-R4
byte colPins[COLS] = {25, 33, 32, 35}; // C1-C4

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

unsigned long lastLcdUpdate = 0;
int screenIndex = 0;
bool authenticated = false;
String inputPassword = "";
const String correctPassword = "2233";

unsigned long lastKeyTime = 0;
char lastKey = '\0';

void setup() {
  Serial.begin(115200);
  dht.begin();
  lcd.init();
  lcd.backlight();

  pinMode(MQ2_PIN, INPUT);
  pinMode(RAIN_SENSOR_PIN, INPUT);
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  // Access Point mode setup
  WiFi.softAP("Weater-system", "12345678"); 
  IPAddress IP = WiFi.softAPIP();  
  Serial.println("Access Point Started");
  Serial.print("AP IP address: ");
  Serial.println(IP);

  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.begin();
  Serial.println("Server started");

  lcd.setCursor(0, 0);
  lcd.print("AP Mode Started");
  lcd.setCursor(0, 1);
  lcd.print(IP);  // 192.168.4.1
  delay(3000);
  lcd.clear();

  askForPassword();
}

void loop() {
  server.handleClient();
  if (authenticated) {
    handleKeypad();
    updateLCD();
  } else {
    handlePasswordInput();
  }
}

void askForPassword() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Password:");
  lcd.setCursor(0, 1);
  inputPassword = "";
}

void handlePasswordInput() {
  char key = keypad.getKey();
  if (key && isDigit(key)) {
    inputPassword += key;
    lcd.setCursor(inputPassword.length() - 1, 1);
    lcd.print('*');

    if (inputPassword.length() == 4) {
      if (inputPassword == correctPassword) {
        lcd.clear();
        lcd.print("Access Granted");
        delay(1000);
        lcd.clear();
        authenticated = true;
        screenIndex = 0;
        lastLcdUpdate = millis();
      } else {
        lcd.clear();
        lcd.print("Wrong Password");
        delay(1000);
        askForPassword();
      }
    }
  }
}

void handleKeypad() {
  char key = keypad.getKey();
  if (key) {
    if (key == '#') {
      screenIndex = (screenIndex + 1) % 4;
      lastLcdUpdate = 0;
    }

    if (key == '5') {
      if (lastKey == '5' && millis() - lastKeyTime < 1000) {
        authenticated = false;
        askForPassword();
      }
      lastKey = '5';
      lastKeyTime = millis();
    } else {
      lastKey = key;
    }
  }
}

void updateLCD() {
  if (millis() - lastLcdUpdate >= 3000) {
    float temp = dht.readTemperature();
    float hum = dht.readHumidity();
    int mq2Digital = digitalRead(MQ2_PIN);
    bool smokeDetected = (mq2Digital == LOW);
    int rainVal = digitalRead(RAIN_SENSOR_PIN);
    int soilRaw = analogRead(SOIL_MOISTURE_PIN);
    bool isRaining = (rainVal == LOW);
    String soilStatus = (soilRaw < 600) ? "Moist" : "Dry";

    lcd.clear();
    switch (screenIndex) {
      case 0:
        lcd.setCursor(0, 0);
        lcd.print("Temp: ");
        lcd.print(temp);
        lcd.print(" C");
        lcd.setCursor(0, 1);
        lcd.print("Humidity: ");
        lcd.print(hum);
        lcd.print("%");
        break;
      case 1:
        lcd.setCursor(0, 0);
        lcd.print("Smoke:");
        lcd.setCursor(0, 1);
        lcd.print(smokeDetected ? "Detected" : "Clear");
        break;
      case 2:
        lcd.setCursor(0, 0);
        lcd.print("Rain:");
        lcd.setCursor(0, 1);
        lcd.print(isRaining ? "Raining" : "Clear");
        break;
      case 3:
        lcd.setCursor(0, 0);
        lcd.print("Soil Moisture:");
        lcd.setCursor(0, 1);
        lcd.print(soilStatus);
        break;
    }

    lastLcdUpdate = millis();
  }
}

void handleRoot() {
  server.send(200, "text/html", R"rawliteral(
    <html>
      <head>
        <meta name="viewport" content="width=device-width, initial-scale=1">
        <meta http-equiv='refresh' content='1'>
        <style>
          body {
            background: #f0f4f8;
            font-family: 'Segoe UI', sans-serif;
            margin: 0;
            padding: 0;
            display: flex;
            flex-direction: column;
            min-height: 100vh;
          }
          header {
            background: #1e88e5;
            color: white;
            padding: 20px;
            text-align: center;
            font-size: 24px;
            font-weight: bold;
          }
          main {
            flex: 1;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
          }
          #sensorData {
            background: white;
            padding: 20px;
            border-radius: 12px;
            box-shadow: 0 6px 15px rgba(0,0,0,0.1);
            font-size: 18px;
            line-height: 1.8;
            color: #333;
            min-width: 250px;
            max-width: 90%;
            text-align: left;
          }
          footer {
            background: #1e88e5;
            color: white;
            text-align: center;
            padding: 15px;
            font-size: 14px;
          }
          .sensorBox {
            padding: 10px;
            border-radius: 8px;
            margin-bottom: 15px;
            color: white;
            font-size: 16px;
            box-shadow: 0 5px 10px rgba(0, 0, 0, 0.1);
          }
          .temp { background-color: #FF6F61; }
          .smoke { background-color: #FFB74D; }
          .rain { background-color: #4CAF50; }
          .water { background-color: #2196F3; }
        </style>
      </head>
      <body>
        <header>Sensor Dashboard</header>
        <main>
          <div id="sensorData">Loading...</div>
        </main>
        <footer>ESP Sensor Monitor | Developed by MYSHO</footer>

        <script>
          function fetchData() {
            fetch("/data")
              .then(response => response.text())
              .then(data => {
                document.getElementById("sensorData").innerHTML = data;
              });
          }
          setInterval(fetchData, 1000);
          fetchData();
        </script>
      </body>
    </html>
  )rawliteral");
}

void handleData() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  int mq2Digital = digitalRead(MQ2_PIN);
  bool smokeDetected = (mq2Digital == LOW);

  int rainVal = digitalRead(RAIN_SENSOR_PIN);
  bool isRaining = (rainVal == LOW);

  int soilRaw = analogRead(SOIL_MOISTURE_PIN);
  String soilStatus = (soilRaw < 600) ? "Moist" : "Dry";

  String data = "";
  data += "<div class='sensorBox temp'><b>Temperature:</b> " + String(temp) + " °C<br><b>Humidity:</b> " + String(hum) + " %</div>";
  data += "<div class='sensorBox smoke'><b>Smoke:</b> " + String(smokeDetected ? "Detected" : "Clear") + "</div>";
  data += "<div class='sensorBox rain'><b>Rain:</b> " + String(isRaining ? "Raining" : "Clear") + "</div>";
  data += "<div class='sensorBox water'><b>Soil Moisture:</b> " + soilStatus + "</div>";

  server.send(200, "text/html", data);

  if (smokeDetected) {
    tone(BUZZER_PIN, 1000, 500);
  }
}

