#!/bin/bash

DEVICE_ID=
if [ $1 == --id ] ; then
  DEVICE_ID=$2
  shift
  shift
fi

SDK_ROOT=$(dirname $0)/../../..
OUTPUT_ROOT=$1
PROJECT_ROOT=$OUTPUT_ROOT/..
NVPARAM_BIN=$OUTPUT_ROOT/nvparam.bin

CLI_PROGRAMMER="$SDK_ROOT/binaries/cli_programmer"
JLINKCOMMANDER="JLinkExe"
JLINKGDB="JLinkGDBServer"

TEMPJSCRIPT=
GDBPID=

cleanup() {
	# cleanup temp script file
	[ ! -z "$TEMPJSCRIPT" ] && echo "Cleaning up..." && rm -f $TEMPJSCRIPT

	# kill gdb, if still alive
	grep $JLINKGDB /proc/$GDBPID/cmdline >/dev/null 2>&1
	if [ $? -eq 0 ] ; then
		kill $GDBPID
	fi
	
}

finish() {
cleanup
echo .
echo .......................................................................................................................
echo ..
echo .. FINISHED!
echo ..
echo .......................................................................................................................
exit $1
}


error() {
	which zenity >/dev/null
	if [ $? -eq 0 ]; then
		zenity --info --title "Programming QSPI Failed" --text "$1"
	else
		echo $1
	fi
}

interrupted() {

	echo "Interrupted...Cleaning up"

	finish 2
}

select_device() {
	
	DEVICE_ARR=$1

	idx=1
	echo
	echo "Multiple JLink devices exist:"
	echo
	for devid in ${DEVICE_ARR[@]} ; do
		echo "$idx: $devid"
		idx=$(($idx+1))
	done

	which zenity >/dev/null
	if [ $? -eq 0 ]; then
		DEVICE_ID=$(zenity --separator=':' --list --column "Devices" --title="Select JLink Device" ${DEVICE_ARR[@]} | cut -d ':' -f1)
		return
	fi

	max=$(($idx-1))
	while : ; do
		echo
		echo -n "Choose one (1-$max): "
		read ans
		[ -z "$ans" ] && continue
		if echo $ans | egrep -q '^[0-9]+$'; then
			if [ $ans -lt 1 -o $ans -gt $max ] ; then
				echo "  * Choice out of range. Try again..."
			else
				ch=$(($ans-1))
				DEVICE_ID=${DEVICE_ARR[$ch]}
				return
			fi
		else
			echo "  * Choice must be a number. Try again..."
		fi
	done

}

get_device_id() {

	TEMPJSCRIPT=`mktemp`
	echo showemulist >> $TEMPJSCRIPT
	echo exit >> $TEMPJSCRIPT
	idx=0
	declare -a DEVICE_ARR
	while read devid; do
		[ -z "$devid" ] && break
		DEVICE_ARR[$idx]=$devid
		idx=$(($idx+1))
	done << EOF
$($JLINKCOMMANDER -CommandFile $TEMPJSCRIPT | sed -n 's/^.*Serial number: \([0-9]*\),.*$/\1/p')
EOF
	case ${#DEVICE_ARR[@]} in
	0)
		error "No JLink found. Connect a board with a JLink and retry. Aborting..."
		finish 1
		;;
	1)
		DEVICE_ID=${DEVICE_ARR[0]}
		echo "Using device with id $DEVICE_ID"
		;;
	*)
		select_device $DEVICE_ARR
		;;
	esac
 	
}

prepare_local_ini_file()
{
  # If local ini file does not exits make cli_programmer do one
  if [ ! -r cli_programmer.ini ] ; then
    $CLI_PROGRAMMER --save cli_programmer.ini >/dev/null 2>&1
  fi

  # Add device selection to ini file
  awk <cli_programmer.ini >cli_programmer.new -F= -v id=$DEVICE_ID -v gdb="$JLINKGDB" '
    /^gdb_server_path/ {
      if (length($2) < 2) {
        sub("=.*", "= " gdb " -if swd -device Cortex-M0 -endian little -speed 0 -singlerun -log jlink.log")
      }
      i = index($0, "-select usb=")
      if (i == 0) {
        print $0 " -select usb=" id
        next
      } else
        sub("-select usb=[0-9]+", "-select usb=" id)
    }
    { print }
  '
  mv cli_programmer.new cli_programmer.ini
}

echo .......................................................................................................................
echo ..
echo .. NV-Parameters PROGRAMMING via JTAG
echo ..
echo .......................................................................................................................
echo .

[ ! $# -eq 1 ] && echo "Usage $0 <project output path>" && exit 1

if [ ! -d $1 ] ; then
    echo "$1 not found!"
    error "Please select the folder which contains the binary you want to program and try again."
    finish 1
fi

if [ ! -x $CLI_PROGRAMMER ] ; then
    error "cli_programmer not found. Please make sure it is build (Release_static configuration) in sdk/utilities/cli_programmer/cli/Release_static"
    finish 1
fi

if [ ! -x `which $JLINKCOMMANDER` ] ; then
	error "$JLINKCOMMANDER not found. Make sure it is installed, and in the system's PATH"
	finish 1
fi

if [ ! -x `which $JLINKGDB` ] ; then
	error "$JLINKGDB not found. Make sure it is installed, and in the system's PATH"
	finish 1
fi

if [ ! -x "$(which arm-none-eabi-gcc)" ]; then
	while [[ ! -x "${ARM_TOOLCHAIN}/arm-none-eabi-gcc" && ! -x "${ARM_TOOLCHAIN}/bin/arm-none-eabi-gcc" ]]; do
	    echo -n "Please enter GNU ARM Toolchain path > "
	    read  ARM_TOOLCHAIN
	done
fi

$SDK_ROOT/utilities/nvparam/create_nvparam.sh $OUTPUT_ROOT $PROJECT_ROOT/config $SDK_ROOT/sdk/bsp/adapters/include
if [ $? -ne 0 ]; then
	error "Could not create nvparam.bin"
	exit 1
fi

trap interrupted SIGINT SIGTERM

if [ -z $DEVICE_ID ] ; then
  # get device id (from user, if more than two connected)
  get_device_id
fi

echo "Programming flash of device $DEVICE_ID"

prepare_local_ini_file $DEVICE_ID

echo $CLI_PROGRAMMER gdbserver write_qspi 0x80000 $NVPARAM_BIN
$CLI_PROGRAMMER gdbserver write_qspi 0x80000 $NVPARAM_BIN
[ $? -ne 0 ] && error "ERROR PROGRAMMING FLASH" && finish 1

finish 0
exit 0
