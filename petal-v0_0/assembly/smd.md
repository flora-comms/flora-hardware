# Petal v0.0 SMD Assembly Notes

Notes from SMD assembly on Friday Oct 25 2024

## Organization

Our system was effective.
Organized by designator, used Altium Assembly assistant tool to guide us.
One person placing, one person preparing.

### Prepare Station

had a pile of component bags.
1. Check next component on assitant, confirm component value and part number.
2. Open the bag, losen tape, place on top of bag.
3. Have one tape ready, one tap on deck, one tape on PnP machine
4. Move bags from left to right based on where they are in the process.
5. Collect completed bags and put off to side.

### Place

1. place components. communicate designators with prepare person.

## Solder paste

- Stencil was too big, had too much paste on PSU
  - Make paste layer smaller, fix issue with paste/mask layer swap
- Designators were confusing, redo them next revision
- Multiple parts for same component, review needs to standardize component types, manufacturers, part numbers, etc.
- Remove 0603s where appropriate.
- Use smaller footprints if able so parts dont shift and "swim" so much.

## Issues

- ESP32 unable to see quality of pad solder. One board doesn't have a funtional blue LED. not sure the cause.
- USB not recognized when C5,6 and R6 are on the lines. they are not necessary. DO NOT PLACE!!!