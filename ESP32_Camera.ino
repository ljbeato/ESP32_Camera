#include "esp_camera.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"

#define LED 2
//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//

// Select camera model
#define CAMERA_MODEL_AI_THINKER // Has PSRAM
// Pin definition for CAMERA_MODEL_AI_THINKER
// Change pin definition if you're using another ESP32 with camera module
#define PWDN_GPIO_NUM     32
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM      0
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       21
#define Y4_GPIO_NUM       19
#define Y3_GPIO_NUM       18
#define Y2_GPIO_NUM        5
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Keep track of number of pictures
unsigned int pictureNumber = 0;
char temo[64];
//Stores the camera configuration parameters
camera_config_t config;

//////////////////////////////////////////////////////////////////
//
// configInitCamera
//
//////////////////////////////////////////////////////////////////
void configInitCamera()
{
  config.ledc_channel = LEDC_CHANNEL_0;
  config.ledc_timer = LEDC_TIMER_0;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.pixel_format = PIXFORMAT_JPEG; //YUV422,GRAYSCALE,RGB565,JPEG

  // Select lower framesize if the camera doesn't support PSRAM
  if(psramFound())
  {
    Serial.print("psram found...");
    config.frame_size = FRAMESIZE_QVGA; // FRAMESIZE_ + QVGA|CIF|VGA|SVGA|XGA|SXGA|UXGA
    config.jpeg_quality = 1; //10-63 lower number means higher quality
    config.fb_count = 2;
  } 
  else 
  {
    Serial.print("psram NOT found...");
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 12;
    config.fb_count = 1;
  }
  
  // Initialize the Camera
  esp_err_t err = esp_camera_init(&config);
  if (err != ESP_OK) {
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }

  sensor_t * s = esp_camera_sensor_get();
  s->set_brightness(s, 1);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 0);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 300);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 5);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable
}
//////////////////////////////////////////////////////////////////
//
// initMicroSDCard
//
//////////////////////////////////////////////////////////////////
void initMicroSDCard(){
  // Start Micro SD card
  Serial.println("Starting SD Card");
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached");
    return;
  }
}
//////////////////////////////////////////////////////////////////
//
// TakePhoto
//
//////////////////////////////////////////////////////////////////
camera_fb_t* TakePhoto(void)
{
  // Take Picture with Camera
  camera_fb_t  *fb = esp_camera_fb_get();

  if(!fb) 
  {
    Serial.println("Camera capture failed");
    return NULL;
  }
  return fb;

}
//////////////////////////////////////////////////////////////////
//
// SavePhoto
//
//////////////////////////////////////////////////////////////////
void SavePhoto(camera_fb_t  *fb, String path){
  // Save picture to microSD card
  fs::FS &fs = SD_MMC; 
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
  } 
  else 
  {
    file.write(fb->buf, fb->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
  }
  file.close();
  //ReleaseFrameBuffer (fb); 
 
}

//////////////////////////////////////////////////////////////////
//
// SaveLine
//
//////////////////////////////////////////////////////////////////
void SaveLine(camera_fb_t  *fb, String path){
  // Save picture to microSD card
  fs::FS &fs = SD_MMC; 
  uint8_t  lineArray[320];
  for (int i = 0; i < 320; i++)
  // to get the center line, 320x240, want line 120, so multiply by 320x120
  {
    // save the center line
    lineArray[i] = fb->buf[320*160 + i];
  }
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.println("Failed to open file in writing mode");
  } 
  else 
  {
    file.write(lineArray, 320); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
  }
  file.close();
  //ReleaseFrameBuffer (fb); 
 
}
/////////////////////////////////////////////////////////////////////
//
// ReleaseFrameBuffer
//
/////////////////////////////////////////////////////////////////////
void ReleaseFrameBuffer(camera_fb_t  *fb)
{
  //return the frame buffer back to the driver for reuse
  esp_camera_fb_return(fb);  
}

/////////////////////////////////////////////////////////////////////
//
// setup
//
/////////////////////////////////////////////////////////////////////
void setup() 
{
  pinMode(LED,OUTPUT);
  // put your setup code here, to run once:
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println();

  //Initialize the camera  
  Serial.print("Initializing the camera module...");
  configInitCamera();
  Serial.println("Ok!");
 
  //Initialize MicroSD
  Serial.print("Initializing the MicroSD card module... ");
  initMicroSDCard(); 

}
/////////////////////////////////////////////////////////////////////
//
// loop
//
/////////////////////////////////////////////////////////////////////
void loop() 
{
  // put your main code here, to run repeatedly:
  //digitalWrite(LED,LOW);  
  delay(500);
  //digitalWrite(LED,HIGH);  
  // take a photo
  Serial.println("Taking Photo");
  camera_fb_t *frame = TakePhoto();
  // replace this with your own function
  // process_image(fb->width, fb->height, fb->format, fb->buf, fb->len);
  String pathJPG = "/picture" + String("JPG-") + String(pictureNumber) +".jpg";
  String pathBMP = "/picture" + String("BMP-") + String(pictureNumber) +".jpg";
  Serial.println("Saving Photo");
  SavePhoto(frame, pathJPG);
  SaveLine(frame, pathBMP);
  // release frame buffer
  ReleaseFrameBuffer(frame);
  pictureNumber++;
  

}
