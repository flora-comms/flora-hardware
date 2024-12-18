# testing

Testing code and procedures for Petal hardware.
Testing results for each Petal version will be found in that version's folder.

## File Tree Overview

Navigating this directory

### lora-pingpong

Code to test the functionality of the LoRa module on the Petal.
Requires PlatformIO.

### lora-sleep

Code to test the LoRa wakeup from light sleep.
Requires platformIO.

### LoraSwr

Code to test hardware SWR and radio performance.
Requires PlatformIO and the test setup found in the test plan.

### sdWrite

Code to test the functionality of the SD card on the Petal.
Requires PlatformIO.

### sleep-test

Code to test light sleep and wakeup sources.
Requires PlatformIO.

### assembly-checklist.xlsx

Pre- and post-assembly checklist. Ensures the board has been built correctly before official testing can begin.

### floar-test-plan.docx

Official test plan. Requires that the Petal has AVAlink firmware from _flora-software_. 
All tests must pass for requirements to be met.
Use the supporting checklists for determining pass-fail criteria.

### lvd-testing.xlsx & psu-testing.xlsx

Supporting checklists for use with the official test plan. 