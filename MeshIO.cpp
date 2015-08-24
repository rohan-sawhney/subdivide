#include "MeshIO.h"
#include "MeshBuilder.h"
#include "Mesh.h"

Index parseFaceIndex(const std::string& token)
{
    std::stringstream in(token);
    std::string indexString;
    int indices[3] = {-1, -1, -1};
    
    int i = 0;
    while(getline(in,indexString,'/')) {
        if (indexString != "\\") {
            std::stringstream ss(indexString);
            ss >> indices[i++];
        }
    }
    
    // decrement since indices in OBJ files are 1-based
    return Index(indices[0]-1,
                 indices[1]-1,
                 indices[2]-1);
}

bool MeshIO::read(std::ifstream& in, Mesh& mesh)
{
    MeshData data;
    
    // parse obj format
    std::string line;
    while(getline(in, line)) {
        std::stringstream ss(line);
        std::string token;
        
        ss >> token;
        
        if (token == "v") {
            double x, y, z;
            ss >> x >> y >> z;
            
            data.positions.push_back(Eigen::Vector3d(x, y, z));
            
        } else if (token == "vt") {
            double u, v;
            ss >> u >> v;
            
            data.uvs.push_back(Eigen::Vector3d(u,v,0));
            
        } else if (token == "vn") {
            double x, y, z;
            ss >> x >> y >> z;
            
            data.normals.push_back(Eigen::Vector3d(x, y, z));
            
        } else if (token == "f") {
            std::vector<Index> faceIndices;
            
            while (ss >> token) {
                Index index = parseFaceIndex(token);
                if (index.position < 0) {
                    getline(in, line);
                    size_t i = line.find_first_not_of("\t\n\v\f\r ");
                    index = parseFaceIndex(line.substr(i));
                }
                
                faceIndices.push_back(index);
            }
            
            data.indices.push_back(faceIndices);
        }
    }
    
    return MeshBuilder::buildMesh(data, mesh);
}

std::string stringRep(const Eigen::Vector3d& v)
{
    return std::to_string(v.x()) + " " + std::to_string(v.y()) + " " + std::to_string(v.z());
}

void MeshIO::write(std::ofstream& out, const Mesh& mesh)
{
    std::unordered_map<std::string, int> vertexMap;
    std::unordered_map<std::string, int> uvMap;
    std::unordered_map<std::string, int> normalMap;
    
    // write vertices
    int index = 1;
    for (VertexCIter v = mesh.vertices.begin(); v != mesh.vertices.end(); v++) {
        out << "v " << v->position.x() << " "
                    << v->position.y() << " "
                    << v->position.z() << std::endl;
        
        vertexMap[stringRep(v->position)] = index;
        index++;
    }
    
    // write uvs
    index = 1;
    for (VectorCIter uv = mesh.uvs.begin(); uv != mesh.uvs.end(); uv++) {
        out << "vt " << uv->x() << " "
                     << uv->y() << std::endl;
        
        uvMap[stringRep(*uv)] = index;
        index++;
    }
    
    // write normals
    index = 1;
    for (VectorCIter n = mesh.uvs.begin(); n != mesh.normals.end(); n++) {
        out << "vn " << n->x() << " "
                     << n->y() << " "
                     << n->z() << std::endl;
        
        normalMap[stringRep(*n)] = index;
        index++;
    }
    
    // write faces
    index = 0;
    for (FaceCIter f = mesh.faces.begin(); f != mesh.faces.end(); f++) {
        HalfEdgeIter he = mesh.faces[index].he;
        
        if (he->onBoundary) {
            continue;
        }
        
        out << "f ";
        int j = 0;
        do {
            out << vertexMap[stringRep(he->vertex->position)] << "/"
                << uvMap[stringRep(he->uv)] << "/"
                << normalMap[stringRep(he->normal)] << " ";
            j++;
            
            he = he->next;
        } while (he != mesh.faces[index].he);
        
        index ++;
    }
}