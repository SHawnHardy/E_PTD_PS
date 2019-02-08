#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file Q_D.py
# @version v0.3
# @author SHawnHardy
# @date 2019-02-08
# @copyright MIT License
#########################

import config

import matplotlib.pyplot as plt
import multiprocessing
import numpy as np
import pandas as pd
import subprocess

try:
    df = pd.read_csv(config.data_path + 'Q_D.csv')
except FileNotFoundError:
    print("Q_D.csv not found. It will be created")
    df = pd.DataFrame({'noise intensity': [x * 0.001 for x in range(1, 101)],
                       'Q': np.nan
                       })

df = df.round(6)
task = list(df.iloc[np.where(np.isnan(df))[0], 0])
len_task = len(task)
print(task)
num_times = 10

count = [0 for i in range(len_task)]
result = [[] for i in range(len_task)]
wait_for_proc = []
current_pointer = 0


def proc(index):
    global df, result
    result[index] = np.mean(result[index])
    print("%f:%f" % (task[index], result[index]))

    df.iloc[np.where(df == task[index])[0], 1] = result[index]
    df = df.round(6)
    df.to_csv(config.data_path + 'Q_D.csv', float_format='%.6f', index=False)


def finish():
    plt.figure()
    df.plot(x='noise intensity', y='Q')
    plt.legend(loc='best')
    plt.show()


def add(index, rst):
    global count, result, wait_for_proc, current_pointer
    result[index].append(rst)
    count[index] += 1

    if count[index] == num_times:
        if current_pointer == index:
            proc(index)
            current_pointer += 1
            while current_pointer < len_task and wait_for_proc and current_pointer == wait_for_proc[0]:
                proc(wait_for_proc.pop())
                current_pointer += 1
            if current_pointer == len_task:
                finish()
                return -1
        else:
            wait_for_proc.append(index)
            wait_for_proc.sort()

    return 0


if not task:
    finish()
    exit(0)

lock = multiprocessing.Lock()
manager = multiprocessing.Manager()
queue = manager.Queue()


def solve(info):
    index, label = info
    result = subprocess.check_output([config.solve_command, '-D ' + str(label)])
    queue.put((index, float(result)))


pool = multiprocessing.Pool(4)

for i in range(len_task):
    for j in range(num_times):
        pool.apply_async(solve, ((i, task[i]),))

pool.close()

while True:
    t = queue.get()
    if add(*t) == -1:
        break
