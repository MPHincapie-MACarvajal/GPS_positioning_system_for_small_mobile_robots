#include <WiFi.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <TinyGPS++.h>
#include <ThingSpeak.h>
#include <ESP32Servo.h>


unsigned long Time;
unsigned long VariationTime = 0;
Servo servo1;
Servo servo2;

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64

//On ESP32: GPIO - 21(SDA) – GPIO - 22(SCL)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define DISPLAY_ADDRESS 0x3C // See datasheet for Address
Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET);

#define RXD2 16
#define TXD2 17
HardwareSerial neogps(1);

TinyGPSPlus gps;

const char* ssid = "Miguel";
const char* pass = "MiguelAngel";


// We define the WiFi client that we will use
WiFiClient client;

// We define the credentials for the connection to the platform
  unsigned long channelID = 1889588;
  const char* WriteAPIKey ="PEZ2BU1EAPFAGB07";

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, pass);
  delay(2000);
    Serial.print("You are connecting to the WiFi network named: ");
    Serial.println(ssid);
    while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.print(".");
      }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

  Serial.println("GPS Installed and ready ");
  
  neogps.begin(9600, SERIAL_8N1, RXD2, TXD2);


  if(!display.begin(SSD1306_SWITCHCAPVCC, DISPLAY_ADDRESS)) {
    Serial.println(F("Failure to assign SSD1306 "));
    for(;;); // Don't proceed, loop forever
  }

  display.clearDisplay();
  display.display();
  delay(2000);   

  ThingSpeak.begin(client);
  delay(3000);
  
  servo1.attach(2, 500, 2500);
  servo2.attach(4, 500, 2500);
        Serial.println("Initialized Servo Motors ");
        delay(2000);  
}

void measure(){ 
  // GPS Latitudee and Lengthe reading
  float Length = gps.location.lng();
  float Latitude = gps.location.lat();
  float Altitude = gps.altitude.meters();  
  // We print the values obtained in the Serial monitor
  Serial.print("Recorded Length: ");
  Serial.print(Length);
  Serial.println("°");
  Serial.print("Recorded Latitude: ");
  Serial.print(Latitude);
  Serial.println("°");
  Serial.print("Recorded Altitude: ");
  Serial.print(Altitude);
  Serial.println("MASL");  
  Serial.println("-----------------------------------------");
  // order by fields according to the ThingSpeak platform.
  ThingSpeak.setField(1,Latitude);
  ThingSpeak.setField(2,Length);
  ThingSpeak.setField(3,Altitude);
}

  
void loop() { 

Time = millis();

if(Time - VariationTime >= 1000){
VariationTime = Time;

          if(Time >= 10000 && Time <= 100000){
            Serial.println("--- Front Advance ---");      
          } 
          if(Time >= 100000 && Time <= 100800){
            Serial.println("--- Change of Position ---");          
          }     
          if(Time >= 100800 && Time <= 106800){
            Serial.println("--- Front Advance ---");        
          }   
          if(Time >= 106800 && Time <= 107600){
            Serial.println("--- Change of Position ---");         
          } 
          if(Time >= 107600 && Time <= 125600){
            Serial.println("--- Front Advance ---");          
          }
          if(Time >= 125600 && Time <= 126100){
            Serial.println("--- Move Finished ---");           
          }                                             
  } 

    if(Time >= 10000 && Time <= 100000){ // Front Movement
          servo1.write(180);
          servo2.write(0);           
    }
    if(Time >= 100000 && Time <= 100800){ // Change of Position
          servo1.write(0);
          servo2.write(90);    
    }
    if(Time >= 100800 && Time <= 106800){ // Front Movement
          servo1.write(180);
          servo2.write(0);
    } 
    if(Time >= 106800 && Time <=106800){ // Change of Position
          servo1.write(0);
          servo2.write(90);    
    }
    if(Time >= 106800&& Time <= 125600){ // Front Movement
          servo1.write(180);
          servo2.write(0);          
    }
    if(Time >= 125600){ // Move Finished
      servo1.detach(); 
      servo2.detach(); 
    }   
  
boolean newData = false;
  //for (unsigned long start = millis(); millis() - start < 1000;)  
  for (unsigned long Time = millis(); millis() - Time < 1000;)
  {
    while (neogps.available())
    {
      if (gps.encode(neogps.read()))
      {
        newData = true;
      }
    }
  }
 
     
  //If newData is true
  if(newData == true)
  {
    newData = false;
    Serial.println(gps.satellites.value());
    print_speed();
  }
  else
  {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.print("No Data");
    display.display();
  }  
  
  delay(500);
  measure();
  // Connection and sending data to the platform using the credentials defined above
  ThingSpeak.writeFields(channelID,WriteAPIKey);
  // We print a sentence indicating the sending of the data
  Serial.println("Data sent to ThingSpeak!");
  //delay(500);
}


void print_speed()
{
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
       
  if (gps.location.isValid() == 1)
  {
    display.setTextSize(1);
    
    display.setCursor(25, 5);
    display.print("Lat: ");
    display.setCursor(50, 5);
    display.print(gps.location.lat(),6);

    display.setCursor(25, 20);
    display.print("Lng: ");
    display.setCursor(50, 20);
    display.print(gps.location.lng(),6);

    display.setCursor(25, 35);
    display.print("Speed: ");
    display.setCursor(65, 35);
    display.print(gps.speed.kmph());
    
    display.setTextSize(1);
    display.setCursor(0, 50);
    display.print("SAT:");
    display.setCursor(25, 50);
    display.print(gps.satellites.value());

    display.setTextSize(1);
    display.setCursor(70, 50);
    display.print("ALT:");
    display.setCursor(95, 50);
    display.print(gps.altitude.meters(), 0);

    display.display();
    
  }
  else
  {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.setTextSize(3);
    display.print("No Data");
    display.display();
  }   
  
}
