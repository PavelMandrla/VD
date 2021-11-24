import matplotlib.pyplot as plt
import numpy as np


def load_file(filename):
    result = []
    with open(filename) as infile:
        line = infile.readline()
        while line:
            line.strip()
            _, _, _, str_val, _ =line.split(',')
            result.append(float(str_val) * 0.1)
            line = infile.readline()
    result.sort()
    return result


def load_data():
    return [load_file("data_2000.txt"), load_file("data_2010.txt"), load_file("data_2020.txt")]


def get_whiskers(q1, q3, data):
    uppers = []
    lowers = []
    for i in range(len(q1)):
        iqr = (q3[i] - q1[i])
        uppers.append(np.clip(q3[i] + 1.5 * iqr,    q3[i], data[i][-1]))
        lowers.append(np.clip((q1[i] - 1.5 * iqr),  q1[i], data[i][0]))
    return uppers, lowers


data = load_data()
fig, ax2 = plt.subplots(nrows=1, ncols=1, figsize=(9, 9))

ax2.set_title('Průměrná teplota v Klementinu')
parts = ax2.violinplot(data, showmeans=False, showmedians=False, showextrema=False)

for pc in parts['bodies']:
    pc.set_facecolor('#8ecae6')

percentiles = [list(np.percentile(d, [25, 50, 75])) for d in data]
quartile1 = [p[0] for p in percentiles]
medians = [p[1] for p in percentiles]
quartile3 = [p[2] for p in percentiles]

inds = np.arange(1, len(data) + 1)
whiskers_min, whiskers_max = get_whiskers(quartile1, quartile3, data)
ax2.vlines(inds, whiskers_min, whiskers_max, color='#219ebc', linestyle='-', lw=1)
ax2.scatter(inds, medians, marker='o', color='#8ecae6', s=30, zorder=3)
ax2.vlines(inds, quartile1, quartile3, color='#219ebc', linestyle='-', lw=12)

# set style for the axes
labels = ['2000', '2010', '2020']


ax2.xaxis.set_tick_params(direction='out')
ax2.xaxis.set_ticks_position('bottom')
ax2.set_xticks(np.arange(1, len(labels) + 1))
ax2.set_xticklabels(labels)
ax2.set_xlim(0.25, len(labels) + 0.75)
ax2.set_xlabel('rok')

#plt.subplots_adjust(bottom=0.15, wspace=0.05)
#plt.show()

plt.savefig('export.pdf')