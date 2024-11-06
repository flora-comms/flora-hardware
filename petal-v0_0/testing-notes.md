# testing notes

05 Nov 2024
- IRQ on 39 is super buggy. Jumpering to 38 makes it work totally fine. Recommend moving IRQ to 38 in firmware. During build, scratch off mask between pads for gpio38 and adjacent Lora_IRQ so they bridge.