#include <RGBConverter.h>

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#define RGB_CHANNELS 3
#define RELAY_PIN 16

struct{
  int r;
  int g;
  int b;
  int colormode;
  int lifetime;
} typedef RGBmode;

enum pelettes {
  STATIC,
  RAINBOW,
  RANDOM, 
  OFF,
};

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
RGBmode rgb[RGB_CHANNELS];
int *temp_rgb = new int[3];
double *rainbow_hue=new double[RGB_CHANNELS];
int relay_power= HIGH;
RGBConverter colorConverter;
uint16_t i=0;
size_t cycles=0;



const char* ssid = "CBL";
const char* password = "cxik-4b3d-pcrb";

ESP8266WebServer server(80);

const int led = 13;

void changeColor(){
  for(int i=0; i<RGB_CHANNELS; i++ ){
    if(rgb[i].lifetime >0 && rgb[i].colormode == STATIC){
      pwm.setPWM(i*3, 0, (rgb[i].r) );
      pwm.setPWM(i*3+1, 0, (rgb[i].g) );
      pwm.setPWM(i*3+2, 0, (rgb[i].b) );
      rgb[i].lifetime--;
    }
    else if(rgb[i].colormode == RANDOM && cycles % rgb[i].lifetime ==0){
      double h = (double)random(0,1000) /1000;
      colorConverter.hslToRgb(h,1.0,0.5,temp_rgb);
      
      pwm.setPWM(i*3, 0, (temp_rgb[0]) );
      pwm.setPWM(i*3+1, 0, (temp_rgb[1]) );
      pwm.setPWM(i*3+2, 0, (temp_rgb[2]) );
    }
    
    else if(rgb[i].colormode == RAINBOW && cycles % rgb[i].lifetime ==0){
      if(rainbow_hue[i] >1.0) rainbow_hue[i] = 0; 
      else rainbow_hue[i] = (rainbow_hue[i]+0.001) ;
      colorConverter.hslToRgb(rainbow_hue[i],1.0,0.5,temp_rgb);
      
      pwm.setPWM(i*3, 0, (temp_rgb[0]) );
      pwm.setPWM(i*3+1, 0, (temp_rgb[1]) );
      pwm.setPWM(i*3+2, 0, (temp_rgb[2]) );
    }    
  }
}

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/plain", "hello from esp8266!");
  Serial.println(server.arg("mode"));
  digitalWrite(led, 0);
}
void handleOff() {
  server.send(200);
  relay_power=HIGH;
  for(int i=0; i<16; i++ ){
    pwm.setPWM(i, 0, 0 );
  }
   
}

void handleFixedRGB(){
  server.send(200);
  relay_power=LOW;
  int r = (server.arg("r")).toInt() % 4096;
  int g = (server.arg("g")).toInt() % 4096;
  int b = (server.arg("b")).toInt() % 4096;

  int ledid = server.arg("ledid").toInt();
  int i;
  if(ledid == -1){
    i=0;
  }
  else{
    i=ledid;
  }
  for(; i<RGB_CHANNELS; i++ ){
    rgb[i].r=r;
    rgb[i].g=g;
    rgb[i].b=b;
    rgb[i].lifetime=1;
    rgb[i].colormode =STATIC;

    if(ledid != -1) break;
  }
}
void handleFixedHSL(){
  server.send(200);
  relay_power=LOW;
  double h = (double)(server.arg("h")).toInt() / 1000.0;
  double s = (double)(server.arg("s")).toInt() / 1000.0;
  double l = (double)(server.arg("l")).toInt() / 1000.0;

  colorConverter.hslToRgb(h,s,l,temp_rgb);
  int ledid = server.arg("ledid").toInt();
  int i;
  if(ledid == -1){
    i=0;
  }
  else{
    i=ledid;
  }
  for(; i<RGB_CHANNELS; i++ ){
    rgb[i].r=temp_rgb[0];
    rgb[i].g=temp_rgb[1];
    rgb[i].b=temp_rgb[2];
    rgb[i].lifetime=1;
    rgb[i].colormode =STATIC;

    if(ledid != -1) break;
  }
}

void handleColorMode(){
  server.send(200);
  relay_power=LOW;
  String palette = server.arg("palette");
  int cycles = server.arg("cycles").toInt();
  int ledid = server.arg("ledid").toInt();
  if(palette == "rainbow" || palette == "random"){
    int i;
    if(ledid == -1){
      i=0;
    }
    else{
      i=ledid;
    }
    for(; i<RGB_CHANNELS; i++ ){
      rgb[i].r=0;
      rgb[i].g=0;
      rgb[i].b=0;
      rgb[i].lifetime=cycles;
      if(palette == "rainbow") rgb[i].colormode =RAINBOW;
      else if(palette == "random") rgb[i].colormode =RANDOM;
      else rgb[i].colormode =STATIC;

      if(ledid != -1) break;
    }
  }
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void setup(void) {
  //RELAY
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);
  //PWM
  pwm.begin();
  pwm.setPWMFreq(1000);  // This is the maximum PWM frequency
  Wire.setClock(400000);
  //HTTP
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/fixrgb", handleFixedRGB);
  
  server.on("/fixhsl", handleFixedHSL);
  
  server.on("/mode", handleColorMode);

  server.on("/off", handleOff);
  

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}


void loop(void) {
  cycles++;
  server.handleClient();
  digitalWrite(RELAY_PIN, relay_power);
  if(relay_power == LOW) changeColor();
  #ifdef ESP8266
  yield();  // take a breather, required for ESP8266
  #endif
 
}
