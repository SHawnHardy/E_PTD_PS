#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_Tau_P.py
# @version v0.1
# @author SHawnHardy
# @date 2019-02-14
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

noise_intensity = 0.015
time_delay = [x * 100 for x in range(301)]
# partial_time_delay_pr = [x * 0.01 for x in range(11)] + [x * 0.05 for x in range(3, 11)]
partial_time_delay_pr = [0.01, 0.03, 0.05, 0.1, 0.3, 0.5]

try:
    df = pd.read_csv(config.data_path + '/Q_Tau_P.csv')
except FileNotFoundError:
    print("Q_Tau_P.csv not found. It will be created")
    time_delay_m, partial_time_delay_pr_m = np.meshgrid(time_delay, partial_time_delay_pr)
    partial_time_delay_pr_m = list(chain(*partial_time_delay_pr_m))
    time_delay_m = list(chain(*time_delay_m))
    df = pd.DataFrame({'noise intensity': noise_intensity,
                       'time delay': time_delay_m,
                       'partial time delay probability': partial_time_delay_pr_m,
                       'Q': np.nan
                       })

df = df.round(6)
ctrl = Ctrl(df, ['noise intensity', 'time delay', 'partial time delay probability'], ['Q'],
            config.data_path + '/Q_Tau_P.csv')
ctrl.num_times = 10

manager = multiprocessing.Manager()
queue = manager.Queue()


def solve(info):
    _index, _task = info
    result = subprocess.check_output([
        config.solve_command,
        '-D' + str(_task['noise intensity']),
        '-T' + str(_task['time delay']),
        '-P' + str(_task['partial time delay probability'])
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
for P in partial_time_delay_pr:
    ndf['P=%.6f' % (P,)] = (df.loc[df["partial time delay probability"] == P, "Q"]).reset_index(drop=True)

ndf = pd.DataFrame(ndf)

ndf.plot(x="time delay")
plt.legend(loc='best')
plt.show()
