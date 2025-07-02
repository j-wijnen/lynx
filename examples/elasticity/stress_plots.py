import pandas as pd
import matplotlib.pyplot as plt 

smallstrain = pd.read_csv('smallstrain/smallstrain_force.csv')
stvenantkirchhoff = pd.read_csv('stvenantkirchhoff/stvenantkirchhoff_force.csv')
hencky = pd.read_csv('hencky/hencky_force.csv')

plt.figure()
plt.plot(smallstrain["disp"], smallstrain["stress"])
plt.plot(stvenantkirchhoff["disp"], stvenantkirchhoff["stress"])
plt.plot(hencky["disp"], hencky["stress"])

# plt.figure()
# plt.plot(smallstrain["strain"], smallstrain["stress"])
# plt.plot(stvenantkirchhoff["strain"], stvenantkirchhoff["stress"])
# plt.plot(hencky["strain"], hencky["stress"])

plt.show()