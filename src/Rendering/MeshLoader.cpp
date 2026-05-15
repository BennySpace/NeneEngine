// MeshLoader.cpp

#include "Rendering/MeshLoader.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include <filesystem>
#include <stdexcept>

namespace NeneEngine
{

	MeshData LoadMeshDataFromFile(const std::string& path)
	{
		if (!std::filesystem::exists(path)) throw std::runtime_error("Mesh file does not exist: " + path);

		Assimp::Importer importer;
		const aiScene* scene = importer.ReadFile(
		    path, aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices |
		              aiProcess_GenSmoothNormals | aiProcess_ImproveCacheLocality | aiProcess_ValidateDataStructure |
		              aiProcess_FlipUVs);

		if (scene == nullptr || scene->mRootNode == nullptr)
			throw std::runtime_error("Assimp failed to load mesh '" + path + "': " + importer.GetErrorString());

		if (scene->mNumMeshes == 0) throw std::runtime_error("Assimp loaded scene without meshes: " + path);

		MeshData meshData{};

		for (unsigned meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex)
		{
			const aiMesh* mesh = scene->mMeshes[meshIndex];
			if (mesh == nullptr) continue;

			const uint32_t baseVertex = static_cast<uint32_t>(meshData.vertices.size());
			meshData.vertices.reserve(meshData.vertices.size() + mesh->mNumVertices);

			for (unsigned vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex)
			{
				Vertex vertex{};

				if (mesh->HasPositions())
				{
					const aiVector3D& position = mesh->mVertices[vertexIndex];
					vertex.position = {position.x, position.y, position.z};
				}

				if (mesh->HasNormals())
				{
					const aiVector3D& normal = mesh->mNormals[vertexIndex];
					vertex.normal = {normal.x, normal.y, normal.z};
				}

				if (mesh->HasTextureCoords(0))
				{
					const aiVector3D& uv = mesh->mTextureCoords[0][vertexIndex];
					vertex.uv = {uv.x, uv.y};
				}

				meshData.vertices.push_back(vertex);
			}

			for (unsigned faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex)
			{
				const aiFace& face = mesh->mFaces[faceIndex];
				for (unsigned index = 0; index < face.mNumIndices; ++index)
					meshData.indices.push_back(baseVertex + face.mIndices[index]);
			}
		}

		if (meshData.vertices.empty() || meshData.indices.empty())
			throw std::runtime_error("Mesh data is empty after loading: " + path);

		return meshData;
	}

	Mesh LoadMeshFromFile(const std::string& path)
	{
		Mesh mesh{};
		mesh.data = LoadMeshDataFromFile(path);
		return mesh;
	}

} // namespace NeneEngine
