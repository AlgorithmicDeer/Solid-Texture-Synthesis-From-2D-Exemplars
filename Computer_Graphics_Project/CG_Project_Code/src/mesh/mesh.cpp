/**
 * @file mesh.cpp
 * @brief "*Need to be Added*"
*/
#include "mesh.hpp"
#define STBI_NO_SIMD
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include "globals/globals.h"
Mesh::Mesh() {
    // Constructor definition if needed
}

//============================================================ Reference for TBN calculation based on Lab7, CSE 333/533: Computer Graphics, Instructor - Ojaswa Sharma =====================================================================================
void computeTangentAndBitangent(
    GLfloat pos1x, GLfloat pos1y, GLfloat pos1z,
    GLfloat pos2x, GLfloat pos2y, GLfloat pos2z,
    GLfloat pos3x, GLfloat pos3y, GLfloat pos3z,
    GLfloat uv1x, GLfloat uv1y,
    GLfloat uv2x, GLfloat uv2y,
    GLfloat uv3x, GLfloat uv3y,
    glm::vec3 &tangent,
    glm::vec3 &bitangent
) {
    // Create vectors for the edges of the triangle in both position and UV space
    glm::vec3 edge1(pos2x - pos1x, pos2y - pos1y, pos2z - pos1z);
    glm::vec3 edge2(pos3x - pos2x, pos3y - pos2y, pos3z - pos2z);

    glm::vec2 deltaUV1(uv2x - uv1x, uv2y - uv1y);
    glm::vec2 deltaUV2(uv3x - uv2x, uv3y - uv2y);

    // Compute the tangent and bitangent
    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangent.x = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangent.y = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangent.z = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangent = glm::normalize(tangent);

    bitangent.x = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangent.y = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangent.z = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangent = glm::normalize(bitangent);
}
//============================================================ Reference for TBN calculation based on Lab7, CSE 333/533: Computer Graphics, Instructor - Ojaswa Sharma =====================================================================================

