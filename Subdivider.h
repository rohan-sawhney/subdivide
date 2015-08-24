#ifndef SUBDIVIDER_H
#define SUBDIVIDER_H

#include "Types.h"
#include "Mesh.h"

class Subdivider {
public:
    // implements loop subdivision scheme
    void process(Mesh& mesh) const;
    
private:
    // creates an edge vertex as a linear combination of neighboring vertices
    void createNewEdgeVertex(Eigen::Vector3d& position,
                             const HalfEdgeCIter& he) const;
    
    // splits a triangle face into four faces
    void createNewFaces(MeshData& data, const std::vector<int>& indices) const;
    
    // updates position of old vertex as a linear combination of neighboring vertices
    void updateOldVertexPosition(MeshData& data, const VertexCIter& v) const;
};

#endif
