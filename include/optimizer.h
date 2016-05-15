#ifndef _OPTIMIZER_H_
#define _OPTIMIZER_H_

#include <problem.h>
#include <string>

template <size_t Dimension, typename Value = double>
class Optimizer {
public:
  virtual Vector<Dimension, Value> optimize(const Problem<Dimension, Value>&  problem) =0;
  virtual std::string getName() const = 0;
};


template <size_t Dimension, typename Value = double>
class MultiplePointRestartAcceleratedGradientDescent: public Optimizer<Dimension, Value> {
  size_t _count;
  size_t _numRepetitions;
public:
  MultiplePointRestartAcceleratedGradientDescent( size_t count, size_t numRepetitions ) : _count(count), _numRepetitions(numRepetitions) { }
  Vector<Dimension, Value> optimize(const Problem<Dimension, Value>&  problem) override final {
    auto bestX = problem.bounds().randomPoint();
    for (size_t i = 0; i < _count; i++) {
      auto x = problem.bounds().randomPoint();
      auto y = x;
      double t = 1;
      auto prevX = x;
      auto prevY = y;
      double prevT = t;
      for (size_t i = 0; i < _numRepetitions; i++) {
        prevX = x;
        prevY = y;
        prevT = t;
        x = prevY - .01 * problem.gradient(prevY);
        t = 0.5 * prevT * (-prevT + sqrt(4 + prevT * prevT));
        y = x + (prevT * (1 - prevT) / (prevT * prevT + t)) * (x - prevX);
        Eigen::Matrix<Value, 1, 1> tmp = (x - prevX).transpose() * problem.gradient(prevY) ;
        if ( tmp(0,0) > 0) {
          t = 1;
        }
      }
      if (problem.function(x) < problem.function(bestX)) {
        bestX = x;
      }
    }

    return bestX;
  }

  std::string getName() const {
    return "Stochastic Gradient Descent";
  }
};

template <size_t Dimension, typename Value = double>
class GradientDescent: public Optimizer<Dimension, Value> {
//  size_t _numRepetitions;
  MultiplePointRestartAcceleratedGradientDescent<Dimension, Value> _mpragd;
public:
  GradientDescent( size_t numRepetitions ) : _mpragd(1, numRepetitions) { }
  Vector<Dimension, Value> optimize(const Problem<Dimension, Value>&  problem) override final {
    return _mpragd.optimize(problem);
  }

  std::string getName() const {
    return "Gradient Descent";
  }
};

template <size_t Dimension, typename Value = double>
class SimulatedAnnealing: public Optimizer<Dimension, Value> {
public:

  Vector<Dimension, Value> optimize(const Problem<Dimension, Value>&  problem) override final{
    return problem.bounds().randomPoint();
  }

  std::string getName() const {
    return "Simulated Annealing";
  }
};

template <size_t Dimension, typename Value = double>
class NewtonsMethod: public Optimizer<Dimension, Value> {
  size_t _count;
  size_t _numRepetitions;
public:
  NewtonsMethod( size_t count, size_t numRepetitions ) : _count(count), _numRepetitions(numRepetitions) { }
  Vector<Dimension, Value> optimize(const Problem<Dimension, Value>&  problem) override final {
    auto returnVal = problem.bounds().randomPoint();
    for (size_t i = 0; i < _count; i++) {
      auto newVal = problem.bounds().randomPoint();
      for (size_t j = 0; j < _numRepetitions; j++) {
        newVal -= problem.ihessian(newVal) * problem.gradient(newVal);
      }
      if (problem.function(newVal) < problem.function(returnVal)) {
        returnVal = newVal;
      }
    }
    return returnVal;
  }

public:
  std::string getName() const {
    return "Newton's Method";
  }
};

template <size_t Dimension, typename Value = double>
class InteriorPointsMethod: public Optimizer<Dimension, Value> {
public:

  Vector<Dimension, Value> optimize(const Problem<Dimension, Value>&  problem) override final {
    return problem.bounds().randomPoint();
  }

  std::string getName() const {
    return "Interior Points Method";
  }
};

template <size_t Dimension, typename Value = double>
class RandomGuessing: public Optimizer<Dimension, Value> {
 size_t _count;
public:
  RandomGuessing( size_t count ) : _count(count) { }
  Vector<Dimension, Value> optimize(const Problem<Dimension, Value>&  problem) override final{
    auto val = problem.bounds().randomPoint();
    for(size_t i=0; i<_count-1; i++){
      auto val2 = problem.bounds().randomPoint();
      if( problem.function( val2 ) < problem.function( val ) ) val = val2;
    }
    return val;
  }

  std::string getName() const {
    return "Random Guessing";
  }
};

#endif
