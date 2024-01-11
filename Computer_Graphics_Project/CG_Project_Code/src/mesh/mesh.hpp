/**
 * @file mesh.hpp
 * @brief "*Need to be Added*"
*/
#ifndef MESH_HPP
#define MESH_HPP

#include "utils/utils.hpp" // Include necessary dependencies

class Mesh {
public:
    Mesh(); // Constructor
    // void createMeshObject(unsigned int &program, unsigned int &VAO, const std::string &objPath,std::string &texturePath);
    // void createMeshObject(unsigned int &program, unsigned int &VAO, const std::string &objPath, const std::string &texturePath, const std::string &normalMapPath);
    void createMeshObject(unsigned int &program, unsigned int &VAO, const std::string &objPath,
                            const std::string &yz_texturePath, const std::string &yz_normalMapPath,
                            const std::string &xz_texturePath, const std::string &xz_normalMapPath,
                            const std::string &xy_texturePath, const std::string &xy_normalMapPath);
    // void loadTexture(std::string texturePath, unsigned int &program, std::vector<int> &vertex_indices, GLfloat *vertex_textures);
    // void loadTexture(const std::string& texturePath, const std::string& normalMapPath, unsigned int& program, std::vector<int>& vertex_indices, GLfloat* textureCoordinates, GLfloat* vertex_tangents, GLfloat* vertex_bitangents);
    void loadTexture(const std::string& texturePath, const std::string& normalMapPath, unsigned int& program, std::vector<int>& vertex_indices, GLfloat* textureCoordinates, GLfloat* vertex_tangents, GLfloat* vertex_bitangents,int planeNumber);

};

#endif // MESH_HPP
