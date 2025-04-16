import subprocess

times = [1, 0.1, 0.01, 0.005, 0.0025]

with open("viscoplastic.i", 'r') as file:
    lines = file.readlines()

for itime in times:
    a = str(itime).replace(".", "")
    lines[0] = f"csv_file = 'viscoplastic_{a}s'\n"
    lines[4] = f"  end_time = {itime}\n"

    with open("viscoplastic.i", 'w') as file:
        file.writelines(lines)

    cmd = "~/codes/lynx/lynx-opt -i viscoplastic.i"
    p = subprocess.run(cmd, shell=True)