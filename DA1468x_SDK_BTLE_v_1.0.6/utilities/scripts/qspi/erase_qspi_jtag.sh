#!/bin/bash

DEVICE_ID=
while true; do
  case $1 in
    --cfg)
      CFG=$2
      ;;
    --id)
      DEVICE_ID=$2
      ;;
    *)
      break
      ;;
  esac
  shift
  shift
done

base_dir="`dirname "$0"`"
sdkroot=${SDKROOT:=$(pushd $base_dir/../../.. >/dev/null; pwd; popd >/dev/null)}
CONFIG_FILE="$base_dir/program_qspi.ini"
CONFIG_SCRIPT="$base_dir/program_qspi_config.sh"
CLI_PROGRAMMER="$sdkroot/binaries/cli_programmer"
JLINKCOMMANDER="JLinkExe"
JLINKGDB="JLinkGDBServer"

TEMPJSCRIPT=
GDBPID=

cleanup() {
	# cleanup temp script file
	[ ! -z "$TEMPJSCRIPT" ] && echo "Cleaning up..." && rm -f $TEMPJSCRIPT

	[ -r $TMPCFG ] && rm $TMPCFG

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

msg() {
	which zenity >/dev/null
	if [ $? -eq 0 ]; then
		zenity --info --title "$2" --text "$1"
	else
		echo $1
	fi
}


error() {
	msg "$1" "Erasing QSPI Failed"
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

ask_erase_ui() {
	which zenity >/dev/null
	if [ $? -eq 0 ]; then
                zenity --question --title "Erase QSPI memory" --text="Are you sure you want to erase QSPI ?" --ok-label "Erase" --cancel-label "Cancel"
                if [ $? -ne 0 ] ; then
                        exit 0
                fi
 	else 
                ask_erase_cli
       	fi

}

ask_erase_cli() {
        echo -n "Are you sure you want to completely erase QSPI (y/N or [ENTER] for n)? "
        read ans
        case $ans in
        Y) return
                ;;
        y) return
                ;;
        *) echo Cancelled
           exit 0
        esac
}


echo .......................................................................................................................
echo ..
echo .. ERASE QSPI via JTAG
echo ..
echo .......................................................................................................................
echo .

[ ! -f $CONFIG_FILE ] && ( $CONFIG_SCRIPT || exit 1 )
. $CONFIG_FILE

if [ ! -x $CLI_PROGRAMMER ] ; then
        error "cli_programmer not found. Please make sure it is built and installed in $(dirname $CLI_PROGRAMMER)"
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

trap interrupted SIGINT SIGTERM

if [ -z $DEVICE_ID ] ; then
  # get device id (from user, if more than two connected)
  get_device_id
fi

ask_erase_ui

echo "Erasing flash of device $DEVICE_ID"

if [ -z ${CFG} ] ; then
  TMPCFG=${CFG:=`mktemp`}


  $base_dir/prepare_local_ini_file.sh --cfg "$CFG" --id $DEVICE_ID
fi


if [ -n "$CLI_PROGRAMMER" ] ; then
	CLI_PROGRAMMER="$CLI_PROGRAMMER --cfg $CFG "
fi

c="$CLI_PROGRAMMER gdbserver chip_erase_qspi"

echo $c; $c

[ $? -ne 0 ] && error "ERROR ERASING QSPI FLASH" && finish 1

msg "Successfully erased QSPI" "Erase QSPI"
finish 0
exit 0
