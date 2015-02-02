#!/bin/bash
sudo cu -h -l /dev/tty.usbmodem214151 -s 9600  
