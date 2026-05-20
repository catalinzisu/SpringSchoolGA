#include <GeneticAlgorithm/Individual.h>

Individual::Individual(int sizeOx, int sizeOy, int sizeOz, double elementSize) :
	m_sizeOx{ sizeOx }, m_sizeOy{ sizeOy }, m_sizeOz{ sizeOz }, m_elementSize{ elementSize }
{
	m_building = std::make_shared<Building>(m_sizeOx, m_sizeOy, m_sizeOz, m_elementSize);
	m_building->Build();
	m_building->AddConstraints();

	m_initialGenes = std::vector<uint8_t>(m_building->GetCubesExistence().size(), 1);
}

Individual::Individual(int sizeOx, int sizeOy, int sizeOz, double elementSize, const std::vector<uint8_t>& cubesExistence) :
	m_sizeOx{ sizeOx }, m_sizeOy{ sizeOy }, m_sizeOz{ sizeOz }, m_elementSize{ elementSize }
{
	m_building = std::make_shared<Building>(m_sizeOx, m_sizeOy, m_sizeOz, m_elementSize);
	m_building->Build();
	m_building->AddConstraints();
	m_building->EliminateCubesBasedOnCubesExistence(cubesExistence);

	m_initialGenes.reserve(cubesExistence.size());
	for (size_t index{0}; index < cubesExistence.size(); ++index)
		m_initialGenes.emplace_back(cubesExistence[index]);
}

Individual::Individual(const Individual& another)
{
	*this = another;
}

Individual::Individual(Individual&& another) noexcept
{
	*this = std::move(another);
}

Individual& Individual::operator=(const Individual& another)
{
	if (this != &another)
	{
		m_sizeOx = another.m_sizeOx;
		m_sizeOy = another.m_sizeOy;
		m_sizeOz = another.m_sizeOz;
		m_elementSize = another.m_elementSize;
		m_maximStress = another.m_maximStress;
		m_initialGenes = another.m_initialGenes;
		m_building = Individual::CreateBuildingFromDetails(another.m_sizeOx, another.m_sizeOy, another.m_sizeOz,
			another.m_elementSize, another.m_building->GetCubesExistence());
	}
	return *this;
}

Individual& Individual::operator=(Individual&& another) noexcept
{
	if (this != &another)
	{
		int resetValue = 0;
		m_sizeOx = std::exchange(another.m_sizeOx, resetValue);
		m_sizeOy = std::exchange(another.m_sizeOy, resetValue);
		m_sizeOz = std::exchange(another.m_sizeOz, resetValue);
		m_elementSize = std::exchange(another.m_elementSize, resetValue);
		m_maximStress = std::exchange(another.m_maximStress, resetValue);
		m_initialGenes = std::exchange(another.m_initialGenes, std::vector<uint8_t>());
		m_building = std::exchange(another.m_building, nullptr);
	}
	return *this;
}

void Individual::SetMaximStress(double maximStress)
{
	m_maximStress = maximStress;
}

const std::shared_ptr<Building> Individual::GetBuilding() const
{
	return m_building;
}

double Individual::Evaluate()
{
	double maximStress = SimulateAndGetMaximStress();
	double value = MINIM_INDIVIDUAL_VALUE;

	if (maximStress >= m_maximStress || maximStress < EPSILON_STRESS)
	{
		return value;
	}

	double R = GetNumberOfRemovedElements();
	double totalCubes = m_sizeOx * m_sizeOy * m_sizeOz;
	
	double removalRatio = std::pow(R / totalCubes, 1.5);
	double stressRatio = (m_maximStress - maximStress) / m_maximStress;

	if (removalRatio == 0.0)
	{
		return value;
	}

	value = removalRatio * stressRatio;

	return value;
}

void Individual::Crossover(IIndividual& other)
{
	size_t numberOfGenes = m_building->GetCubesExistence().size();

	int p1 = RandomNumbersGenerator::GenerateIntegerNumberInRange(0, static_cast<int>(numberOfGenes) - 2);
	int p2 = RandomNumbersGenerator::GenerateIntegerNumberInRange(p1 + 1, static_cast<int>(numberOfGenes) - 1);

	Individual& otherIndividual = dynamic_cast<Individual&> (other);

	const auto& myGenes = m_building->GetCubesExistence();
	const auto& otherGenes = otherIndividual.m_building->GetCubesExistence();

	std::vector<uint8_t> newCubesExistence = myGenes;
	std::vector<uint8_t> newOtherCubesExistence = otherGenes;

	for (size_t index = p1; index < p2; ++index)
	{
		newCubesExistence[index] = otherGenes[index];
		newOtherCubesExistence[index] = myGenes[index];
	}

	m_building->EliminateCubesBasedOnCubesExistence(newCubesExistence);
	m_building->AddCubesBasedOnCubesExistence(newCubesExistence);

	otherIndividual.m_building->EliminateCubesBasedOnCubesExistence(newOtherCubesExistence);
	otherIndividual.m_building->AddCubesBasedOnCubesExistence(newOtherCubesExistence);

	m_isDirty = true;
	otherIndividual.m_isDirty = true;
}