// void Mesh::createMeshObject(unsigned int &program, unsigned int &VAO, const std::string &objPath,std::string &texturePath) {
// void Mesh::createMeshObject(unsigned int &program, unsigned int &VAO, const std::string &objPath, const std::string &texturePath, const std::string &normalMapPath){
void Mesh::createMeshObject(unsigned int &program, unsigned int &VAO, const std::string &objPath,
                            const std::string &yz_texturePath, const std::string &yz_normalMapPath,
                            const std::string &xz_texturePath, const std::string &xz_normalMapPath,
                            const std::string &xy_texturePath, const std::string &xy_normalMapPath) {
                                
    vector<int> vertex_indices, uv_indices, normal_indices;
    vector<glm::vec3> temp_vertices;
    vector<glm::vec2> temp_uvs;
    vector<glm::vec3> temp_normals;
    int ct = 0;

    scale = 0.05; // Change Scale of the model as needed
    if (objPath == "src/Objects/dragon.obj") {
        scale = 1; 
    }
     if (objPath == "src/Objects/penguin.obj") {
        scale = 30; 
    }
    FILE *file = fopen(objPath.c_str(), "r");
    if (file == NULL) {
        fprintf(stderr, "File not found: %s\n", objPath.c_str());
        return;
    }

    while(true){

        char head[128];
        // read the first word of the line
        int res = fscanf(file, "%s", head);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.

        if ( strcmp( head, "v" ) == 0 ){
            glm::vec3 vertex;
            fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
            // printf("%f %f %f\n", vertex.x, vertex.y, vertex.z );
            temp_vertices.push_back(vertex);
            ct++;
        }

        else if ( strcmp( head, "vt" ) == 0 ){
            glm::vec2 uv;
            fscanf(file, "%f %f\n", &uv.x, &uv.y );
            // printf("%f %f\n", uv.x, uv.y );
            temp_uvs.push_back(uv);
        }

        else if ( strcmp( head, "vn" ) == 0 ){
            glm::vec3 normal;
            fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z );
            // printf("%f %f %f\n", normal.x, normal.y, normal.z );
            temp_normals.push_back(normal);
        }

        else if ( strcmp( head, "f" ) == 0 ){
            string vertex1, vertex2, vertex3;
            int vertexIndex[3], uvIndex[3], normalIndex[3];
            int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vertexIndex[0], &uvIndex[0], &normalIndex[0], &vertexIndex[1], &uvIndex[1], &normalIndex[1], &vertexIndex[2], &uvIndex[2], &normalIndex[2] );
            if (matches != 9){
                printf("OBJ File may not contain texture coordinates or normal coordinates\n");
            }
            vertex_indices.push_back(vertexIndex[0]);
            vertex_indices.push_back(vertexIndex[1]);
            vertex_indices.push_back(vertexIndex[2]);
            uv_indices.push_back(uvIndex[0]);
            uv_indices.push_back(uvIndex[1]);
            uv_indices.push_back(uvIndex[2]);
            normal_indices.push_back(normalIndex[0]);
            normal_indices.push_back(normalIndex[1]);
            normal_indices.push_back(normalIndex[2]);
        }
    }
    fclose(file);

    glUseProgram(program);

    //Bind shader variables
    int vVertex_attrib = glGetAttribLocation(program, "vVertex");
    if(vVertex_attrib == -1) {
        fprintf(stderr, "Could not bind location: vVertex\n");
        exit(0);
    }

    int vNormal_attrib = glGetAttribLocation(program, "vNormal");
    if(vNormal_attrib == -1) {
        std::cout << "Could not bind location: vNormal\n" ;
    }
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    int axisOfInterest = glGetAttribLocation(program, "axisOfInterest");
    if(axisOfInterest == -1) {
        // std::cout << "Could not bind location: axisOfInterest\n" ;
    }
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    // else {
    //     // std::cout << "vNormal found at location " << vNormal_attrib << std::endl;
    // }

    GLfloat *shape_vertices = new GLfloat[vertex_indices.size()*3];
    GLfloat *vertex_normals = new GLfloat[normal_indices.size()*3];
    // Allocate memory for texture coordinates (2 per vertex)
    GLfloat *textureCoordinates = new GLfloat[vertex_indices.size() * 2];

//============================================================ Reference for TBN calculation based on Lab7, CSE 333/533: Computer Graphics, Instructor - Ojaswa Sharma =====================================================================================
    GLfloat *vertex_tangents = new GLfloat[vertex_indices.size()*3];
    GLfloat *vertex_bitangents = new GLfloat[vertex_indices.size()*3];
    glm::vec3 x_axis = glm::vec3(1, 0, 0);
    glm::vec3 y_axis = glm::vec3(0, 1, 0);
    glm::vec3 z_axis = glm::vec3(0, 0, 1);
