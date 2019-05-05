#!/usr/bin/env python3
# -*- coding: utf-8 -*-
#
#########################
# file plot.py
# @version v0.1
# @author SHawnHardy
# @date 2019-05-04
# @copyright MIT License
#########################

from tools import config

import matplotlib.pyplot as plt
import pandas as pd

# Q_D
csv_path = config.data_path + '/Q_D.csv'
df = pd.read_csv(csv_path)
df.plot(x='noise intensity', y='Q', legend=False)
plt.ylim(0.0, 0.7)
plt.xlabel('Noise Intensity D')
plt.ylabel('Response Intensity Q')
plt.plot(0.015000, 0.619, marker='o', color='orange', markersize=8)
plt.text(0.015000, 0.629, 'D=0.015, Q=0.619')
plt.plot(0.160000, 0.263, marker='o', color='orange', markersize=8)
plt.text(0.160000, 0.273, 'D=0.160, Q=0.263')
plt.show()

df.plot(x='noise intensity', y='ISI', legend=False)
plt.ylim(0.0, 16.0)
plt.xlabel('Noise Intensity D')
plt.ylabel('ISI')
plt.plot(0.015000, 6.0, marker='o', color='orange', markersize=8)
plt.text(0.015000, 6.3, 'D=0.015, ISI=6.000')
plt.plot(0.160000, 3.0, marker='o', color='orange', markersize=8)
plt.text(0.160000, 3.3, 'D=0.160, ISI=3.000')
plt.show()

# Q_Tau_0.015
time_delay = [x * 0.1 for x in range(301)]
partial_time_delay_pr = [0.01, 0.03, 0.05, 0.1, 0.3, 0.5]
csv_path = config.data_path + '/Q_Tau_P_D0.0150.csv'
df = pd.read_csv(csv_path)

Q_df = {'time delay': time_delay}
ISI_df = {'time delay': time_delay}
cnt = 0
for P in partial_time_delay_pr:
    cnt += 1
    Q = (df.loc[df["partial time delay probability"] == P, "Q"]).reset_index(drop=True)
    ISI = (df.loc[df["partial time delay probability"] == P, "ISI"]).reset_index(drop=True)

    plt.figure(3)
    plt.subplot(2, 3, cnt)
    plt.plot(time_delay, Q)
    plt.xlim(0.0, 30.0)
    plt.ylim(0.0, 0.7)
    plt.title('D=0.015,P=%.2f' % (P,))
    plt.xlabel('Time Delay Value (s)')
    plt.ylabel('Response Intensity Q')

    plt.figure(4)
    plt.subplot(2, 3, cnt)
    plt.plot(time_delay, ISI)
    plt.xlim(0.0, 30.0)
    plt.ylim(0.0, 16.0)
    plt.title('D=0.015,P=%.2f' % (P,))
    plt.xlabel('Time Delay Value (s)')
    plt.ylabel('ISI')

    Q_df['P=%.6f' % (P,)] = Q
    ISI_df['P=%.6f' % (P,)] = ISI

Q_df = pd.DataFrame(Q_df)
ISI_df = pd.DataFrame(ISI_df)

Q_df.plot(x='time delay')
plt.ylim(0.0, 0.7)
plt.xlabel('Time Delay Value (s)')
plt.ylabel('Response Intensity Q')
plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))

ISI_df.plot(x='time delay')
plt.ylim(0.0, 16.0)
plt.xlabel('Time Delay Value (s)')
plt.ylabel('ISI')
plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))
plt.show()

# Q_Tau_0.160
time_delay = [x * 0.1 for x in range(301)]
partial_time_delay_pr = [0.01, 0.03, 0.05, 0.1, 0.3, 0.5]
csv_path = config.data_path + '/Q_Tau_P_D0.1600.csv'
df = pd.read_csv(csv_path)

