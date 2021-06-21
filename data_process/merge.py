import csv
import sys
import os

def parseFailCsv(filename):
    fail_path = []
    fail_fund = []
    indices = []
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
    return (fail_path, fail_fund)


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
    for filename in os.listdir(dir):
        if filename.endswith("_fail.csv"):
            (fail_p, fail_f) = parseFailCsv(os.path.join(dir, filename))
            fail_path.append(fail_p)
            fail_fund.append(fail_f)
            fail_tag.append(filename.split("_fail.csv")[0])
        else:
            continue
    return (time, fail_tag, fail_path, fail_fund, imb_tag, imbalance)


def outCsv(dir):
    (time, fail_tag, fail_path, fail_fund, imb_tag, imbalance) = parseDir(dir)
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

if __name__ == '__main__':
    if (len(sys.argv)) < 2:
        print('No directory')
        exit()
    outCsv(sys.argv[1])
