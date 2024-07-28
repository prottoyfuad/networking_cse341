
/**
 *  author  : prottoyfuad
 *  code    : VLSM Subnet Calculator
 *  created : June 10, 2021
**/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/time.h>

#define nax 100005
#define inf 1000000007

unsigned _rng;
void init_random() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  _rng = tv.tv_sec ^ tv.tv_usec;
}
int _rand() { return _rng = (_rng * 3) >> 1; }
int _random(int l, int r) { return l + (_rand() % (r - l)); }
void swap(int* a, int* b) { int c = *a; *a = *b; *b = c; }

void sort(int *ea, int *ind, int l, int r) {
  while (l < r) {
    int i = l, j = l, k = r;
    int e = ind[_random(l, r)];
    while (j < k) {
      int c = ea[ind[j]] != ea[e] ? ea[ind[j]] - ea[e] : ind[j] - e;
      if (c == 0) j++;
      else if (c < 0) swap(&ind[i++], &ind[j++]);
      else swap(&ind[j], &ind[--k]);
    }
    sort(ea, ind, l, i);
    l = k;
  }
}

int bintodec(char* bin, int l, int r) {
  int dec = 0;
  for (int i = l; i < r; i++) {
    dec = dec * 10 + bin[i] - 48;
  }
  return dec;
}

int getWidth(int *ea) {
  int ret = 19;
  for (int i = 0; i < 5; i++) {
    ret -= (ea[i] < 10) + (ea[i] < 100);
  }
  return ret;
}

void print_address(int *ea) {
  for (int i = 0; i < 5; i++) {
    if (i > 0) printf("%c", i == 4 ? '/' : '.');
    printf("%d", ea[i]);
  }
}

