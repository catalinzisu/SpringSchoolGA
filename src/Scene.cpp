#include <Scene.h>

Scene::Scene()
{
}

void Scene::InitializeSystem(const std::shared_ptr<chrono::ChSystemSMC>& system)
{
	auto application = std::make_shared<chrono::irrlicht::ChIrrApp>(system.get(),
		L"Optimization result", irr::core::dimension2d<irr::u32>(800, 600));

	ConfigureSystem configureSystem(application, system);

	configureSystem.ConfigureIrrllichtScene();
	configureSystem.InitializeIrrlichtScene();
	configureSystem.SetIrrlichtSceneTimestep(0.001);

	configureSystem.SetSystemTimestepper();
	configureSystem.SetSystemSover();
	configureSystem.Simulate(0.1);

	configureSystem.RunIrllichtScene();
}

void Scene::Show(const std::shared_ptr<Building>& building)
{
	SetVisualizationProperties(building);
	InitializeSystem(building->GetSystem());
}

std::shared_ptr<Building> Scene::CreateSimpleWallScene()
{
	auto customBuilding = std::make_shared<Building>(1, 1, 1, 2);

	customBuilding->Build();
	customBuilding->AddConstraints();

	return customBuilding;
}

std::shared_ptr<Building> Scene::CreateComplexWallScene()
{
	auto customBuilding = std::make_shared<Building>(20, 8, 5, 0.05);
	customBuilding->Build();

	customBuilding->AddConstraints();

	std::vector<uint8_t> importanceForEliminatingCubes(20 * 8 * 5, 1);

	for (size_t i{0}; i < importanceForEliminatingCubes.size(); ++i)
		importanceForEliminatingCubes[i] = 0;

	customBuilding->EliminateCubesBasedOnCubesExistence(importanceForEliminatingCubes);

	for (size_t i{0}; i < importanceForEliminatingCubes.size(); ++i)
		importanceForEliminatingCubes[i] = 1;

	customBuilding->AddCubesBasedOnCubesExistence(importanceForEliminatingCubes);

	importanceForEliminatingCubes[0] = 0;
	importanceForEliminatingCubes[1] = 0;
	importanceForEliminatingCubes[19] = 0;
	importanceForEliminatingCubes[18] = 0;

	customBuilding->EliminateCubesBasedOnCubesExistence(importanceForEliminatingCubes);

	importanceForEliminatingCubes[1] = 1;
	importanceForEliminatingCubes[5] = 1;
	importanceForEliminatingCubes[19] = 1;

	customBuilding->AddCubesBasedOnCubesExistence(importanceForEliminatingCubes);

	return customBuilding;
}

std::shared_ptr<Building> Scene::CreateCustomWallScene()
{
	return CreateSimpleWallScene();
}

void Scene::SetVisualizationProperties(const std::shared_ptr<Building>& building)
{
	ObjectProperties::SetVisualizationMesh(building->GetMesh(),
		std::make_shared<chrono::fea::ChVisualizationFEAmesh>(*(building->GetMesh().get())));
	ObjectProperties::SetVisualizationMeshReference(building->GetMesh(),
		std::make_shared<chrono::fea::ChVisualizationFEAmesh>(*(building->GetMesh().get())));
	ObjectProperties::SetVisualizationMeshPoints(building->GetMesh(),
		std::make_shared<chrono::fea::ChVisualizationFEAmesh>(*(building->GetMesh().get())));
}
