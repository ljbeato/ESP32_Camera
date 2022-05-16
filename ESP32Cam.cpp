#include "ESP32Cam.h"

void InitMicroSDCard()
{
  // Start Micro SD card
  Serial.println("Starting SD Card\n");
  if(!SD_MMC.begin()){
    Serial.println("SD Card Mount Failed\n");
    return;
  }
  uint8_t cardType = SD_MMC.cardType();
  if(cardType == CARD_NONE){
    Serial.println("No SD Card attached\n");
    return;
  }
}

//////////////////////////////////////////////////////////////////////////
//
// Constructor
//
//
ESP32Cam::ESP32Cam (void)
{
  m_frame =  NULL;
  pinMode(RED_LED, OUTPUT);
  pinMode(FLASH_LED, OUTPUT);
}
//////////////////////////////////////////////////////////////////////////
//
// Destructor
//
//
ESP32Cam::~ESP32Cam(void)
{
  ReleasePhoto (); 
  m_frame = NULL;
}
//////////////////////////////////////////////////////////////////////////
//
// ConnectToWiFi
//
//
void ESP32Cam::ConnectToWiFi(String ssid, String password)
{
  // Connect to Wi-Fi
  WiFi.begin(ssid.c_str(), password.c_str());
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  return;
}
//////////////////////////////////////////////////////////////////////////
//
// TakePhoto
//
//
void ESP32Cam::TakePhoto(void)
{
    // Take Picture with Camera
  m_frame = esp_camera_fb_get();
  // did we get a good pointer???
  if(!m_frame) 
  {
    Serial.println("Camera capture failed");
    return;
  }
  return;
}

byte* ESP32Cam::GetDataBuffer(int plane)
{
  // FIX ME  - where does each plane start????
  byte *buf;
  buf = m_frame->buf;
  return buf;
}
//////////////////////////////////////////////////////////////////////////
//
// SavePhoto
//
//
void ESP32Cam::SavePhoto(String path)
{
  // Save picture to microSD card
  Serial.printf("Image size = %d\n", m_frame->len);
  fs::FS &fs = SD_MMC; 
  File file = fs.open(path.c_str(), FILE_WRITE);
  if(!file){
    Serial.printf("Failed to open file %s in writing mode", path.c_str());
  } 
  else 
  {

    file.write(m_frame->buf, m_frame->len); // payload (image), payload length
    Serial.printf("Saved file to path: %s\n", path.c_str());
  }
  file.close();
  //ReleaseFrameBuffer (fb); 
 
}
//////////////////////////////////////////////////////////////////////////
//
// ReleasePhoto
//
//
void ESP32Cam::ReleasePhoto(void)
{
    //return the frame buffer back to the driver for reuse
  esp_camera_fb_return(m_frame);  
}
//////////////////////////////////////////////////////////////////////////
//
// RED_LED
//
//
void ESP32Cam::RedLED(int state)
{
    //return the frame buffer back to the driver for reuse
    if (state == 0)
      digitalWrite(RED_LED, LOW); 
    else
    {
      digitalWrite(RED_LED, HIGH); 
    }
}

//////////////////////////////////////////////////////////////////////////
//
// RED_LED
//
//
void ESP32Cam::FlashLED(int state)
{
    //return the frame buffer back to the driver for reuse
    if (state == 0)
      digitalWrite(FLASH_LED, LOW); 
    else
      digitalWrite(FLASH_LED, HIGH); 
}
//////////////////////////////////////////////////////////////////////////
//
// Init
//
//
void ESP32Cam::InitCamera(pixformat_t pixFormat)
{
  //camera_config_t config;
  m_config.ledc_channel = LEDC_CHANNEL_0;
  m_config.ledc_timer = LEDC_TIMER_0;
  m_config.pin_d0 = Y2_GPIO_NUM;
  m_config.pin_d1 = Y3_GPIO_NUM;
  m_config.pin_d2 = Y4_GPIO_NUM;
  m_config.pin_d3 = Y5_GPIO_NUM;
  m_config.pin_d4 = Y6_GPIO_NUM;
  m_config.pin_d5 = Y7_GPIO_NUM;
  m_config.pin_d6 = Y8_GPIO_NUM;
  m_config.pin_d7 = Y9_GPIO_NUM;
  m_config.pin_xclk = XCLK_GPIO_NUM;
  m_config.pin_pclk = PCLK_GPIO_NUM;
  m_config.pin_vsync = VSYNC_GPIO_NUM;
  m_config.pin_href = HREF_GPIO_NUM;
  m_config.pin_sscb_sda = SIOD_GPIO_NUM;
  m_config.pin_sscb_scl = SIOC_GPIO_NUM;
  m_config.pin_pwdn = PWDN_GPIO_NUM;
  m_config.pin_reset = RESET_GPIO_NUM;
  m_config.xclk_freq_hz = 20000000;
  m_config.pixel_format = pixFormat; //YUV422,GRAYSCALE,RGB565,JPEG

  
  m_config.frame_size = FRAMESIZE_QVGA;  //320x240
  m_config.jpeg_quality = 10;   //  lower is better quality, 0=best
  m_config.fb_count = 1;
 
  // Init Camera
  esp_err_t err = esp_camera_init(&m_config);
  if (err != ESP_OK){
    Serial.printf("Camera init failed with error 0x%x", err);
    return;
  }
  sensor_t * s = esp_camera_sensor_get();

   s->set_brightness(s, 0);     // -2 to 2
  s->set_contrast(s, 0);       // -2 to 2
  s->set_saturation(s, 0);     // -2 to 2
  s->set_special_effect(s, 2); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 1);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 0);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 0);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 0);       // -2 to 2
  s->set_aec_value(s, 300);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 0);        // 0 = disable , 1 = enable
  s->set_vflip(s, 0);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable 
  //InitMicroSDCard();
 
}
