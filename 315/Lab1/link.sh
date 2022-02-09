#!/bin/sh
################################################################################
# link.sh
# Creates a hard link from one file to the main source file in the SDK
# Makes it easier to switch lab parts
# and lets you edit in your favourite editor instead of the SDK
# Usage:
#   ./link.sh <FILE>
#   Then every time you make a change, focus on the editor window in the SDK
#   You'll get a message that the file changed.
#   Say 'yes' to replace the editor contents
################################################################################

# Make sure to configure these variables to point to the right file!
project_dir="$HOME/Documents/School/kypd_ssd_gpio"
app_dir="$project_dir/kypd_ssd_gpio.sdk/keypad_ssd"
main_source_file="$app_dir/src/freertos_hello_world.c"

ln -f $1 $main_source_file