/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/

// Load Wi-Fi library
#include "esp_camera.h"
#include <WiFi.h>
#include <SPIFFS.h>
#include <FS.h>

// Replace with your network credentials
const char* ssid     = "ESP32-CAM";
const char* password = "123456789";

#define photo_path "/image.jpg"

// Set web server port number to 80
WiFiServer server(80);

// Variable to store the HTTP request
String header;

// Auxiliar variables to store the current output state
String output26State = "off";
String output27State = "off";

// Assign output variables to GPIO pins
const int output26 = 26;
const int output27 = 27;


/////// camera initialization  //////////
#define BOARD_ESP32CAM_AITHINKER

// ESP32Cam (AiThinker) PIN Map
#ifdef BOARD_ESP32CAM_AITHINKER
  #define CAM_PIN_PWDN 32
  #define CAM_PIN_RESET -1 //software reset will be performed
  #define CAM_PIN_XCLK 0
  #define CAM_PIN_SIOD 26
  #define CAM_PIN_SIOC 27
  #define CAM_PIN_D7 35
  #define CAM_PIN_D6 34
  #define CAM_PIN_D5 39
  #define CAM_PIN_D4 36
  #define CAM_PIN_D3 21
  #define CAM_PIN_D2 19
  #define CAM_PIN_D1 18
  #define CAM_PIN_D0 5
  #define CAM_PIN_VSYNC 25
  #define CAM_PIN_HREF 23
  #define CAM_PIN_PCLK 22
#endif


static camera_config_t camera_config = {
    .pin_pwdn = CAM_PIN_PWDN,
    .pin_reset = CAM_PIN_RESET,
    .pin_xclk = CAM_PIN_XCLK,
    .pin_sscb_sda = CAM_PIN_SIOD,
    .pin_sscb_scl = CAM_PIN_SIOC,

    .pin_d7 = CAM_PIN_D7,
    .pin_d6 = CAM_PIN_D6,
    .pin_d5 = CAM_PIN_D5,
    .pin_d4 = CAM_PIN_D4,
    .pin_d3 = CAM_PIN_D3,
    .pin_d2 = CAM_PIN_D2,
    .pin_d1 = CAM_PIN_D1,
    .pin_d0 = CAM_PIN_D0,
    .pin_vsync = CAM_PIN_VSYNC,
    .pin_href = CAM_PIN_HREF,
    .pin_pclk = CAM_PIN_PCLK,

    //XCLK 20MHz or 10MHz for OV2640 double FPS (Experimental)
    .xclk_freq_hz = 20000000,
    .ledc_timer = LEDC_TIMER_0,
    .ledc_channel = LEDC_CHANNEL_0,


    .pixel_format = PIXFORMAT_JPEG, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_SVGA,    //QVGA-UXGA Do not use sizes above QVGA when not JPEG
    .jpeg_quality = 10, //0-63 lower number means higher quality
    .fb_count = 1,       //if more than one, its runs in continuous mode. Use only with JPEG
    //.grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

static esp_err_t init_camera()
{
  //initialize the camera
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK)
  {
      Serial.println("Camera Init Failed");
      return err;
  }

  return ESP_OK;
}

camera_fb_t *pic  = NULL;
size_t sending_progress=0;
int chunk=500000;



/////// camera initialization  //////////

void setup() {
  Serial.begin(115200);
  //Initial camera configuration
  if(ESP_OK != init_camera())
  {
    return;
  }
  
  // Initialize the output variables as outputs
  pinMode(output26, OUTPUT);
  pinMode(output27, OUTPUT);
  // Set outputs to LOW
  digitalWrite(output26, LOW);
  digitalWrite(output27, LOW);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  server.begin();
}

void loop(){
  WiFiClient client = server.available();   // Listen for incoming clients

  if (client) {                             // If a new client connects,
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character

          
          //Serial.println("============start===============");
          //Serial.println(header);
          //Serial.println("=============end=============");

          
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            


            if (header.indexOf("GET /shot") >= 0) {
              Serial.println("Picture request");
              
              Serial.println("[ Taking picture ] ");
              pic = esp_camera_fb_get();
              if (!pic) {
                Serial.println("Failed");
                return;
              }

              Serial.println("[ Sending picture ]");
              client.println("HTTP/1.1 200 OK");
              client.println("Content-Type: image/jpg");
              client.printf("Content-Length: %d", pic->len);
              client.println("Content-Disposition: inline; filename=\"image.jpg\"");
              client.println("Connection: close");
              // The HTTP response ends with another blank line
              client.println();
              
              client.write(pic->buf, pic->len);
              // The HTTP response ends with another blank line
              client.println();

              esp_camera_fb_return(pic);
              // Break out of the while loop
              Serial.println("[ Picture sent ]");
              break;
            }

            
          } else { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
  }
}
