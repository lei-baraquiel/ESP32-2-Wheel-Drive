#include <WiFi.h>
#include <WebServer.h>

// WiFi Configuration
const char* ssid = "My2WheelDrive";
const char* password = "password123";

// Motor Pins
const int motorA_PWM = 25; // Speed control
const int motorA_IN1 = 26; // Direction 1
const int motorA_IN2 = 27; // Direction 2

const int motorB_PWM = 18; // Speed control
const int motorB_IN1 = 19; // Direction 1
const int motorB_IN2 = 21; // Direction 2

WebServer server(80);
int currentSpeed = 200;

// HTML Interface
// The R"=====( ... )=====" allows to write raw HTML
const char HTML_CONTENT[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <style>
    body { user-select:none; font-family: sans-serif; text-align: center; background: #222; color: white; }
    .btn { width: 80px; height: 80px; margin: 10px; background: #444; color: #00e676; border: 2px solid #00e676; border-radius: 10px; font-size: 24px; }
    .btn:active { background: #00e676; color: black; }
    .slider { width: 80%; margin-top: 40px; }
  </style>
</head>
<body>
  <h2>Robot Control</h2>
  <div><button class="btn" ontouchstart="send('forward')" ontouchend="send('stop')">F</button></div>
  <div>
    <button class="btn" ontouchstart="send('left')" ontouchend="send('stop')">L</button>
    <button class="btn" ontouchstart="send('right')" ontouchend="send('stop')">R</button>
  </div>
  <div><button class="btn" ontouchstart="send('backward')" ontouchend="send('stop')">B</button></div>
 
  <p>Speed: <input type="range" min="150" max="255" class="slider" onchange="fetch('/speed?val=' + this.value)"></p>

  <script>
    function send(cmd) { fetch('/' + cmd); }
  </script>
</body>
</html>
)=====";

// Motor Logic
void drive(int speedA, int speedB, int a1, int a2, int b1, int b2) {
  digitalWrite(motorA_IN1, a1); digitalWrite(motorA_IN2, a2);
  digitalWrite(motorB_IN1, b1); digitalWrite(motorB_IN2, b2);
  analogWrite(motorA_PWM, speedA);
  analogWrite(motorB_PWM, speedB);
}

void setup() {
  Serial.begin(115200);
 
  // Set all motor pins as outputs
  pinMode(motorA_PWM, OUTPUT); pinMode(motorA_IN1, OUTPUT); pinMode(motorA_IN2, OUTPUT);
  pinMode(motorB_PWM, OUTPUT); pinMode(motorB_IN1, OUTPUT); pinMode(motorB_IN2, OUTPUT);

  // Start WiFi Access Point, basically a hotspot
  WiFi.softAP(ssid, password);

  // Display ssid and ip address to serial monitor
  Serial.print("Connect to WiFi: "); Serial.println(ssid);
  Serial.print("Visit: "); Serial.println(WiFi.softAPIP());

  // When the user connects to the wifi and go to the ip address,
  // the HTML_CONTENT will be served / shown to the user,
  // it contains the controls as written in the html
  server.on("/", []() { server.send(200, "text/html", HTML_CONTENT); });
 
  // These will run based on what button the user taps
  server.on("/forward",  []() { drive(currentSpeed, currentSpeed, 1, 0, 1, 0); server.send(200); });
  server.on("/backward", []() { drive(currentSpeed, currentSpeed, 0, 1, 0, 1); server.send(200); });
  server.on("/left",     []() { drive(currentSpeed, currentSpeed, 0, 1, 1, 0); server.send(200); });
  server.on("/right",    []() { drive(currentSpeed, currentSpeed, 1, 0, 0, 1); server.send(200); });
  server.on("/stop",     []() { drive(0, 0, 0, 0, 0, 0); server.send(200); });

  // Basically grabs the speed from the slider created on the HTML_CONTENT
  server.on("/speed", []() {
    if (server.hasArg("val")) currentSpeed = server.arg("val").toInt();
    server.send(200);
  });

  server.begin();
}

void loop() {
  // serve client
  server.handleClient();
}
