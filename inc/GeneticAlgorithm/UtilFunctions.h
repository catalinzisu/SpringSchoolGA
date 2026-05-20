#pragma once

#include <GeneticAlgorithm/IIndividual.h>
#include <GeneticAlgorithm/Individual.h>

#include <Services/IOIndividualManager.h>

class UtilFunctions
{
public:
	static std::function<IIndividual* ()> CreateIndividualFunction(int sizeOx, int sizeOy, int sizeOz, 
		double elementSize, double maximStress);

	static std::function<IIndividual* ()> CreateIndividualFromFileFunction(int sizeOx, int sizeOy, int sizeOz,
		double elementSize, const std::vector<uint8_t>& cubesExistence, double maximStress);
};