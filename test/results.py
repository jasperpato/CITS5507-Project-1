import sys, csv

def read_file(fname):
  with open(fname, 'r') as f:
    return csv.DictReader(f)

def get_avgs(results, n, p, n_threads, s):
  avgs = [0 for i in range(7)]
  return avgs

if __name__ == '__main__':
  
  fname = ''
  n, p, n_threads, s = -1, -1, -1, -1
  l = len(sys.argv)
  if l > 1: fname = sys.argv[1]
  if l > 2: n = int(sys.argv[2])
  if l > 3: p = int(float(sys.argv[3])*100)
  if l > 4: n_threads = int(sys.argv[4])
  if l > 5: n_threads = int(sys.argv[5])

  if l < 3 or not fname:
    print('Invalid arguments')
    exit()
  
  results = read_file(fname)
  avgs = get_avgs(results, n, p, n_threads, s)
