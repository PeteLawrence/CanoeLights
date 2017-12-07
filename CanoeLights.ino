#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Adafruit_NeoPixel.h>

/* Set these to your desired credentials. */
const char *ssid = "CanoeLights";
const char *password = "eaglecanoe";

ESP8266WebServer server(80);

#define NEOPIXEL_PIN D4
#define NEOPIXEL_LEDS 150

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NEOPIXEL_LEDS, D4, NEO_GRB + NEO_KHZ800);

uint32_t Black = strip.Color(0,0,0);
uint32_t Red = strip.Color(255,3,3);
uint32_t Green = strip.Color(0,200,0);
uint32_t Blue = strip.Color(0,0,200);
uint32_t Yellow = strip.Color(255,200,0);
uint32_t Orange = strip.Color(255,40,0);
uint32_t Purple = strip.Color(200,0,211);
uint32_t White = strip.Color(255,255,255);

String mode = "navlights";
String colour = "red";
int brightness = 25;
int staticNth = 1;
int delayTime = 250;
int chaseOffset = 0;

/* Just a little test message.  Go to http://192.168.4.1 in a web browser
 * connected to this access point to see it.
 */
void handleRoot() {
  //Print out details about the request
  Serial.println("Request:");
  Serial.println("- " + server.uri());
  for (uint8_t i=0; i<server.args(); i++){
    Serial.println("- " + server.argName(i) + ": " + server.arg(i));
  }

  //Handle the request
  if (server.hasArg("mode")) {
    mode = server.arg("mode");
  }
  if (server.hasArg("colour")) {
    colour = server.arg("colour");
  }
  if (server.hasArg("brightness")) {
    brightness = server.arg("brightness").toInt();
  }
  if (server.hasArg("n")) {
    staticNth = server.arg("n").toInt();
  }
  if (server.hasArg("speed")) {
    delayTime = server.arg("speed").toInt();
  }


  //Serve the page
  server.send(200, "text/html", "<html>\
      <head>\
        <title>Canoe Lights</title>\
        <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\
        <style>\
          body {font-family: -apple-system, BlinkMacSystemFont, \"Segoe UI\", Roboto, Helvetica, Arial, sans-serif, \"Apple Color Emoji\", \"Segoe UI Emoji\", \"Segoe UI Symbol\";}\
        </style>\
       </head>\
       <body>\
         <h1>Canoe Lights</h1>\
         <h2>Mode</h2>\
         <ul>\
           <li><a href=\"/?mode=off\">Off</a></li>\
           <li>Static: (<a href=\"/?mode=static&n=1\">1</a> | <a href=\"/?mode=static&n=2\">2</a> | <a href=\"/?mode=static&n=3\">3</a> | <a href=\"/?mode=static&n=4\">4</a> | <a href=\"/?mode=static&n=5\">5</a>)</li>\
           <li>Twinkle: (<a href=\"/?mode=twinkle&n=25\">25</a> | <a href=\"/?mode=twinkle&n=50\">50</a> | <a href=\"/?mode=twinkle&n=75\">75</a> | <a href=\"/?mode=twinkle&n=100\">100</a> )</li>\
           <li>Chase: (<a href=\"/?mode=chase&n=2\">2</a> | <a href=\"/?mode=chase&n=3\">3</a> | <a href=\"/?mode=chase&n=5\">5</a> | <a href=\"/?mode=chase&n=10\">10</a> | <a href=\"/?mode=chase&n=15\">15</a> | <a href=\"/?mode=chase&n=20\">20</a> | <a href=\"/?mode=chase&n=25\">25</a>)</li>\
           <li>Random: (<a href=\"/?mode=random&n=1\">1</a> | <a href=\"/?mode=random&n=2\">2</a> | <a href=\"/?mode=random&n=3\">3</a> | <a href=\"/?mode=random&n=4\">4</a> | <a href=\"/?mode=random&n=5\">5</a>)</li>\
           <li><a href=\"/?mode=linein\">Line In</a></li>\
           <li><a href=\"/?mode=navlights\">Nav Lights</a></li>\
         </ul>\
         \
         <h2>Colour</h2>\
         <a href=\"/?colour=red\">Red</a> | <a href=\"/?colour=green\">Green</a> | <a href=\"/?colour=blue\">Blue</a> | <a href=\"/?colour=yellow\">Yellow</a> | <a href=\"/?colour=orange\">Orange</a> | <a href=\"/?colour=purple\">Purple</a> | <a href=\"/?colour=white\">White</a> | <a href=\"/?colour=christmas\">Christmas</a> | <a href=\"/?colour=fire\">Fire</a> | <a href=\"/?colour=rainbow\">Rainbow</a>\
         \
         <h2>Brightness</h2>\
         <a href=\"/?brightness=10\">10%</a> | <a href=\"/?brightness=20\">20%</a> | <a href=\"/?brightness=30\">30%</a> | <a href=\"/?brightness=40\">40%</a> | <a href=\"/?brightness=50\">50%</a> | <a href=\"/?brightness=60\">60%</a> | <a href=\"/?brightness=70\">70%</a>\
         \
         <h2>Speed</h2>\
         <a href=\"/?speed=3000\">3s</a> | <a href=\"/?speed=2000\">2s</a> | <a href=\"/?speed=1000\">1s</a> | <a href=\"/?speed=500\">1/2s</a> | <a href=\"/?speed=250\">1/4</a> | <a href=\"/?speed=125\">1/8</a> | <a href=\"/?speed=63\">1/16</a> | <a href=\"/?speed=31\">1/32</a>\
       </body>\
    </html>");
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  Serial.print("Configuring access point...");
  /* You can remove the password parameter if you want the AP to be open. */
  WiFi.softAP(ssid, password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);
  server.on("/", handleRoot);
  server.begin();
  Serial.println("HTTP server started");

  strip.begin();
  strip.setBrightness(brightness);

  pinMode(A0, INPUT);
}

