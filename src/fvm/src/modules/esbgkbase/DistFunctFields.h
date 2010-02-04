#ifndef _DISTFUNCTFIELDS_H_
#define _DISTFUNCTFIELDS_H_

#include "quadrature.h"
#include "Field.h"
#include "Vector.h"
#include "Mesh.h"
#include "MacroParameters.h"
#include <math.h>

template <class T>

/**
 * Class DistFunctFields for Distribution function in ESBGK simulations
 * A collection of distribution function fields in all N123 directions 
 * velocity quadrature points defined in quadrature.h
 */
class DistFunctFields
{
 public:
  typedef Array<T> TArray;
  typedef Vector<T,3> VectorT3;
  typedef Array<VectorT3> VectorT3Array;
  
  //std::vector<shared_ptr<Field> > DistFunctFieldsPtr;
  std::vector<Field*> dsf;
 

 /**
   * Maxwellian Distribution - sets one direction as one field
   * ix, iy - spatial location
   * j - discrete velocity ordinate
   * f[ix][iy][j]=Rho[ix][iy]/pow((pi*Temp[ix][iy]),1.5)*
   *             exp(-((pow(cx[j]-xVel[ix][iy]),2.0)
   *            +pow((cy[j]-yVel[ix][iy]),2.0)+pow(cz[j],2.0))/Temp[ix][iy])
   * @param mesh - spatial mesh with cells,faces
   * @param macroPr - Fields of macroparameters such as density, temperature, velocity, pressure, viscosity
   * @param quad - velocity quadrature with abscissa and weights
   */
 DistFunctFields(const Mesh& mesh,const MacroParameters& macroPr, const Quadrature<T>& quad) 
    {
      
      const StorageSite& cells = mesh.getCells();
      const int nCells = cells.getSelfCount();
      double pi(3.14159);
      
      const TArray& density = dynamic_cast<const TArray&>(macroPr.density[cells]);
      const TArray& temperature = dynamic_cast<const TArray&>(macroPr.temperature[cells]);
      const VectorT3Array& v = dynamic_cast<const VectorT3Array&>(macroPr.velocity[cells]);
      /**
       * integer N123
       * total number of velocity directions.
       */
      const int N123 = quad.getDirCount(); 
      const TArray& cx = dynamic_cast<const TArray&>(quad.cxPtr[cells]);
      const TArray& cy = dynamic_cast<const TArray&>(quad.cyPtr[cells]);
      const TArray& cz = dynamic_cast<const TArray&>(quad.czPtr[cells]);

      /*
      TArray* distfunAPtr;   
      distfunAPtr=new TArray(nCells);
      TArray & distfunA= *distfunAPtr;  
      for(int j=0;j<N123;j++){
     	for(int c=0; c<nCells;c++)
	  {
	    distfunA[c]=density[c]/pow((pi*temperature[c]),1.5)*
	      exp(-(pow((cx[j]-v[c][0]),2.0)+pow((cy[j]-v[c][1]),2.0)+
		    pow((cz[j]-v[c][2]),2.0))/temperature[c]);
	  } 
      }
      */
            
      for(int j=0;j<N123;j++){
       	Field& fnd= *dsf[j]; 
	const TArray& fc = dynamic_cast<const TArray&>(fnd[cells]);
	for(int c=0; c<nCells;c++)
	  {
	    fc[c]=density[c]/pow((pi*temperature[c]),1.5)*
	      exp(-(pow((cx[j]-v[c][0]),2.0)+pow((cy[j]-v[c][1]),2.0)+
		    pow((cz[j]-v[c][2]),2.0))/temperature[c]);
	  } 
	//DistfunPtr[j]->f
      }
    }
  
  
 
  
/*
  void Maxwellian(const Mesh& mesh, const Field& density, const Field& velocity, const Field& temperature, const Quadrature& quad)
  {
    const StorageSite& cells = mesh.getCells();
    const int nCells = cells.getSelfCount();
    const int N123 = quad.getDirCount();
    double pi(3.14159);
    
    const TArray& density = dynamic_cast<const TArray&>(density[cells]);
    const TArray& temperature = dynamic_cast<const TArray&>(temperature[cells]);
    const VectorT3Array& v = dynamic_cast<const VectorT3Array&>(velocity[cells]);
    //const TArray& cx = dynamic_cast<const TArray&>(quad.cxPtr[cells]);
    //const TArray& cy = dynamic_cast<const TArray&>(quad.cyPtr[cells]);
    _cx(quad.cxPtr);
    int j= direction;
    for(int c=0; c<nCells;c++)
      {
	distfun[c]=density[c]/pow((pi*temperature[c]),1.5);
	   //exp(-(pow((cx[j]-v[c][0]),2.0)+pow((cy[j]-v[c][1]),2.0)+pow((cz[j]-v[c][2]),2.0))/temperature[c]);      
      } 
  }
  */ 
};
 

/*
//Fgamma, Macroparameters, -ESBGKModel.h
void Fgamma_alphas(){
  for (int j=0;j<N123;j++){
    malpha_BGK[j][0]=1.0;
    malpha_BGK[j][1]=cx[j];
    malpha_BGK[j][2]=pow(cx[j],2.0)+pow(cy[j],2.0)+pow(cz[j],2.0);  
    malpha_BGK[j][3]=cy[j];
  }
  if(solver == 2){
    for (int j=0;j<N123;j++){
      malpha_ES[j][0]=1.0;
      malpha_ES[j][1]=cx[j];
      malpha_ES[j][2]=cy[j];
      malpha_ES[j][3]=pow(cx[j],2.0);
      malpha_ES[j][4]=pow(cy[j],2.0);
      malpha_ES[j][5]=pow(cz[j],2.0);
      malpha_ES[j][6]=cx[j]*cy[j];
    }}

}
*/

#endif
