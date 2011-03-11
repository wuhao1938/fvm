#include "misc.h"
#include "FloatVarDict.h"
#include "TractionVal.h"
#include "AMG.h"

template<class T>
struct PlateBC : public FloatVarDict<T>
{
  PlateBC()
  {
      this->defineVar("specifiedXRotation",T(0.0));
      this->defineVar("specifiedYRotation",T(0.0));
      this->defineVar("specifiedZDeformation",T(0.0));
  }
  string bcType;
};

template<class T>
struct PlateVC : public FloatVarDict<T>
{
  PlateVC()
  {
      this->defineVar("ym",T(1.0));
      this->defineVar("nu",T(1.0));
      this->defineVar("density",T(1.0));
  }
  string vcType;
};


template<class T>
struct PlateModelOptions : public FloatVarDict<T>
{
  PlateModelOptions()
  {
    this->defineVar("initialXRotation",T(0.0));
    this->defineVar("initialYRotation",T(0.0));
    this->defineVar("initialZDeformation",T(0.0));
    this->defineVar("deformationURF",T(0.7));
    this->defineVar("timeStep",T(0.1));
    this->defineVar("operatingTemperature",T(300.0));

    this->deformationTolerance=1e-4;
    this->printNormalizedResiduals = true;
    this->transient = false;
    this->timeDiscretizationOrder=1;
    this->scf=1.0; //lateral shear correction factor
    this->variableTimeStep = false;
    this->timeStepN1=0.1;
    this->timeStepN2=0.1;
    this->deformationLinearSolver = 0;

    this->incompressible = true;
  }
  
  bool printNormalizedResiduals;
  double deformationTolerance;
  bool transient;
  int timeDiscretizationOrder;
  double scf;
  bool variableTimeStep;
  double timeStepN1;
  double timeStepN2;
  LinearSolver *deformationLinearSolver;

  bool incompressible;
#ifndef SWIG
  LinearSolver& getDeformationLinearSolver()
  {
    if (this->deformationLinearSolver == 0)
    {
        LinearSolver* ls(new AMG());
        ls->relativeTolerance = 1e-1;
        ls->nMaxIterations = 20;
        ls->verbosity=0;
        this->deformationLinearSolver = ls;
    }
    return *this->deformationLinearSolver ;
  }
#endif
};

