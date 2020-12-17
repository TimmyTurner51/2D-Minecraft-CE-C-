# ----------------------------
# Set NAME to the program name
# Set ICON to the png icon file name
# Set DESCRIPTION to display within a compatible shell
# Set COMPRESSED to "YES" to create a compressed program
# ----------------------------

NAME        ?= MC2DCE
COMPRESSED  ?= YES
ICON        ?= icon.png
DESCRIPTION ?= "2D Minecraft CE (C) DEV_ALPHA v1.0.04"

# ----------------------------

include $(CEDEV)/include/.makefile
