#include <GeneticAlgorithm/GeneticAlgorithm.h>
#include <fstream>
#include <chrono>

GeneticAlgorithm::GeneticAlgorithm(
	std::function<IIndividual* ()> createIndividual,
	size_t populationSize, size_t numberOfEpochs,
	double crossoverProbabillity, double mutationProbability) :
	m_createIndividual{ createIndividual },
	m_populationSize{ populationSize },
	m_numberOfEpochs{ numberOfEpochs },
	m_crossoverProbability{ crossoverProbabillity },
	m_mutationProbability{ mutationProbability }
{
}

void GeneticAlgorithm::Run()
{
	InitializePopulation();

	for (int index = 0; index < m_numberOfEpochs; ++index)
	{
		std::cout << '\n' << "Epoch: " << index + 1 << '\n';

		auto startFitness = std::chrono::high_resolution_clock::now();
		m_fitnessValues = CalculateFitnessValues();
		auto endFitness = std::chrono::high_resolution_clock::now();
		auto fitnessTime = std::chrono::duration_cast<std::chrono::milliseconds>(endFitness - startFitness).count();

		auto startSelection = std::chrono::high_resolution_clock::now();
		Selection();
		auto endSelection = std::chrono::high_resolution_clock::now();
		auto selectionTime = std::chrono::duration_cast<std::chrono::milliseconds>(endSelection - startSelection).count();

		auto startCrossover = std::chrono::high_resolution_clock::now();
		Crossover();
		auto endCrossover = std::chrono::high_resolution_clock::now();
		auto crossoverTime = std::chrono::duration_cast<std::chrono::milliseconds>(endCrossover - startCrossover).count();

		auto startMutation = std::chrono::high_resolution_clock::now();
		Mutation();
		auto endMutation = std::chrono::high_resolution_clock::now();
		auto mutationTime = std::chrono::duration_cast<std::chrono::milliseconds>(endMutation - startMutation).count();

		WriteWinners(index);

		std::ofstream profileFile("profiling_stats.csv", index == 0 ? std::ios::out : std::ios::app);
		if (index == 0)
		{
			profileFile << "Epoch,FitnessTime_ms,SelectionTime_ms,CrossoverTime_ms,MutationTime_ms\n";
		}
		profileFile << index + 1 << "," << fitnessTime << "," << selectionTime << "," << crossoverTime << "," << mutationTime << "\n";
	}
}

IIndividual* GeneticAlgorithm::GetWinnerIndividual()
{
	double maxValue{0.0};
	IIndividual* winner{nullptr};

	for (const auto& [individual, fitness] : m_fitnessValues)
		if (fitness > maxValue)
		{
			maxValue = fitness;
			winner = individual;
		}

	return winner;
}

void GeneticAlgorithm::InitializePopulation()
{
	for (size_t index{0}; index < m_populationSize; ++index)
	{
		std::cout << "Created individual " << index + 1 << '\n';

		m_population.emplace_back(std::shared_ptr<IIndividual>(m_createIndividual()));
		m_workingPopulation.push_back(m_population[index]);
	}
}

std::map<IIndividual*, double> GeneticAlgorithm::CalculateFitnessValues()
{
	std::map<IIndividual*, double> fitnessValues;
	for (const auto& individual : m_workingPopulation)
	{
		double value = individual->Evaluate();
		fitnessValues[individual.get()] = value;
	}

	return fitnessValues;
}

double GeneticAlgorithm::CalculateSumOfFitnessValues()
{
	double sum{0.0};
	for (const auto& [individual, fitness] : m_fitnessValues)
	{
		sum += fitness;
	}

	return sum;
}

std::vector<double> GeneticAlgorithm::CalculateProbabilityOfSelection()
{
	std::vector<double> probabilityOfSelectionVector;
	double sum = CalculateSumOfFitnessValues();

	for (const auto& individual : m_workingPopulation)
	{
		probabilityOfSelectionVector.emplace_back(m_fitnessValues[individual.get()] / sum);
	}

	return probabilityOfSelectionVector;
}

std::vector<double> GeneticAlgorithm::CalcutateCumulativeProbabilityOfSelection()
{
	std::vector<double> cumulativeProbabilityOfSelectionVector;
	auto probabilityOfSelectionVector = CalculateProbabilityOfSelection();

	for (size_t currentIndividualIndex{0}; currentIndividualIndex < m_populationSize; ++currentIndividualIndex)
	{
		double probability{0.0};

		for (size_t index{0}; index <= currentIndividualIndex; ++index)
		{
			probability += probabilityOfSelectionVector[index];
		}

		cumulativeProbabilityOfSelectionVector.emplace_back(probability);
	}

	return cumulativeProbabilityOfSelectionVector;
}

