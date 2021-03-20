#include "Game.h"
#include "RuntimeObjectSystem/IRuntimeObjectSystem.h"
#include "RuntimeCompiler/IFileChangeNotifier.h"
#include "IUpdateable.h"
#include "IObjectUtils.h"

bool Game::Init()
{
	m_RCCpp = new RCCpp(this);
	m_EntityUpdateProtector.pEntitySystem = m_RCCpp->systemTable->pEntitySystem;
	m_RCCpp->Init();
	return true;
}

//All the Updateable object will call Update here
void Game::EntityUpdateProtector::ProtectedFunc()
{
	for (size_t i = 0; i < entities.Size(); ++i)
	{
		IAUEntity* pEnt = pEntitySystem->Get(entities[i]);
		if (pEnt) // Safety check in case entity was deleted during this update by another object
		{
			IAUUpdateable* pUpdateable = pEnt->GetUpdateable();
			if (pUpdateable)
			{
				// If dropped here after a runtime failure, your crash was likely
				// somewhere directly in the pUpdatable object's Update method
				pUpdateable->Update(fDeltaTime);
			}
		}
	}
}

void Game::Update(float dt)
{
	bool bLoadModule = false;
	if (m_RCCpp->systemTable->runtimeObjectSystem->GetIsCompiledComplete())
	{
		bLoadModule = true; //we load module after update/display, to get notification on screen correct
	}
	m_EntityUpdateProtector.pEntitySystem->GetAll(m_EntityUpdateProtector.entities);
	//m_EntityUpdateProtector.fDeltaTime = fClampedDelta;

	if (!m_RCCpp->systemTable->runtimeObjectSystem->TryProtectedFunction(&m_EntityUpdateProtector))
	{
		CORE_CRITICAL("Have caught an exception in main entity Update loop, code will not be run until new compile - please fix.");
		//m_pEnv->sys->pLogSystem->Log(eLV_ERRORS, "Have caught an exception in main entity Update loop, code will not be run until new compile - please fix.\n");
	}

	if (bLoadModule)
	{
		bool bSuccess = m_RCCpp->systemTable->runtimeObjectSystem->LoadCompiledModule();
	
		// load module when compile complete, and notify console - TODO replace with event system 
		//m_pConsole->OnCompileDone(bSuccess);
		//if (bSuccess)
		//{
		//	float compileAndLoadTime = (float)(pTimeSystem->GetSessionTimeNow() - m_CompileStartedTime);
		//	m_pEnv->sys->pLogSystem->Log(eLV_COMMENTS, "Compile and Module Reload Time: %.1f s\n", compileAndLoadTime);

		//}
		//m_CompileStartedTime = 0.0;
	}
	if (!m_RCCpp->systemTable->runtimeObjectSystem->GetIsCompiling())
	{
		m_RCCpp->systemTable->runtimeObjectSystem->GetFileChangeNotifier()->Update(dt);
	}
	//else
	//{
	//    ResetPowerSaveCountDown();
	//}
}

void Game::Shutdown()
{
	m_RCCpp->Shutdown();
	if(m_RCCpp->systemTable->runtimeObjectSystem)
	{
		m_RCCpp->systemTable->runtimeObjectSystem->CleanObjectFiles();
	}
	delete IObjectUtils::GetUniqueObject("MainObject");
}

Game::~Game()
{
	delete m_RCCpp;
}