//============================================================ Reference for TBN calculation based on Lab7, CSE 333/533: Computer Graphics, Instructor - Ojaswa Sharma =====================================================================================
    nVertices = vertex_indices.size()*3;

    for(int i=0; i<vertex_indices.size(); i++){
        int vertexIndex = vertex_indices[i];
        shape_vertices[i*3] = temp_vertices[vertexIndex-1][0]*scale;
        shape_vertices[i*3+1] = temp_vertices[vertexIndex-1][1]*scale;
        shape_vertices[i*3+2] = temp_vertices[vertexIndex-1][2]*scale;
    }
  
    //generated normals for the triangle mesh
    for(int i=0;i<vertex_indices.size();i+=3){
        glm::vec3 v1 = glm::vec3(shape_vertices[(i+1)*3]-shape_vertices[i*3], shape_vertices[(i+1)*3+1]-shape_vertices[i*3+1],shape_vertices[(i+1)*3+2]-shape_vertices[i*3+2]);
	    glm::vec3 v2 = glm::vec3(shape_vertices[(i+2)*3]-shape_vertices[(i+1)*3], shape_vertices[(i+2)*3+1]-shape_vertices[(i+1)*3+1],shape_vertices[(i+2)*3+2]-shape_vertices[(i+1)*3+2]);

        glm::vec3 n = norm(v1, v2);
        
        vertex_normals[i*3] = n.x;
        vertex_normals[(i+1)*3] = n.x;
        vertex_normals[(i+2)*3] = n.x;
        vertex_normals[i*3+1] = n.y;
        vertex_normals[(i+1)*3+1] = n.y;
        vertex_normals[(i+2)*3+1] = n.y;
        vertex_normals[i*3+2] = n.z;
        vertex_normals[(i+1)*3+2] = n.z;
        vertex_normals[(i+2)*3+2] = n.z;
    
//============================================================ Reference for TBN calculation based on Lab7, CSE 333/533: Computer Graphics, Instructor - Ojaswa Sharma =====================================================================================
    glm::vec3 tangent, bitangent;
    
    computeTangentAndBitangent(
            shape_vertices[i*3], shape_vertices[i*3+1], shape_vertices[i*3+2], // current vertex
            shape_vertices[(i+1)*3], shape_vertices[(i+1)*3+1], shape_vertices[(i+1)*3+2], // next vertex
            shape_vertices[(i+2)*3], shape_vertices[(i+2)*3+1], shape_vertices[(i+2)*3+2], // next of next vertex
            textureCoordinates[i*2], textureCoordinates[i*2+1], // current uv
            textureCoordinates[(i+1)*2], textureCoordinates[(i+1)*2+1], // next uv
            textureCoordinates[(i+2)*2], textureCoordinates[(i+2)*2+1], // next of next uv
            tangent, bitangent
        );

        for (int j = 0; j < 3; j++) 
        {
            vertex_tangents[(i+j)*3] = tangent.x;
            vertex_tangents[(i+j)*3+1] = tangent.y;
            vertex_tangents[(i+j)*3+2] = tangent.z;

            vertex_bitangents[(i+j)*3] = bitangent.x;
            vertex_bitangents[(i+j)*3+1] = bitangent.y;
            vertex_bitangents[(i+j)*3+2] = bitangent.z;
        }
    }
//============================================================ Reference for TBN calculation based on Lab7, CSE 333/533: Computer Graphics, Instructor - Ojaswa Sharma =====================================================================================

//------------------------------------------------------------------------------------------------   
    // Initialize index variable
    int uvIndexIterator = 0;

    // Iterate while uvIndexIterator is less than the number of uv indices
    while (uvIndexIterator < uv_indices.size()) {
        // Retrieve the current uv index
        int uvIndex = uv_indices[uvIndexIterator]-1;

        // Calculate the starting index for the current texture coordinate pair
        int textureIndex = uvIndexIterator * 2;

        // Iterate through each component of the texture coordinates (u and v)
        for (int j = 0; j < 2; j++) {
            // If the current component is the first (u), assign the corresponding value from temp_uvs
            if (j == 0) {
                textureCoordinates[textureIndex + j] = temp_uvs[uvIndex][0]; // Assign u component
            }
            // If the current component is the second (v), assign the corresponding value from temp_uvs
            else {
                textureCoordinates[textureIndex + j] = temp_uvs[uvIndex][1]; // Assign v component
            }
        }

        // Increment the index variable for the next iteration
        uvIndexIterator++;
    }
