import numpy as np

def med(fstop1, fmed, fstop2):
    fpass1 = np.sqrt(fstop1*fmed)
    fpass2 = np.sqrt(fstop2*fmed)
    fmed1 = np.sqrt(fstop1*fstop2)

    print(f'{fmed} ({int(fmed1)})- Fstop1 = {int(fstop1)}, Fpass1 = {int(fpass1)}, Fpass2 = {int(fpass2)}, Fstop2 = {int(fstop2)}')

med(20, 32, 40)
med(40, 64, 80)
med(80, 125, 160)
med(160, 250, 300)
med(300, 500, 600)
med(600, 1000, 1200)
med(1200, 2000, 2400)
med(2400, 4000, 5000)
med(5000, 8000, 10000)
med(10000, 16000, 20000)