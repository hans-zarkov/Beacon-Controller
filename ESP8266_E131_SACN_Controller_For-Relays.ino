// ESP SACN controller for relays
// This is used to control the Beacon relay board
// Uses the ESPAsyncE131 library from ForkinEye - https://github.com/forkineye/ESPAsyncE131

#include <ESPAsyncE131.h>

#define UNIVERSE 45                         // First DMX Universe to listen for
#define UNIVERSE_COUNT 1                    // Total number of Universes to listen for, starting at UNIVERSE

// Specify your access point SSID(s) and password(s) below
// Adjust the number of entries in the square brackets based on the number of access points
// you want the system to attempt to connect to.  It will start with the first AP and cycle through until it connects
// This is useful if you have multiple AP's and you move your controller between them
String ssids[2] = {"SSID1","SSID2"};
String passphrases[2] = {"Password1","Password2"};

int ap_connect_time=50;
int ap_ptr=0;
int ap_max=1;

// ESPAsyncE131 instance with UNIVERSE_COUNT buffer slots
ESPAsyncE131 e131(UNIVERSE_COUNT);

int OutputPins[] = {12, 14};    // Pins on the controller that are connected to the relay board
int PinCount= sizeof(OutputPins) / sizeof(OutputPins[0]);

void setup() {
  Serial.begin(115200);
  delay(10);

  // Set all pins HIGH - turning off the relays
  for (byte i = 0; i < PinCount; i++) {
    pinMode(OutputPins[i], OUTPUT);
    digitalWrite(OutputPins[i], HIGH);
  }

  WiFi.mode(WIFI_STA);

  while (WiFi.status() != WL_CONNECTED) {
    int wait_count=0;
    Serial.println("");
    Serial.print(F("Connecting to "));
    Serial.print(ssids[ap_ptr]);
    WiFi.begin(ssids[ap_ptr], passphrases[ap_ptr]);

    while (WiFi.status() != WL_CONNECTED and wait_count < ap_connect_time) {
        delay(500);
        Serial.print(".");
        wait_count++;
    }
    // Set up the next AP ready to try (just incase we didn't connect)
    ap_ptr++;
    if (ap_ptr > ap_max)  ap_ptr=0;
  }

  Serial.println("");
  Serial.print(F("Connected with IP: "));
  Serial.println(WiFi.localIP());

  // Choose one to begin listening for E1.31 data
  if (e131.begin(E131_UNICAST))                               // Listen via Unicast
  //if (e131.begin(E131_MULTICAST, UNIVERSE, UNIVERSE_COUNT))   // Listen via Multicast
      Serial.println(F("Listening for data..."));
  else
      Serial.println(F("*** e131.begin failed ***"));
}

void loop() {

  // Check the e131 buffer
  if (!e131.isEmpty()) {
    // We have some data...
    e131_packet_t packet;
    e131.pull(&packet);     // Pull packet from ring buffer

    // Loop over the data items setting the pin level based on the payload values
    for (byte i = 1; i <= PinCount; i++) {

      //Serial.print("I:");
      //Serial.print(i);
      //Serial.print(" Val:");
      //Serial.println(packet.property_values[i]);

      if (packet.property_values[i]!=0) {
        digitalWrite(OutputPins[i-1], LOW);
      } else {
        digitalWrite(OutputPins[i-1], HIGH);
      }
    }
  }

}
