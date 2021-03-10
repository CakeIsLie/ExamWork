#include <SoftwareSerial.h>
#include "WiFiEsp.h"
#include "SoftwareSerial.h"
#include <MySQL_Connection.h>
#include <MySQL_Cursor.h>

//SQL stuff
char ssid[] = " "; //Wi-Fi name
char pass[] = " "; //Wi-Fi password
int status = WL_IDLE_STATUS;
WiFiEspServer server(80);
WiFiEspClient client;
MySQL_Connection conn((Client *)&client);
IPAddress server_addr(0,0,0,0); //IP adress
char user[] = " "; // MySQL user
char password[] = " "; // MySQL password

//PIRsensor
int inputPin = 2; //choose the input pin (for PIR sensor)
int pirState = LOW; //we start thinking no motion detected
int val = 0; //variable for reading the pin status

//WIFI
SoftwareSerial SerialWifi(9, 8); //TX, RX
int movment = 0; //counting how many times PIR sensor have gone off
unsigned long updateInterval = 60000; 
unsigned long timeNow = 0;

void setup() {
  //Wi-Fi module stuff
    Serial.begin(115200); //have to be on 115200 to see if it conected
  SerialWifi.begin(9600);
  WiFi.init(&SerialWifi);
  
  Serial.println(F("Initialising connection"));
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, pass);
  if (WiFi.status() == WL_NO_SHIELD) {
    Serial.println("WiFi shield not present");
    while (true);
  }
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);

// Print your WiFi shield's IP address
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

    while (conn.connect(server_addr, 3306, user, password) != true)  //waiting on a conection to MySQL-server
  {
    delay(200);
    Serial.print ( "." );
  }

  Serial.println("");
  Serial.println("Connected to SQL Server!"); 
    server.begin();
  }
pinMode(inputPin, INPUT); //declare sensor as input
Serial.begin(9600);
}

void loop() {
 PIRSensor(); //what my PIRsensor do

 if(millis() > timeNow + updateInterval){
 ToMySQL(); //my MySQL server

 timeNow = millis();
 }
 else{Serial.println("Error");}
}

void PIRSensor()
{
 pirState = digitalRead(inputPin);
 if (pirState == HIGH){
  movment++;
  }
 }
void ToMySQL() //my MySQL
{
  Serial.println(F("Recording data."));
  MySQL_Cursor *cur_mem = new MySQL_Cursor(&conn);
  //Inserting sensordata
  String query = "INSERT INTO `exjobb`.`datastore`(`CountData`)VALUES (\" String (movment) \")";//Send the data to my MySQL

  Serial.println(query);
  char *q = query.c_str();
  cur_mem->execute(q);
  delete cur_mem;
  Serial.println(F("Recorded."));
}
  
