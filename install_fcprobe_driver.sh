#！/bin/sh



module=fcprobe_main
device=fcprobe

echo "insmod driver...."
insmod $module.ko || exit 1
echo "insmod ok"

echo "get node id"
major=$(awk '$2=="FC-PROBE" {print $1}' /proc/devices)
echo "fcprobe_main major id is :" $major

echo "mknod...."
mknod /dev/${device} c $major 0
