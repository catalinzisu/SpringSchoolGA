#pragma once

#include <functional>
#include <iostream>
#include <map>

#include <GeneticAlgorithm/IIndividual.h>

#include <Services/RandomNumbersGenerator.h>
#include <Services/IOIndividualManager.h>
#include <Services/constants.h>

class GeneticAlgorithm
{
public:
	GeneticAlgorithm(
		std::function<IIndividual* ()> createIndividual,
		size_t populationSize,
		size_t numberOfEpochs,
		double crossoverProbabillity,
		double mutationProbability);

	GeneticAlgorithm(const GeneticAlgorithm& other) = delete;
	GeneticAlgorithm(GeneticAlgorithm&& other) = delete;

	GeneticAlgorithm& operator=(const GeneticAlgorithm& other) = delete;
	GeneticAlgorithm& operator=(GeneticAlgorithm&& other) = delete;

	~GeneticAlgorithm() = default;

	void Run();

	[[nodiscard]] IIndividual* GetWinnerIndividual();

private:
	void InitializePopulation();

	[[nodiscard]] std::map<IIndividual*, double> CalculateFitnessValues();

	[[nodiscard]] double CalculateSumOfFitnessValues();
	[[nodiscard]] std::vector<double> CalculateProbabilityOfSelection();
	[[nodiscard]] std::vector<double> CalcutateCumulativeProbabilityOfSelection();

	void Selection();
	void Crossover();
	void Mutation();

	[[nodiscard]] bool IsGraterThan(double value, double lowerBound) const;
	[[nodiscard]] bool IsLessThanOrEqualTo(double value, double upperBound) const;

	void WriteWinners(int epoch);

private:
	std::vector<std::shared_ptr<IIndividual>> m_population;
	std::vector<std::shared_ptr<IIndividual>> m_workingPopulation;

	std::function<IIndividual* ()> m_createIndividual;

	std::map<IIndividual*, double> m_fitnessValues;

	size_t m_populationSize;
	size_t m_numberOfEpochs;

	double m_crossoverProbability{0.0};
	double m_mutationProbability{0.0};
};