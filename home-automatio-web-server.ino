#include <WiFi.h>
#include <ESP32Servo.h>

const char *ssid = "YourWifiPassword";
const char *password = "s3krit";

const int ledPin = 2;
const int servoPin = 4;

WiFiServer server(80);
Servo doorServo;
bool lightState = false;
bool doorState = false;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  doorServo.attach(servoPin);
  doorServo.write(0);

  delay(10);

  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client.");
    String currentLine = "";
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        if (c == '\n') {

          if (currentLine.length() == 0) {
            // HTTP headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            client.println("<!DOCTYPE html><html>");
            client.println("<head>");
            client.println("<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"stylesheet\" href=\"https://maxcdn.bootstrapcdn.com/bootstrap/4.5.2/css/bootstrap.min.css\">");
            client.println("</head>");
            client.println("<body class='text-center'>");

            client.println("<div class='container mt-5'>");
            client.println("<h1>Smart Home Voice automated control</h1>");
            client.println("<p>Say 'light on', 'light off', 'open door', or 'close door':</p>");
            client.println("<p id='status'></p>");

            // Icons for light and door
            client.println("<div class='my-3'>");
            client.println("<div id='light-icon'></div>");
            client.println("<div id='door-icon'></div>");
            client.println("</div>");

            // JavaScript for Web Speech API
            client.println("<script>");
            client.println("let lightState = " + String(lightState) + ";");
            client.println("let doorState = " + String(doorState) + ";");

            client.println("function startListening() {");
            client.println("    const recognition = new (window.SpeechRecognition || window.webkitSpeechRecognition)();");
            client.println("    recognition.lang = 'en-US';");
            client.println("    recognition.start();");
            client.println("    document.getElementById('status').textContent = 'sige pag yakan...';");

            client.println("    recognition.onresult = (event) => {");
            client.println("        const command = event.results[0][0].transcript.toLowerCase();");
            client.println("        document.getElementById('status').textContent = 'Nak binati: ' + command;");

            // Light on/off logic with state check
            client.println("        if (command.includes('light on')) {");
            client.println("            if (lightState) {");
            client.println("                document.getElementById('status').textContent = 'Naka ON na ngane....';");
            client.println("            } else {");
            client.println("                window.location.href = '/toggleLightOn';");
            client.println("            }");
            client.println("        } else if (command.includes('light off')) {");
            client.println("            if (!lightState) {");
            client.println("                document.getElementById('status').textContent = 'Naka OFF na ngane....';");
            client.println("            } else {");
            client.println("                window.location.href = '/toggleLightOff';");
            client.println("            }");
            client.println("        }");

            // Door open/close logic with state check
            client.println("        else if (command.includes('open door')) {");
            client.println("            if (doorState) {");
            client.println("                document.getElementById('status').textContent = 'Abredo na ngane....';");
            client.println("            } else {");
            client.println("                window.location.href = '/openDoor';");
            client.println("            }");
            client.println("        } else if (command.includes('close door')) {");
            client.println("            if (!doorState) {");
            client.println("                document.getElementById('status').textContent = 'Naka sarado na ngane....!';");
            client.println("            } else {");
            client.println("                window.location.href = '/closeDoor';");
            client.println("            }");
            client.println("        } else {");
            client.println("            document.getElementById('status').textContent = 'Dre ko maintindihan!';");
            client.println("        }");

            client.println("        // Restart listening after processing command");
            client.println("        setTimeout(startListening, 1000);");
            client.println("    };");

            client.println("    recognition.onerror = (event) => {");
            client.println("        document.getElementById('status').textContent = 'Ngek: ' + ' Hahaha ' + event.error;");
            client.println("        // Wait la! ");
            client.println("        setTimeout(startListening, 1000);");
            client.println("    };");
            client.println("}");

            client.println("startListening();");

            client.println("function updateIcons() {");
            client.println("    document.getElementById('light-icon').innerHTML = lightState ? '<img src=\"https://img.icons8.com/?size=100&id=19209&format=png&color=000000\" width=\"100\"/>' : '<img src=\"https://img.icons8.com/?size=100&id=zu4XG69PVx2m&format=png&color=000000\" width=\"100\"/>';"); 
            client.println("    document.getElementById('door-icon').innerHTML = doorState ? '<img src=\"https://img.icons8.com/?size=100&id=113116&format=png&color=000000\" width=\"100\"/>' : '<img src=\"https://img.icons8.com/?size=100&id=113120&format=png&color=000000\" width=\"100\"/>';"); 
            client.println("}"); 
            client.println("updateIcons();");

            client.println("</script>");

            client.println("</div>");
            client.println("</body></html>");

            client.println();

            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        // New routes for light and door control with specific actions
        if (currentLine.endsWith("GET /toggleLightOn")) {
          lightState = true;
          digitalWrite(ledPin, HIGH);
        }
        if (currentLine.endsWith("GET /toggleLightOff")) {
          lightState = false;
          digitalWrite(ledPin, LOW);
        }
        if (currentLine.endsWith("GET /openDoor")) {
          doorState = true;
          doorServo.write(90);
        }
        if (currentLine.endsWith("GET /closeDoor")) {
          doorState = false;
          doorServo.write(0);
        }
      }
    }

    client.stop();
    Serial.println("Client Disconnected.");
  }
}
