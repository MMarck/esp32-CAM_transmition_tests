// Rutina simple para enviar una imagen desde la ESP32-CAM hacia un dispositivo conectado por medio de bluetooth que ejecute reciver.py

#include "esp_camera.h"
#include "BluetoothSerial.h"
#include <time.h>

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

    .pixel_format = PIXFORMAT_RGB565, //YUV422,GRAYSCALE,RGB565,JPEG
    .frame_size = FRAMESIZE_QVGA,    //QQVGA-UXGA Do not use sizes above QVGA when not JPEG

    .jpeg_quality = 63, //0-63 lower number means higher quality
    .fb_count = 1,       //if more than one, its runs in continuous mode. Use only with JPEG
    .grab_mode = CAMERA_GRAB_WHEN_EMPTY,
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

size_t cnv_buf_len;
uint8_t * cnv_buf = NULL;
char    fnsuffix[25];

void setup() {
  Serial.begin(115200);
  //iniciar conexion bluetooth
  SerialBT.begin("ESP32 Bluetooth");

  //Configuracion inicial de la camara
  if(ESP_OK != init_camera()) {
    return;
  }
     
}
 
void loop() {

  // Se toma una foto
  camera_fb_t *pic = esp_camera_fb_get();

  //se convierte la foto a JPG
  bool isConverted = frame2jpg(pic, 100, &cnv_buf, &cnv_buf_len);
  if(!isConverted){   SerialBT.println("failed to convert");  }
  
  struct timeval st, et;
  gettimeofday(&st,NULL);
  
  SerialBT.println();
  SerialBT.println("start");
  SerialBT.println(cnv_buf_len);
  SerialBT.write(cnv_buf,cnv_buf_len);
  SerialBT.flush();

  gettimeofday(&et,NULL);

  int elapsed = ((et.tv_sec - st.tv_sec) * 1000000) + (et.tv_usec - st.tv_usec);
  printf("time: %d micro seconds\n",elapsed);
  esp_camera_fb_return(pic);

}
