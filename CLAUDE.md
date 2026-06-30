I got a Keychron Q2 Max with an German ISO Layout.
I want to create a custom firmware for my keyboard.
This repo contains the code needed to build my firmware.
Some keys are remapped so when I refer to some keys, always make sure to account for my german iso layout and potential remappings.

If you make changes in the code, always verify it using the following command:

```bash
make keychron/q2_max/iso_encoder:vial
```
