
action=$1
argc=$#

clean_up() {
	echo "---------------------------------------------------"
	echo "+                                                 +"
	echo "+            FCProbe Driver Clean                 +"
	echo "+                                                 +"
	echo "---------------------------------------------------"
	rm *.ko
	cd src/driver
	make clean
	rm .*.cmd
	rm -rf .tmp_versions/
	exit
}


build_all() {
	echo "---------------------------------------------------"
	echo "+                                                 +"
	echo "+            FCProbe Driver Build                 +"
	echo "+                                                 +"
	echo "---------------------------------------------------"
	cd src/driver
	make
	cp fcprobe_main.ko ../../
	cd ../../
	exit
}

build_start(){

	if [ $argc -gt 0 ]; then
		if [ $action = "clean" ]; then
			clean_up
		fi
	else
		build_all
	fi
}


build_start
