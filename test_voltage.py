#!/usr/bin/python

import RPi.GPIO as GPIO
import time

def setup():
	GPIO.cleanup()
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(3,GPIO.IN,GPIO.PUD_DOWN)

def main():	
	while (GPIO.input(3) == 0):
		continue
	print "Pin 2 high"

if __name__ == "__main__":
	setup()
	main()
