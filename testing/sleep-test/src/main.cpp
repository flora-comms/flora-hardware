#include "config.h"

// Define task variables
TaskHandle_t greenTaskHandle = NULL;
TaskHandle_t blueTaskHandle = NULL;
TaskHandle_t sleepTaskHandle = NULL;

bool greenDone = false;
bool blueDone = false;

void ledController(int led_CHANNEL, int brightness, int blinks, int onTime,
                   int offTime);

void greenTask(void *pvParameters) {
  while (true) {
    if (!greenDone) {
      ledController(G_CHANNEL, 127, 3, 1000, 300);
    }
    greenDone = true;
    vTaskSuspend(NULL); // Suspend this task
  }
}

void blueTask(void *pvParameters) {
  while (true) {
    if (!blueDone) {
      ledController(B_CHANNEL, 127, 5, 1000, 500);
    }
    blueDone = true;
    vTaskSuspend(NULL); // Suspend this task
  }
}

void sleepTask(void *pvParameters) {
  while (true) {
    if (greenDone && blueDone) {
      // Go to Sleep
      Serial.println(greenDone);
      Serial.println(blueDone);
      Serial.println("going to sleep now");
      esp_light_sleep_start();
      esp_sleep_wakeup_cause_t wakeupCause = esp_sleep_get_wakeup_cause();
      Serial.println(wakeupCause);
      // wake up and reset flags then resume tasks
      greenDone = false;
      blueDone = false;
      vTaskResume(greenTaskHandle);
      vTaskResume(blueTaskHandle);
    }
    // Add a small delay to avoid unnecessary CPU usage
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  // Initialize serial communication
  Serial.begin(115200);

  // Setup RGB
  ledcSetup(R_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(R_PIN, R_CHANNEL);
  ledcSetup(G_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(G_PIN, G_CHANNEL);
  ledcSetup(B_CHANNEL, PWM_FREQ, PWM_RES);
  ledcAttachPin(B_PIN, B_CHANNEL);

  // Set up button pin as input with pull-up
  // pinMode((gpio_num_t)USER_BTN, INPUT_PULLUP);
  // gpio_wakeup_enable((gpio_num_t)USER_BTN, GPIO_INTR_LOW_LEVEL);
  // esp_sleep_enable_gpio_wakeup();

  esp_sleep_enable_ext0_wakeup((gpio_num_t)USER_BTN, 0);
  rtc_gpio_pullup_en((gpio_num_t)USER_BTN);
  rtc_gpio_pulldown_dis((gpio_num_t)USER_BTN);

  // Create FreeRTOS tasks for LED blinking and sleep
  xTaskCreate(&greenTask, "Green Task", 1024, NULL, 1, &greenTaskHandle);
  xTaskCreate(&blueTask, "Blue Task", 1024, NULL, 1, &blueTaskHandle);
  xTaskCreate(&sleepTask, "Sleep Task", 1024, NULL, 1, &sleepTaskHandle);
}

void loop() {
  // Empty loop, as we are using FreeRTOS tasks
}

void ledController(int led_CHANNEL, int brightness, int blinks, int onTime_ms,
                   int offTime_ms) {
  for (int i = 0; i < blinks; i++) {
    ledcWrite(led_CHANNEL, brightness);
    vTaskDelay(onTime_ms / portTICK_PERIOD_MS);
    ledcWrite(led_CHANNEL, 0);
    vTaskDelay(offTime_ms / portTICK_PERIOD_MS);
  }
}
