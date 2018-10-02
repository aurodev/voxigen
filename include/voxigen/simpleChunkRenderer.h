#ifndef _voxigen_simpleChunkRenderer_h_
#define _voxigen_simpleChunkRenderer_h_

#include "voxigen/rendererSettings.h"

#include "voxigen/voxigen_export.h"
#include "voxigen/chunk.h"
#include "voxigen/chunkHandle.h"
#include "voxigen/chunkInfo.h"
#include "voxigen/cubicMeshBuilder.h"
#include "voxigen/textureAtlas.h"
#include "voxigen/chunkTextureMesh.h"

//#include "PolyVox/CubicSurfaceExtractor.h"
//#include "PolyVox/Mesh.h"

#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include <opengl_util/program.h>

namespace voxigen
{

enum class RenderAction
{
    Idle,
    RequestRenderer,
    Meshing,
    Updating
};


/////////////////////////////////////////////////////////////////////////////////////////
//SimpleChunkRenderer
/////////////////////////////////////////////////////////////////////////////////////////
//template<typename _Block>
template<typename _Parent, typename _Chunk>
class SimpleChunkRenderer
{
public:
    typedef _Parent RenderType;
    typedef _Chunk ChunkType;
    typedef ChunkHandle<ChunkType> ChunkHandleType;
    typedef std::shared_ptr<ChunkHandleType> SharedChunkHandle;

//    typedef PolyVox::CubicVertex<typename ChunkVolume<_Chunk>::VoxelType> CubicVertex;
//    typedef PolyVox::Mesh<CubicVertex> MeshType;
//    typedef std::shared_ptr<ChunkType> SharedChunk;

    SimpleChunkRenderer();
    ~SimpleChunkRenderer();
    
    enum State
    {
        Init,
        Invalid,
        Occluded,
        Query, 
        QueryWait,
        Dirty,
        Copy,
        Built,
        Empty
    };

    State getState() { return m_state; }
    void setDirty() { m_state=Dirty; }

    RenderAction getAction(){ return m_action; }
    void setAction(RenderAction action){m_action=action;}

    Key getKey(){return Key(m_chunkHandle->regionHash(), m_chunkHandle->hash());}

    void setParent(RenderType *parent);
//    void setRegionHash(RegionHash hash);
    void setChunk(SharedChunkHandle chunk);
    void setTextureAtlas(SharedTextureAtlas textureAtlas) { m_textureAtlas=textureAtlas; }
    void setEmpty();
//    void setChunkOffset(glm::vec3 chunkOffset);
    const glm::vec3 &getChunkOffset() { return m_chunkOffset; }

    void build();//unsigned int instanceData);
    void buildOutline(unsigned int instanceData);

    void buildMesh();
    MeshBuffer clearMesh();
    MeshBuffer setMesh(MeshBuffer &mesh);
    bool update();
    void updated();

    void updateOutline();
    void invalidate();

    void releaseChunkMemory();

    bool incrementCopy();
    void draw();

    void startOcculsionQuery();
    void drawOcculsionQuery();
    bool checkOcculsionQuery(unsigned int &samples);

//#ifndef NDEBUG
    void drawOutline(opengl_util::Program *program, size_t colorId);
//#endif //NDEBUG

    const RegionHash getRegionHash() { return m_chunkHandle->regionHash(); }
    const glm::ivec3 getRegionIndex() { return m_chunkHandle->regionIndex(); }
    const ChunkHash getChunkHash() { return m_chunkHandle->hash(); }
    const glm::ivec3 getChunkIndex() { return m_chunkHandle->chunkIndex(); }
    SharedChunkHandle getChunkHandle() { return m_chunkHandle; }
    const glm::ivec3 &getPosition() { return m_chunkHandle->chunk()->getPosition(); }
    glm::vec3 getGridOffset() const { return m_chunkHandle->regionOffset();/* m_chunkHandle->chunk->getGridOffset();*/ }
    
    size_t getLod() { return m_lod; }
    void setLod(size_t lod);

    void clear();

    unsigned int refCount;
private:
    void calculateMemoryUsed();

    RenderType *m_parent;

    State m_state;
    RenderAction m_action;
//    RegionHash m_regionHash;
    SharedChunkHandle m_chunkHandle;
    SharedTextureAtlas m_textureAtlas;

 //   bool m_empty;

//#ifndef NDEBUG
    bool m_outlineBuilt;
//#endif
    unsigned int m_queryId;

    MeshBuffer m_meshBuffer;
//    unsigned int m_vertexBuffer;
//    unsigned int m_indexBuffer;
//    GLenum m_indexType;

    glm::vec3 m_chunkOffset;
//    unsigned int m_validBlocks;

    bool m_vertexArrayGen;
    unsigned int m_vertexArray;
    unsigned int m_offsetVBO;
    std::vector<glm::vec4> m_ChunkInfoOffset;

    bool m_lodUpdated;
    size_t m_lod;

    size_t m_memoryUsed;
//    MeshType m_mesh;
//    ChunkMesh m_mesh;
    ChunkTextureMesh m_mesh;

    GLsync m_vertexBufferSync;
    int m_delayedFrames;

    bool m_outlineGen;
    unsigned int m_outlineVertexArray;
    unsigned int m_outlineOffsetVBO;
};

}//namespace voxigen

#include "simpleChunkRenderer.inl"

#endif //_voxigen_simpleChunkRenderer_h_