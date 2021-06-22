import csv
import sys
import os
import matplotlib.pyplot as plt
import numpy as np

def parseFailCsv(filename):
    fail_path = []
    fail_fund = []
    indices = []
    maximum = 0
    with open(filename) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        line_count = 0
        for row in csv_reader:
            if line_count == 0:
                indices = [i for i, x in enumerate(row) if x == "average"]
                line_count += 1
            else:
                fail_path.append(row[indices[0]])
                fail_fund.append(row[indices[1]])
                line_count += 1
                if (maximum < float(row[indices[0]])):
                    maximum = float(row[indices[0]])
                if (maximum < float(row[indices[1]])):
                    maximum = float(row[indices[1]])
    return (fail_path, fail_fund, maximum)


def parseImbalanceCsv(filename):
    time = []
    imbalance = []
    indices = []
    with open(filename) as csv_file:
        csv_reader = csv.reader(csv_file, delimiter=',')
        line_count = 0
        for row in csv_reader:
            if line_count == 0:
                id_time = [i for i, x in enumerate(row) if x == "time"]
                indices = [i for i, x in enumerate(row) if x == "average"]
                line_count += 1
            else:
                time.append(row[id_time[0]])
                imbalance.append(row[indices[0]])
                line_count += 1
    return (time, imbalance)

def parseDir(dir):
    fail_path = []
    fail_fund = []
    imbalance = []
    imb_tag = []
    fail_tag = []
    time = []
    for filename in os.listdir(dir):
        if filename.endswith("_imbalance.csv"):
            (tm, imb) = parseImbalanceCsv(os.path.join(dir, filename))
            imbalance.append(imb)
            imb_tag.append(filename.split("_imbalance.csv")[0])
            time = tm
        else:
            continue
    y_max = 0
    for filename in os.listdir(dir):
        if filename.endswith("_fail.csv"):
            (fail_p, fail_f, fail_max) = parseFailCsv(os.path.join(dir, filename))
            if (fail_max > y_max):
                y_max = fail_max
            fail_path.append(fail_p)
            fail_fund.append(fail_f)
            fail_tag.append(filename.split("_fail.csv")[0])
        else:
            continue
    return (time, fail_tag, fail_path, fail_fund, imb_tag, imbalance, y_max)


def plot(dir, time, fail_tag, fail_path, fail_fund, imb_tag, imbalance,
         fail_max):
    time = [float(i) for i in time]
    for j, ff in enumerate(fail_fund):
        ff = [float(i) for i in fail_fund[j]]
        plt.plot(time, ff, label=fail_tag[j])
    plt.axis([0, int(time[len(time) - 1] + 1), 0, fail_max + 10])
    plt.xlabel('time')
    plt.ylabel('fail transmit')
    plt.legend()
    figure = plt.gcf()
    figure.set_size_inches(10.5, 5.5)
    plt.savefig(os.path.join(dir, 'fail.png'))
    plt.clf()
    for j, tag in enumerate(fail_tag):
        if 'opt' in tag:
            ff = []
            cp_tag = 'default_' + tag.split('_')[1]
            cp_idx = fail_tag.index(cp_tag)
            for k, opt_f in enumerate(fail_fund[j]):
                ff.append(float(fail_fund[cp_idx][k]) - float(opt_f))
            plt.plot(time, ff, label=tag.split('_')[1])
    plt.xlabel('time')
    plt.ylabel('fail transmit')
    plt.legend()
    figure = plt.gcf()
    figure.set_size_inches(10.5, 5.5)
    plt.savefig(os.path.join(dir, 'fail_compare.png'))
    plt.clf()
    for j, imb in enumerate(imbalance):
        imb = [float(i) for i in imbalance[j]]
        plt.plot(time, imb, label=imb_tag[j])
    plt.axis([0, int(time[len(time) - 1] + 1), 0, 1])
    plt.xlabel('time')
    plt.ylabel('channel imbalance')
    plt.legend()
    figure = plt.gcf()
    figure.set_size_inches(10.5, 5.5)
    plt.savefig(os.path.join(dir, 'imbalance.png'))


def outCsv(dir):
    (time, fail_tag, fail_path, fail_fund, imb_tag, imbalance, fail_max) = parseDir(dir)
    with open(os.path.join(dir, 'merge.csv'), 'w') as csv_file:
        writer = csv.writer(csv_file)
        tag = ['time'] + imb_tag + ['']
        tag = tag + ['time'] + fail_tag + ['']
        tag = tag + ['time'] + fail_tag
        writer.writerow(tag)
        for i in range(len(fail_path[0])):
            row = []
            row.append(time[i])
            for d in imbalance:
                row.append(d[i])
            row.append('')
            row.append(time[i])
            for d in fail_path:
                row.append(d[i])
            row.append('')
            row.append(time[i])
            for d in fail_fund:
                row.append(d[i])
            writer.writerow(row)
    plot(dir, time, fail_tag, fail_path, fail_fund, imb_tag, imbalance,
         fail_max)


if __name__ == '__main__':
    if (len(sys.argv)) < 2:
        print('No directory')
        exit()
    outCsv(sys.argv[1])
