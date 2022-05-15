#ifndef  _ESP32_CAM_
#define _ESP32_CAM_
#include "Arduino.h"
#include "esp_camera.h"
#include "FS.h"                // SD Card ESP32
#include "SD_MMC.h"            // SD Card ESP32
#include "soc/soc.h"           // Disable brownout problems
#include "soc/rtc_cntl_reg.h"  // Disable brownout problems
#include "driver/rtc_io.h"
#include "WiFi.h"
#include <ESPAsyncWebServer.h>
#include <StringArray.h>
#include <SPIFFS.h>
#include <FS.h>
/*
After downloading the library, you can go to 
Sketch > Include Library > Add .ZIP library… and select the library
OR:
Move the downloaded unzipped folders to your Arduino IDE installation libraries folder
*/
#define LED 2
#define RED_LED 33
#define FLASH_LED 4

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
/*
FRAMESIZE_UXGA (1600 x 1200)
FRAMESIZE_QVGA (320 x 240)
FRAMESIZE_CIF (352 x 288)
FRAMESIZE_VGA (640 x 480)
FRAMESIZE_SVGA (800 x 600)
FRAMESIZE_XGA (1024 x 768)
FRAMESIZE_SXGA (1280 x 1024)
*/
class ESP32Cam 
{
  public:
  ESP32Cam(void);
  ~ESP32Cam(void);
  void InitCamera(pixformat_t pixFormat);
  void TakePhoto(void);
  void SavePhoto(String filename);
  void ReleasePhoto(void);
  void RedLED (int state); // 0 = off, 1 = on
  void FlashLED (int state); // 0 = off, 1 = on
  void ConnectToWiFi (String SSID, String password);
  byte* GetDataBuffer(int plane);
  protected:
  camera_config_t m_config;
  camera_fb_t  *m_frame;

};
#endif
