#ifndef CONFIG_H
#define CONFIG_H

// Include necessary libraries
#include <Arduino.h>
#include <FreeRTOS.h>
#include <esp_sleep.h>

// Set up the RGB LED pins
#define R_PIN 17
#define G_PIN 18
#define B_PIN 16
#define R_CHANNEL 0
#define G_CHANNEL 1
#define B_CHANNEL 2
#define PWM_FREQ 12000
#define PWM_RES 8

#define USER_BTN 14

#endif // CONFIG_H
