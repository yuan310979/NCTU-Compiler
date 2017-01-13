#pragma source off
#pragma token off
#pragma statistic off

const int i = 4;
const float f = 2.45;
const string g = "Peter";
const bool b = true;
bool ans;

float ff[2][3];
int ii[4][3];
const float fff = 3e-4;

int main(){
  int irr;
  float frr;
  double drr;
  string srr;
  bool brr;
  ans = false;
  //brr = 2 + 3e-4;

  i = i%4%g;
  i = i%irr+b;
  i = i%frr;
  i = g%b;

  i = 8*(2+3);
  irr = -4+5;
  //b = b && true;
  //b = b && ans || b;
  ans = ans || brr && (4+fff);
  ans = !b && !g;

  irr = -true + -g;

  frr = 3.4;
  ff = ii;
  ii = ff;
  ii[2] = ff[3];
  ff = ii[3];
  ff[2] = ii[3];

}