void Individual::Mutation(double mutationProbability)
{
	size_t numberOfGenes = m_building->GetCubesExistence().size();

	std::vector<double> randomNumbers = RandomNumbersGenerator::GenerateRealNumbers(
		LOWER_BOUND, UPPER_BOUND, numberOfGenes);

	std::vector<uint8_t> newCubesExistence = m_building->GetCubesExistence();

	for (size_t index = 0; index < numberOfGenes; ++index)
	{
		if (randomNumbers[index] < mutationProbability)
		{
			if (!IsOnTopLayer(index) && m_initialGenes[index])
			{
				newCubesExistence[index] = m_building->GetCubesExistence()[index] ? 0 : 1;
			}
		}
	}

	m_building->EliminateCubesBasedOnCubesExistence(newCubesExistence);
	m_building->AddCubesBasedOnCubesExistence(newCubesExistence);

	m_isDirty = true;
}

bool Individual::operator==(const Individual& other) const
{
	const auto existence = m_building->GetCubesExistence();
	const auto otherExistence = other.m_building->GetCubesExistence();
	for (size_t index{0}; index < existence.size(); ++index)
		if (existence[index] != otherExistence[index])
			return false;

	return
		m_sizeOx == other.m_sizeOx &&
		m_sizeOy == other.m_sizeOy &&
		m_sizeOz == other.m_sizeOz &&
		m_elementSize == other.m_elementSize &&
		m_maximStress == other.m_maximStress;
}

std::shared_ptr<Building> Individual::CreateBuildingFromDetails(int sizeOx, int sizeOy, int sizeOz,
	double elementSize, const std::vector<uint8_t>& cubesExistence)
{
	auto building = std::make_shared<Building>(sizeOx, sizeOy, sizeOz, elementSize);
	building->Build();
	building->AddConstraints();

	building->EliminateCubesBasedOnCubesExistence(cubesExistence);

	return building;
}

int Individual::GetNumberOfRemovedElements()
{
	int numberOfRemovedElements{0};
	const auto& cubesExistence = m_building->GetCubesExistence();

	for (const auto cubeExistence : cubesExistence)
		if (!cubeExistence)
			++numberOfRemovedElements;

	return numberOfRemovedElements;
}

double Individual::SimulateAndGetMaximStress()
{
	if (!m_isDirty) return m_cachedStress;

	auto clone = CreateBuildingFromDetails(m_sizeOx, m_sizeOy, m_sizeOz, m_elementSize, m_building->GetCubesExistence());
	
	ConfigureSystem configureSystem(clone->GetSystem());
	configureSystem.SetSystemTimestepper();
	configureSystem.SetSystemSover();
	configureSystem.Simulate(0.1);

	double maximStress{0.0};
	const auto& elements = clone->GetMesh()->GetElements();

	for (const auto& element : elements)
	{
		auto castedElement = std::dynamic_pointer_cast<chrono::fea::ChElementHexaCorot_8>(element);
		auto stress = castedElement->GetStress(0.5, 0.5, 0.5);

		double stressOnOx, stressOnOy, stressOnOz;

		stress.ComputePrincipalStresses(stressOnOx, stressOnOy, stressOnOz);

		if (std::abs(stressOnOx) > maximStress)
			maximStress = std::abs(stressOnOx);

		if (std::abs(stressOnOy) > maximStress)
			maximStress = std::abs(stressOnOy);

		if (std::abs(stressOnOz) > maximStress)
			maximStress = std::abs(stressOnOz);
	}

	m_cachedStress = maximStress;
	m_isDirty = false;

	return maximStress;
}

bool Individual::IsOnTopLayer(size_t possition)
{
	uint16_t currentOyLayer = possition / (m_building->GetCubesExistence().size() / m_sizeOy);
	double currentOyCoord = currentOyLayer * m_elementSize - m_elementSize;
	double maximOyCoord = m_sizeOy * m_elementSize - 2 * m_elementSize;

	if (std::abs(currentOyCoord - maximOyCoord) > EPSILON)
		return false;

	return true;
}

std::ostream& operator<<(std::ostream& out, const Individual& individual)
{
	out << '\n';
	out << individual.m_sizeOx << '\n';
	out << individual.m_sizeOy << '\n';
	out << individual.m_sizeOz << '\n';
	out << individual.m_elementSize << '\n';

	const auto& existence = individual.m_building->GetCubesExistence();
	const size_t size = existence.size();
	for (size_t index{0}; index < size; ++index)
		if (index != size - 1)
			out << static_cast<int>(existence[index]) << " ";
	out << static_cast<int>(existence[size - 1]);

	return out;
}
