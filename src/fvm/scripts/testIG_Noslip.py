#!/usr/bin/env python

import sys
sys.setdlopenflags(0x100|0x2)

import fvmbaseExt
import importers

atype = 'double'
#atype = 'tangent'

if atype == 'double':
    import models_atyped_double as models
    import exporters_atyped_double as exporters
elif atype == 'tangent':
    import models_atyped_tangent_double as models
    import exporters_atyped_tangent_double as exporters


from FluentCase import FluentCase

#fvmbaseExt.enableDebug("cdtor")

fileBase = None
numIterations = 10000
#fileBase = "/home/sm/prism-meshes/slipbc/Noslipbc_ig"
#fileBase = "/home/sm/a/data/wj"
fileBase="/home/aerosun/a/schigull/FVM-trial/src/fvm/test/testIG_Noslip"
def usage():
    print "Usage: %s filebase [outfilename]" % sys.argv[0]
    print "Where filebase.cas is a Fluent case file."
    print "Output will be in filebase-prism.dat if it is not specified."
    sys.exit(1)

def advance(models,dmodel,nstart,niter):
    for i in range(nstart,niter):
        try:
            converged = True
            for m in models:
                if not m.advance(1):
                    converged = False
            if (i%5)== 0:
                dmodel.advance(1)
            if converged:
                break
        except KeyboardInterrupt:
            break
        if ((i+1)%1000)==0:
            saveData(flowFields,reader,fileBase,i)
def saveData(flowFields,reader,fileBase,i):
    writer = exporters.FluentDataExporterA(reader,fileBase+str(i+1)+"-memosanew.dat",False,0)
    writer.init()
    writer.writeScalarField(flowFields.pressure,1)
    writer.writeScalarField(flowFields.density,101)
    writer.writeVectorField(flowFields.velocity,111)
    writer.writeScalarField(flowFields.massFlux,18)
    writer.finish()
        
def advancenew(models,dmodel,nstart,niter):
    for i in range(nstart,niter):
        try:
            for m in models:
                m.advance(1)
                    
            if (i%5)== 0:
                dmodel.advance(1)
            
        except KeyboardInterrupt:
            break
        if ((i+1)%1000)==0:
            saveData(flowFields,reader,fileBase,i)
            
outfile = None
if __name__ == '__main__' and fileBase is None:
    if len(sys.argv) < 2:
        usage()
    fileBase = sys.argv[1]
    if len(sys.argv) == 3:
        outfile = sys.argv[2]

if outfile == None:
    outfile = fileBase+"-memosanew.dat"
    
reader = FluentCase(fileBase+".cas")
#caseBase = fileBase + "Noslipbc_igc"
#reader = FluentCase(caseBase+".cas")
#import debug
reader.read();

meshes = reader.getMeshList()

import time
t0 = time.time()

geomFields =  models.GeomFields('geom')
metricsCalculator = models.MeshMetricsCalculatorA(geomFields,meshes)

metricsCalculator.init()

if atype == 'tangent':
    metricsCalculator.setTangentCoords(0,7,1)

flowFields =  models.FlowFields('flow')

fmodel = models.FlowModelA(geomFields,flowFields,meshes)

dmodel = models.IdealGasDensityModelA(geomFields,flowFields,meshes)

## set density  model settings
for i,vc in dmodel.getVCMap().iteritems():
    vc['pressure'] = flowFields.pressure
    vc['temperature']= 300.0
    vc['operatingPressure'] = 101325.0
    vc['molecularWeight']= 28.9645
    
    

reader.importFlowBCs(fmodel)

momSolver = fvmbaseExt.AMG()
momSolver.relativeTolerance = 1e-1
momSolver.nMaxIterations = 20
momSolver.maxCoarseLevels=20
momSolver.verbosity=0

contSolver = fvmbaseExt.AMG()
#pc = fvmbaseExt.AMG()
#pc.verbosity=0
#contSolver = fvmbaseExt.BCGStab()
#contSolver.preconditioner = pc
contSolver.relativeTolerance = 1e-1
contSolver.nMaxIterations = 20
contSolver.verbosity=0
contSolver.maxCoarseLevels=20

foptions = fmodel.getOptions()

foptions.momentumLinearSolver = momSolver
foptions.pressureLinearSolver = contSolver

foptions.momentumTolerance=1e-5
foptions.continuityTolerance=1e-6
foptions.setVar("momentumURF",0.7)
foptions.setVar("pressureURF",0.3)
foptions.printNormalizedResiduals=False

"""
if atype=='tangent':
    vcMap = fmodel.getVCMap()
    for i,vc in vcMap.iteritems():
        print vc.getVar('viscosity')
        vc.setVar('viscosity',(1.7894e-5,1))
"""
#import debug

fmodel.init()
#fmodel.advance(numIterations)
initIterations=200
advancenew([fmodel],dmodel,0,initIterations)
advance([fmodel],dmodel,initIterations,numIterations)


t1 = time.time()
if outfile != '/dev/stdout':
    print '\nsolution time = %f' % (t1-t0)

#writer = exporters.FluentDataExporterA(reader,fileBase+"-prism.dat",False,0)
        

if (atype=='tangent'):
    writer = exporters.FluentDataExporterA(reader,fileBase+"-prism-tangent.dat",False,1)
    writer.init()
    writer.writeScalarField(flowFields.pressure,1)
    writer.writeVectorField(flowFields.velocity,111)
    writer.writeScalarField(flowFields.massFlux,18)
    writer.finish()
    
saveData(flowFields,reader,fileBase,1)

