// Copyright (c) GWENDESIGN. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#ifndef _GDTOUCHKEYBOARD_H_
#define _GDTOUCHKEYBOARD_H_

#include <M5Unified.h>




class GDTouchKeyboard
{
public:
  typedef enum
  {
    KEY_MODE_LETTER = 0,
    KEY_MODE_NUMBER = 1,
    KEY_MODE_HEX = 2,
    KEY_MODE_MAC = 3,
  } key_mode_t;

  typedef bool (*input_validator_t)(const String& candidate);
  typedef void (*screenshot_handler_t)(void);

  GDTouchKeyboard();
  ~GDTouchKeyboard();

  String run(String text = "", uint16_t setColourIn = 0x0ad9,
             bool getIsEditable = true,
             const lgfx::v1::IFont* fontIn = &fonts::Font0,
             key_mode_t modeIn = KEY_MODE_LETTER,
             bool preserveMacSeparators = true);
  void setMode(key_mode_t modeIn);
  void setAvailableModes(uint8_t modeMask);
  void setInputLength(uint16_t minLength, uint16_t maxLength = 0);
  void setInputValidator(input_validator_t validator);
  void setTouchFeedback(bool enabled);
  void setScreenshotHandler(screenshot_handler_t handler);
  void processScreenshotRequest(void);

private:

  #define KEYBOARD_X (2)
  #define KEYBOARD_Y (26)

  #define KEY_W (45)
  #define KEY_H (50)

  #define COLS (7)
  #define ROWS (4)

  #define MAX_SHIFT_MODE (6)

  const char keymap[MAX_SHIFT_MODE][ROWS][COLS] =
  {
    {
    {'a', 'b', 'c', 'd', 'e', 'f', 'g'},
    {'h', 'i', 'j', 'k', 'l', 'm', 'n'},
    {'o', 'p', 'q', 'r', 's', 't', 'u'},
    {'v', 'w', 'x', 'y', 'z', ' ', '\002'}, // 002 = shift
    },
    {
    {'A', 'B', 'C', 'D', 'E', 'F', 'G'},
    {'H', 'I', 'J', 'K', 'L', 'M', 'N'},
    {'O', 'P', 'Q', 'R', 'S', 'T', 'U'},
    {'V', 'W', 'X', 'Y', 'Z', ' ', '\002'}, // 002 = shift
    },
    {
    {'0', '1', '2', '3', '4', '5', '6'},
    {'7', '8', '9', '`', '-', '=', '['},
    {']', '\\', ';', '\'', ',', '.', '/'},
    {' ', ' ', ' ', ' ', ' ', ' ', '\002'}, // 002 = shift
    },
    {
    {'~', '!', '@', '#', '$', '%', '^'},
    {'&', '*', '(', ')', '_', '+', '{'},
    {'}', '|', ':', '"', '<', '>', '?'},
    {' ', ' ', ' ', ' ', ' ', ' ', '\002'}, // 002 = shift
    },
    {
    {'0', '1', '2', '3', '\001', '\001', '\001'},
    {'4', '5', '6', '7', '\001', '\001', '\001'},
    {'8', '9', 'A', 'B', '\001', '\001', '\001'},
    {'C', 'D', 'E', 'F', '\001', '\001', '\001'},
    },
    {
    {'0', '1', '2', '3', '\001', '\001', '\001'},
    {'4', '5', '6', '7', '\001', '\001', '\001'},
    {'8', '9', 'A', 'B', '\001', '\001', '\001'},
    {'C', 'D', 'E', 'F', '\001', '\001', '\001'},
    },
  };



  void _processInput(void);
  void _updateInputText(void);
  void _initKeyboard(String text = "");
  void _deinitKeyboard(void);
  void _drawKeyboard(void);
  bool _isValidInput(const String& candidate, bool complete) const;
  void _startTouchFeedback(void);
  String _formatMacInput(const String& value) const;
  String _stripMacSeparators(const String& value) const;



  uint16_t themeColor = 0x0ad9;
  String _input_text = "";
  String _old_input_text = "";
  key_mode_t _key_mode = KEY_MODE_LETTER;
  bool _shift_mode = false;
  bool _keyboard_done = false;
  uint32_t _cursor_last;
  bool _cursor_state = false;
  const lgfx::v1::IFont* font = &fonts::Font0;
  bool isEditable = false;
  String promptText = "";
  uint8_t _available_modes = 0x07;
  uint16_t _minimum_length = 0;
  uint16_t _maximum_length = 0;
  input_validator_t _input_validator = nullptr;
  bool _touch_feedback = false;
  uint32_t _vibration_stop_at = 0;
  bool _preserve_mac_separators = true;
  screenshot_handler_t _screenshot_handler = nullptr;
};

extern GDTouchKeyboard GDTK;

#endif // _GDTOUCHKEYBOARD_H_
