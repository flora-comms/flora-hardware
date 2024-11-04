# lora-pingpong GUIDE

How to use this code to test a LoRa module.

1. Open the "lora-pingpong.code-workspace" file in VS Code.
2. Connect a working LoRa board. Supported boards are Petal Radio v0.0, LilyGo T3 S3 v1.2 SX1262, and the Heltec 32 LoRa V3.
3. Uncomment the board you would like to use in main.cpp.
4. Select the correct PIO environment.
5. Upload.
6. Open the Serial port.
7. Connect the Petal under test ("DUT").
8. Comment out the previous board define and uncomment the Petal define.
9. Uncomment the "INITIATING_NODE" define
10. Upload
11. Open the serial monitor on both and observe the packets ping ponging. If not, troubleshoot.