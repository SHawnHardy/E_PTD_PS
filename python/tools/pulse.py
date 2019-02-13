#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file pulse.py
# @version v0.1
# @author SHawnHardy
# @date 2019-02-10
# @copyright MIT License
#########################

import matplotlib.pyplot as plt
import numpy as np
import pandas as pd


class Pulse:
    def __init__(self, df=None, df_path=None):
        if df:
            self.df = df
        elif df_path:
            self.df = pd.read_csv(df_path)

    def isi(self):
        log = {}
        for index, row in self.df.iterrows():
            neuron = int(row['neuron'])
            if neuron not in log:
                log[neuron] = [-1, row['time(s)'], 0.0]

            log[neuron][-1] = row['time(s)']
            log[neuron][0] += 1

        result = 0.0
        log_num = 0
        for _, value in log.items():
            if value[0]:
                result += value[-1] - value[-2]
                log_num += value[0]
        return result / log_num if log_num else np.nan

    def plot(self):
        plt.figure()
        self.df.plot.scatter(x='neuron', y='time(s)', s=2)
        plt.show()
