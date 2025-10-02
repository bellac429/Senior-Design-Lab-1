#include <WiFi.h>
#include <HTTPClient.h>
#include <sstream>
 
const char* ssid = "yourNetworkName";
const char* password =  "yourNetworkName";
 
const char * messagingServiceSid = "yourMessagingServiceId";
const char * to = "destinationPhoneNumber";
const char * accountNr = "yourAccountNr;
const char * twilioPassword = "yourApiKey";
 
bool sendSMS(const char * body){
 
  std::stringstream url;
  url << "https://api.twilio.com/2010-04-01/Accounts/" << accountNr <<"/Messages";
 
  std::stringstream urlEncodedBody;
  urlEncodedBody << "MessagingServiceSid=" << messagingServiceSid << "&To=" << to << "&Body=" << body;
 
  Serial.print("\nURL: ");
  Serial.println(url.str().c_str());
  Serial.print("Encoded body: ");
  Serial.println(urlEncodedBody.str().c_str());
   
 
  HTTPClient http;
 
  http.begin(url.str().c_str());
  http.addHeader("Content-Type", "application/x-www-form-urlencoded");
  http.setAuthorization(accountNr, twilioPassword);
   
  int httpCode = http.POST(urlEncodedBody.str().c_str());                                               
  
  if (httpCode > 0) {
 
      String payload = http.getString();
 
      Serial.print("\nHTTP code: ");
      Serial.println(httpCode);
 
      Serial.print("\nResponse: ");
      Serial.println(payload);
    }
 
  else {
    Serial.println("Error on HTTP request:");
    Serial.println(httpCode);
  }
 
  http.end();
 
  return httpCode == 201;
 
}
 
void setup() {
  
  Serial.begin(115200);
 
  WiFi.begin(ssid, password); 
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }
  
  Serial.println("Connected to the WiFi network");
 
  bool result = sendSMS("Test SMS from ESP32");
 
  if(result){
    Serial.println("\nSMS sent");
  }else{
    Serial.println("\nSMS failed");
  }
 
}
  
 
void loop() { }