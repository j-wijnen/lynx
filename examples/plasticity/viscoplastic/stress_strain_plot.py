import pandas as pd
import matplotlib.pyplot as plt

classical = pd.read_csv("classical_data.csv")

plt.plot(classical["strain"], classical["stress"], label="Classical", color="black")

viscoplastic_files = ["1s", "01s", "001s", "0005s", "00025s"]

for i, viscoplastic_file in enumerate(viscoplastic_files):
    viscoplastic = pd.read_csv(f"viscoplastic_{viscoplastic_file}.csv")
    plt.plot(viscoplastic["strain"], viscoplastic["stress"], linestyle="--", color=f"C{i}", label=viscoplastic_file)

plt.xlabel("Strain")
plt.ylabel("Stress")

plt.legend(ncols=3)
plt.show()
