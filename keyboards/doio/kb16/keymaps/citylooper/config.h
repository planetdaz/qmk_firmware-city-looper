/* City Looper Configuration
 * DOIO KB16 / KB16B-01 Macropad
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */

#pragma once

// Skip the default DOIO logo on boot - show our custom boot screen immediately
#define OLED_LOGO_TIMEOUT 0

// OLED display settings
#define OLED_TIMEOUT 0              // Never turn off OLED
#define OLED_BRIGHTNESS 255         // Maximum brightness

// CRITICAL: Throttle OLED updates to prevent display corruption
// Without this, oled_clear() doesn't work properly because the display
// isn't finished rendering before the next frame starts
#define OLED_UPDATE_INTERVAL 50     // Update every 50ms (20 fps)

