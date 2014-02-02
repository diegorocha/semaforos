#!/usr/bin/env python
import random

def createMatrix(ordem):
		m = []
		r = random
		for i in range(ordem):
			l = []
			for j in range(ordem):	
				l.append(round(r.random() * 10, 2))
			m.append(l)
		return m
		
def createFile(i, a, b):
	filename = 'bin/f%d' % i
	f = open(filename, 'w')
	f.write(str(len(a)))
	f.write('\n')
	for l in a:
		for e in l:
			f.write(str(e))
			f.write(' ')
		f.write('\n')
	for l in b:
		for e in l:
			f.write(str(e))
			f.write(' ')
		f.write('\n')
	f.close()

for i in range(25):
	ordem = 3
	a = createMatrix(ordem)
	b = createMatrix(ordem)
	createFile(i, a, b)
