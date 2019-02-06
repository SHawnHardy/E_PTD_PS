#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file config.py
# @version v0.2
# @author SHawnHardy
# @date 2019-02-05
# @copyright MIT License
#########################

solve_command = '../build/solve'


class Ctrl:
    label = []
    count = []
    result = []
    num_times = 10
    wait_for_proc = [-1]
    current_pointer = 0

    def __init__(self, size):
        for i in range(size):
            self.label.append(i * 0.001)
            self.count.append(0)
            self.result.append([])

    def add(self, index, result):
        self.result[index].append(result)
        self.count[index] += 1
        print(self.count)

        if self.count[index] == self.num_times:
            if self.current_pointer == index:
                self.proc(index)
                self.current_pointer += 1
                while self.current_pointer < len(self.label) and self.current_pointer == self.wait_for_proc[-1]:
                    self.proc(self.wait_for_proc.pop())
                    self.current_pointer += 1
                if self.current_pointer == len(self.label):
                    self.finish()
            else:
                self.wait_for_proc.append(index)
                self.wait_for_proc.sort()

    def proc(self, index):
        print(self.result[index])

    def finish(self):
        print(len(self.label))
