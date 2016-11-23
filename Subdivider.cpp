#include "Subdivider.h"
#include "MeshBuilder.h"
#include <map>

void Subdivider::createNewEdgeVertex(Eigen::Vector3d& position,
                                     const HalfEdgeCIter& he) const
{
    VertexCIter v1 = he->vertex;
    VertexCIter v2 = he->flip->vertex;
    
    if (he->flip->onBoundary) {
        position = (v1->position + v2->position) * 0.5;
        
    } else {
        VertexCIter v3 = he->next->flip->vertex;
        VertexCIter v4 = he->flip->next->flip->vertex;
        
        position = ((v1->position + v2->position) * 3.0 +
                    (v3->position + v4->position)) * 0.125;
    }
}

void Subdivider::createNewFaces(MeshData& data, const std::vector<int>& indices) const
{
    std::vector<Index> faceIndices;
    faceIndices.push_back(Index(indices[0],-1,-1));
    faceIndices.push_back(Index(indices[1],-1,-1));
    faceIndices.push_back(Index(indices[5],-1,-1));
    data.indices.push_back(faceIndices);
    
    faceIndices.clear();
    faceIndices.push_back(Index(indices[5],-1,-1));
    faceIndices.push_back(Index(indices[1],-1,-1));
    faceIndices.push_back(Index(indices[3],-1,-1));
    data.indices.push_back(faceIndices);
    
    faceIndices.clear();
    faceIndices.push_back(Index(indices[3],-1,-1));
    faceIndices.push_back(Index(indices[1],-1,-1));
    faceIndices.push_back(Index(indices[2],-1,-1));
    data.indices.push_back(faceIndices);
    
    faceIndices.clear();
    faceIndices.push_back(Index(indices[5],-1,-1));
    faceIndices.push_back(Index(indices[3],-1,-1));
    faceIndices.push_back(Index(indices[4],-1,-1));
    data.indices.push_back(faceIndices);
}

void Subdivider::updateOldVertexPosition(MeshData& data, const VertexCIter& v) const
{
    HalfEdgeCIter he = v->he;
    
    Eigen::Vector3d sumPosition;
    sumPosition.setZero();
    bool boundaryVert = false;
    
    int n = 0;
    do {
        if (he->onBoundary) {
            
            data.positions[v->index] = data.positions[v->index] * 6.0 +
                                       he->flip->vertex->position;
            
            if (he->flip->next->onBoundary) {
                data.positions[v->index] += he->flip->next->flip->vertex->position;
                
            } else {
                HalfEdgeCIter curr = he->flip->next;
                HalfEdgeCIter prev = he;
                do {
                    prev = curr;
                    curr = curr->flip->next;
                    
                } while (curr != he);
                
                data.positions[v->index] += prev->vertex->position;
            }
            
            data.positions[v->index] *= 0.125;
            
            boundaryVert = true;
            break;
            
        } else {
            sumPosition += he->flip->vertex->position;
            n ++;
        }
        
        he = he->flip->next;
        
    } while (he != v->he);
    
    if (!boundaryVert) {
        double beta = 0.0;
        if (n == 3) {
            beta = 0.1875;
            
        } else {
            beta = 3.0 * 0.125 / n;
        }
        data.positions[v->index] = (1 - n*beta) * data.positions[v->index] + beta * sumPosition;
    }
}

void Subdivider::process(Mesh& mesh) const
{
    MeshData data;
    
    // transfer current vertex positions
    for (VertexCIter v = mesh.vertices.begin(); v != mesh.vertices.end(); v++) {
        data.positions.push_back(v->position);
    }
    
    std::map<EdgeCIter, int> edgeIndexMap;
    for (FaceCIter f = mesh.faces.begin(); f != mesh.faces.end(); f++) {
        
        HalfEdgeIter he = f->he;
        std::vector<int> indices;
        
        // calculate positions of new vertices
        do {
            indices.push_back(he->vertex->index);
            
            if (!he->edge->seen) {
                
                Eigen::Vector3d position;
                createNewEdgeVertex(position, he);
                
                edgeIndexMap[he->edge] = (int)data.positions.size();
                data.positions.push_back(position);
                
                he->edge->seen = true;
            }
            
            indices.push_back(edgeIndexMap[he->edge]);
            he = he->next;
            
        } while (he != f->he);
        
        // create four new faces
        createNewFaces(data, indices);
    }
    
    // update old vertex position
    for (VertexIter v = mesh.vertices.begin(); v != mesh.vertices.end(); v++) {
        updateOldVertexPosition(data, v);
    }
    
    MeshBuilder::buildMesh(data, mesh);
}
