#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_D_ISI.py
# @version v0.1
# @author SHawnHardy
# @date 2019-02-10
# @copyright MIT License
#########################

from tools import config
from tools.ctrl import Ctrl
from tools.pulse import Pulse

import matplotlib.pyplot as plt
import multiprocessing
import numpy as np
import os
import pandas as pd
import progressbar as pgb
import subprocess

if not os.path.exists(config.data_path + '/Q_D_ISI'):
    os.mkdir(config.data_path + '/Q_D_ISI')

try:
    df = pd.read_csv(config.data_path + '/Q_D_ISI/Q_D.csv')
except FileNotFoundError:
    print("Q_D.csv not found. It will be created")
    df = pd.DataFrame({'noise intensity': [x * 0.001 for x in range(301)],
                       'Q': np.nan
                       })

df = df.round(6)
ctrl = Ctrl(df, ['noise intensity'], ['Q'], config.data_path + '/Q_D_ISI/Q_D.csv')
ctrl.num_times = 10

manager = multiprocessing.Manager()
queue = manager.Queue()


def solve(info):
    _index, _task, _num = info
    result = subprocess.check_output(
        [config.solve_command,
         '-D' + str(_task['noise intensity']),
         '--log_pulse', config.data_path + '/Q_D_ISI/D_%.6f_%d_pulse.csv' % (_task['noise intensity'], _num),
         ]
    )
    queue.put((_index, pd.Series({'Q': float(result)})))


pool = multiprocessing.Pool(config.num_processes)
no_task_left = True
for index, task in ctrl.get_task():
    no_task_left = False
    for i in range(ctrl.num_times):
        pool.apply_async(solve, ((index, task, i),))

pool.close()

while not no_task_left:
    t = queue.get()
    no_task_left = ctrl.add(*t)

ctrl.df.plot(x='noise intensity', y='Q')

try:
    df = pd.read_csv(config.data_path + '/Q_D_ISI/ISI.csv')
except FileNotFoundError:
    print("ISI.csv not found. It will be created")
    df = pd.DataFrame({'noise intensity': [x * 0.001 for x in range(301)],
                       'ISI': np.nan
                       })

df = df.round(6)

bar = pgb.ProgressBar()

for index in bar(range(len(df))):
    if not np.isnan(df.loc[index, 'ISI']):
        continue
    isi = []
    for num in range(ctrl.num_times):
        p = Pulse(
            df_path=config.data_path + '/Q_D_ISI/D_%.6f_%d_pulse.csv' % (df.loc[index, 'noise intensity'], num))
        isi.append(p.isi())
    df.loc[index, 'ISI'] = np.nanmean(isi)
    df.to_csv(config.data_path + '/Q_D_ISI/ISI.csv', index=False, float_format='%.6f')


df.plot(x='noise intensity', y='ISI')
plt.show()