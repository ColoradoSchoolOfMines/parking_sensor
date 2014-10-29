#!/usr/bin/python
import os
import sys


alpha = .1

inputFile = sys.argv[1]
outputFile = sys.argv[2]

while alpha > .001:
	for threshold in range(4, 15):
		command = "./a.out %f %i %s >> %s" % (alpha, threshold, inputFile, outputFile)
		os.system(command)
	alpha -= .001





