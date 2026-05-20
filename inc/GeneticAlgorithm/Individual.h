#pragma once

#include <vector>
#include <cmath>
#include <iostream>
#include <cstdint>

#include <fea/ChMesh.h>

#include <GeneticAlgorithm/IIndividual.h>

#include <GraphicalObjects/Building.h>

#include <Services/constants.h>
#include <Services/AlgorithmSettings.h>
#include <Services/RandomNumbersGenerator.h>

#include <System/ConfigureSystem.h>

class Individual : public IIndividual
{
public:
	Individual(int sizeOx, int sizeOy, int sizeOz, double elementSize);
	Individual(int sizeOx, int sizeOy, int sizeOz, double elementSize, const std::vector<uint8_t>& cubesExistence);

	Individual(const Individual& another);
	Individual(Individual&& another) noexcept;

	Individual& operator=(const Individual& another);
	Individual& operator=(Individual&& another) noexcept;

	~Individual() = default;

	void SetMaximStress(double maximStress);

	[[nodiscard]] const std::shared_ptr<Building> GetBuilding() const;

	[[nodiscard]] double Evaluate() override;

	void Crossover(IIndividual& other) override;
	void Mutation(double mutationProbability) override;

	bool operator==(const Individual& other) const;

	friend std::ostream& operator<<(std::ostream& out, const Individual& individual);

	static std::shared_ptr<Building> CreateBuildingFromDetails(int sizeOx, int sizeOy, int sizeOz,
		double elementSize, const std::vector<uint8_t>& cubesExistence);

private:
	[[nodiscard]] int GetNumberOfRemovedElements();
	[[nodiscard]] double SimulateAndGetMaximStress();
	[[nodiscard]] bool IsOnTopLayer(size_t possition);

private:
	std::shared_ptr<Building> m_building{nullptr};

	double m_maximStress{0.0};
	int m_sizeOx{0};
	int m_sizeOy{0};
	int m_sizeOz{0};
	double m_elementSize{0.0};

	std::vector<uint8_t> m_initialGenes;

	bool m_isDirty{true};
	double m_cachedStress{0.0};
};