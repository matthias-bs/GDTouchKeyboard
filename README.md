# Gwendesign Touch Keyboard

Touch keyboard for M5Unified-compatible ESP32 devices.

## Forks

### @ejagombar/GDTouchKeyboard

Modified to allow for custom theming as well as several other improvements.

### @matthias-bs/GDTouchKeyboard

Additional changes:

- Ported the library and example to M5Unified and M5GFX.
- Added hexadecimal mode in addition to letter and number keyboard modes.
- Added MAC address mode with automatic colon insertion and optional separator preservation in the returned value.
- Added mode masks to restrict the available keyboard modes.
- Added minimum and maximum input-length limits.
- Added caller-supplied input validation callbacks.
- Added optional tactile feedback for touch input.
- Added the `GDTouchKeyboardValidation` example, demonstrating validated configuration entries, mode restrictions, physical-button navigation, and tactile feedback.
