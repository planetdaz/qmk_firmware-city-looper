/* City Looper Firmware
 * DOIO KB16 / KB16B-01 Macropad
 *
 * Custom firmware for city selection, row count input, and automated
 * HID output loop with abort capability.
 *
 * Copyright 2024
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#include QMK_KEYBOARD_H
#include "matrix.h"

// ============================================================================
// CONSTANTS
// ============================================================================

#define NUM_CITIES 4
#define MAX_ROW_DIGITS 3
#define MAX_ROW_VALUE 999
#define LOOP_DELAY_MS 30

// Matrix position of X key (for abort detection during loop)
#define ABORT_KEY_ROW 3
#define ABORT_KEY_COL 0

// City names array
static const char* city_names[NUM_CITIES] = {
    "Dallas",
    "San Antonio",
    "Austin",
    "Houston"
};

// ============================================================================
// STATE MACHINE
// ============================================================================

// Operating modes
typedef enum {
    MODE_CITY,
    MODE_ROW
} operating_mode_t;

// Global state variables
static operating_mode_t current_mode = MODE_CITY;
static uint8_t          city_index = 0;         // 0-3 for city selection
static uint16_t         row_param = 0;          // Saved row count (0-999)
static uint16_t         entry_buffer = 0;       // Temporary buffer for row entry
static uint8_t          entry_digits = 0;       // Number of digits entered
static bool             is_executing = false;   // True during GO loop
static bool             first_boot = true;      // Show boot message on startup

// ============================================================================
// CUSTOM KEYCODES
// ============================================================================

enum custom_keycodes {
    CL_CITY = SAFE_RANGE,  // Switch to City mode
    CL_ROW,                // Switch to Row mode (#)
    CL_CLEAR,              // Clear entry buffer (X)
    CL_ENTER,              // Save entry buffer to row_param
    CL_GO,                 // Execute the loop
    CL_0, CL_1, CL_2, CL_3, CL_4,
    CL_5, CL_6, CL_7, CL_8, CL_9
};

// ============================================================================
// LAYER DEFINITION
// ============================================================================

enum layer_names {
    _BASE
};

/*
 * Physical Layout:
 *
 *     ┌───┬───┬───┬───┐   ┌───┐ ┌───┐
 *     │ 7 │ 8 │ 9 │CTY│   │ENC│ │ENC│
 *     ├───┼───┼───┼───┤   └───┘ └───┘
 *     │ 4 │ 5 │ 6 │ # │
 *     ├───┼───┼───┼───┤
 *     │ 1 │ 2 │ 3 │   │      ┌───┐
 *     ├───┼───┼───┼───┤      │ENC│  <- Bottom encoder cycles cities
 *     │ X │ 0 │ENT│GO │      └───┘
 *     └───┴───┴───┴───┘
 *
 * Matrix positions (19 keys):
 * Row 0: [0,0] [0,1] [0,2] [0,3] + [0,4] encoder click
 * Row 1: [1,0] [1,1] [1,2] [1,3] + [1,4] encoder click
 * Row 2: [2,0] [2,1] [2,2] [2,3] + [2,4] encoder click (bottom)
 * Row 3: [3,0] [3,1] [3,2] [3,3]
 */

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [_BASE] = LAYOUT(
        // Row 0: 7, 8, 9, City, (enc0 click)
        CL_7,     CL_8,    CL_9,    CL_CITY,  KC_NO,
        // Row 1: 4, 5, 6, #, (enc1 click)
        CL_4,     CL_5,    CL_6,    CL_ROW,   KC_NO,
        // Row 2: 1, 2, 3, (unused), (enc2 click - bottom)
        CL_1,     CL_2,    CL_3,    KC_NO,    KC_NO,
        // Row 3: X, 0, Enter, GO
        CL_CLEAR, CL_0,    CL_ENTER, CL_GO
    )
};


// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Append a digit to the entry buffer (max 3 digits, max value 999)
static void append_digit(uint8_t digit) {
    if (entry_digits < MAX_ROW_DIGITS) {
        uint16_t new_value = entry_buffer * 10 + digit;
        if (new_value <= MAX_ROW_VALUE) {
            entry_buffer = new_value;
            entry_digits++;
        }
    }
}

// Clear the entry buffer
static void clear_entry(void) {
    entry_buffer = 0;
    entry_digits = 0;
}

// Save entry buffer to row_param
static void save_entry(void) {
    row_param = entry_buffer;
}

// Cycle city index
static void cycle_city(bool forward) {
    if (forward) {
        city_index = (city_index + 1) % NUM_CITIES;
    } else {
        city_index = (city_index + NUM_CITIES - 1) % NUM_CITIES;
    }
}

// Get current city name
static const char* get_city_name(void) {
    return city_names[city_index];
}

// ============================================================================
// GO EXECUTION LOGIC
// ============================================================================

// Check if the X (abort) key is currently pressed by scanning matrix directly
static bool is_abort_key_pressed(void) {
    // Scan the matrix to get current state
    matrix_scan();
    // Check if the abort key position is active
    return matrix_is_on(ABORT_KEY_ROW, ABORT_KEY_COL);
}

