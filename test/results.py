import sys, csv

def read_file(fname):
  with open(fname, 'r') as f:
    return csv.DictReader(f)

if __name__ == '__main__':
  results = read_file(sys.argv[1])
  