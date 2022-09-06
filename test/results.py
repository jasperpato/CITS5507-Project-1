import csv
from argparse import ArgumentParser

P_RES = 1e-3

def read_file(fname):
  rows = []
  with open(fname, 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
      for k, v in row.items(): row[k] = int(v)
      rows.append(row)
  return rows

def get_avgs(results, n, p, n_threads):
  avgs = [0 for _ in range(7)]
  count = 0
  for row in results:
    if((n is None or n == row['n']) and (p is None or abs(p-row['p']) < P_RES) and (n_threads is None or n_threads == row['n_threads'])):
      for i, v in enumerate(('max_cluster_size', 'num_clusters', 'rperc', 'cperc', 'perc_time', 'join_time', 'total_time')):
        avgs[i] += int(row[v])
        count += 1
  if count:
    for i in range(7): avgs[i] /= count
  return avgs, count

if __name__ == '__main__':
  a = ArgumentParser()
  for v in ('--n', '--p', '--n_threads', 'fname'):
    a.add_argument(v)
  args = a.parse_args()
  
  results = read_file(args.fname)

  n = int(args.n) if args.n else None
  p = float(args.p) if args.p else None
  n_threads = int(args.n_threads) if args.n_threads else None

  print(n, p, n_threads)

  avgs, count = get_avgs(results, n, p, n_threads)
  print(avgs)
