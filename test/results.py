import csv
import numpy as np
import matplotlib.pyplot as plt
from argparse import ArgumentParser

P_RES = 1e-3

def read_file(fname):
  rows = []
  with open(fname, 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
      for i, (k, v) in enumerate(row.items()):
        if i == 1 or i > 7: row[k] = float(v)
        else: row[k] = int(v)
      rows.append(row)
  return rows

def get_data(results, const, c):
  x = 'p' if c == 'n' else 'n'
  data = tuple({} for _ in range(4))
  for row in results:
    t = row['n_threads']-1
    if abs(const-row[c]) < P_RES:
      if row[x] not in data[t]: data[t][row[x]] = []
      data[t][row[x]].append(row['total_time'])
  return data

def remove_outliers(data, stds=1):
  for t in range(4):
    for y, xs in data[t].items():
      new = []
      mean, std = np.mean(xs), np.std(xs)
      for i, x in enumerate(xs):
        if abs(x-mean) < stds*std: new.append(x)
      data[t][y] = new
    
def means(data):
  m = tuple({} for _ in range(4))
  for t in range(4):
    for k, v in data[t].items():
      m[t][k] = np.mean(v)
  return m

def graph(results, const, c):
  data = get_data(results, const, c)
  remove_outliers(data)
  m = means(data)

  for t in range(4):
    plt.plot(m[t].keys(), m[t].values(), label=f'{t+1}')
  plt.xlabel('p' if c == 'n' else 'n')
  plt.ylabel('Total time (s)')
  plt.title(f"{c} = {const}")
  plt.legend(title='Num threads')
  plt.show(block=True)

if __name__ == '__main__':
  a = ArgumentParser()
  for p in ('fname', '-n', '-p'):
    a.add_argument(p)
  args = a.parse_args()
  
  results = read_file(args.fname)

  if(args.n): graph(results, int(args.n), 'n')
  elif(args.p): graph(results, float(args.p), 'p')