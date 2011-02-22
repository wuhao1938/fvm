#include "Mesh.h"
#include "Array.h"
#include "Field.h"
#include "Vector.h"
#include "KSearchTree.h"
#include "ContactModel.h"
#include "PhysicsConstant.h"

template<class T>
class ContactModel<T>::Impl
{

 public:
  typedef Array<T> TArray;
  typedef Array<int> IntArray;
  typedef Vector<T,3> VectorT3;
  typedef Vector<double, 3> VectorD3;
  typedef Array<VectorT3> VectorT3Array;


   Impl(const GeomFields& geomFields,
      ContactFields& contactFields,
      const MeshList& meshes) :
    _meshes(meshes),
    _geomFields(geomFields),
    _contactFields(contactFields)
    { }

   void init()
   {}

   void computeSolidSurfaceForce(const StorageSite& solidFaces, bool perUnitArea)
   {
     const int nSolidFaces = solidFaces.getCount();

     boost::shared_ptr<VectorT3Array>
      forcePtr( new VectorT3Array(nSolidFaces));
     
     VectorT3Array& force = *forcePtr;

     force.zero();

     _contactFields.force.addArray(solidFaces,forcePtr);

     const VectorT3Array& solidFaceCentroid =
       dynamic_cast<const VectorT3Array&>(_geomFields.coordinate[solidFaces]);

     const VectorT3Array& solidFaceArea =
      dynamic_cast<const VectorT3Array&>(_geomFields.area[solidFaces]);

     const TArray& solidFaceAreaMag =
      dynamic_cast<const TArray&>(_geomFields.areaMag[solidFaces]);

     
     vector<NearestCell> solidFacesNearestCell(nSolidFaces);
   

     //for each face in solidFaces, find out the nearest neighbor on substrate

     const int numMeshes = _meshes.size();
     for (int n=0; n<numMeshes; n++)
     {
	 const Mesh& mesh = *_meshes[n];
	 foreach(const FaceGroupPtr fgPtr, mesh.getBoundaryFaceGroups())
	 {
	     const FaceGroup& fg = *fgPtr;
	     if (fg.id == 5){
	     const StorageSite& faces = fg.site;
	     const VectorT3Array& fluidFaceCoords =
	       dynamic_cast<const VectorT3Array&>(_geomFields.coordinate[faces]);

	     FILE *fp = fopen("./fluidface.dat","w");
	     for (int i=0; i<fg.site.getCount();i++){
	       fprintf(fp, "%i\t%e\t%e\t%e\n", i, fluidFaceCoords[i][0],  fluidFaceCoords[i][1], fluidFaceCoords[i][2]);
	     }
	     fclose(fp);

	     KSearchTree searchTree(fluidFaceCoords);

	     Array<int> fluidNeighbors(1);
     
	     for(int f=0; f<nSolidFaces; f++)
	     {
	       const VectorT3& xf = solidFaceCentroid[f];
	       searchTree.findNeighbors(xf, 1, fluidNeighbors);
	       const int c = fluidNeighbors[0];
	       const VectorT3& xc = fluidFaceCoords[c];
	       const double distanceSquared = mag2(xf-xc);
	       NearestCell& nc = solidFacesNearestCell[f];
	       if ((nc.mesh == 0) || (nc.distanceSquared > distanceSquared))
		 {
		   nc.mesh = &mesh;
		   nc.cell = c;
		   nc.distanceSquared = distanceSquared;
		 }
	     }
	     }
	 }
     }
     
     //for each face in solidFaces, calculate the contact force based on the nearest distance
     const double H = 0.23e-20;
     const double B = 3529e3;
     const double alpha = 0.1127;
     const double gamma = 22.69e9;
     const double alpha01 = 1.6e-9;
     const double alpha02 = 1.99e-9; 

     for(int f=0; f<nSolidFaces; f++)       {
       const double distance = sqrt(solidFacesNearestCell[f].distanceSquared);

       force[f] = -H/(6*PI) * ((1-alpha)/pow(distance,3) + alpha/pow(distance-alpha01,3)) 
	                     + B*exp(-(distance-alpha02)*gamma);

       if (!perUnitArea){
	 force[f] *= solidFaceAreaMag[f];
       }       
     }  

     FILE *fp = fopen("./distance.dat","w");
     for(int f=0; f<nSolidFaces; f++)       {
       const double distance = sqrt(solidFacesNearestCell[f].distanceSquared);
       fprintf(fp, "%i\t%e\n", f, distance);
     }
     fclose(fp); 
     FILE *fp2 = fopen("./force.dat","w");
     for(int f=0; f<nSolidFaces; f++)       {
       fprintf(fp2, "%i\t%e\t%e\t%e\n", f, force[f][0],force[f][1],force[f][2]);
     }
     fclose(fp2); 
     FILE *fp1 = fopen("./beamCoords.dat","w");
     for(int f=0; f<nSolidFaces; f++)       {
       fprintf(fp1, "%i\t%e\t%e\t%e\n", f, solidFaceCentroid[f][0], solidFaceCentroid[f][1],solidFaceCentroid[f][2]); 
     }
     fclose(fp1);
     
   }
 private:
   const MeshList _meshes;
   const GeomFields& _geomFields;
   ContactFields& _contactFields;


};

template<class T>
ContactModel<T>::ContactModel(const GeomFields& geomFields,
                              ContactFields& contactFields,
                              const MeshList& meshes) :
  Model(meshes),
  _impl(new Impl(geomFields,contactFields,meshes))
{
  logCtor();
}


template<class T>
ContactModel<T>::~ContactModel()
{
  logDtor();
}

template<class T>
void
ContactModel<T>::init()
{
  _impl->init();
}
 
template<class T>
void
ContactModel<T>::computeSolidSurfaceForce(const StorageSite& particles)
{
  return _impl->computeSolidSurfaceForce(particles,false);
}

template<class T>
void
ContactModel<T>::computeSolidSurfaceForcePerUnitArea(const StorageSite& particles)
{
  return _impl->computeSolidSurfaceForce(particles,true);
}