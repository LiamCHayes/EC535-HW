#!/bin/sh

echo "[INFO] Building modules..."
cd liamh_lab3/ul/
make
cd ../km
make
cd ../../

cp -r liamh_lab3/ rootfs/root
# cp -r hw4_targets/ rootfs/root
cp *attack*.sh rootfs/root
./rebuild-rootfs.sh

qemu-system-arm -M virt -m 512 -kernel stock-zImage -nographic \
	  -drive file=rootfs.img,if=none,format=raw,id=rfs \
	    -device virtio-blk-device,drive=rfs \
	      -append "earlyprintk=serial root=/dev/vda"
