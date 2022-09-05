import sys, csv

def read_file(fname):
  with open(fname, 'r') as f:
    reader = csv.DictReader(f)
    for row in reader: print(row)

if __name__ == '__main__':
  results = read_file(sys.argv[1])