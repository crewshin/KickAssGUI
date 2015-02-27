#include <QString>

#ifndef ARGUMENTBUILDER_H
#define ARGUMENTBUILDER_H

class ArgumentBuilder
{
public:
    ArgumentBuilder();

    QString argResX;
    QString argResY;
    QString argAA;
    QString argDiffuseSamples;
    QString argGlossySamples;
    QString argMBShutterStart;
    QString argMBShutterEnd;
    QString argAAFilter;
    QString argBucketSize;
    QString argBucketScanning;
    QString argVerboseLevel;
    
    QString argRayTotalDepth;
    QString argRayDiffuseDepth;
    QString argRayGlossyDepth;
    QString argRayReflectionDepth;
    QString argRayRefractionDepth;
    
    QString argGammaOutput;
    QString argGammaTexture;
    QString argGammaLight;
    QString argGammaShader;
    
    QString argIgnoreTextureMaps;
    QString argIgnoreShaders;
    QString argIgnoreBackgroundShaders;
    QString argIgnoreAtmosphereShaders;
    QString argIgnoreLights;
    QString argIgnoreShadows;
    QString argIgnoreMeshSubdivision;
    QString argIgnoreDisplacement;
    QString argIgnoreMotionBlur;
    QString argIgnoreSSS;
    
    QString argMiscInteractiveNav;
    QString argMiscFlatShading;
    
    QString argDisableProgressive;
    QString argDisableDirectLighting;
    
    
private:
    
};

#endif // ARGUMENTBUILDER_H
