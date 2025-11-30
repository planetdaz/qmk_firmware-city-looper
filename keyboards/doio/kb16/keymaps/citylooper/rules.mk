# City Looper Firmware
# Build options for DOIO KB16 macropad

# Required features
OLED_ENABLE = yes           # Enable OLED display
ENCODER_ENABLE = yes        # Enable rotary encoders

# Disable encoder map since we handle encoders manually
ENCODER_MAP_ENABLE = no

# Optional: Disable unused features to save space
MOUSEKEY_ENABLE = no        # Not needed for this firmware
CONSOLE_ENABLE = no         # Disable debug console
COMMAND_ENABLE = no         # Disable command feature

# Keep RGB enabled (inherited from keyboard defaults)
# RGB_MATRIX_ENABLE is set in keyboard config

