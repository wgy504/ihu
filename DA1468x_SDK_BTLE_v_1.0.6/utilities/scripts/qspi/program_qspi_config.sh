#/bin/sh

CONFIG_FILE="program_qspi.ini"
GUI=
PRODUCT_ID=
ENABLE_UART=
RAM_SHUFFLING=
echo cwd: `pwd`
which zenity > /dev/null
[ $? -eq 0 ] && GUI=y

#### Dump old config section

dump_config_text() {
       echo Existing Configuration
       echo ----------------------
       echo "PRODUCT_ID: $PRODUCT_ID"
       if [ x"$PRODUCT_ID" = x"DA14681-00" ] || [ x"$PRODUCT_ID" = x"DA14680-00" ] ; then
  	     echo "ENABLE_UART=$ENABLE_UART"
         echo "RAM_SHUFFLING=$RAM_SHUFFLING"
       fi
       echo ----------------------
}

dump_config() {
	eval $(cat $CONFIG_FILE)
	if [ -z "$GUI" ] ; then
		dump_config_text
		ask_change
	else
		zenity --question --title "QSPI programming" --text="`dump_config_text 2>&1`" --ok-label "Change" --cancel-label "Keep"
		if [ $? -ne 0 ] ; then
			exit 0
		fi
	fi 
		
}

ask_change() {
	echo -n "Change existing configuration (y/N or [ENTER] for n)? "
	read ans
	case $ans in
	Y) return
		;;
	y) return
		;;
	*) echo Keeping old config
 	   exit 0
	esac
}

# Get chip revision section
get_prod_id() {
	if [ -z "$GUI" ] ; then
		PRODUCT_ID=
		while : ; do
			echo "Product id options:"
			echo "0:  DA14680/1-00"
			echo "1:  DA14680/1-01 (default)"
			echo -n "Product id ? (0..1 or [ENTER] for 1)"
			read ans
			case $ans in
			0) PRODUCT_ID="DA14681-00"
				;;
			1) PRODUCT_ID="DA14681-01"
				;;
			"") PRODUCT_ID="DA14681-01"
				;;
			*) echo ; echo "Invalid chip revision. Try again " ; echo
			esac
			[ -z "$PRODUCT_ID" ] || break
		done
	else
		while : ; do
			PRODUCT_ID=`zenity --separator=':' --list --column "Product Id" --title="Select Product Id" DA14680/1-00 DA14680/1-01`
			[ $? -eq 0 ] && PRODUCT_ID=`echo $PRODUCT_ID | cut -d ':' -f1 | sed 's/0\///'` && break
		done
	
	fi
			

}

# enable uart question
get_enable_uart() {
	if [ -z "$GUI" ] ; then
		ENABLE_UART=
		while : ; do
			echo -n "Enable UART (y/n or [ENTER] for y)? "
			read ans
			case $ans in
			y) ENABLE_UART=y
				;;
			Y) ENABLE_UART=y
				;;
			n) ENABLE_UART=n
				;;
			N) ENABLE_UART=n
				;;
			"") ENABLE_UART=y
				;;				
			*) echo ; echo "Invalid answer. Try again " ; echo; ENABLE_UART=
				
			esac
			[ -z "$ENABLE_UART" ] || break
		done
	else
		zenity --question --text "Enable UART?" --ok-label Yes --cancel-label No
		if [ $? -eq 0 ]; then
			ENABLE_UART=y
		else
			ENABLE_UART=n
		fi
	fi
}

# Get ram shuffling config
get_ram_shuffling() {
        if [ -z "$GUI" ] ; then
                RAM_SHUFFLING=
                while : ; do
			echo "RAM Shuffling options:"
			echo "       0:  8 - 24 - 32 (default)"
			echo "       1: 24 -  8 - 32"
			echo "       2: 32 -  8 - 24"
			echo "       3: 32 - 24 -  8"
			echo -n "Ram shuffling (0-3 or [ENTER] for 0)? "

                        read ans
		
	                [ -z "$ans" ] && ans=0
        	        if echo $ans | egrep -q '^[0-9]+$'; then
                	        if [ $ans -lt 0 -o $ans -gt 3 ] ; then
                        	        echo "  * Choice out of range. Try again..."
	                        else
        	                        RAM_SHUFFLING=$ans
        	                        return
                	        fi
	                else
	                        echo "  * Choice must be a number. Try again..."
	                fi
		done	
        else
		while : ; do
	                RAM_SHUFFLING=$(zenity --separator=':' --list --column "Mode" --column "Description" --title="Select RAM Shuffling mode" 0 " 8 - 24 - 32" 1 "24 -  8 - 32" 2 "32 -  8 - 24" 3 "32 - 24 -  8")
			[ $? -eq 0 ] && RAM_SHUFFLING=`echo $RAM_SHUFFLING | cut -d ':' -f1` && break
		done

        fi


}

if [ -f $CONFIG_FILE ] ; then
	dump_config
fi


get_prod_id

if [ "$PRODUCT_ID" = "DA14681-00" ] ; then
	get_enable_uart
	get_ram_shuffling
fi

echo "export PRODUCT_ID=\"$PRODUCT_ID\"" > $CONFIG_FILE
if [ "$PRODUCT_ID" = "DA14681-00" ] ; then
	echo "export ENABLE_UART=\"$ENABLE_UART\"" >> $CONFIG_FILE
	echo "export RAM_SHUFFLING=$RAM_SHUFFLING" >> $CONFIG_FILE
fi

