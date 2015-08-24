#include "Face.h"
#include "HalfEdge.h"
#include "Vertex.h"

bool Face::isBoundary() const
{
    return he->onBoundary;
}

double Face::area() const
{
    if (isBoundary()) {
        return 0;
    }
    
    return 0.5 * normal().norm();
}

Eigen::Vector3d Face::normal() const
{
    Eigen::Vector3d a = he->vertex->position;
    Eigen::Vector3d b = he->next->vertex->position;
    Eigen::Vector3d c = he->next->next->vertex->position;
    
    Eigen::Vector3d v1 = a - b;
    Eigen::Vector3d v2 = c - b;
    
    return v1.cross(v2);
}

double Face::signedVolume() const
{
    Eigen::Vector3d a = he->vertex->position;
    Eigen::Vector3d b = he->next->vertex->position;
    Eigen::Vector3d c = he->next->next->vertex->position;
    
    double v321 = c.x() * b.y() * a.z();
    double v231 = b.x() * c.y() * a.z();
    double v312 = c.x() * a.y() * b.z();
    double v132 = a.x() * c.y() * b.z();
    double v213 = b.x() * a.y() * c.z();
    double v123 = a.x() * b.y() * c.z();
    
    return (-v321 + v231 + v312 - v132 - v213 + v123) / 6.0;
}