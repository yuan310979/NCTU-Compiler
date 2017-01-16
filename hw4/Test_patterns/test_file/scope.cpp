#pragma source off
#pragma token off
#pragma statistic off



int main()
{
  const int b = 2;
  int a = 3;

  const float b = 3.2;
  {
    int a = 4;
    b = 4.5;

    {
      float a = 3e-7;
      {
         const float b = 3.2;
      }
    }
  }
}