// ------------------------------------------------------------------------------------------------
 
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    GLfloat *orientedVertices = new GLfloat[uv_indices.size()*3];
    for(int i=0;i<vertex_indices.size();i+=3){
        glm::vec3 selected_axis;
        glm::vec3 normal;
        for (int j = 0; j < 3; ++j) {
            normal[j] = vertex_normals[i * 3 + j];
        }
        if (normal.x >= normal.y && normal.x >= normal.z) {
            if (normal.x > 0.0f) {
                selected_axis = x_axis;
            } else {
                selected_axis = -x_axis;
            }
        } else if (normal.y > normal.x && normal.y >= normal.z) {
            if (normal.y > 0.0f) {
                selected_axis = y_axis;
            } else {
                selected_axis = -y_axis;
            }
        } else {
            if (normal.z > 0.0f) {
                selected_axis = z_axis;
            } else {
                selected_axis = -z_axis;
            }
        }

        for (int j = 0; j < 3; ++j) {
        orientedVertices[i * 3 + j] = selected_axis[j];
        orientedVertices[(i + 1) * 3 + j] = selected_axis[j];
        orientedVertices[(i + 2) * 3 + j] = selected_axis[j];
        }
    }
	
// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$

    //Generate VAO object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    //Create VBOs for the VAO
    GLuint vertex_VBO; // Vertex Buffer
    glGenBuffers(1, &vertex_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertex_indices.size()*3, shape_vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vVertex_attrib);
    glVertexAttribPointer(vVertex_attrib, 3, GL_FLOAT, GL_FALSE, 00, 0);
    delete []shape_vertices;

    GLuint normal_VBO; // Normal Buffer
    glGenBuffers(1, &normal_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, normal_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertex_indices.size()*3, vertex_normals, GL_STATIC_DRAW);
    glEnableVertexAttribArray(vNormal_attrib);
    glVertexAttribPointer(vNormal_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
    delete []vertex_normals;

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    GLuint axis_VBO; // Axis Buffer
    glGenBuffers(1, &axis_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, axis_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat)*vertex_indices.size()*3, orientedVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(axisOfInterest);
    glVertexAttribPointer(axisOfInterest, 3, GL_FLOAT, GL_FALSE, 0, 0);

    // Load textures for yz plane
    loadTexture(yz_texturePath,yz_normalMapPath,program, vertex_indices, textureCoordinates,vertex_tangents,vertex_bitangents,1);
    // // Load textures for xz plane
    loadTexture(xz_texturePath,xz_normalMapPath,program, vertex_indices, textureCoordinates,vertex_tangents,vertex_bitangents,2);
    // // Load textures for xy plane
    loadTexture(xy_texturePath,xy_normalMapPath,program, vertex_indices, textureCoordinates,vertex_tangents,vertex_bitangents,3); 

// $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$4

//  loadTexture(texturePath,normalMapPath,program, vertex_indices, textureCoordinates,vertex_tangents,vertex_bitangents);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0); 

}

