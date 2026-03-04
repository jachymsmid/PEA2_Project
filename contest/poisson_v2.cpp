#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <vector>
#include <utility>
#include <immintrin.h>
#include <algorithm>


using RealType = float;

template <class RealType>
class FlatArray
{
private:

    int rows, cols;
    std::vector<RealType> data;

public:

    // constructor
    FlatArray(int r, int c) : rows(r), cols(c), data(r*c,0.0f) {}

    // access the data with matrix indexing
    inline RealType& operator()(int r, int c)
    {
        return data[r * cols + c];
    }

    inline const RealType& operator()(int r, int c) const
    {
        return data[r * cols + c];
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

    // using swap instead of copying the arrays, I only need the array_old
    // just trust the arrays are of the same size xd
    void swap(FlatArray& other)
    {
      std::swap(rows, other.rows);
      std::swap(cols, other.cols);
      data.swap(other.data);
    }

};

int main()
{
  // define constants
  const int n = 8192; // number of nodes
  const RealType length = 1.f;
  const RealType step = length/(float)n;
  const RealType pi = M_PI;
  const int iter_stop = 100;
  const RealType tolerance = 1e-6;
  const RealType coeff = step*step/4;

  // Pre-load constants into 256-bit registers (8 copies of each)
  __m256 vec_quarter = _mm256_set1_ps(0.25f);
  __m256 vec_coeff   = _mm256_set1_ps(coeff);
  __m256 vec_max_err = _mm256_setzero_ps(); // Holds 8 local maximums

  // create thrree flattened arrays
  FlatArray<RealType> array_old(n,n);
  FlatArray<RealType> array_new(n,n);
  FlatArray<RealType> f_array(n,n);

  // fill the arrays
  float x,y;
  for (int i=0; i < n-1; i++)
  {
    for (int j=0; j < n-1; j++)
    {
      // implicit grid coordinates
      x = i*step;
      y = j*step;

      // fill the rhs array
      f_array(i,j) = 2*pi*pi*std::sin(pi*x)*std::sin(pi*y);
    }
  }

  
  RealType err;
  std::size_t iter = 0;

  // main loop
  auto start = std::chrono::high_resolution_clock::now();
  while (iter < 10)
  {
    err = 0.0;
    // update the inner values
    for (int i = 1; i < n-1; i++)
    {
      for (int j = 1; j <= n - 1 - 8; j += 8)
      {
        // 1. LOAD: Grab 8 floats from each neighbor direction
        // _mm256_loadu_ps means "load unaligned packed single-precision floats"
        __m256 top    = _mm256_loadu_ps(&array_old(i - 1, j));
        __m256 bottom = _mm256_loadu_ps(&array_old(i + 1, j));
        __m256 left   = _mm256_loadu_ps(&array_old(i, j - 1));
        __m256 right  = _mm256_loadu_ps(&array_old(i, j + 1));
        __m256 f_val  = _mm256_loadu_ps(&f_array(i, j));
        __m256 old_c  = _mm256_loadu_ps(&array_old(i, j)); // old center

        // 2. COMPUTE: Add the neighbors together
        __m256 sum = _mm256_add_ps(top, bottom);
        sum = _mm256_add_ps(sum, left);
        sum = _mm256_add_ps(sum, right);

        // 3. COMPUTE: Apply coefficients (0.25 * sum + coeff * f_val)
        __m256 term1   = _mm256_mul_ps(vec_quarter, sum);
        __m256 term2   = _mm256_mul_ps(vec_coeff, f_val);
        __m256 new_val = _mm256_add_ps(term1, term2);

        // 4. STORE: Write the 8 new values back to memory
        _mm256_storeu_ps(&array_new(i, j), new_val);

        // 5. ERROR CALCULATION: Get absolute difference
        // Trick: abs(x) is the maximum of (a-b) and (b-a)
        __m256 diff1 = _mm256_sub_ps(new_val, old_c);
        __m256 diff2 = _mm256_sub_ps(old_c, new_val);
        __m256 abs_diff = _mm256_max_ps(diff1, diff2);

        // Update the running maximum error vector
        vec_max_err = _mm256_max_ps(vec_max_err, abs_diff);
      }
    }
    
    // BC need not be imposed as they stay zero, default value for std::vector
    // swap the arrays, instead of copying them
    array_old.swap(array_new);
    iter++;
    std::cout << err << std::endl;
  }

  auto end = std::chrono::high_resolution_clock::now();

  // print the metrics
  std::cout << "Converged in " << iter << " iterations" << std::endl;
  std::cout << "Final Error: " << std::scientific << err << std::endl;
  std::chrono::duration<double> elapsed = end - start;
  std::cout << "Elapsed time: " << elapsed.count() << " seconds" << std::endl;

  auto flopsPerSecond = (double)(n - 2) * (n - 2) * 7 * iter / elapsed.count();
  std::cout << "Performance: " << std::fixed << std::setprecision(2) << flopsPerSecond / 1e9 << " GFLOPS" << std::endl;

  auto memoryPerSecond = (double)(n - 2) * (n - 2) * 7 * sizeof(RealType) * iter / elapsed.count();
  std::cout << "Memory Bandwidth: " << std::fixed << std::setprecision(2) << memoryPerSecond / (1024 * 1024 * 1024) << " GB/s" << std::endl;

  return 0;
}