int main() {
  static int users[nax], bits[nax], order[nax], eoc[32], net[5], cout[5], usernet[nax][5];
  static char original[20], line[100];
  static int* candidates[32];
  init_random();
  printf("How many networks:\n");
  int n;
  scanf("%d", &n);
  while (n < 2 || n + 5 > nax) {
    puts("No. of network must be in range[2, 10^5], input again.");
    scanf("%d", &n);
  }
  char foo = 1;
  printf("Input %d numbers denoting the number of users in these %d networks.\n", n, n);
  while (foo) {
    foo = 0;
    long long tot = 0;
    for (int i = 0; i < n; i++) {
      scanf("%d", users + i);
      foo |= users[i] < 0;
      tot += users[i];
    }
    if (foo) {
      printf("Number of users can't be negative, input %d number of users again.\n", n);
    } else if (tot > (long long)inf) {
      foo = 1;
      printf("Total users can't be more than 10^9, input %d number of users again.\n", n);
    }
  }
  printf("Input the network address which will be subnetted with the mask[2, 30].\n");
  foo = 1;
  while (foo) {
    foo = 0;
    for (int i = 0; i < 5; i++) net[i] = -1;
    scanf("%s", original);
    int k = strlen(original);
    for (int i = 0, j = 0; j < 5; i++, j++) {
      if (i >= k) {
        puts("Invalid Network Address!");
        foo = 1; break;
      }
      if (j == 4) {
        if (original[i] == '/') i++;
        else {
          puts("No Subnet Mask Found!");
          foo = 1; break;
        }
      }
      else if (j > 0) {
        if (original[i] == '.') i++;
        else {
          puts("Invalid Network Address!");
          foo = 1; break;
        }
      }
      if (original[i] < '0' || original[i] > '9') {
        printf("Expected digit at : %d\n", i + 1);
        foo = 1; break;
      }
      int was = i;
      while (i + 1 < k && original[i + 1] >= '0' && original[i + 1] <= '9') i++;
      net[j] = bintodec(original, was, i + 1);
      if (j == 4) {
        if (net[j] < 2 || net[j] > 30) foo = 1;
      }
      else if (net[j] < 0 || net[j] > 255) foo = 1;
      if (foo) {
        printf("%dth number is not within limit - %d.\n", j + 1, net[j]);
        break;
      }
    }
    if (!foo) {
      int zero = 0, o;
      for (o = 3; o >= 0; o--) {
        if (net[o] == 0) zero += 8;
        else break;
      }
      if (o >= 0) {
        int seg = net[o];
        while (seg && ~seg & 1) ++zero, seg >>= 1;
        if (zero < 32 - net[4]) foo = 1;
      }
      if (foo) printf("Existing usable bits needs to be zero - %d!\n", zero);
    }
    if (foo) puts("Input the Network Address Again.");
  }

  int have = 1 << (32 - net[4]), total = 0;
  for (int i = 0; i < n; i++) {
    int x = users[i] + 2;
    bits[i] = (x & (x - 1)) == 0 ? -1 : 0;
    while (x) ++bits[i], x >>= 1;
    total += 1 << bits[i];
  }
  if (have < total) {
    puts("This network address doesn't have enough space for the given number of users\n");
    return 0;
  }
  for (int i = 0; i < n; i++) order[i] = i;
  sort(users, order, 0, n);
  for (int i = 0; i < n >> 1; i++) {
    swap(&order[i], &order[n - i - 1]);
  }
  for (int i = 1; i < n; i++) {
    assert(users[order[i]] <= users[order[i - 1]]);
  }

  for (int i = 0; i < 32; i++) {
    eoc[i] = 0;
    candidates[i] = (int *) malloc(2 * sizeof *candidates);
  }
  eoc[net[4] - 1] = 1;
  candidates[net[4] - 1][0] = 0;
  for (int i = 0; i < 4; i++) {
    candidates[net[4] - 1][0] <<= 8;
    candidates[net[4] - 1][0] |= net[i];
  }
  for (int e = 0; e < n; e++) {
    int i = order[e], b = bits[i];
    int u = 31 - b, du = u;
    while (du > 0 && eoc[du] == 0) du--;
    assert(du > 0);
    usernet[i][4] = u + 1;
    int o = --eoc[du];
    int choose = candidates[du][o];
    for (int j = du + 1; j <= u; j++) {
      o = eoc[j]++;
      if (o > 1 && (o & (o - 1)) == 0) {
        candidates[j] = (int *) realloc(candidates[j], o * 2 * sizeof *candidates);
      }
      candidates[j][o] = choose | (1 << (31 - j));
    }
    for (int j = 3; j >= 0; j--) {
      usernet[i][j] = choose & 255;
      choose >>= 8;
    }
  }
  for (int i = 0; i < 32; i++) free(candidates[i]);

  puts("\nHere is the subnet Information for the given networks:\n");
  int w = 5, k = 0;
  while (w--) putchar(' '), line[k++] = ' ';
  printf("Network Address");
  for (int i = 0; i < 15; i++) line[k++] = '=';
  w = 5;
  while (w--) putchar(' '), line[k++] = ' ';
  printf("Broadcast Address");
  for (int i = 0; i < 17; i++) line[k++] = '=';
  w = 3;
  while (w--) putchar(' '), line[k++] = ' ';
  printf("First Host");
  for (int i = 0; i < 10; i++) line[k++] = '=';
  w = 10;
  while (w--) putchar(' '), line[k++] = ' ';
  printf("Last Host");
  for (int i = 0; i < 9; i++) line[k++] = '=';
  w = 10;
  while (w--) putchar(' '), line[k++] = ' ';
  for (int i = 0; i < 5; i++) line[k++] = '=';
  line[k] = '\0';
  puts("Extra");
  puts(line);
  for (int i = 0; i < n; i++) {
    printf("\n%3d. ", i + 1);
    print_address(usernet[i]);
    w = 20 - getWidth(usernet[i]);
    while (w--) putchar(' ');
    for (int j = 0; j < 5; j++) cout[j] = usernet[i][j];
    k = 32 - usernet[i][4];
    for (int j = 3; k > 0 && j >= 0; j--, k -= 8) {
      if (k >= 8) cout[j] = 255;
      else cout[j] |= (1 << k) - 1;
    }
    print_address(cout);
    w = 20 - getWidth(cout);
    while (w--) putchar(' ');
    for (int j = 0; j < 5; j++) cout[j] = usernet[i][j];
    cout[3] |= 1;
    print_address(cout);
    w = 20 - getWidth(cout);
    while (w--) putchar(' ');
    for (int j = 0; j < 5; j++) cout[j] = usernet[i][j];
    k = 32 - usernet[i][4];
    for (int j = 3; k > 0 && j >= 0; j--, k -= 8) {
      if (k >= 8) cout[j] = 255;
      else cout[j] |= (1 << k) - 1;
    }
    cout[3] ^= 1;
    print_address(cout);
    w = 20 - getWidth(cout);
    while (w--) putchar(' ');
    printf("%d", (1 << bits[i]) - (users[i] + 2));
  }
  printf("\n"), fflush(stdout);
  return 0;
}

/*
  Case 01:

  Case 02:

  Case 03:

  Case 04:

  Case 05:

  Case 06:

  Case 07:
*/
