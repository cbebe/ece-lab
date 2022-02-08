#!/bin/sh

project_dir="$HOME/Documents/School/kypd_ssd_gpio"
app_dir="$project_dir/kypd_ssd_gpio.sdk/keypad_ssd"
hello_world_file="$app_dir/src/freertos_hello_world.c"

# cp --remove-destination $1 $hello_world_file
ln -f $1 $hello_world_file