// Loads a texture from the specified file path and associates it with the provided vertex indices and texture coordinates.
// Inspired by: https://learnopengl.com/Getting-started/Textures
// void Mesh::loadTexture(std::string texturePath, unsigned int &program, std::vector<int> &vertex_indices, GLfloat *textureCoordinates) {
void Mesh::loadTexture(const std::string& texturePath, const std::string& normalMapPath, unsigned int& program, std::vector<int>& vertex_indices, GLfloat* textureCoordinates, GLfloat* vertex_tangents, GLfloat* vertex_bitangents,int planeNumber) {
    // Flip the texture vertically during loading for compatibility with OpenGL's coordinate system
    stbi_set_flip_vertically_on_load(true);

    // Load the texture data from the specified file path
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);

    // Check if the texture data was loaded successfully
    if (data) {
        // Generate a new texture object
        glGenTextures(1, &texture);
        // Activate the texture unit
        glActiveTexture(GL_TEXTURE0);
        // Bind the texture to the 2D texture target
        glBindTexture(GL_TEXTURE_2D, texture);
        // Generate the texture image
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        // Set the texture parameters for minification and magnification
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    } else {
        // Print an error message if the texture failed to load
        std::cout << "Failed to load texture" << std::endl;
    }

    // Free the loaded image data
    stbi_image_free(data);

    // Enable 2D texturing
    glEnable(GL_TEXTURE_2D);
    // Enable texture coordinate generation for the S, T texture coordinates
    glEnable(GL_TEXTURE_GEN_S);
    glEnable(GL_TEXTURE_GEN_T);
    // Set the texture generation mode for S, T
    glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
    glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

    // // Generate a new buffer object for the texture VBO
    // GLuint texture_VBO;
    // glGenBuffers(1, &texture_VBO);
    // // Bind the texture VBO
    // glBindBuffer(GL_ARRAY_BUFFER, texture_VBO);
    // // Fill the texture VBO with the provided texture coordinates
    // glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_indices.size() * 2, textureCoordinates, GL_STATIC_DRAW);

    // // Get the attribute location of the texture coordinates in the shader program
    // int vTexture_attrib = glGetAttribLocation(program, "vTexCoord");
    // if (vTexture_attrib == -1) {
    //     // Print an error message if the texture coordinate attribute location cannot be found
    //     std::cout << "Could not bind location: TexCoord\n";
    // }

    // // Enable the attribute array for the texture coordinates
    // glEnableVertexAttribArray(vTexture_attrib);
    // // Specify the attribute array data for the texture coordinates
    // glVertexAttribPointer(vTexture_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);

    if (planeNumber == 1) {
        // For yz plane
          // Generate a new buffer object for the texture VBO
        GLuint texture_VBO_yz;
        int vTexture_attrib;
        glGenBuffers(1, &texture_VBO_yz);
        glBindBuffer(GL_ARRAY_BUFFER, texture_VBO_yz);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_indices.size() * 2, textureCoordinates, GL_STATIC_DRAW);
        vTexture_attrib = glGetAttribLocation(program, "vTexCoord");
        if (vTexture_attrib == -1) {
            std::cout << "Could not bind location: vTexCoord\n";
        } else {
            std::cout << "vTexCoord found at location " << vTexture_attrib << std::endl;
        }
        // Enable the attribute array for the texture coordinates
        glEnableVertexAttribArray(vTexture_attrib);
        // Specify the attribute array data for the texture coordinates
        glVertexAttribPointer(vTexture_attrib, 2, GL_FLOAT, GL_FALSE, 0, 0);
       
    } else if (planeNumber == 2) {
        // For xz plane
        GLuint texture_VBO_xz;
        int vTexture_attrib2;
        glGenBuffers(1, &texture_VBO_xz);
        glBindBuffer(GL_ARRAY_BUFFER, texture_VBO_xz);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_indices.size() * 2, textureCoordinates, GL_STATIC_DRAW);
        vTexture_attrib2 = glGetAttribLocation(program, "vTexCoord2");
        if (vTexture_attrib2 == -1) {
            std::cout << "Could not bind location: vTexCoord2\n";
        } else {
            std::cout << "vTexCoord2 found at location " << vTexture_attrib2 << std::endl;
        }
        // Enable the attribute array for the texture coordinates
        glEnableVertexAttribArray(vTexture_attrib2);
        // Specify the attribute array data for the texture coordinates
        glVertexAttribPointer(vTexture_attrib2, 2, GL_FLOAT, GL_FALSE, 0, 0);
       
    } else if (planeNumber == 3) {
        // For xy plane
        GLuint texture_VBO_xy;
        int vTexture_attrib3;
        glGenBuffers(1, &texture_VBO_xy);
        glBindBuffer(GL_ARRAY_BUFFER, texture_VBO_xy);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_indices.size() * 2, textureCoordinates, GL_STATIC_DRAW);
        vTexture_attrib3 = glGetAttribLocation(program, "vTexCoord3");
        if (vTexture_attrib3 == -1) {
            std::cout << "Could not bind location: vTexCoord3\n";
        } else {
            std::cout << "vTexCoord3 found at location " << vTexture_attrib3 << std::endl;
        }
        // Enable the attribute array for the texture coordinates
        glEnableVertexAttribArray(vTexture_attrib3);
        // Specify the attribute array data for the texture coordinates
        glVertexAttribPointer(vTexture_attrib3, 2, GL_FLOAT, GL_FALSE, 0, 0);
       
    } else {
        std::cout << "Invalid plane number" << std::endl;
        return;
    }

