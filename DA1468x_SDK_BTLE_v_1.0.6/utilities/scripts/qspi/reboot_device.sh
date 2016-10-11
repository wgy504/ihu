#!/bin/bash

#
# Usage:
# reboot_device [serial_number]
# if serial number not given one device will be rebooted
#

base_dir="`dirname "$0"`"
JLINKCOMMANDER="JLinkExe"

DEVICE_ID=

if [ "$1" != "" ] ; then
  DEVICE_ID=$1
else
  DEVICE_ID=`$JLINKCOMMANDER -CommandFile ${base_dir}/jlink_showemulist.script | sed -n 's/^.*Serial number: \([0-9]*\),.*$/\1/p' | tail -n 1`
fi

if [ ! -z $DEVICE_ID ] ; then
  $JLINKCOMMANDER -if SWD -device Cortex-M0 -speed auto -SelectEmuBySN $DEVICE_ID -CommandFile $base_dir/reboot.script
fi