Q_df = {'time delay': time_delay}
ISI_df = {'time delay': time_delay}
cnt = 0
for P in partial_time_delay_pr:
    cnt += 1
    Q = (df.loc[df["partial time delay probability"] == P, "Q"]).reset_index(drop=True)
    ISI = (df.loc[df["partial time delay probability"] == P, "ISI"]).reset_index(drop=True)

    plt.figure(7)
    plt.subplot(2, 3, cnt)
    plt.plot(time_delay, Q)
    plt.xlim(0.0, 30.0)
    plt.ylim(0.0, 0.4)
    plt.title('D=0.160,P=%.2f' % (P,))
    plt.xlabel('Time Delay Value (s)')
    plt.ylabel('Response Intensity Q')

    plt.figure(8)
    plt.subplot(2, 3, cnt)
    plt.plot(time_delay, ISI)
    plt.xlim(0.0, 30.0)
    plt.ylim(0.0, 5.0)
    plt.title('D=0.160,P=%.2f' % (P,))
    plt.xlabel('Time Delay Value (s)')
    plt.ylabel('ISI')

    Q_df['P=%.6f' % (P,)] = Q
    ISI_df['P=%.6f' % (P,)] = ISI

Q_df = pd.DataFrame(Q_df)
ISI_df = pd.DataFrame(ISI_df)

Q_df.plot(x='time delay')
plt.ylim(0.0, 0.4)
plt.xlabel('Time Delay Value (s)')
plt.ylabel('Response Intensity Q')
plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))

ISI_df.plot(x='time delay')
plt.ylim(0.0, 5.0)
plt.xlabel('Time Delay Value (s)')
plt.ylabel('ISI')
plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))
plt.show()

# Q_N_0.015
normal_distribution_mean = [2.0 + x * 0.1 for x in range(231)]
normal_distribution_standard_deviation = [1.0, 0.8, 0.5]
partial_time_delay_pr = [0.1, 0.3, 0.5, 0.7]
csv_path = config.data_path + '/Q_N_gauss_D0.0150.csv'
df = pd.read_csv(csv_path)
# old_df = pd.read_csv(csv_path + '.old')

for SD in normal_distribution_standard_deviation:
    t_df = df.loc[df["normal distribution standard deviation"] == SD]
    Q_df = {'normal distribution mean': normal_distribution_mean}
    ISI_df = {'normal distribution mean': normal_distribution_mean}
    for P in partial_time_delay_pr:
        Q_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "Q"]).reset_index(drop=True)
        ISI_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "ISI"]).reset_index(drop=True)

    # Q_df['P=%.6f' % (1.0,)] = (old_df.loc[old_df["normal distribution standard deviation"] == SD, "Q"]).reset_index(
    #     drop=True)
    # ISI_df['P=%.6f' % (1.0,)] = (old_df.loc[old_df["normal distribution standard deviation"] == SD, "ISI"]).reset_index(
    #     drop=True)

    Q_df = pd.DataFrame(Q_df)
    ISI_df = pd.DataFrame(ISI_df)

    Q_df.plot(x='normal distribution mean')
    plt.ylim(0.0, 0.7)
    plt.xlabel('Gaussion Distribution Mean')
    plt.ylabel('Response Intensity Q')
    plt.title('D=0.015,$\sigma$=%.2f' % (SD,))
    plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))

    ISI_df.plot(x='normal distribution mean')
    plt.ylim(0.0, 45.0)
    plt.xlabel('Gaussion Distribution Mean')
    plt.ylabel('ISI')
    plt.title('D=0.015,$\sigma$=%.2f' % (SD,))
    plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))
    plt.show()

# Q_N_0.160
normal_distribution_mean = [2.0 + x * 0.1 for x in range(231)]
normal_distribution_standard_deviation = [1.0, 0.8, 0.5]
partial_time_delay_pr = [0.1, 0.3, 0.5, 0.7]
csv_path = config.data_path + '/Q_N_gauss_D0.1600.csv'
df = pd.read_csv(csv_path)
# old_df = pd.read_csv(csv_path + '.old')

