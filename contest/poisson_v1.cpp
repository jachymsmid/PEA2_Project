#include <cmath>
#include <iostream>
#include <vector>

int index(int i, int j, int n)
{
  return i*n+j;
}

int main()
{
  const int n = 10; // number of nodes
  const float epsilon = 1e-6;
  const float length = 1.f;
  const float step = length/(float)n;
  const float pi = M_PI;
  const int iter_stop=100;
  const float coeff = step*step/4;

  // create thrree flattened arrays
  std::vector<float> array_old(n*n,1);
  std::vector<float> array_new(n*n,1);
  std::vector<float> f_array(n*n,1);

  // fill the arrays
  float x,y;
  for (int i=0; i < n/2; i++)
  {
    for (int j=0; j < n/2; j++)
    {
      std::cout << index(i,j,n) << std::endl;
      x = i*step;
      y = j*step;

      array_old[index(i,j,n)] = 1.f;
      f_array[index(i,j,n)] = 2*pi*pi*std::sin(pi*x)*std::sin(pi*y);
    }
  }

  /*
  float err,max_err=0;
  for (int iter=0; iter < iter_stop; iter++)
  {
    for (int i = 1; i < n-1;i++)
    {
      for (int j = 1; j < n-1;j++)
      {
        array_new[index(i,j,n)] = 0.25(array_old[index(i-1,j,n)]+array_old[index(i+1,j,n)]+array_old[index(i,j-1,n)]+array_old[index(i,j+1,n)])+coeff*f_array[index(i,j,n)];
        err = std::abs(array_new[index(i,j,n)]-array_old[index(i,j,n)]);
        if (err > max_err) max_err = err;
      }
    }
    */

    // BC
    for (int k=0; k<n; k++)
    {
      std::cout << k << std::endl;
      // upper boundary
      std::cout<<"upper ";
      array_new[k] = 0.f;

      // lower boundary
      std::cout<<"lower ";
      array_new[(n-k)*n] = 0.f;

      // right boundary
      array_new[k*n] = 0.f;
      std::cout<<"right ";
      
      // left boundary
      array_new[k*(n-1)+1] = 0.f;
      std::cout<<"left"<<std::endl;
    }
  std::cout << "\tworks" << std::endl;
    // if (err < epsilon) break;
  //}
  
  std::cout << "printing array" << std::endl;
  for (int i=0; i<n;i++)
  {
    for (int j=0; j<n;j++)
    {
      std::cout<<array_new[index(i,j,n)] << " ";
    }
    std::cout<<std::endl;
  }

  return 0;
}
