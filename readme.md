# City Looper

> Custom QMK firmware for the DOIO KB16 / KB16B-01 macropad.

---

## 1. Project Overview

**Name:** City Looper

**Device:** DOIO KB16 / KB16B-01 style 16-key macropad with 3 rotary encoders and OLED display.

**Purpose:** This firmware transforms the DOIO macropad into a specialized automation controller for repetitive data-entry workflows. It types a selected city name into a text field, then loops through a specified number of rows by pressing Down Arrow and Enter.

**Current Status:** Fully functional.

---

## 2. How the Keymap Works

City Looper has two operating modes: **City Mode** and **Row Mode**. You switch between them using the bottom encoder's button.

### Physical Layout

```
┌─────┬─────┬─────┬─────┐   ┌───────┐
│  7  │  8  │  9  │ (X) │   │ENC TOP│  ← Top encoder: unused
├─────┼─────┼─────┼─────┤   └───────┘
│  4  │  5  │  6  │(ENT)│   ┌───────┐
├─────┼─────┼─────┼─────┤   │ENC MID│  ← Middle encoder: unused
│  1  │  2  │  3  │ GO  │   └───────┘
├─────┼─────┼─────┼─────┤   ┌───────┐
│  0  │ --- │ --- │ --- │   │ENC BOT│  ← Bottom encoder: city select / row adjust
└─────┴─────┴─────┴─────┘   └───────┘
                                 ↑
                          Click = toggle mode
```

### City Mode (default)

**OLED shows:** `DFW x 120` / `CITY MODE`

| Control | Action |
|---------|--------|
| **Bottom encoder (rotate)** | Cycle through cities: Dallas (DFW), San Antonio (SATX), Austin (ATX), Houston (HOU) |
| **Bottom encoder (click)** | Switch to Row Mode |
| **GO key** | Execute the automation loop |
| **X key** | (no action in idle) |

### Row Mode

**OLED shows:** `Rows:_` / `ENT=save  X=clr` (while typing)

| Control | Action |
|---------|--------|
| **Number keys 0-9** | Type digits to set row count (max 3 digits, max value 999) |
| **Bottom encoder (rotate)** | Increment/decrement the entry value by 1 |
| **Enter key** | Save the entered value and return to City Mode |
| **X key** | Clear the current entry (start over) |
| **Bottom encoder (click)** | Switch back to City Mode (without saving) |

### Executing the Macro (GO)

When you press **GO**, the macro runs:

1. Types the full city name (e.g., "Dallas")
2. Presses Tab
3. For each row (up to the saved count):
   - Presses Down Arrow
   - Presses Enter
   - Waits 30ms between iterations

**OLED shows:** `RUN DFW x120` / `Hold X to stop`

**To abort:** Hold the **X** key. The macro stops after the current iteration.

### OLED Display Summary

| State | Line 1 | Line 2 |
|-------|--------|--------|
| Boot (before first keypress) | `city-looper` | `Ready` |
| City Mode (idle) | `DFW x 120` | `CITY MODE` |
| Row Mode (typing) | `Rows:45` | `ENT=save  X=clr` |
| Row Mode (idle, value saved) | `Rows:_` | `Saved: 120` |
| Executing | `RUN DFW x120` | `Hold X to stop` |

### City Abbreviations

| Full Name | OLED Abbreviation | Macro Output |
|-----------|-------------------|--------------|
| Dallas | DFW | "Dallas" |
| San Antonio | SATX | "San Antonio" |
| Austin | ATX | "Austin" |
| Houston | HOU | "Houston" |

---

## 3. Hardware Summary

| Component | Details |
|-----------|---------|
| **Keys** | 16 mechanical switches in a 4×4 grid |
| **Encoders** | 3 rotary encoders (all clickable) |
| **Display** | Small OLED screen (SSD1306-based) |
| **Lighting** | Per-key RGB backlighting (WS2812-based) |
| **MCU** | STM32F103 (ARM Cortex-M3) |
| **Bootloader** | STM32Duino / Maple DFU |
| **Connection** | USB-C |

### Bootloader Identity

When in DFU mode, the device appears as:

```
Maple 003 (1EAF:0003)
```

QMK Toolbox will display: `STM32Duino device connected`

---

## 4. How to Enter Bootloader Mode

This is critical for flashing firmware. Even if your firmware is completely broken, this method **always works**.

### Steps:

1. **Unplug** the macropad from USB
2. **Hold down key 0,0** (the upper-left key in the 4×4 grid)
3. **While holding the key**, plug in the USB-C cable
4. **Release the key** after 1-2 seconds
5. The board is now in **Maple DFU bootloader mode**

### Verification:

- QMK Toolbox should display: `STM32Duino device connected (DFU mode)`
- Windows Device Manager shows: `Maple 003` under USB devices

