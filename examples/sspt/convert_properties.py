import numpy as np
import csv

with open("x60#1_thermal.inp", 'r') as file:
    line = ""

    while line[:8] != "*density":
        line = file.readline()

    nvalues = int(file.readline())

    density = np.empty((nvalues,3))

    for ivalue in range(nvalues):
        density[ivalue] = file.readline().split()

    density = np.column_stack((density[:,2], density[:,0], density[:,0], density[:,0], density[:,0], density[:,1]))

    file.seek(0)

    while line[:13] != "*conductivity":
        line = file.readline()

    nvalues = int(file.readline())

    conductivity = np.empty((nvalues,3))

    for ivalue in range(nvalues):
        conductivity[ivalue] = file.readline().split()

conductivity = np.column_stack((conductivity[:,2], conductivity[:,0], conductivity[:,0], conductivity[:,0], conductivity[:,0], conductivity[:,1]))

with open("density.csv", 'w') as file:
    writer = csv.writer(file)

    writer.writerow(["temperature", "ferrite", "pearlite", "bainite", "martensite", "austenite"])
    for idata in density:
        writer.writerow(idata)

with open("conductivity.csv", 'w') as file:
    writer = csv.writer(file)

    writer.writerow(["temperature", "ferrite", "pearlite", "bainite", "martensite", "austenite"])
    for idata in conductivity:
        writer.writerow(idata)

with open("properties.csv", 'w') as file:
    writer = csv.writer(file)

    writer.writerow(["temperature", 
        "density_ferrite", "density_pearlite", "density_bainite", "density_martensite", "density_austenite",
        "thermal_conductivity_ferrite", "thermal_conductivity_pearlite", "thermal_conductivity_bainite", "thermal_conductivity_martensite", "thermal_conductivity_austenite"])
    for idensity, iconductivity in zip(density, conductivity):
        writer.writerow(np.concatenate((idensity, iconductivity[1:])))
