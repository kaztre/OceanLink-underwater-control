#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "Module2_AP";  // Replace with desired SSID for Module 2's Access Point
const char* password = "password";  // Replace with desired password for Module 2's Access Point

WebServer server(80);

String state = "close";
bool stateChangedByBrowser = false;
bool shouldRevertToPwm = false;

const int pwmPin = 4; // Pin to read PWM signal

unsigned long browserClickTimeout = 5000; // Timeout in milliseconds
unsigned long lastBrowserClickTime = 0;

void handleMainPage() {
  server.send(200, "text/html",
    "<html>"
    "<head>"
      "<style>"
        "html, body { height: 100%; margin: 0; }"
        "body {"
          "display: flex;"
          "align-items: center;"
          "justify-content: center;"
          "flex-direction: column;"
          "font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;"
          "background-color: #f5f5f5;"
        "}"
        "a.button {"
          "display: inline-block;"
          "padding: 30px 60px;"
          "font-size: 32px;"
          "text-align: center;"
          "margin: 10px;"
          "text-decoration: none;"
          "color: white;"
          "background-color: #4CAF50;"
          "border-radius: 10px;"
          "box-shadow: 0 4px 8px rgba(0, 0, 0, 0.2);"
          "transition: all 0.3s;"
          "border: 2px solid #333;"
        "}"
        "a.button:hover {"
          "background-color: #3f8c45;"
          "box-shadow: 0 8px 16px rgba(0, 0, 0, 0.3);"
          "transform: translateY(-3px);"
        "}"
        "h2 {"
          "font-size: 36px;"
          "margin-top: 30px;"
          "color: #333;"
          "padding: 10px;"
          "border: 2px dashed #333;"
          "border-radius: 10px;"
        "}"
    "</style>"
      "<script>"
        "function updateState(newState) {"
          "var xhttp = new XMLHttpRequest();"
          "xhttp.onreadystatechange = function() {"
            "if (this.readyState == 4 && this.status == 200) {"
              "document.getElementById('state').innerHTML = newState;"
            "}"
          "};"
          "xhttp.open('GET', '/' + newState, true);"
          "xhttp.send();"
        "}"
        
        "function fetchState() {"
          "var xhttp = new XMLHttpRequest();"
          "xhttp.onreadystatechange = function() {"
            "if (this.readyState == 4 && this.status == 200) {"
              "document.getElementById('state').innerHTML = this.responseText;"
            "}"
          "};"
          "xhttp.open('GET', '/getstate', true);"
          "xhttp.send();"
        "}"
        
        "setInterval(fetchState, 1000);" // Fetch state every 1 second (1000 milliseconds)
      "</script>"
    "</head>"
    "<body>"
      "<a class=\"button\" href=\"#\" onclick=\"updateState('open')\">Open</a>"
      "<br>"
      "<a class=\"button\" href=\"#\" onclick=\"updateState('close')\">Close</a>"
      "<br>"
      "<h2>Current State: <span id=\"state\">close</span></h2>"
    "</body>"
    "</html>"
  );
}


void handleGetState() {
  server.send(200, "text/plain", state);
}

void handleOpen() {
  state = "open";
  stateChangedByBrowser = true;
  shouldRevertToPwm = true;
  lastBrowserClickTime = millis();
  server.send(200, "text/plain", "State changed to OPEN");
}

void handleClose() {
  state = "close";
  stateChangedByBrowser = true;
  shouldRevertToPwm = true;
  lastBrowserClickTime = millis();
  server.send(200, "text/plain", "State changed to CLOSE");
}

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  Serial.println("Access point started");
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());

  server.on("/", handleMainPage); // This line is added
  server.on("/getstate", handleGetState);
  server.on("/open", handleOpen);
  server.on("/close", handleClose);
  server.begin();
  Serial.println("HTTP server started");

  pinMode(pwmPin, INPUT);
}


void loop() {
  server.handleClient();

  unsigned long currentTime = millis();
  unsigned long pwmValue = pulseIn(pwmPin, HIGH, 25000); // Read PWM value with 25ms timeout

  if (stateChangedByBrowser) {
    if (currentTime - lastBrowserClickTime > browserClickTimeout) {
      stateChangedByBrowser = false;
    }
  } else {
    if (pwmValue >= 1600 && state != "open") {
      state = "open";
    } else if (pwmValue <= 1400 && state != "close") {
      state = "close";
    }
    // If pwmValue is between 1400 and 1600, state remains unchanged
  }
}