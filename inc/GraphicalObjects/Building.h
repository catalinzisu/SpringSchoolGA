#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <cmath>
#include <cstdint>

#include <fea/ChMesh.h>

#include <chrono/fea/ChNodeFEAxyz.h>
#include <chrono/fea/ChElementHexaCorot_8.h>
#include <chrono/fea/ChLinkPointFrame.h>

#include <chrono/physics/ChSystemSMC.h>
#include <chrono/physics/ChBody.h>

#include <chrono/assets/ChBoxShape.h>

#include <GraphicalObjects/ObjectProperties.h>
#include <GraphicalObjects/BodyReference.h>

class Building
{
public:
	Building(uint16_t numberOfCubesOx, uint16_t numberOfCubesOy, uint16_t numberOfCubesOz, double cubeSize);
	
	Building(const Building& another);
	Building(Building&& another) noexcept;

	Building& operator=(const Building& another);
	Building& operator=(Building&& another) noexcept;
	
	~Building() = default;

	[[nodiscard]] const std::shared_ptr<chrono::fea::ChMesh> GetMesh() const;
	[[nodiscard]] const std::vector<uint8_t>& GetCubesExistence() const;
	[[nodiscard]] const std::shared_ptr<chrono::ChSystemSMC> GetSystem() const;

	void Build();

	void AddConstraints();

	void EliminateCubesBasedOnCubesExistence(const std::vector<uint8_t>& importance);
	void AddCubesBasedOnCubesExistence(const std::vector<uint8_t>& importance);

private:
	std::vector<std::shared_ptr<chrono::fea::ChNodeFEAxyz>> BuildLateralNodesForLayers(
		uint16_t layerOx, uint16_t layerOy, uint16_t layerOz);
	void BuildCube(const std::vector<std::shared_ptr<chrono::fea::ChNodeFEAxyz>>& nodesLeftSide,
		const std::vector<std::shared_ptr<chrono::fea::ChNodeFEAxyz>>& nodesRightSide);

	void AddNodesInMesh(const std::vector<std::shared_ptr<chrono::fea::ChNodeFEAxyz>>& nodes);
	void AddConstraintBetweenNodeAndBase(const std::shared_ptr<chrono::fea::ChNodeFEAxyz>& node,
		const std::shared_ptr<chrono::ChBody>& base);
	void EliminateConstaints();

	int GetElementPositionFromImportanceVector(int position);

	std::string GetNodeKey(double x, double y, double z) const;

	bool HasAddedElementNeighbors(int position, const std::vector<uint8_t>& importance);

private:
	std::shared_ptr<chrono::fea::ChMesh> m_mesh;

	uint16_t m_numberOfCubesOx;
	uint16_t m_numberOfCubesOy;
	uint16_t m_numberOfCubesOz;

	double m_cubeSize{0.0};
	std::vector<uint8_t> m_cubesExistence;
	std::unordered_map<std::string, std::shared_ptr<chrono::fea::ChNodeFEAxyz>> m_nodeMap;

	BodyReference m_base;

	std::shared_ptr<chrono::ChSystemSMC> m_system;
};
