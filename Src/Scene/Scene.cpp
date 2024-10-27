#include "Scene.h"
#include "spdlog/spdlog.h"
#include "../Exception/ParseException.h"
#include "../Utility/SmoothingNormals.h"

Scene::Scene(float shadowIntensity) : m_name{}, m_cameraCoordinates{}, m_shadowIntensity{ shadowIntensity } {}

Scene::Scene(const std::string& inputFile, float shadowIntensity) : m_name{ inputFile }, m_cameraCoordinates{}, m_shadowIntensity{ shadowIntensity } {
    LoadObj(inputFile);
}

// TODO: add support for textures
// TODO: default material
void Scene::LoadObj(const std::string& inputFile) {
    try {
        tinyobj::ObjReader reader{ CreateReader(inputFile) };
        auto& attrib{ reader.GetAttrib() };
        auto& shapes{ reader.GetShapes() };
        auto& materials{ reader.GetMaterials() };

        tinyobj::attrib_t outattrib;
        std::vector<tinyobj::shape_t> outshapes;
        computeSmoothingShapes(attrib, shapes, outshapes, outattrib);
        computeAllSmoothingNormals(outattrib, outshapes);
        auto& att = outattrib;
        auto& sha = outshapes;

        for (size_t s = 0; s < sha.size(); ++s) {
            if (sha[s].name.find("point_light") != std::string::npos) {
                spdlog::trace("Scene: found {}", sha[s].name);
                m_pointLights.push_back(ReadLight(sha[s], att, materials[sha[s].mesh.material_ids[0]]));
                continue;
            }
            if (sha[s].name.compare("camera") == 0) {
                spdlog::trace("Scene: found {}", sha[s].name);
                m_cameraCoordinates = ReadCamera(sha[s], att);
                continue;
            }
            spdlog::trace("Scene: found shape {}", sha[s].name);
            m_shapes.push_back(ReadShape(sha[s], att, materials[sha[s].mesh.material_ids[0]]));
        }
    } catch (ParseException e) {
        spdlog::error("TinyObjReader: {}", e.what());
    }
}

tinyobj::ObjReader Scene::CreateReader(const std::string& inputFile) const {
    tinyobj::ObjReaderConfig readerConfig;
    readerConfig.mtl_search_path = "";
    tinyobj::ObjReader reader;
    if (!reader.ParseFromFile(inputFile, readerConfig)) {
        if (!reader.Error().empty()) {
            throw ParseException{reader.Error().c_str()};
        }
    }
    return reader;
}

Shape Scene::ReadShape(const tinyobj::shape_t& shape, const tinyobj::attrib_t& att, const tinyobj::material_t& mat) const {
    Shape s{};
    s.name = shape.name;
    size_t indexOffset{ 0 };
    for (size_t f = 0; f < shape.mesh.num_face_vertices.size(); ++f) {
        size_t fv{ size_t(shape.mesh.num_face_vertices[f]) };
        for (size_t v = 0; v < fv; ++v) {
            tinyobj::index_t idx = shape.mesh.indices[indexOffset + v];
            glm::vec3 vertex{
                att.vertices[3 * size_t(idx.vertex_index) + 0],
                att.vertices[3 * size_t(idx.vertex_index) + 1],
                att.vertices[3 * size_t(idx.vertex_index) + 2]
            };
            glm::vec3 normal{
                att.normals[3 * size_t(idx.normal_index) + 0],
                att.normals[3 * size_t(idx.normal_index) + 1],
                att.normals[3 * size_t(idx.normal_index) + 2]
            };
            int vertexID{ s.GetVertexID(vertex, normal) };
            if (vertexID == -1) {
                s.AddVertex(vertex, normal);
            } else {
                s.indices.push_back(vertexID);
            }
        }
        indexOffset += fv;
    }
    // Wrong -> different faces have different materials
    s.material.createFrom(mat);
    return s;
}

Light Scene::ReadLight(const tinyobj::shape_t& shape, const tinyobj::attrib_t& att, const tinyobj::material_t& mat) const {
    Light light{};
    tinyobj::index_t idx = shape.mesh.indices[0];
    light.pos = glm::vec3{
        att.vertices[3 * size_t(idx.vertex_index) + 0],
        att.vertices[3 * size_t(idx.vertex_index) + 1],
        att.vertices[3 * size_t(idx.vertex_index) + 2]
    };
    light.material.createFrom(mat);
    return light;
}

const glm::vec3 Scene::ReadCamera(const tinyobj::shape_t& shape, const tinyobj::attrib_t& att) const {
    tinyobj::index_t idx = shape.mesh.indices[0];
    return glm::vec3{
        att.vertices[3 * size_t(idx.vertex_index) + 0],
        att.vertices[3 * size_t(idx.vertex_index) + 1],
        att.vertices[3 * size_t(idx.vertex_index) + 2]
    };
}

RTCScene Scene::Build(RTCDevice device) const {
	if (m_name.empty()) {
		spdlog::error("Scene: no scene loaded");
		return RTCScene{};
	}
    RTCScene scene{ rtcNewScene(device) };
    for (auto& shape : m_shapes) {
        RTCGeometry geom{ rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE) };
        rtcSetGeometryVertexAttributeCount(geom, 1);

        float* vertices{
            static_cast<float*>(rtcSetNewGeometryBuffer(
                geom,
                RTC_BUFFER_TYPE_VERTEX,
                0,
                RTC_FORMAT_FLOAT3,
                3 * sizeof(float),
                shape.vertices.size() / 3
            )) 
        };
        for (int i = 0; i < shape.vertices.size(); ++i) {
            vertices[i] = shape.vertices[i];
        }

        float* normals{
            static_cast<float*>(rtcSetNewGeometryBuffer(
                geom,
                RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                0,
                RTC_FORMAT_FLOAT3,
                3 * sizeof(float),
                shape.normals.size() / 3
            ))
        };
        for (int i = 0; i < shape.normals.size(); ++i) {
            normals[i] = shape.normals[i];
        }

        unsigned* indices{
            static_cast<unsigned*>(rtcSetNewGeometryBuffer(
                geom,
                RTC_BUFFER_TYPE_INDEX,
                0,
                RTC_FORMAT_UINT3,
                3 * sizeof(unsigned),
                shape.GetNumFaces()
            ))
        };
        for (int i = 0; i < shape.indices.size(); ++i) {
            indices[i] = shape.indices[i];
        }
        
        rtcCommitGeometry(geom);
        rtcAttachGeometry(scene, geom); // geomId is sequential
        rtcReleaseGeometry(geom);
    }

    rtcCommitScene(scene);
	return scene;
}

size_t Scene::GetNumShapes() const {
    return m_shapes.size();
}

const Shape& Scene::GetShape(int id) const {
    return m_shapes[id];
}

size_t Scene::GetNumPointLights() const {
    return m_pointLights.size();
}

const Light& Scene::GetPointLight(int id) const {
    return m_pointLights[id];
}

glm::vec3 Scene::GetCameraCoordinates() const {
    return m_cameraCoordinates;
}

float Scene::GetShadowIntensity() const {
    return m_shadowIntensity;
}