### Troubleshooting:

| Problem | Solution |
|---------|----------|
| QMK Toolbox shows "NO DRIVER" | Install WinUSB driver using Zadig (see Section 5) |
| Device not detected at all | Try a different USB cable (must support data, not just charging) |
| Device detected but flashing fails | Hold key 0,0 longer before plugging in; ensure Toolbox shows DFU mode |
| Accidentally flashed bad firmware | No problem—bootloader mode bypasses firmware entirely |

> **You cannot brick this device.** The bootloader lives in protected memory. Key 0,0 + USB always works.

---

## 5. Required Tools (Flashing Only)

This project is designed so you **never need to build firmware locally**.

### You Need:

| Tool | Purpose | Link |
|------|---------|------|
| **QMK Toolbox** | Flash `.bin` firmware to the macropad | [Download](https://github.com/qmk/qmk_toolbox/releases) |
| **Zadig** | Install WinUSB driver for DFU mode (one-time setup) | [Download](https://zadig.akeo.ie/) |
| **Web Browser** | Download firmware artifacts from GitHub Actions | — |
| **USB-C Cable** | Connect macropad to computer | — |

### You Do NOT Need:

- ❌ Python
- ❌ MSYS2 / MinGW
- ❌ WSL (Windows Subsystem for Linux)
- ❌ `make` or any build tools
- ❌ `qmk` CLI
- ❌ Git (for building—only needed if you modify code)
- ❌ Any compiler toolchain

**GitHub Actions builds everything for you in the cloud.**

### One-Time Zadig Setup:

If QMK Toolbox shows "NO DRIVER" when the macropad is in DFU mode:

1. Open **Zadig**
2. Go to **Options → List All Devices**
3. Select **Maple 003** from the dropdown
4. Set the driver to **WinUSB**
5. Click **Install Driver** (or Replace Driver)
6. Close Zadig
7. QMK Toolbox should now detect the device

You only need to do this once per computer.

---

## 6. How Firmware Gets Built (GitHub Actions)

Firmware is compiled automatically in the cloud. You never need to set up a local build environment.

### How It Works:

1. Code changes are pushed to the repository
2. GitHub Actions workflow triggers automatically (on push to `master`)
3. An Ubuntu runner installs QMK and the ARM toolchain
4. Firmware is compiled: `qmk compile -kb doio/kb16/rev2 -km citylooper`
5. The resulting `.bin` file is uploaded as an artifact

### Workflow File Location:

```
.github/workflows/build.yml
```

### Manual Build Trigger:

You can trigger a build manually without pushing code:

1. Go to the repository on GitHub
2. Click **Actions** tab
3. Select **Build QMK Firmware** workflow
4. Click **Run workflow** → **Run workflow**

### Downloading Firmware:

1. Go to GitHub → **Actions** tab
2. Click on the latest successful workflow run
3. Scroll to **Artifacts** section
4. Download **firmware**
5. Extract the ZIP file
6. You'll find: `doio_kb16_rev2_citylooper.bin`

This is the file you flash with QMK Toolbox.

---

## 7. Flashing Firmware via QMK Toolbox

### Step-by-Step:

1. **Download** the firmware `.bin` from GitHub Actions artifacts
2. **Open** QMK Toolbox
3. **Click "Open"** and select the `.bin` file
4. **Enter bootloader mode:**
   - Unplug the macropad
   - Hold key 0,0 (upper-left)
   - Plug in USB-C while holding the key
   - Release after 1-2 seconds
5. **Verify** Toolbox shows: `STM32Duino device connected (DFU mode)`
6. **Click "Flash"**
7. Wait for: `Flash complete`
8. The macropad **automatically reboots** into the new firmware

### Post-Flash Verification:

- Press keys and verify they send the expected keycodes
- Check OLED display (if firmware includes OLED code)
- Test encoders (if firmware includes encoder code)

### Troubleshooting:

| Problem | Solution |
|---------|----------|
| "No device connected" | Enter bootloader mode correctly (key 0,0 + USB) |
| "NO DRIVER" appears | Run Zadig to install WinUSB driver |
| Flash fails mid-way | Unplug, re-enter bootloader mode, try again |
| Keys don't work after flash | Re-flash; verify correct `.bin` file was used |
| Device won't exit DFU mode | Unplug and replug without holding any key |


---

## 8. Repository Structure

```
qmk_firmware-city-looper/
├── .github/
│   └── workflows/
│       └── build.yml              # GitHub Actions build workflow
├── keyboards/
│   └── doio/
│       └── kb16/
│           ├── info.json          # Keyboard metadata
│           ├── kb16.c             # Keyboard-level code
│           ├── rev2/
│           │   ├── keyboard.json  # Rev2 hardware config
│           │   └── ...
│           └── keymaps/
│               ├── default/       # Stock DOIO keymap
│               └── citylooper/    # ← YOUR CUSTOM KEYMAP
│                   ├── keymap.c   # Key mappings and logic
│                   └── rules.mk   # Build options
└── README.md                      # This file
```

### Important Paths:

| What | Path |
|------|------|
| Keyboard definition | `keyboards/doio/kb16/` |
| Hardware config (Rev2) | `keyboards/doio/kb16/rev2/keyboard.json` |
| **Your keymap** | `keyboards/doio/kb16/keymaps/citylooper/` |
| Build workflow | `.github/workflows/build.yml` |

### Hardware Compatibility Note:

The **DOIO KB16B-01** (the version with the B suffix) uses the **KB16 Rev2** definition. This has been tested and confirmed working.

---

## 9. Adding or Modifying Firmware

All your custom logic goes in the keymap folder:

```
keyboards/doio/kb16/keymaps/citylooper/
```

### Key Files to Edit:

| File | Purpose |
|------|---------|
| `keymap.c` | Define key layouts, layers, and all custom behavior |
| `rules.mk` | Enable/disable QMK features (OLED, encoders, RGB, etc.) |
| `config.h` | Override default settings (create if needed) |

### Where to Add Custom Logic:

#### Key Press Handling
```c
bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    // Your state machine logic here
    return true;
}
```

#### Encoder Rotation
```c
bool encoder_update_user(uint8_t index, bool clockwise) {
    // index = which encoder (0, 1, or 2)
    // clockwise = rotation direction
    return true;
}
```

#### OLED Display
```c
bool oled_task_user(void) {
    // Draw to OLED here
    return false;
}
```

#### Matrix Scan (runs every cycle)
```c
void matrix_scan_user(void) {
    // Continuous logic here (use sparingly)
}
```

### Enabling Features in rules.mk:

```makefile
# Example rules.mk options
OLED_ENABLE = yes           # Enable OLED display
ENCODER_ENABLE = yes        # Enable rotary encoders
RGB_MATRIX_ENABLE = yes     # Enable RGB lighting
MOUSEKEY_ENABLE = yes       # Enable mouse keys
```

### QMK Documentation:

- [Keycodes](https://docs.qmk.fm/#/keycodes)
- [Layers](https://docs.qmk.fm/#/feature_layers)
- [OLED Driver](https://docs.qmk.fm/#/feature_oled_driver)
- [Encoders](https://docs.qmk.fm/#/feature_encoders)
- [RGB Matrix](https://docs.qmk.fm/#/feature_rgb_matrix)
- [Custom Keycodes](https://docs.qmk.fm/#/custom_quantum_functions)

---

## 10. Notes for Future You

Hey, future me. You've forgotten all of this. That's okay. Here's what you need to know:

### The Big Picture:

- **You do NOT need to install any toolchains locally.**
- **GitHub Actions builds the firmware for you automatically.**
- **QMK Toolbox + Zadig is all you need to flash.**
- **Bootloader mode (key 0,0 + USB) is your escape hatch if anything breaks.**

### Quick Reference:

| Task | How |
|------|-----|
| Build firmware | Push to `master` (or trigger manually in Actions) |
| Download firmware | GitHub → Actions → Latest run → Artifacts → `firmware` |
| Flash firmware | QMK Toolbox → Load `.bin` → Enter bootloader → Flash |
| Enter bootloader | Hold upper-left key + plug in USB |
| Fix "NO DRIVER" | Zadig → Maple 003 → Install WinUSB |

### You Cannot Brick This Device:

The bootloader is in protected memory. No matter how badly you mess up the firmware, holding key 0,0 while plugging in USB will always get you into bootloader mode. Always.

### If Something Doesn't Work:

1. Re-read this README
2. Check that you're using the correct `.bin` file
3. Try a different USB cable
4. Re-enter bootloader mode and try again
5. Check GitHub Actions logs for build errors

### Development Workflow:

1. Edit files in `keyboards/doio/kb16/keymaps/citylooper/`
2. Commit and push to `master`
3. Wait for GitHub Actions to build
4. Download the artifact
5. Flash with QMK Toolbox
6. Test
7. Repeat

---

## 11. Links and Resources

| Resource | URL |
|----------|-----|
| QMK Documentation | https://docs.qmk.fm |
| QMK Toolbox Releases | https://github.com/qmk/qmk_toolbox/releases |
| Zadig (USB Driver Tool) | https://zadig.akeo.ie |
| QMK Discord | https://discord.gg/qmk |
| This Repository | https://github.com/planetdaz/qmk_firmware-city-looper |

---

## License

This project is based on QMK Firmware, which is licensed under the GNU General Public License v2.

---

*Last updated: November 2025*
