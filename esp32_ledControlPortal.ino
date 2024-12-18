// Load Wi-Fi library
#include <WiFi.h>

// Replace with your network credentials
const char* ssid     = ".....";  // Your Wi-Fi SSID (network name)
const char* password = ".........";  // Your Wi-Fi Password

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

//variables to store the current LED state
String statePin2 = "off";
// Output variable to GPIO pin
const int ledPin2 = 12;  // LED connected to D2 (GPIO 2)

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds
const long timeoutTime = 2000;

void setup() {
  Serial.begin(115200);
  
  pinMode(ledPin2, OUTPUT);      // Set the LED pin mode
  digitalWrite(ledPin2, LOW);    // Turn LED off by default

  // Connect to Wi-Fi network
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  
  // Wait until the ESP32 connects to the Wi-Fi network
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting...");
  }
  
  // Print the IP address
  Serial.println("WiFi connected.");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  // IP address assigned by the router
  
  // Start the web server
  server.begin();
}

void loop() {
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // Print a message out in the serial port
    String currentLine = "";                // Make a String to hold incoming data from the client

    while (client.connected() && currentTime - previousTime <= timeoutTime) {
      currentTime = millis();
      if (client.available()) {             // If there's bytes to read from the client,
        char c = client.read();             // Read a byte, then
        Serial.write(c);                    // Print it out the serial monitor
        header += c;
        if (c == '\n') {                    // If the byte is a newline character
          // If the current line is blank, you've got two newline characters in a row.
          // That's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Turn the GPIO on or off based on the request
            if (header.indexOf("GET /2/on") >= 0) {
              statePin2 = "on";
              digitalWrite(ledPin2, HIGH);  // Turn the LED on
            } else if (header.indexOf("GET /2/off") >= 0) {
              statePin2 = "off";
              digitalWrite(ledPin2, LOW);   // Turn the LED off
            }

            // Display the HTML web page
            client.println("<!DOCTYPE html><html>");
            client.println("<head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">");
            client.println("<link rel=\"icon\" href=\"data:,\">");
            // CSS to style the on/off buttons
            client.println("<style>html { font-family: monospace; display: inline-block; margin: 0px auto; text-align: center;}");
            client.println(".button { background-color: yellowgreen; border: none; color: white; padding: 16px 40px;");
            client.println("text-decoration: none; font-size: 32px; margin: 2px; cursor: pointer;}");
            client.println(".button2 {background-color: gray;}</style></head>");

            client.println("<body><h1>ESP32 Web Server</h1>");
            client.println("<p>Control LED State</p>");

            if (statePin2 == "off") {
              client.println("<p><a href=\"/2/on\"><button class=\"button\">ON</button></a></p>");
            } else {
              client.println("<p><a href=\"/2/off\"><button class=\"button button2\">OFF</button></a></p>");
            }
            client.println("</body></html>");

            // The HTTP response ends with another blank line
            client.println();
            // Break out of the while loop
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
