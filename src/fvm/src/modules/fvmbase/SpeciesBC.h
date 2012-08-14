#include "FloatVarDict.h"
#include "AMG.h"

template<class T>
struct SpeciesBC : public FloatVarDict<T>
{
  SpeciesBC()
  {
      this->defineVar("specifiedMassFraction",T(0.0));
      this->defineVar("specifiedMassFlux",T(0.0));
      
  }
  string bcType;
};

template<class T>
struct SpeciesVC : public FloatVarDict<T>
{
  SpeciesVC()
  {
      this->defineVar("massDiffusivity",T(1.e-9));
  }
  string vcType;
};

template<class T>
struct SpeciesModelOptions : public FloatVarDict<T>
{
  SpeciesModelOptions()
  {
    this->defineVar("initialMassFraction0",T(1.0));
    this->defineVar("initialMassFraction1",T(1.0));
    this->defineVar("A_coeff",T(1.0));
    this->defineVar("B_coeff",T(0.0));
    this->defineVar("timeStep",T(0.1));
    this->defineVar("interfaceUnderRelax",T(1.0));
    this->relativeTolerance=1e-8;
    this->absoluteTolerance=1e-16;
    this->linearSolver = 0;
    this->useCentralDifference=false;
    this->transient = false;
    this->ButlerVolmer = false;
    this->timeDiscretizationOrder=1;
  }
  double relativeTolerance;
  double absoluteTolerance;
  bool useCentralDifference;
  LinearSolver *linearSolver;
  bool transient;
  bool ButlerVolmer;
  int timeDiscretizationOrder;

#ifndef SWIG
  LinearSolver& getLinearSolver()
  {
    if (this->linearSolver == 0)
    {
        LinearSolver* ls(new  AMG());
        ls->relativeTolerance = 1e-1;
        ls->nMaxIterations = 20;
        ls->verbosity=0;
        this->linearSolver = ls;
    }
    return *this->linearSolver ;
  }
#endif
};

