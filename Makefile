board=adafruit:samd:adafruit_feather_m0
sketch=sapflow_protothread
#sketch=mcp3424_code
bossac=/home/marshal/.arduino15/packages/arduino/tools/bossac/1.8.0*/bossac
port=/dev/ttyACM0
read_fault=$(HOME)/Arduino/libraries/FeatherFault/tools/recover_fault/recover_fault.py

all: build flash

build:
	arduino-cli compile -b $(board) $(sketch)

flash:
	#FIXME: Open port and set baud rate to 1200 to start Arduino bootloader
	$(bossac) --port=$(port) --offset=0x2000 -e -w -v -R $(sketch)/*bin

.PHONY: monitor
monitor:
	screen $(port)

copy:
	scp $(sketch)/*bin flip2:
	ssh flip2 -t scp *bin pi:Documents/$(sketch)

# Read the latest fault from the board
.PHONY: recover
recover:
	python $(read_fault) recover $(port) --bossac-path $(bossac)

.PHONY: docs
docs:
	doxygen $(sketch)/Doxyfile

clean:
	rm $(sketch)/*.bin
	rm $(sketch)/*.elf
