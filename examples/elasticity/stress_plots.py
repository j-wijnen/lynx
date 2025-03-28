import numpy as np 
import matplotlib.pyplot as plt 

smallstrain = np.loadtxt('smallstrain/smallstrain_force.csv', skiprows=1, delimiter=',')

plt.plot(smallstrain[:,0], smallstrain[:,1])
plt.show()