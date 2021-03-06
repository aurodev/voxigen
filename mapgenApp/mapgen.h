#ifndef _voxigen_mapgen_h_
#define _voxigen_mapgen_h_

#include "./testApp/world.h"
#include "colorMap.h"

#include <glbinding/gl/gl.h>
using namespace gl;

#include <imgui.h>

class MapGen
{
public:
    MapGen();
    
    void initialize();
    void setSize(int width, int height);

    void draw();

private:
    void updateTexture();
    void updatePlateTexture(std::vector<GLubyte> &textureBuffer);
    void updatePlateInfoTexture(std::vector<GLubyte> &textureBuffer);
    void updatePlateDistanceTexture(std::vector<GLubyte> &textureBuffer);
//    void updateContinentTexture(std::vector<GLubyte> &textureBuffer);
//    void updateGeometryTexture(std::vector<GLubyte> &textureBuffer);
    void updateHeightMapTexture(std::vector<GLubyte> &textureBuffer);
//    void updateCollisionInfoTexture(std::vector<GLubyte> &textureBuffer);
//    void updatePlateWithInfoTexture(std::vector<GLubyte> &textureBuffer);

    void generate();

    bool m_show;

    int m_width;
    int m_height;

    World m_world;
    int m_worldWidth;
    int m_worldHeight;
    int m_worldDepth;

    WorldGenerator *m_worldGenerator;
    int m_noiseSeed;
    float m_plateFrequency;
    float m_continentFrequency;

    bool m_textureValid;
    GLuint m_textureId;
    int m_textureWidth;
    int m_textureHeight;

    int m_layerIndex;
    std::vector<char> m_layerNames;
    int m_info;
    int m_overlay;
    int m_overlayVector;

    int m_plateCount;
    std::vector<std::tuple<int, int, int>> m_plateColors;
    
    voxigen::ColorMap m_tempColorMap;
    voxigen::ColorMap m_moistureColorMap;
    voxigen::ColorMap2D m_biomeColorMap;

    ImVec2 lastDrawPos;
    ImVec2 lastDrawSize;
};

#endif//_voxigen_mapgen_h_