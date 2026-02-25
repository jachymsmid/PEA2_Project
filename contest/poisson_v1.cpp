#include <cmath>
#include <iostream>
#include <vector>

using RealType = float;

template <class DataType>
class FlatArray
{
private:

    int rows, cols;
    std::vector<DataType> data;

public:

    // constructor
    FlatArray(int r, int c) : rows(r), cols(c) {data.resize(r*c);}

    // access the data with matrix indexing
    DataType& operator()(int r, int c)
    {
        return data[r * cols + c];
    }

    // access the data with vector indexing
    DataType& operator()(int n)
    {
        return data[n];
    }

    // print the stored data
    void print()
    {
      for (int i = 0; i<rows; i++)
      {
        for (int j = 0; j<cols; j++)
        {
          std::cout<< data[i*cols+j] << " ";
        }
        std::cout<<std::endl;
      }
    }

    void operator=(FlatArray& other)
    {
      for (int i = 0; i<rows; i++)
      {
        for (int j = 0; j<cols; j++)
        {                                                                                                                                   
          data[i*cols+j] = other(i,j);
        }
      }
    }

};

int main()
{
  const int n = 10; // number of nodes
  const RealType epsilon = 1e-6;
  const RealType length = 1.f;
  const RealType step = length/(float)n;
  const RealType pi = M_PI;
  const int iter_stop=100;
  const RealType coeff = step*step/4;

  // create thrree flattened arrays
  FlatArray<RealType> array_old(n,n);
  FlatArray<RealType> array_new(n,n);
  FlatArray<RealType> f_array(n,n);

  // fill the arrays
  float x,y;
  for (int i=0; i < n; i++)
  {
    for (int j=0; j < n; j++)
    {
      x = i*step;
      y = j*step;

      array_old(i,j) = 1.f;
      f_array(i,j) = 2*pi*pi*std::sin(pi*x)*std::sin(pi*y);
    }
  }

  
  RealType err,max_err=0;
  for (int iter=0; iter < iter_stop; iter++)
  {
    for (int i = 1; i < n-1;i++)
    {
      for (int j = 1; j < n-1;j++)
      {
        array_new(i,j) = 0.25*(array_old(i,j)+array_old(i,j)+array_old(i,j)+array_old(i,j))+coeff*f_array(i,j);
        err = std::abs(array_new(i,j)-array_old(i,j));
        if (err > max_err) max_err = err;
      }
    }
    
    // BC
    for (int k=0; k<n; k++)
    {
      // upper boundary
      array_old(k) = 0.f;
      // lower boundary
      array_old((n-1)*n+k) = 0.f;
      // left boundary
      array_old(k*n) = 0.f;
      // right boundary
      array_old(k*n+(n-1)) = 0.f;
    }

    array_old = array_new;

    if (err < epsilon) break;
  }

  array_new.print();
  return 0;
}
