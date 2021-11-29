from datetime import datetime

is_date = True

data = []
date = 0
val = 0

for line in open('data_raw','r').readlines():
    if line.strip() == '':
        continue
    if is_date:
        is_date = False
        date = datetime.strptime(line.strip(), '%d.%m.%Y').date()
        #print(date)
    else:
        is_date = True
        val = float(line[:-3])
        #print(val)
        data.append((date, val))

for x in data:
    print(x)

with open('data_processed.csv', 'w') as out:
    for line in data:
        out.write('%s,%0.1f\n' % (str(line[0]), line[1]))