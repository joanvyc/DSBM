#!/usr/bin/python

import RPi.GPIO as GPIO
import time
import numpy as np

def setup():
	GPIO.cleanup()
	GPIO.setmode(GPIO.BCM)
	GPIO.setup(2,GPIO.OUT)
	GPIO.setup(3,GPIO.OUT)

def discharge_cap():
	GPIO.setup(3,GPIO.OUT)
	GPIO.output(3,False)
	GPIO.output(2,False)
	time.sleep(0.5)

def read_cap():
	time_ini = time.time()
	GPIO.setup(3,GPIO.IN)
	GPIO.output(2,True)
	while (GPIO.input(3) == 0):
		continue
	time_end = time.time()
	return (time_end - time_ini)

def timeToR(t):
	return (1000*1000*t)/(0.79*0.5520)

def main():
	

	acum = 0
	vals = []
	for i in range(60):
		discharge_cap()
		elapsed_time = read_cap()
		acum += elapsed_time
		vals.append(timeToR(elapsed_time))
		print "Time (us): " + str(elapsed_time*1000*1000)

	print "time Mean: " + str((acum*1000*1000)/60)
	print "R Mean : " +str(np.mean(vals))
	print "Standard deviation " + str(np.std(vals))

if __name__ == "__main__":
	setup()
	main()
