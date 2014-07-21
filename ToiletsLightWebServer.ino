/*
  Theodo Toilet
 
 recreated 4th July 2014
 by Benjamin Grandfond, Fabrice Bernhard
 */

#include <Ethernet.h>
#include <SPI.h>

// assign a MAC address for the ethernet controller.
// fill in your address here:
byte mac[] = { 
  0xDE, 0xED, 0xBE, 0xEF, 0xFE, 0xED};
// assign an IP address for the controller:
IPAddress ip(192,168,102,13);
IPAddress gateway(192,168,102,1);	
IPAddress subnet(255, 255, 255, 0);


// Initialize the Ethernet server library
// with the IP address and port you want to use 
// (port 80 is default for HTTP):
EthernetServer server(80);

int sensorPin = A0;    // select the input pin for the potentiometer
long light = 0;
boolean occupationStatus = false;
long lastReadingTime = 0;
long lastChangeTime = millis();
boolean lastOccupationStatus = false;

void setup() {

  // start the Ethernet connection and the server:
  Ethernet.begin(mac, ip);
  server.begin();

  Serial.begin(9600);

  // give the sensor and Ethernet shield time to set up:
  delay(1000);

}

void loop() { 
  // check for a reading no more than once a second.
  if (millis() - lastReadingTime > 1000){
    // if there's a reading ready, read it:
    // don't do anything until the data ready pin is high:
    light = analogRead(sensorPin) / 3;
    delay(200);
    light += analogRead(sensorPin) / 3;
    delay(200);
    light += analogRead(sensorPin) / 3;
    
    occupationStatus = (light >= 900);
    if (lastOccupationStatus != occupationStatus) {
      lastChangeTime = millis();
      lastOccupationStatus = occupationStatus;
    }
    Serial.print(light);
    Serial.println(" V");
    // timestamp the last time you got a reading:
    lastReadingTime = millis();
    
  }

  // listen for incoming Ethernet connections:
  listenForEthernetClients();
}


void listenForEthernetClients() {
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("Got a client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: application/json");
          client.println();
          // print the current readings, in HTML format:
          client.print("[{\"id\": 1, \"type\": \"men\", \"value\": ");
          client.print(light);
          client.print(", \"occupationStatus\": ");
          if (occupationStatus) {
            client.print("true");
          } else {
            client.print("false");
          }
          client.print(", \"time\": ");
          client.print(lastReadingTime);
          client.print(", \"since\": ");
          client.print(lastChangeTime);
              
          client.println("}]");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } 
        else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
  }
} 



