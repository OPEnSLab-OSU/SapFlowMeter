board=adafruit:samd:adafruit_feather_m0
sketch=sapflow_protothread
bossac=/home/marshal/.arduino15/packages/arduino/tools/bossac/1.7.0-arduino3/bossac
port=/dev/ttyACM0

all: build flash

build:
	arduino-cli compile -b $(board) $(sketch)

flash:
	#FIXME: Open port and set baud rate to 1200 to start Arduino bootloader
	$(bossac) --port=$(port) --force_usb_port=true -e -w -v -R $(sketch)/*bin

monitor:
	screen $(port)

copy:
	scp $(sketch)/*bin flip2:
	ssh flip2 -t scp *bin pi:Documents/$(sketch)
