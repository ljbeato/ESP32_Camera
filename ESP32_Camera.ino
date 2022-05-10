#include "ESP32Cam.h"


//
// WARNING!!! PSRAM IC required for UXGA resolution and high JPEG quality
//            Ensure ESP32 Wrover Module or other board with PSRAM is selected
//            Partial images will be transmitted if image exceeds buffer size
//


// Keep track of number of pictures
unsigned int pictureNumber = 0;
char temo[64];
//Stores the camera configuration parameters
ESP32Cam  cam;


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
  Serial.println("Ok!");

  cam.Init();
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
  cam.Init();
  Serial.println("Taking Photo");
  cam.TakePhoto();
  // replace this with your own function
  // process_image(fb->width, fb->height, fb->format, fb->buf, fb->len);
  String pathJPG = "/picture" + String("JPG-") + String(pictureNumber) +".jpg";
  String pathBMP = "/picture" + String("BMP-") + String(pictureNumber) +".jpg";
  Serial.println("Saving Photo");
  cam.SavePhoto(pathJPG);
  //SaveLine(frame, pathBMP);
  // release frame buffer
  cam.ReleaseFrameBuffer();
  pictureNumber++;
  

}
