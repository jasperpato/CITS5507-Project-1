'''
Reads results file and graphs all n_threads on a plot. Keeps either n or p constant as specified.

USAGE: python3 graph.py [-n N | -p P] [--fname RESULTS_FILE]

N keep lattice size constant at N
P keep probability constant at P
'''

import csv
from tkinter import E
import numpy as np
import matplotlib.pyplot as plt
from argparse import ArgumentParser

P_RES = 1e-3
N_THREADS = 4

'''
Returns a list of dictionaries - one for each row in csvfile
'''
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

'''
Gather data from rows that match the const (could be n or p)
'''
def get_data(results, const, c):
  x = 'p' if c == 'n' else 'n'
  data = tuple({} for _ in range(N_THREADS))
  for row in results:
    t = row['n_threads']-1
    if abs(const-row[c]) < P_RES:
      if row[x] not in data[t]: data[t][row[x]] = []
      data[t][row[x]].append(row['total_time'])
  return data

'''
Remove total_times that are further than (stds * std) from mean
'''
def remove_outliers(data, stds=4):
  count = 0
  for t in range(4):
    for y, xs in data[t].items():
      new = []
      mean, std = np.mean(xs), np.std(xs)
      for i, x in enumerate(xs):
        if abs(x-mean) <= stds*std: new.append(x)
        else: count += 1
      data[t][y] = new
  print(f'Removed data points (stds={stds}): {count}')

'''
Replace lists of data with their mean to be graphed
'''    
def means(data):
  lens, count, min_len, max_len = 0, 0, 0, 0
  
  m = tuple({} for _ in range(N_THREADS))
  for t in range(N_THREADS):
    for k, v in data[t].items():
      lens += len(v)
      if len(v) < min_len or min_len == 0: min_len = len(v)
      if len(v) > max_len: max_len = len(v)
      count += 1
      m[t][k] = np.mean(v)

  print(f'Minimum data points per parameter set: {min_len}')

  return m

'''
Graph all n_threads on one axis, keeping either n or p constant
'''
def graph(results, const, c):
  data = get_data(results, const, c)
  remove_outliers(data)
  m = means(data)

  for t in range(N_THREADS):
    plt.plot(m[t].keys(), m[t].values(), label=f'{t+1}')
  plt.xlabel('p' if c == 'n' else 'n')
  plt.ylabel('Total time (s)')
  plt.title(f"{c} = {const}")
  plt.legend(title='Num threads')
  plt.show(block=True)

if __name__ == '__main__':
  a = ArgumentParser()
  a.add_argument('--fname', default='results.csv')
  a.add_argument('--n_threads', default=4, type=int)
  for p in ('-n', '-p'): a.add_argument(p)
  args = a.parse_args()
  
  results = read_file(args.fname)
  N_THREADS = args.n_threads
  
  if(args.n): graph(results, int(args.n), 'n')
  elif(args.p): graph(results, float(args.p), 'p')