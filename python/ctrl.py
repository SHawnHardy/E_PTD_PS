#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file ctrl.py
# @version v0.1
# @author SHawnHardy
# @date 2019-02-10
# @copyright MIT License
#########################

import numpy as np
import pandas as pd


class Ctrl:
    num_times = 10
    count = []
    task = []
    df_index = []
    result = []
    num_processed = 0

    def __init__(self, df, param_label, result_label, csv_path):
        self.df = df
        self.param_label = param_label
        self.result_label = result_label
        self.csv_path = csv_path

    def get_task(self):
        task_index = 0
        for index, row in self.df.iterrows():
            if np.any(np.isnan(row[self.result_label])):
                self.task.append(row[self.param_label])
                self.df_index.append(index)
                self.result.append([])
                self.count.append(0)
                yield task_index, self.task[-1]
                task_index += 1

    def proc(self, index):
        print(self.task[index])
        result = pd.DataFrame(self.result[index])
        print(result)
        result = np.mean(result)
        print(result)
        for label, data in result.iteritems():
            self.df.loc[self.df_index[index]][label] = data

        self.df = self.df.round(6)
        self.df.to_csv(self.csv_path, index=False, float_format='%.6f')
        self.num_processed += 1

    def add(self, index, result):
        self.result[index].append(result)
        self.count[index] += 1
        if self.count[index] == self.num_times:
            self.proc(index)
        return self.num_processed == len(self.task)
