#include <ESP8266WiFi.h>

// Define the local Wifi network to connect to
const char* ssid="XXXX";
const char* password="XXXX";

// Define what relay performs Open or Close action
const byte RelayOpen  = 0;
const byte RelayClose = 1;

WiFiServer server(80);

void setup() {
  // Setup code
 
  Serial.begin(115200);
  Serial.println();
  Serial.print("Wifi connecting to: ");
  Serial.println( ssid );
  
  WiFi.begin(ssid,password);

  // Hide SSID from nodemcu module
  WiFi.mode(WIFI_STA);
 
  Serial.println();
  Serial.print("Connecting");

  // config static IP
  IPAddress ip(XX, XX, XX, XX); // where xx is the desired IP Address
  IPAddress gateway(XX, XX, XX, X); // set gateway to match your network
  IPAddress subnet(255, 255, 255, 0); // set subnet mask to match your network
  WiFi.config(ip, gateway, subnet);
    
  while( WiFi.status() != WL_CONNECTED ){
      delay(500);
      Serial.print(".");
  }

  if( WiFi.status() == WL_CONNECTED ){
      Serial.println(); 
      Serial.print("Connected to: ");
      Serial.println( ssid );
      Serial.println(WiFi.localIP());
  }

  server.begin();         // start the web server on port 80
  printWifiStatus();      // you're connected now, so print out the status
}

void loop() {
  // Main code here, runs repeatedly

  WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();

            // the content of the HTTP response follows the header:
            client.print("<b>To Open or Close gate</b><br>");
            client.print("Click <a href=\"/opengate\">here</a> Open gate<br>");
            client.print("Click <a href=\"/closegate\">here</a> Close gate<br>");

            // The HTTP response ends with another blank line:
            client.println();
            
            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        // Check to see if the client request was "GET /opengate" or "GET /closegate":
        if (currentLine.endsWith("GET /opengate")) {
          switchRelay(RelayOpen,1);
          delay(1000);
          switchRelay(RelayOpen,0);
        }
        if (currentLine.endsWith("GET /closegate")) {
          switchRelay(RelayClose,1);
          delay(1000);
          switchRelay(RelayClose,0);
        }
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disonnected");
  }
}

void printWifiStatus() {
  // print the SSID of the network you're attached to
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi shield's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}

void switchRelay(int relayNum, int relayState) {
    // Switch Relay to open or close the gate
    
    Serial.write(0xA0);
    Serial.write((0x01 + relayNum));
    Serial.write((0x00 + relayState));
    Serial.write((0xA1 + relayState + relayNum));
    Serial.flush();
    delay(1);
}
