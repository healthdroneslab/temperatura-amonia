//#include "DHT.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

const char* ssid = "Redmi"; // SSID / nome da rede WI-FI que deseja se conectar
const char* password = "12345678@"; // Senha da rede WI-FI que deseja se conectar

//#define WIFI_AP "lika_0"
//#define WIFI_PASSWORD "RdD7InPv9"

#define TOKEN "dRrvmF38r7BZiAq1g5A4"

// DHT
//#define DHTPIN 4
//#define DHTTYPE DHT22

//char thingsboardServer[] = "192.168.1.132";
char thingsboardServer[] = "192.168.43.52";


// Initialize the Ethernet client object
//WiFiEspClient espClient;
WiFiClient espClient;

// Initialize DHT sensor.
//DHT dht(DHTPIN, DHTTYPE);

PubSubClient client(espClient);

SoftwareSerial soft(2, 3); // RX, TX

int status = WL_IDLE_STATUS;
unsigned long lastSend;

void InitWiFi()
{
  Serial.println("Connecting to AP ...");
  // attempt to connect to WiFi network
  WiFi.begin(ssid, password);
  while ( WiFi.status() != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network
    //WiFi.begin(ssid, password);
    delay(500);
  }
  Serial.println("Connected to AP");
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Connecting to ThingsBoard node ...");
    // Attempt to connect (clientId, username, password)
    if ( client.connect("NodeMCU", TOKEN, NULL) ) {
      Serial.println( "[DONE]" );
    } else {
      Serial.print( "[FAILED] [ rc = " );
      Serial.print( client.state() );
      Serial.println( " : retrying in 5 seconds]" );
      // Wait 5 seconds before retrying
      delay( 5000 );
    }
  }
}

void setup() {
  // initialize serial for debugging
  Serial.begin(9600);
  //dht.begin();
  InitWiFi();
  client.setServer( thingsboardServer, 1883 );
  //lastSend = 0;
}

void loop() {
  status = WiFi.status();
  if ( status != WL_CONNECTED) {
    while ( status != WL_CONNECTED) {
      Serial.print("Attempting to connect to WPA SSID: ");
      Serial.println(ssid);
      // Connect to WPA/WPA2 network
      status = WiFi.begin(ssid, password);
      Serial.println(status);
      delay(500);
    }
    Serial.println("Connected to AP");
  }

  if ( !client.connected() ) {
    reconnect();
  }

  if ( millis() - lastSend > 10000 ) { // Update and send only after 1 seconds
    getAndSendTemperatureAndHumidityData();
    lastSend = millis();
  }

  client.loop();
}

void getAndSendTemperatureAndHumidityData()
{
  Serial.println("Collecting temperature data.");
  Serial.println("Collecting ammonia data.");
  
  float a = random(10, 40);
  //float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = random(17, 30);
  //float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(a) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  Serial.print("Ammonia: ");
  Serial.print(a);
  Serial.print(" ppm ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C ");

  String temperature = String(t);
  String ammonia = String(a);


  // Just debug messages
  Serial.print( "Sending temperature and ammonia : [" );
  Serial.print( temperature ); Serial.print( "," );
  Serial.print( ammonia );
  Serial.print( "]   -> " );

  // Prepare a JSON payload string
  String payload = "{";
  payload += "\"temperature\":"; payload += temperature; payload += ",";
  payload += "\"ammonia\":"; payload += ammonia;
  payload += "}";

  // Send payload
  char attributes[100];
  payload.toCharArray( attributes, 100 );
  client.publish( "v1/devices/me/telemetry", attributes );
  Serial.println( attributes );
}