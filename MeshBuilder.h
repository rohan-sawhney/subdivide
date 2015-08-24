#ifndef MESH_BUILDER_H
#define MESH_BUILDER_H

#include "Types.h"

class Index {
public:
    Index() {}
    
    Index(int v, int vt, int vn): position(v), uv(vt), normal(vn) {}
    
    bool operator<(const Index& i) const {
        if (position < i.position) return true;
        if (position > i.position) return false;
        if (uv < i.uv) return true;
        if (uv > i.uv) return false;
        if (normal < i.normal) return true;
        if (normal > i.normal) return false;
        
        return false;
    }
    
    int position;
    int uv;
    int normal;
};

class MeshData {
public:
    std::vector<Eigen::Vector3d> positions;
    std::vector<Eigen::Vector3d> uvs;
    std::vector<Eigen::Vector3d> normals;
    std::vector<std::vector<Index>> indices;
};

class MeshBuilder {
public:
    // builds the halfedge mesh
    static bool buildMesh(const MeshData& data, Mesh& mesh);
    
private:
    // reserves spave for mesh vertices, uvs, normals and faces
    static void preallocateMeshElements(const MeshData& data, Mesh& mesh);
    
    // sets index for vertices
    static  void indexVertices(Mesh& mesh);
    
    // checks if any vertex is not contained in a face
    static void checkIsolatedVertices(const Mesh& mesh);
    
    // checks if a vertex is non-manifold
    static void checkNonManifoldVertices(const Mesh& mesh);
};

#endif
