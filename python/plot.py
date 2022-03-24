#! python3

"""
Author: Yuan Xianbin
Date: 2022-3-21
Version: 1.0.0

Target:
1.Generate time dalay information plot from the binary file
2.Generate lost, disorder, repeated frame information plot from the binary file
"""

import struct, numpy
from matplotlib import ticker
import os, re, time
import matplotlib.pyplot as plt
from statistics import mean
from datetime import datetime

curr_seq = []
total_loss = []
total_disorder = []
total_repeat = []
curr_loss = []
curr_disorder = []
curr_repeat = []
total_frame = []
delay_time = []
time_jump = []
curr_time = []
local_time = []
over_second = []
mcu_time = []
want_tick = []

class read_file:
    def __init__(self):
        self.bin_path = "./file/adcanif"
        self.f_exist = 0

    def file_exist(self):
        if not(os.path.exists(self.bin_path)):
            print("binary file don't exists\n")
            exit(-1)
        else:
            return 1
    
    def file_read(self):
        self.f_exist = self.file_exist()
        if self.f_exist == 1:
            file = open(self.bin_path, 'rb')
            size = os.path.getsize(self.bin_path)
            index = 0
            while True:
                content = file.read(60)
                index += 1
                if(size == file.tell()):
                    break
                if(content != b''):
                    tmp = struct.unpack('=8Iq1I2Q', content)
                    curr_seq.append(tmp[0])
                    total_loss.append(tmp[1])
                    total_disorder.append(tmp[2])
                    total_repeat.append(tmp[3])
                    #if(tmp[0] % 9):
                        #curr_loss.append(2)
                    #else:
                    curr_loss.append(tmp[4])
                    #if(tmp[0] % 9):
                        #curr_disorder.append(3)
                    #else:
                    curr_disorder.append(tmp[5])
                    #if(tmp[0] % 9):
                        #curr_repeat.append(1)
                    #else:
                    curr_repeat.append(tmp[6])
                    total_frame.append(tmp[7])
                    #delay_time.append(tmp[8]/1000)
                    over_second.append(tmp[8]/1000000000)
                    if(tmp[8] > 25000000):
                        delay_time.append(25000000/1000000)
                    elif(tmp[8] < 0):
                        delay_time.append(-25000000/1000000)
                    else:
                        delay_time.append(tmp[8]/1000000)
                    time_jump.append(tmp[9])
                    curr_time.append(tmp[10])
                    if(tmp[8] < 0):
                        want_tick.append(len(over_second))
                    mcu_time.append(tmp[11])
                    #timestamp = tmp[10]/1000000000
                    #time_local = time.localtime(timestamp)
                    time_local = datetime.fromtimestamp(tmp[10]/1000000000)
                    dt = time_local.strftime("%H:%M:%S.%f")
                    local_time.append(dt)
                    #print("timestamp: ", tmp[10])
                    #print("local_time: ", dt)
                else:
                    break
            file.close()

    def draw_plot(self):
        ax1 = plt.subplot(2, 2, 1)
        #plt.xlim(min(local_time), max(local_time))
        #plt.ylim(-0.5, max(curr_disorder))
        plt.xlabel("Local time")
        plt.ylabel("Number")
        ax1.xaxis.set_major_locator(ticker.MultipleLocator(10000))
        plt.xticks(rotation = 45)
        plt.scatter(local_time, curr_disorder, marker='x', color='blue', label='disorder')
        plt.scatter(local_time, curr_loss, marker='+', color='green', label='lost')
        plt.scatter(local_time, curr_repeat, marker='o', color='red', label='repeat')
        plt.plot(local_time, total_loss, color='green', linewidth=1.0, linestyle='-')
        plt.plot(local_time, total_disorder, color='blue', linewidth=1.0, linestyle='-')
        plt.plot(local_time, total_repeat, color='red', linewidth=1.0, linestyle='-')
        #plt.plot(local_time, time_jump, color='black', linewidth=1.0, linestyle='-')
        plt.legend(loc='upper right')
        print("len(local_time):    ", len(local_time))
        print("len(curr_disorder): ", len(curr_disorder))
        print("total_loss: ", max(total_loss))
        print("total_disorder: ", max(total_disorder))
        print("total_repeat: ", max(total_repeat))
        #plt.plot(local_time, curr_disorder, color='red', linewidth=1.0, linestyle='-')

        plt.subplot(2, 2, 2)
        plt.xlim(min(curr_seq), max(curr_seq))
        plt.ylim(min(over_second), max(over_second))
        plt.xlabel("Sequence")
        plt.ylabel("Over sencond(S)")
        plt.plot(curr_seq, over_second, color='purple', linewidth=1.0, linestyle='-')

        plt.subplot(2, 2, 3)
        plt.xlabel("Sequence")
        plt.ylabel("Timestamp")
        mcu_show = []
        soc_show = []
        x_seq_show = []
        if(len(want_tick) > 0):
            mcu_show = [mcu_time[i] for i in range((want_tick[0]-10), (want_tick[0]+10))]
            soc_show = [curr_time[i] for i in range((want_tick[0]-10), (want_tick[0]+10))]
            x_seq_show = [curr_seq[i] for i in range((want_tick[0]-10), (want_tick[0]+10))]
        #plt.plot(x_seq_show, soc_show, color='brown', linewidth=1.0, linestyle='-')
        plt.plot(x_seq_show, mcu_show, color='orange', linewidth=1.0, linestyle='-')

        plt.subplot(2, 2, 4)
        plt.xlim(min(curr_seq), max(curr_seq))
        print("curr_seq: max - ", max(curr_seq))
        print("curr_seq: min - ", min(curr_seq))
        plt.ylim(min(delay_time)-1, max(delay_time))
        print("delay_time: max - ", max(over_second))
        print("delay_time: min - ", min(over_second))
        print("delay_time: percentile %99 - ", numpy.percentile(over_second, 99))
        print("delay_time: mean - ", mean(over_second))
        print("time_jump: max - ", max(time_jump))
        plt.xlabel("Sequence")
        plt.ylabel("Time delay(ms)")
        #plt.yticks([mean(delay_time)], ["average"])
        plt.plot(curr_seq, delay_time, color='pink', linewidth=1.0, linestyle='-')
        plt.axhline(y=(mean(over_second)*1000), c='b', ls='-', lw=1)
        plt.axhline(y=(numpy.percentile(over_second, 99)*1000), c='g', ls='-', lw=1)
        plt.show()

if __name__ == "__main__":
    read_obj = read_file()
    read_obj.file_exist()
    read_obj.file_read()
    read_obj.draw_plot()
