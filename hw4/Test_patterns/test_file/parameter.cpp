#pragma source off
#pragma token off
#pragma statistic off

int func(float V1[2][3], int V2);
float func2(double V2[3][4], float V3[3]);

int main(){
  int a;
  int b;
  int c[4][2][3];
  int d[4][3];
  float e[2][3][4];
  func(a[2],b);
  func(c[2],b);
  func(c,b);
  func2(c,b);
  func2(e[1],d[2]);
}
