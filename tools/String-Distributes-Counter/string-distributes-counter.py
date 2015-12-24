#!/usr/bin/python

import os
import os.path
import codecs

rootdir = "data"
charFreq = {}

def countChars(fileName):
	f = codecs.open(fileName, 'r', 'utf-16-le')
	for line in f:
		for i, ch in enumerate(line):
			val = charFreq.setdefault(ch, 0)
			charFreq[ch] += 1

	f.close()

for parent, dirnames, filenames in os.walk(rootdir):
	for fileName in filenames:
			countChars(os.path.join(parent, fileName))

print charFreq