//============================================================ Reference for TBN calculation based on Lab7, CSE 333/533: Computer Graphics, Instructor - Ojaswa Sharma =====================================================================================
//     // Create VBO for tangents
//     GLuint tangent_VBO;
//     glGenBuffers(1, &tangent_VBO);
//     glBindBuffer(GL_ARRAY_BUFFER, tangent_VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_indices.size() * 3, vertex_tangents, GL_STATIC_DRAW);
//     // Get the attribute location for tangents in the shader program
//     int vTangent_attrib = glGetAttribLocation(program, "vTangent");
//     if (vTangent_attrib == -1) {
//         std::cout << "Could not bind location: vTangent\n";
//     } else {
//         // std::cout << "vTangent found at location " << vTangent_attrib << std::endl;
//     }
//     glEnableVertexAttribArray(vTangent_attrib);
//     glVertexAttribPointer(vTangent_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
//     // delete[] vertex_tangents;

//     // Create VBO for bitangents
//     GLuint bitangent_VBO;
//     glGenBuffers(1, &bitangent_VBO);
//     glBindBuffer(GL_ARRAY_BUFFER, bitangent_VBO);
//     glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * vertex_indices.size() * 3, vertex_bitangents, GL_STATIC_DRAW);
//     int vBitangent_attrib = glGetAttribLocation(program, "vBitangent");
//     if (vBitangent_attrib == -1) {
//         std::cout << "Could not bind location: vBitangent\n";
//     } else {
//         // std::cout << "vBitangent found at location " << vBitangent_attrib << std::endl;
//     }
//     // Enable the attribute array for tangents
//     glEnableVertexAttribArray(vBitangent_attrib);
//     // Specify the attribute array data for tangents
//     glVertexAttribPointer(vBitangent_attrib, 3, GL_FLOAT, GL_FALSE, 0, 0);
//     // delete[] vertex_bitangents;

//     // Set the uniform locations for the shaders
//     int textureLocation = glGetUniformLocation(program, "textureSampler");
//     int normalMapLocation = glGetUniformLocation(program, "normalMap");

//     // Set uniforms to correspond to the GL_TEXTURE0 and GL_TEXTURE1 units
//     glUniform1i(textureLocation, 0); // Texture unit 0 is for the diffuse texture
//     glUniform1i(normalMapLocation, 1); // Texture unit 1 is for the normal map

//     // Load the normal map
//     glGenTextures(1, &normalMap);
//     glActiveTexture(GL_TEXTURE1); // Activate texture unit 1
//     glBindTexture(GL_TEXTURE_2D, normalMap);

//     // Flip the texture on load to match OpenGL's bottom-left origin
//     stbi_set_flip_vertically_on_load(true);

//     // Load the image data for the normal map
//     unsigned char* data2 = stbi_load(normalMapPath.c_str(), &width, &height, &nrChannels, 0);
//     if (data2) {
//         glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
//         glGenerateMipmap(GL_TEXTURE_2D); // Generate mipmaps for the normal map
//     } else {
//         std::cout << "Failed to load normal map" << std::endl;
//     }
//     stbi_image_free(data2); // Free the image memory

//     // Set texture parameters for the normal map
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

//     // Delete the buffers after use
//     glDeleteBuffers(1, &tangent_VBO);
//     glDeleteBuffers(1, &bitangent_VBO);
}

//============================================================ Reference for TBN calculation based on Lab7, CSE 333/533: Computer Graphics, Instructor - Ojaswa Sharma =====================================================================================





