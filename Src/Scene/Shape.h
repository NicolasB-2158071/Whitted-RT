#pragma once

#include <vector>
#include <string>
#include <glm/glm.hpp>
#include "Material.h"

struct Shape {
	std::string name;
	std::vector<float> vertices; // Unpacked, duplicate free
	std::vector<float> normals;
	std::vector<unsigned> indices;
	Material material;
	// textures

	size_t GetNumVertices() const {
		return indices.size();
	}

	size_t GetNumFaces(int vertexFace = 3) const {
		return GetNumVertices() / vertexFace;
	}

	bool IsVertexEqual(int i, const glm::vec3& vertex) const {
		return vertices[i] == vertex.x && vertices[i + 1] == vertex.y && vertices[i + 2] == vertex.z;
	}

	bool IsNormalEqual(int i, const glm::vec3& normal) const {
		return normals[i] == normal.x && normals[i + 1] == normal.y && normals[i + 2] == normal.z;
	}

	int GetVertexID(const glm::vec3& vertex, const glm::vec3& normal) const {
		for (int i = 0; i < vertices.size(); i += 3) {
			if (IsVertexEqual(i, vertex) && IsNormalEqual(i, normal)) {
				return i / 3;
			}
		}
		return -1;
	}

	void AddVertex(const glm::vec3& vertex, const glm::vec3& normal) {
		vertices.push_back(vertex.x);
		vertices.push_back(vertex.y);
		vertices.push_back(vertex.z);
		normals.push_back(normal.x);
		normals.push_back(normal.y);
		normals.push_back(normal.z);
		indices.push_back((vertices.size() / 3) - 1);
	}

	//glm::vec3 CalculateNormal(int faceID) const {
	//	// Outward pointing normal defined by ordering of vertices
	//	// https://github.com/scratchapixel/scratchapixel-code/blob/main/ray-tracing-overview/whitted.cpp#L261
	//	int offset{ faceID * 3 };
	//	glm::vec3 v0{ vertices[indices[offset] * 3], vertices[indices[offset] * 3 + 1], vertices[indices[offset++] * 3 + 2] };
	//	glm::vec3 v1{ vertices[indices[offset] * 3], vertices[indices[offset] * 3 + 1], vertices[indices[offset++] * 3 + 2] };
	//	glm::vec3 v2{ vertices[indices[offset] * 3], vertices[indices[offset] * 3 + 1], vertices[indices[offset++] * 3 + 2] };

	//	glm::vec3 e0{ glm::normalize(v1 - v0) };
	//	glm::vec3 e1{ glm::normalize(v2 - v1) };

	//	return glm::normalize(glm::cross(e0, e1));
	//}
};