for SD in normal_distribution_standard_deviation:
    t_df = df.loc[df["normal distribution standard deviation"] == SD]
    Q_df = {'normal distribution mean': normal_distribution_mean}
    ISI_df = {'normal distribution mean': normal_distribution_mean}
    for P in partial_time_delay_pr:
        Q_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "Q"]).reset_index(drop=True)
        ISI_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "ISI"]).reset_index(drop=True)

    # Q_df['P=%.6f' % (1.0,)] = (old_df.loc[old_df["normal distribution standard deviation"] == SD, "Q"]).reset_index(
    #     drop=True)
    # ISI_df['P=%.6f' % (1.0,)] = (old_df.loc[old_df["normal distribution standard deviation"] == SD, "ISI"]).reset_index(
    #     drop=True)

    Q_df = pd.DataFrame(Q_df)
    ISI_df = pd.DataFrame(ISI_df)

    Q_df.plot(x='normal distribution mean')
    plt.ylim(0.0, 0.4)
    plt.xlabel('Gaussion Distribution Mean')
    plt.ylabel('Response Intensity Q')
    plt.title('$D=0.160,\sigma$=%.2f' % (SD,))
    plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))

    ISI_df.plot(x='normal distribution mean')
    plt.ylim(0.0, 10.0)
    plt.xlabel('Gaussion Distribution Mean')
    plt.ylabel('ISI')
    plt.title('D=0.160,$\sigma$=%.2f' % (SD,))
    plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))
    plt.show()

# Q_U_0.015
uniform_distribution_mean = [2.0 + x * 0.1 for x in range(231)]
uniform_distribution_standard_deviation = [1.0, 0.8, 0.5]
partial_time_delay_pr = [0.1, 0.3, 0.5, 0.7]
csv_path = config.data_path + '/Q_U_D0.0150.csv'
df = pd.read_csv(csv_path)

for SD in uniform_distribution_standard_deviation:
    t_df = df.loc[df["uniform distribution standard deviation"] == SD]
    Q_df = {'uniform distribution mean': uniform_distribution_mean}
    ISI_df = {'uniform distribution mean': uniform_distribution_mean}
    for P in partial_time_delay_pr:
        Q_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "Q"]).reset_index(drop=True)
        ISI_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "ISI"]).reset_index(drop=True)

    Q_df = pd.DataFrame(Q_df)
    ISI_df = pd.DataFrame(ISI_df)

    Q_df.plot(x='uniform distribution mean')
    plt.ylim(0.0, 0.7)
    plt.xlabel('Uniform Distribution Mean')
    plt.ylabel('Response Intensity Q')
    plt.title('$D=0.015,\sigma$=%.2f' % (SD,))
    plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))

    ISI_df.plot(x='uniform distribution mean')
    plt.ylim(0.0, 20.0)
    plt.xlabel('Uniform Distribution Mean')
    plt.ylabel('ISI')
    plt.title('$D=0.015,\sigma$=%.2f' % (SD,))
    plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))
    plt.show()

# Q_U_0.160
uniform_distribution_mean = [2.0 + x * 0.1 for x in range(231)]
uniform_distribution_standard_deviation = [1.0, 0.8, 0.5]
partial_time_delay_pr = [0.1, 0.3, 0.5, 0.7]
csv_path = config.data_path + '/Q_U_D0.1600.csv'
df = pd.read_csv(csv_path)

for SD in uniform_distribution_standard_deviation:
    t_df = df.loc[df["uniform distribution standard deviation"] == SD]
    Q_df = {'uniform distribution mean': uniform_distribution_mean}
    ISI_df = {'uniform distribution mean': uniform_distribution_mean}
    for P in partial_time_delay_pr:
        Q_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "Q"]).reset_index(drop=True)
        ISI_df['P=%.6f' % (P,)] = (t_df.loc[t_df["partial time delay probability"] == P, "ISI"]).reset_index(drop=True)

    Q_df = pd.DataFrame(Q_df)
    ISI_df = pd.DataFrame(ISI_df)

    Q_df.plot(x='uniform distribution mean')
    plt.ylim(0.0, 0.3)
    plt.xlabel('Uniform Distribution Mean')
    plt.ylabel('Response Intensity Q')
    plt.title('$D=0.160,\sigma$=%.2f' % (SD,))
    plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))

    ISI_df.plot(x='uniform distribution mean')
    plt.ylim(0.0, 5.0)
    plt.xlabel('Uniform Distribution Mean')
    plt.ylabel('ISI')
    plt.title('$D=0.160,\sigma$=%.2f' % (SD,))
    plt.legend(loc='upper left', bbox_to_anchor=(1, 0, 0.5, 1))
    plt.show()
