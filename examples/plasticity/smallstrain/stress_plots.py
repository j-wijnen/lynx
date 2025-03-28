import numpy as np 
import matplotlib.pyplot as plt 

data = np.loadtxt('homogeneous_force.csv', skiprows=1, delimiter=',')

plt.plot(data[:,0], data[:,1])
plt.show()