
/**
 * @return int number of digits in integer x, required for printing lattices
 */
int num_digits(int x) {
  int c = 0;
  while(x != 0) {
    x /= 10;
    ++c;
  }
  return c;
}