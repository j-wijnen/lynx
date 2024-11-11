import numpy as np
import csv

with open("x60#1_thermal.inp", 'r') as file:
	line = ""

	while line[:8] != "*density":
		line = file.readline()

	nvalues = int(file.readline())

	data = np.empty((nvalues,3))

	for ivalue in range(nvalues):
		data[ivalue] = file.readline().split()


data = np.column_stack((data[:,2], data[:,0], data[:,0], data[:,0], data[:,0], data[:,1]))

with open("density.csv", 'w') as file:
	writer = csv.writer(file)

	writer.writerow(["temperature", "ferrite", "pearlite", "bainite", "martensite", "austenite"])
	for idata in data:
		writer.writerow(idata)