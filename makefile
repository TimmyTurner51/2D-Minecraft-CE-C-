# ----------------------------
# Set NAME to the program name
# Set ICON to the png icon file name
# Set DESCRIPTION to display within a compatible shell
# Set COMPRESSED to "YES" to create a compressed program
# ----------------------------

NAME        ?= MC2DCE
COMPRESSED  ?= NO
ICON        ?= icon.png
DESCRIPTION ?= "2D Minecraft CE (C) alpha v1.0.00"

# ----------------------------

include $(CEDEV)/include/.makefile
