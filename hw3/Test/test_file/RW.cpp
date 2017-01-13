#pragma source off
#pragma token off
#pragma statistic off

int main()
{
  int a;
  float b[2];
  double c[3][4];
  string g;
  bool h[2][3][5];

  print a;
  print b;
  read c;
  read g;
  read h[2][3];
  read h[2][3][4];

}