void GeneticAlgorithm::Selection()
{
	std::vector<std::shared_ptr<IIndividual>> newPopulation;

	auto bestIndividual = m_workingPopulation[0];
	auto maxFitness = m_fitnessValues[bestIndividual.get()];

	for (const auto& individual : m_workingPopulation)
	{
		if (m_fitnessValues[individual.get()] > maxFitness)
		{
			maxFitness = m_fitnessValues[individual.get()];
			bestIndividual = individual;
		}
	}

	newPopulation.push_back(bestIndividual);

	constexpr int kTournamentSize{3};

	for (size_t i{1}; i < m_populationSize; ++i)
	{
		std::shared_ptr<IIndividual> tournamentWinner{nullptr};
		double tournamentMaxFitness{-1.0};

		for (int k{0}; k < kTournamentSize; ++k)
		{
			int randomIndex = RandomNumbersGenerator::GenerateIntegerNumberInRange(0, static_cast<int>(m_populationSize) - 1);
			auto& candidate = m_workingPopulation[randomIndex];
			double candidateFitness = m_fitnessValues[candidate.get()];

			if (tournamentWinner == nullptr || candidateFitness > tournamentMaxFitness)
			{
				tournamentMaxFitness = candidateFitness;
				tournamentWinner = candidate;
			}
		}

		newPopulation.push_back(tournamentWinner);
	}

	m_workingPopulation = std::move(newPopulation);
}

void GeneticAlgorithm::Crossover()
{
	std::vector<std::shared_ptr<IIndividual>> selectedPopulationForCrossover;
	std::vector<std::shared_ptr<IIndividual>> newPopulation;

	std::vector<double> randomNumbers = RandomNumbersGenerator::GenerateRealNumbers(LOWER_BOUND, UPPER_BOUND, m_populationSize);

	for (size_t index = 0; index < m_populationSize; ++index)
	{
		if (randomNumbers[index] < m_crossoverProbability)
		{
			selectedPopulationForCrossover.push_back(m_workingPopulation[index]);
		}
	}

	if (selectedPopulationForCrossover.size() % 2 != 0)
	{
		selectedPopulationForCrossover.pop_back();
	}

	for (size_t index = 0; index < selectedPopulationForCrossover.size(); index += 2)
	{
		selectedPopulationForCrossover[index]->Crossover(*selectedPopulationForCrossover[index + 1]);
	}
}

void GeneticAlgorithm::Mutation()
{
	for (auto& individual : m_workingPopulation)
	{
		individual->Mutation(m_mutationProbability);
	}
}

bool GeneticAlgorithm::IsGraterThan(double value, double lowerBound) const
{
	return value > lowerBound;
}

bool GeneticAlgorithm::IsLessThanOrEqualTo(double value, double upperBound) const
{
	return value <= upperBound;
}

void GeneticAlgorithm::WriteWinners(int epoch)
{
	IIndividual* winner = GetWinnerIndividual();

	if (epoch == 0)
	{
		IOIndividualManager::WriteIndividualValueInFile(epoch + 1, m_fitnessValues[winner], false);
	}
	else
	{
		if (epoch == m_numberOfEpochs - 1)
		{
			IOIndividualManager::WriteIndividualDetailsInFile(winner);
		}
		IOIndividualManager::WriteIndividualValueInFile(epoch + 1, m_fitnessValues[winner], true);
	}

	double bestFitness = m_fitnessValues[winner];
	double sumFitness{0.0};
	double worstFitness = m_fitnessValues.begin()->second;

	for (const auto& [individual, fitness] : m_fitnessValues)
	{
		sumFitness += fitness;
		if (fitness < worstFitness)
		{
			worstFitness = fitness;
		}
	}

	double averageFitness = sumFitness / m_fitnessValues.size();

	std::ofstream statsFile("fitness_stats_nou.csv", epoch == 0 ? std::ios::out : std::ios::app);
	if (epoch == 0)
	{
		statsFile << "Epoch,Best Fitness,Average Fitness,Worst Fitness\n";
	}
	statsFile << epoch + 1 << "," << bestFitness << "," << averageFitness << "," << worstFitness << "\n";
}
