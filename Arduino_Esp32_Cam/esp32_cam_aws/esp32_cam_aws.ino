
#include "secrets.h"
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "WiFi.h"
#include "esp_camera.h"
#include <base64.h>
#include <string.h>
#include <iot.h>

#define BUTTON 15
#define LOCK 12

#define CAMERA_MODEL_AI_THINKER
#define CAMERA_MODULE_OV2640

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

#define LED_PIN           33 // Status led
#define LED_ON           LOW // - Pin is inverted.
#define LED_OFF         HIGH //
#define LAMP_PIN           4 // LED FloodLamp.

const char *topic_subscribe = "esp32/sub";

camera_fb_t *fb;
WiFiClientSecure secureClient = WiFiClientSecure();
PubSubClient mqttClient(secureClient);
IOT iotclient(secureClient, mqttClient);

// Status and illumination LED's
bool get_data(void)
{
    fb = esp_camera_fb_get();
    if (!fb)
    {
        Serial.println("Failed to get image");
        return false;
    }
    else
    {
        esp_camera_fb_return(fb);
        return true;
    }
}
// Hàm xử lý khi có dữ liệu nhận được
void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);

  // Chuyển payload từ mảng byte sang chuỗi
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Payload: ");
  Serial.println(message);

  // Parse JSON payload
  StaticJsonDocument<200> doc; // Tùy chỉnh kích thước nếu JSON lớn hơn
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  // Truy xuất các giá trị trong JSON
  const char* action = doc["action"]; // Truy cập giá trị của khóa "action"
  Serial.print("Action: ");
  Serial.println(action);

  // So sánh giá trị và thực hiện hành động
  if (strcmp(action, "open_door") == 0) {
    digitalWrite(LOCK, HIGH); // Mở khóa
    delay(1000);              // Giữ mở trong 100ms
    digitalWrite(LOCK, LOW); // Đóng khóa
   } else {
    Serial.println("Invalid action!");
   }
}

void setup()
{
    Serial.begin(115200);
    Serial.setDebugOutput(true);
    pinMode(BUTTON,INPUT_PULLUP);
    pinMode(LOCK, OUTPUT);

    digitalWrite(LOCK, LOW);


    camera_config_t config;
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
    config.pixel_format = PIXFORMAT_JPEG;
    config.frame_size = FRAMESIZE_QVGA;
    config.jpeg_quality = 5;
    config.fb_count = 1;
    config.ledc_timer = LEDC_TIMER_0;

    // camera init
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK)
    {
        Serial.printf("Camera init failed with error 0x%x", err);
        return;
    }
    sensor_t *s = esp_camera_sensor_get();
    //initial sensors are flipped vertically and colors are a bit saturated
    if (s->id.PID == OV3660_PID)
    {
        s->set_vflip(s, 1);       //flip it back
        s->set_brightness(s, 1);  //up the blightness just a bit
        s->set_saturation(s, -2); //lower the saturation
    }
    s->set_framesize(s, FRAMESIZE_QVGA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.println("Failed to connect to WiFi");
        sleep(5);
    }
    Serial.print("Connected to WiFi with IP address ");
    Serial.println(WiFi.localIP());
    iotclient.setup();
    iotclient.print_on_publish(true);
    mqttClient.subscribe(topic_subscribe);
    mqttClient.setCallback(mqttCallback);
}

void loop()
{
  mqttClient.loop();
  if(digitalRead(BUTTON) == LOW){
    Serial.print("The button was press");
      if (get_data()){
        if (iotclient.publish("esp32/pub", fb->buf, fb->len))
        {
            Serial.println(String("Successfully posted ") + String(fb->len) + String(" bytes"));
        }
        else
        {
            Serial.println(String("Failed to post to MQTT"));
        }
        fb = NULL;
    } 
  }

}
