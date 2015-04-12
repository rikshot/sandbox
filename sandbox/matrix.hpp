#pragma once

#include <vector>
#include <exception>

namespace sandbox {

  template<typename T = float>
  class matrix {
  public:
    matrix(unsigned const rows) : rows_(rows), columns_(1), size_(rows * columns_), data_(size_, T()) {
    }

    matrix(unsigned const rows, unsigned const columns) : rows_(rows), columns_(columns), size_(rows * columns), data_(size_, T()) {
    }

    T operator ()(unsigned const row) const {
      if(row > rows_) throw std::range_error("Invalid index!");
      return data_[row];
    }

    T & operator ()(unsigned const row) {
      if(row > rows_) throw std::range_error("Invalid index!");
      return data_[row];
    }

    T operator ()(unsigned const row, unsigned const column) const {
      if(row > rows_ || column >= columns_) throw std::range_error("Invalid index!");
      return data_[columns_ * row + column];
    }

    T & operator ()(unsigned const row, unsigned const column) {
      if(row > rows_ || column >= columns_) throw std::range_error("Invalid index!");
      return data_[columns_ * row + column];
    }

    matrix operator +(matrix const & rhs) const {
      matrix copy(*this);
      for(unsigned i(0); i < size_; ++i) copy.data_[i] += rhs.data_[i];
      return copy;
    }

    void operator +(matrix const & rhs) {
      for(unsigned i(0); i < size_; ++i) data_[i] += rhs.data_[i];
    }

    matrix operator -(matrix const & rhs) const {
      matrix copy(*this);
      for(unsigned i(0); i < size_; ++i) copy.data_[i] -= rhs.data_[i];
      return copy;
    }

    void operator -(matrix const & rhs) {
      for(unsigned i(0); i < size_; ++i) data_[i] -= rhs.data_[i];
    }

    matrix operator *(T const & rhs) const {
      matrix copy(*this);
      for(unsigned i(0); i < size_; ++i) copy.data_[i] *= rhs;
      return copy;
    }

    void operator *=(T const & rhs) {
      for(unsigned i(0); i < size_; ++i) data_[i] *= rhs;
    }

    matrix operator *(matrix const & rhs) const {
      if(columns_ != rhs.rows_) throw std::range_error("Invalid matrix!");
      matrix temp(rows_, rhs.columns_);
      for(unsigned row(0); row < rows_; ++row) {
        for(unsigned column(0); column < rhs.columns_; ++column) {
          for(unsigned i(0); i < columns_; ++i) {
            temp(row, column) += operator ()(row, i) * rhs(i, column);
          }
          if(temp(row, column) < 0.0f && -10e-10f < temp(row, column)) {
            temp(row, column) = 0.0f;
          }
        }
      }
      return temp;
    }

    matrix transpose() const {
      matrix transposed(columns_, rows_);
      for(unsigned row(0); row < rows_; ++row) {
        for(unsigned column(0); column < columns_; ++column) {
          transposed(column, row) = operator ()(row, column);
        }
      }
      return transposed;
    }

    matrix solve() {
      matrix<> x(rows_);
      
      unsigned const n(rows_);
      std::vector<unsigned> nrow(n);
      for(unsigned i(0); i < n; ++i) {
        nrow[i] = i;
      }

      for(unsigned i(0); i < n - 1; ++i) {
        int p(i);
        float max(std::abs(operator ()(nrow[p], i)));
        for(unsigned j(i + 1); j < n; ++j) {
          if(std::abs(operator ()(nrow[j], i)) > max) {
            max = std::abs(operator ()(nrow[j], i));
            p = j;
          }
        }

        if(nrow[i] != nrow[p]) {
          int ncopy(nrow[i]);
          nrow[i] = nrow[p];
          nrow[p] = ncopy;
        }

        for(unsigned j(i + 1); j < n; ++j) {
          float m(operator ()(nrow[j], i) / operator ()(nrow[i], i));
          for(unsigned k(0); k < n + 1; ++k) {
            operator ()(nrow[j], k) -= m * operator ()(nrow[i], k);
          }
        }
      }

      x(n - 1) = operator ()(nrow[n - 1], n) / operator ()(nrow[n - 1], n - 1);
      for(int i(n - 2); i >= 0; --i) {
        float sum(0.0f);
        for(int j(i + 1); j < n; ++j) {
          sum += operator ()(nrow[i], j) * x(j);
        }
        x(i) = (operator ()(nrow[i], n) - sum) / operator ()(nrow[i], i); 
      }

      return x;
    }

  private:
    unsigned const rows_;
    unsigned const columns_;
    unsigned const size_;
    std::vector<T> data_;
  };

}