static void execute_go(void) {
    is_executing = true;

    const char* city = get_city_name();
    uint16_t count = row_param;

    for (uint16_t i = 0; i < count; i++) {
        // Check for abort by directly reading the matrix
        if (is_abort_key_pressed()) {
            break;
        }

        // Send the city name
        send_string(city);

        // Tap DOWN arrow
        tap_code(KC_DOWN);

        // Small delay between iterations, but check for abort during wait
        for (uint8_t w = 0; w < LOOP_DELAY_MS; w++) {
            wait_ms(1);
            if (is_abort_key_pressed()) {
                is_executing = false;
                return;
            }
        }
    }

    is_executing = false;
}

// ============================================================================
// KEY PROCESSING
// ============================================================================

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Clear boot message on first keypress
    if (first_boot && record->event.pressed) {
        first_boot = false;
    }

    // Block all keys during execution (abort is handled via direct matrix scan)
    if (is_executing) {
        return false;
    }

    // Process custom keycodes
    switch (keycode) {
        // Mode switching
        case CL_CITY:
            if (record->event.pressed) {
                current_mode = MODE_CITY;
            }
            return false;

        case CL_ROW:
            if (record->event.pressed) {
                current_mode = MODE_ROW;
                // Reset entry buffer when entering row mode
                clear_entry();
            }
            return false;

        // Clear/X key
        case CL_CLEAR:
            if (record->event.pressed) {
                if (current_mode == MODE_ROW) {
                    clear_entry();
                }
                // In city mode, X does nothing (or could be used for other purposes)
            }
            return false;

        // Enter key - save row entry
        case CL_ENTER:
            if (record->event.pressed) {
                if (current_mode == MODE_ROW) {
                    save_entry();
                }
            }
            return false;

        // GO key - execute the loop
        case CL_GO:
            if (record->event.pressed) {
                execute_go();
            }
            return false;

        // Numeric keys - only active in Row mode
        case CL_0:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(0);
            }
            return false;

        case CL_1:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(1);
            }
            return false;

        case CL_2:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(2);
            }
            return false;

        case CL_3:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(3);
            }
            return false;

        case CL_4:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(4);
            }
            return false;

        case CL_5:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(5);
            }
            return false;

        case CL_6:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(6);
            }
            return false;

        case CL_7:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(7);
            }
            return false;

        case CL_8:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(8);
            }
            return false;

        case CL_9:
            if (record->event.pressed && current_mode == MODE_ROW) {
                append_digit(9);
            }
            return false;

        default:
            // Block all other keys from sending HID
            return false;
    }
}


// ============================================================================
// ENCODER HANDLING
// ============================================================================

bool encoder_update_user(uint8_t index, bool clockwise) {
    // Only handle bottom encoder (index 2) for city cycling
    // Encoders: 0 = top-left, 1 = top-right, 2 = bottom
    if (index == 2) {
        // Only cycle cities when in City mode and not executing
        if (current_mode == MODE_CITY && !is_executing) {
            cycle_city(clockwise);
        }
    }

    // Return false to indicate we've handled the encoder
    // This prevents any default encoder behavior
    return false;
}

// ============================================================================
// OLED DISPLAY
// ============================================================================

#ifdef OLED_ENABLE

// Convert number to string (into provided buffer, returns pointer to buffer)
static char* itoa_simple(uint16_t num, char* buf) {
    if (num == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    int i = 0;
    uint16_t n = num;

    // Build string in reverse
    while (n > 0 && i < 7) {
        buf[i++] = '0' + (n % 10);
        n /= 10;
    }
    buf[i] = '\0';

    // Reverse the string in place
    for (int j = 0; j < i / 2; j++) {
        char tmp = buf[j];
        buf[j] = buf[i - 1 - j];
        buf[i - 1 - j] = tmp;
    }

    return buf;
}

// Force clear entire OLED by writing spaces to all positions
static void force_clear_oled(void) {
    for (uint8_t i = 0; i < 4; i++) {
        oled_set_cursor(0, i);
        oled_write_P(PSTR("                     "), false);  // 21 spaces (max width)
    }
    oled_set_cursor(0, 0);
}

bool oled_task_user(void) {
    char num_buf[8];

    // Force clear by overwriting with spaces
    force_clear_oled();

    // Boot screen - shown until first keypress
    if (first_boot) {
        oled_set_cursor(0, 0);
        oled_write_P(PSTR("city-looper"), false);
        oled_set_cursor(0, 2);
        oled_write_P(PSTR("Ready"), false);
        return false;
    }

    // Execution screen - shown during GO loop
    if (is_executing) {
        oled_set_cursor(0, 0);
        oled_write_P(PSTR("Running..."), false);
        oled_set_cursor(0, 2);
        oled_write_P(PSTR("Hold X to stop"), false);
        return false;
    }

    // Mode-specific display
    switch (current_mode) {
        case MODE_CITY:
            oled_set_cursor(0, 0);
            oled_write_P(PSTR("City:"), false);
            oled_set_cursor(0, 1);
            oled_write(get_city_name(), false);
            oled_set_cursor(0, 3);
            oled_write_P(PSTR("Rows: "), false);
            oled_write(itoa_simple(row_param, num_buf), false);
            break;

        case MODE_ROW:
            oled_set_cursor(0, 0);
            oled_write_P(PSTR("Rows:"), false);
            oled_set_cursor(0, 1);
            if (entry_digits == 0) {
                oled_write_P(PSTR("_"), false);
            } else {
                oled_write(itoa_simple(entry_buffer, num_buf), false);
            }
            oled_set_cursor(0, 3);
            oled_write_P(PSTR("Saved: "), false);
            oled_write(itoa_simple(row_param, num_buf), false);
            break;
    }

    return false;
}

#endif // OLED_ENABLE
