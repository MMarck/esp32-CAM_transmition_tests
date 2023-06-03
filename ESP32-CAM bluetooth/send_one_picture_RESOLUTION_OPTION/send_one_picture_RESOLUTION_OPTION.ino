// Rutina simple para enviar una imagen desde la ESP32-CAM cuando un dispositivo conectado por medio de bluetooth lo solicite

#include "esp_camera.h"
#include "BluetoothSerial.h"
#include <Arduino.h>

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

BluetoothSerial SerialBT;


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
    .frame_size = FRAMESIZE_QVGA,    //QVGA-UXGA Do not use sizes above QVGA when not JPEG
    .jpeg_quality = 100, //0-63 lower number means higher quality
    .fb_count = 1,       //if more than one, its runs in continuous mode. Use only with JPEG
    //.grab_mode = CAMERA_GRAB_WHEN_EMPTY,
};

static esp_err_t init_camera()
{
  //initialize the camera
  esp_err_t err = esp_camera_init(&camera_config);
  if (err != ESP_OK)
  {
      SerialBT.println("Camera Init Failed");
      return err;
  }

  return ESP_OK;
}

camera_fb_t *pic = NULL;
size_t sending_progress=0;
int chunk=500000;


void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param){
  if(event == ESP_SPP_SRV_OPEN_EVT){
    Serial.println("[CLIENT CONNECTED]");
  }

  if(event == ESP_SPP_DATA_IND_EVT){
    Serial.println("\n[DATA RECIVED]");
    
    
    Serial.println("Reading buffer");
    char option = SerialBT.read();

    if( option == 'r' )
    {
      sensor_t *sensor = esp_camera_sensor_get();
      char param = SerialBT.read();

      switch(param){
        case '6':
          sensor->set_framesize(sensor, FRAMESIZE_UXGA);
         Serial.println("Resolution set at FRAMESIZE_UXGA");
        break;
    
        case '5':
          sensor->set_framesize(sensor, FRAMESIZE_SXGA);
          Serial.println("Resolution set at FRAMESIZE_SXGA");
        break;
    
        case '4':
          sensor->set_framesize(sensor, FRAMESIZE_XGA);
          Serial.println("Resolution set at FRAMESIZE_XGA");
        break;
    
        case '3':
          sensor->set_framesize(sensor, FRAMESIZE_SVGA);
          Serial.println("Resolution set at FRAMESIZE_SVGA");
        break;
    
        case '2':
          sensor->set_framesize(sensor, FRAMESIZE_VGA);
          Serial.println("Resolution set at FRAMESIZE_VGA");
        break;

        case '1':
          sensor->set_framesize(sensor, FRAMESIZE_CIF);
          Serial.println("Resolution set at FRAMESIZE_CIF");
        break;

        default:
          sensor->set_framesize(sensor, FRAMESIZE_QVGA);
          Serial.println("Resolution set at FRAMESIZE_QVGA");
        break;
      }
    }
    
    if( option == 'x' )
    {
      sending_progress = 0;
      Serial.println("Start character recived, taking a picture...");
      pic = esp_camera_fb_get();

      Serial.println("Picture Taken");
      
      
    }

    if( option == 'c' )
    {
      Serial.println("Start character recived, Sending image...");

      if( pic->len > sending_progress)
      {
        Serial.printf("PROGRESS %d/%d\n", sending_progress, pic->len);

        int slice = chunk;
        if( pic->len < sending_progress + chunk)
        {
          slice = pic->len - sending_progress;
        }

      
        SerialBT.println("START");
        SerialBT.println(slice);
        SerialBT.write(pic->buf,slice);
        SerialBT.flush();
        Serial.printf("%d bits sent\n", slice);
  
        pic->buf += slice;
        sending_progress += slice;
      }
      else
      {
        SerialBT.println("END");
        sending_progress = 0;
        esp_camera_fb_return(pic);
//        pic->buf = NULL;/
//        pic->len = 0;/
        pic = NULL;
        Serial.println("No image to send");
      }
      
    }
    
  }
}

void setup() {
  
  //Setting baud rate to print on Serial Monitor 
  Serial.begin(115200);
  SerialBT.register_callback(callback);
  
  //Configuracion inicial de la camara
  if(ESP_OK != init_camera())
  {
    return;
  }

  if( !SerialBT.begin("ESP32 Bluetooth") )
  {
    Serial.println("An error occurred initializing Bluetooth");
    
  }
  else
  {
    Serial.println("Bluetooth initialized");
  }

  
     
}
 
void loop() {}
