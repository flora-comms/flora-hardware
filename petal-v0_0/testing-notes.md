# testing notes

05 Nov 2024
- IRQ on 39 is super buggy. Jumpering to 38 makes it work totally fine. Recommend moving IRQ to 38 in firmware. During build, scratch off mask between pads for gpio38 and adjacent Lora_IRQ so they bridge.

07 Nov 2024
- GPIO38 and 39 are not RTC IOs on the S3 (These are GPIO 0-21), so it cannot be used to wake up from deep sleep (only light sleep using esp_sleep_enable_gpio_wakeup()). Future revisions that want to use deep sleep will need to put Lora IRQ on an RTCIO pin.