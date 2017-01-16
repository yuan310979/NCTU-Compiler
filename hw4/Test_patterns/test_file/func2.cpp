#pragma source off
#pragma token off
#pragma statistic off

/*void a(int a, int b){
  string c = a + b;
  return a;
  }

  void a(int a, int b){
  return a;
  }*/

/*int v2(int a[2])
  {
  int c;
  int b = a[2];
  c = a[2];
  c = a;
  if(a[1] == 2){
  return a[1];
  }
  return a;
  }*/

/*
   void foo(){
   int a[3] = {1,2,3}; // legal
   int b[4] = {1,2,3,4,5}; // illegal
   int c[2][3] = {1,2,3,4,5,6,7}; // illegal
   int d[5] = {1}; // legal
   int e = "This is a string"; // illegal
   }

   int foo(int a[3][3], int b[5][3], int i) {
   a[0][0] = i; // legal
   i = a[1][1]; // legal
   a[0][1] = b[1][2]; // legal
   int k;
   int k = a[3]; // legal : note that bounds are not checked
   string ss = 2.3;
   i = 3+a[0]; // illegal : array arithmetic
   a = b; // illegal : array assignment
   a[1] = b[2]; // illegal : array assignment
   return a[0][0]; // legal : 'a[0][0]' is a scalar type, but 'a' is an array type.
   }*/

void f1(float a);
void f2(double a[10]);
int f3();
double f3(int a, float b, double c, int d[10]) {
  int aaaaaa;
  string bb;
  {
    float bbbbb;
    string cccc;
  }
  int iam2;
  bool iam3;
/*  b = a; // a is converted from int to float
  c = a; // a is converted from int to double
  c = b; // b is converted from float to double
  c = a + b; // a is converted to float before addition,
  // which produces a float value that is converted to double.
  f1(a); // legal, a is converted from int to float
  c = f3(); // legal, the return type int is converted to double
  f2(d); // legal, d is converted from an array of 10 int's to an array of 10 double's
  a = c; // illegal, a double value cannot be converted to an integer
  while (b == c) {} // legal, b is converted to double before comparison
    return "sss"; // legal, the return value is implicitly converted to double*/
}

int main(){
  int a;
  int b;
  bool c[2][3];
  if(a){
    break;
  }
  while(c[2]){
    break;
  }
  for(a;a>b;a){
    break;
  }
  continue;
}
