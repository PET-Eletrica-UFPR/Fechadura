#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <SoftwareSerial.h>

#define GP0 0
#define GP1 1 // TXD
#define GP2 2
#define GP3 3 // RXD

char *ssid = "Claro_2G97B71F";
char *password = "E297B71F";

const char *mqttServer = "pet.eletrica.ufpr.br";
const int mqttPort = 2283;
const char *mqttUser = "iopet";
const char *mqttPassword = "pet";

SoftwareSerial BLE_serial = SoftwareSerial(GP3, GP1); // rxPin = 3 , txPin = 1

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length)
{

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.println("Message:");
  for (int i = 0; i < length; i++)
  {
    Serial.println((char)payload[i]);

    if ((char)payload[i] == '1')
    {
      digitalWrite(GP2, LOW); // turn the LED on (HIGH is the voltage level)
      delay(3000);
      digitalWrite(GP2, HIGH); // turn the LED on (HIGH is the voltage level)
    }
  }
}

void setup()
{
  Serial.begin(115200);
  BLE_serial.begin(115200); // Terminal BLE
  Serial.println("Configurar módulo BLE1010 Soft com comandos AT");
  Serial.println("Digite AT e observe a resposta OK\n");

  WiFi.begin(ssid, password);

  pinMode(GP2, OUTPUT);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("Connected to the WiFi network");

  client.setServer(mqttServer, mqttPort);
  client.setCallback(callback);

  while (!client.connected())
  {
    Serial.println("Connecting to MQTT...");

    if (client.connect("ESP32Client", mqttUser, mqttPassword))
    {

      Serial.println("connected");
    }
    else
    {

      Serial.print("failed with state ");
      Serial.print(client.state());
      delay(2000);
    }
  }

  client.subscribe("Fechadura");
}

void loop()
{
  client.loop();
  // Alimenta dados do BLE para o Monitor Serial do Arduino
  if (BLE_serial.available())
  {
    int Mensagem = BLE_serial.read();
    if (Mensagem == 1)
    {
      digitalWrite(0, LOW); // turn the LED on (HIGH is the voltage level)
      delay(3000);
      digitalWrite(0, HIGH); // turn the LED on (HIGH is the voltage level)
    }
  }
  // Alimenta dados do Monitor Serial do Arduino para o BLE
  if (Serial.available())
  {
    BLE_serial.write(Serial.read());
  }