void loop() {
  server.handleClient();

  //Update LED's every 1/2 second
  strip.setBrightness(brightness);


  //Clear every pixel
  for(int i = 0; i < NEOPIXEL_LEDS; i++) {
    strip.setPixelColor(i, Black);
  }

  if (mode == "static") {
    for(int i = 0; i < NEOPIXEL_LEDS; i+= staticNth) {
      strip.setPixelColor(i, getColour(i));

    }
  } else if (mode == "twinkle") {
    for (int i = 0; i < NEOPIXEL_LEDS; i++) {
      int r = random(101);
      if (r >= staticNth) {
        strip.setPixelColor(i, getColour(i));
      }
    }
    strip.show();
    delay(100);
    for (int i = 0; i < NEOPIXEL_LEDS; i++) {
      strip.setPixelColor(i, Black);
    }
  } else if (mode == "chase") {
    for (int i = 0; i < NEOPIXEL_LEDS; i++) {
      if (i % staticNth == chaseOffset) {
        strip.setPixelColor(i, getColour(i));
      }

    }
    chaseOffset++;
    if (chaseOffset >= staticNth) {
      chaseOffset = 0;
    }
  }  else if (mode == "random") {
    for (int i = 0; i < NEOPIXEL_LEDS; i++) {
      int r = random(6);
      if (r >= staticNth) {
        strip.setPixelColor(i, getColour(i));
      }
    }
  } else if (mode == "linein") {
    int in = analogRead(A0);
    strip.setBrightness(in * 15);
    for(int i = 0; i < NEOPIXEL_LEDS; i+= staticNth) {
      strip.setPixelColor(i, getColour(i));
    }
  } else if (mode == "navlights") {
    for (int i = 0; i < 5; i++) {
      strip.setPixelColor(i, White);
    }
    for (int i = 70; i < 80; i++) {
      strip.setPixelColor(i, Red);
    }
    for (int i = 145; i <= 150; i++) {
      strip.setPixelColor(i, White);
    }
  }

  strip.show();
  delay(delayTime);

  Serial.println(analogRead(A0));
}


uint32_t getColour(int pos)
{
  if (colour == "red") {
    return Red;
  } else if (colour == "green") {
    return Green;
  }  else if (colour == "blue") {
    return Blue;
  } else if (colour == "yellow") {
    return Yellow;
  } else if (colour == "orange") {
    return Orange;
  } else if (colour == "purple") {
    return Purple;
  } else if (colour == "white") {
    return White;
  } else if (colour == "christmas") {
    if (pos % 2 == 0) {
      return Red;
    } else {
      return Green;
    }
  } else if (colour == "rainbow") {
    if (pos % 6 == 0) {
      return Red;
    } else if (pos % 6 == 1) {
      return Orange;
    } else if (pos % 6 == 2) {
      return Yellow;
    } else if (pos % 6 == 3) {
      return Green;
    } else if (pos % 6 == 4) {
      return Blue;
    } else if (pos % 6 == 5) { 
      return Purple;
    }
  } else if (colour == "halloween") {
    if (pos % 3 == 0) {
      return Orange;
    } else if (pos % 3 == 1) {
      return Yellow;
    } else if (pos % 3 == 2) {
      return Black;
    }
  } else if (colour == "fire") {
    if (pos % 3 == 0) {
      return Red;
    } else if (pos % 3 == 1) {
      return Orange;
    } else if (pos % 3 == 2) {
      return Yellow;
    }
  } else if (colour == "random") {
    int r = random(8);
    if (r % 7 == 0) {
      return Red;
    } else if (r % 7 == 1) {
      return Orange;
    } else if (r % 7 == 2) {
      return Yellow;
    } else if (r % 7 == 3) {
      return Green;
    } else if (r % 7 == 4) {
      return Blue;
    } else if (r % 7 == 5) {
      return Purple;
    } else if (r % 7 == 7) {
      return White;
    }
  }
}
