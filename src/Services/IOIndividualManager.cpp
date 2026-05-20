#include <Services/IOIndividualManager.h>

void IOIndividualManager::ReadIndividualsDetailsAndCreateBuildings()
{
	std::ifstream file{std::string{FILE_NAME_INDIVIDUAL}, std::ios::in};

	if (file.is_open())
	{
		int sizeOx, sizeOy, sizeOz;
		double elementSize;
		int cubeExistence;
		std::vector<uint8_t> cubesExistence;

		while (!file.eof())
		{
			file >> sizeOx >> sizeOy >> sizeOz >> elementSize;

			for (int index{0}; index < sizeOx * sizeOy * sizeOz; ++index)
			{
				file >> cubeExistence;
				cubesExistence.emplace_back(static_cast<uint8_t>(cubeExistence));
			}

			auto readBuilding = Individual::CreateBuildingFromDetails(sizeOx, sizeOy, sizeOz, elementSize, cubesExistence);
			Scene scene;
			scene.Show(readBuilding);

			cubesExistence.clear();
		}
		file.close();
	}
	else
	{
		std::cerr << "Could not open " << FILE_NAME_INDIVIDUAL << " file!";
	}
}

std::vector<uint8_t> IOIndividualManager::ReadInitialIndividual(int individualSize)
{
	std::ifstream file{std::string{FILE_NAME_INITIAL_INDIVIDUAL}, std::ios::in};

	int cubeExistence;
	std::vector<uint8_t> cubesExistence;

	if (file.is_open())
	{
		for (int index{0}; index < individualSize; ++index)
		{
			file >> cubeExistence;
			cubesExistence.emplace_back(static_cast<uint8_t>(cubeExistence));
		}

		file.close();
	}
	else
	{
		std::cerr << "Could not open " << FILE_NAME_INITIAL_INDIVIDUAL << " file!";
	}

	return cubesExistence;
}

void IOIndividualManager::WriteIndividualDetailsInFile(IIndividual* individual)
{
	std::ofstream file{std::string{FILE_NAME_INDIVIDUAL}};

	if (file.is_open())
	{
		auto* castedIndividual = dynamic_cast<Individual*>(individual);
		file << *castedIndividual;
		file.close();
	}
	else
	{
		std::cerr << "Could not open " << FILE_NAME_INDIVIDUAL << " file!";
	}
}

void IOIndividualManager::WriteIndividualValueInFile(int epoch, double fitnessValue, bool append)
{
	std::ofstream file{std::string{FILE_NAME_INDIVIDUAL_VALUES},
		append ? std::ios_base::app : std::ios_base::out};

	if (file.is_open())
	{
		if (!append)
		{
			file << "EPOCH" << "," << "VALUE" << '\n';
		}

		file << epoch << "," << fitnessValue << '\n';
		file.close();
	}
	else
	{
		std::cerr << "Could not open " << FILE_NAME_INDIVIDUAL_VALUES << " file!";
	}
}
