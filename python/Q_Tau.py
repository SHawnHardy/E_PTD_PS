#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_Tau.py
# @version v0.1
# @author SHawnHardy
# @date 2019-02-10
# @copyright MIT License
#########################

from tools import config
from tools.ctrl import Ctrl

from itertools import chain
import multiprocessing
import matplotlib.pyplot as plt
import numpy as np
import pandas as pd
import subprocess

noise_intensity = [0.015, 0.134, 0.160]
time_delay = [x * 100 for x in range(301)]

try:
    df = pd.read_csv(config.data_path + '/Q_Tau.csv')
except FileNotFoundError:
    print("Q_D.csv not found. It will be created")
    time_delay_m, noise_intensity_m = np.meshgrid(time_delay, noise_intensity)
    noise_intensity_m = list(chain(*noise_intensity_m))
    time_delay_m = list(chain(*time_delay_m))
    df = pd.DataFrame({'noise intensity': noise_intensity_m,
                       'time delay': time_delay_m,
                       'Q': np.nan
                       })

df = df.round(6)
ctrl = Ctrl(df, ['noise intensity', 'time delay'], ['Q'], config.data_path + '/Q_Tau.csv')
ctrl.num_times = 10

manager = multiprocessing.Manager()
queue = manager.Queue()


def solve(info):
    _index, _task = info
    result = subprocess.check_output([
        config.solve_command,
        '-D' + str(_task['noise intensity']),
        '-T' + str(_task['time delay'])
    ])
    queue.put((_index, pd.Series({'Q': float(result)})))


pool = multiprocessing.Pool(config.num_processes)

no_task_left = True
for index, task in ctrl.get_task():
    no_task_left = False
    for i in range(ctrl.num_times):
        pool.apply_async(solve, ((index, task),))

pool.close()

while not no_task_left:
    t = queue.get()
    no_task_left = ctrl.add(*t)

df = ctrl.df
ndf = {'time delay': time_delay}
for D in noise_intensity:
    ndf['D=%.6f' % (D,)] = (df.loc[df["noise intensity"] == D, "Q"]).reset_index(drop=True)

ndf = pd.DataFrame(ndf)

ndf.plot(x="time delay")
plt.legend(loc='best')
plt.show()