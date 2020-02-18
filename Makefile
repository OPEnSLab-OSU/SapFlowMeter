board=adafruit:samd:adafruit_feather_m0
sketch=sapflow_protothread

all:
	arduino-cli compile -b $(board) $(sketch)

flash:
	arduino-cli upload -p /dev/ttyACM0 -b $(board) $(sketch)

copy:
	scp $(sketch)/*bin flip2:
	ssh flip2 -t scp *bin pi:Documents/$(sketch)
