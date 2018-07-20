#include <MFRC522.h> //library responsible for communicating with the module RFID-RC522
#include <SPI.h> //library responsible for communicating of SPI bus
#include <WiFi.h>// library responsible for wifi connection
#include <AfricasTalkingCloud.h>//library responsible for mqtt connection
#define SS_PIN    21
#define RST_PIN   22

//used in authentication
MFRC522::MIFARE_Key key;
//authentication return status code
MFRC522::StatusCode status;
// Defined pins to module RC522
MFRC522 mfrc522(SS_PIN, RST_PIN); 

String payload,holder; //

//Topics
const char request_topic[]= "topic";
const char verification_topic[] = "Verification topic";
const char birth_topic[] = "birth_topic";

//wifi
WiFiClient client32;
const char* ssid     = "ssid";
const char* password = "key";
void subMsgCallBack(char* topic, String payload, unsigned int length);
AfricasTalkingCloudClient client(subMsgCallBack, client32);
uint8_t qos = 1;

//MQTT
const char mqtt_username[] = "username";
const char mqtt_passowrd[] = "pass";
const char device_id[] = "ESP32";

void connectToNetwork();
void _keepAlive();
char* stringToChar(String str);

void setup() 
{
  Serial.begin(9600);
  SPI.begin(); // Init SPI bus
  

// Init MFRC522
  mfrc522.PCD_Init(); 
 
    
 //init Wifi connection
 while (!Serial)
  {
    ;
  }
  connectToNetwork();

  }
  
void loop()
{
  if (!client.connected())
  {
    _keepAlive();
  }
  if (!client.loop())
  {
    Serial.println(F("Disconnected..."));
    if (client.connect("client", "username", "pass"))
    {
      Serial.println(F("Reconnected..."));
    }
    else {
      Serial.println(F("Failed Reconnection..."));
    }
  }

    
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  Serial.print("UID tag :");
  String content= "";
  byte letter;


  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
//initilize arrays
char * uid[]={"A8 5E E9 27","C4 93 4A 49","B6 90 2E AC"};
char * uid0[]={"A8 5E E9 27","phone_no","amount"};
char * uid1[]={"C4 93 4A 49","phonr_no","amount"};
char * uid2[]={"B6 90 2E AC","phone_no","amount"};
//x == no. of elements in array uid
int x = 0;
// loop sorts through element contents
while(x<3)
{
  holder = uid[x];
    // if statement checks if content is same as defined uid tags in the array
  if(content.substring(1) == uid[x]){
    if(x==0)
    {
    payload = "";
    payload = payload + uid0[1] +", " + uid0[2];
    char* _payload = stringToChar(payload);
    client.publish("topic", _payload, qos);
    
   }
    else if(x==1){
    payload = "";
    payload = payload + uid1[1] +", " + uid1[2];
    char* _payload = stringToChar(payload);
    client.publish("topic", _payload, qos);
    }
    else if (x==2){
    payload = "";
    payload = payload + uid2[1] +", " + uid2[2];
    char* _payload = stringToChar(payload);
    client.publish("topic", _payload, qos);
     }
    Serial.println("Processing Payment...");
    Serial.println(payload); 
    delay(3000);
    break;
    // break statement can be added to terminate loop if uid is found 
    }
    else{
        // second if statement checks if all array contents have been sorted and checked
        if(x>=2){
        Serial.println("Payment Declined!");
        delay(3000);
        }
        x++; // increment of while loop
        }

}
}    

//connect to wifi

void connectToNetwork()
{
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected");
    //Serial.println("IP address: ");
    //Serial.println(WiFi.localIP());
    
}

void subMsgCallBack(char* topic, String payload, unsigned int length)
{
  Serial.print(F("Message arrived "));
  Serial.print(topic);
  Serial.print(F("] "));
}

void _keepAlive()
{
  while (!client.connected() && !client.loop()) {
    Serial.println(F("Attempting MQTT Connection"));
    if (client.connect("client", "username", "pass")) {
      Serial.println("connected");
      Serial.println("Approach your reader card...");
      client.publish("birth_topic", "birth");
      //client.subscribe("topic"); 
    }
    else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 7 seconds");
      delay(7000);
    }

}}

char* stringToChar(String str)
{
  if(str.length()!=0){
    char *s = const_cast<char*>(str.c_str());
    return s;
    }
}


