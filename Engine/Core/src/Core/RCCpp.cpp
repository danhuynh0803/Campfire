//#include "Core/RCCpp.h"
//
//IRuntimeObjectSystem* RCCpp::s_pRuntimeObjectSystem = nullptr;
//
//bool RCCpp::Init()
//{
//	s_pRuntimeObjectSystem = new RuntimeObjectSystem;
//	if (!s_pRuntimeObjectSystem->Initialise(NULL, NULL))
//	{
//		delete s_pRuntimeObjectSystem;
//		s_pRuntimeObjectSystem = NULL;
//		return false;
//	}
//	return true;
//}
//
//void RCCpp::Shutdown()
//{
//	delete s_pRuntimeObjectSystem;
//}
//
//void RCCpp::Update(float dt)
//{
//	//check status of any compile
//	if (s_pRuntimeObjectSystem->GetIsCompiledComplete())
//	{
//		// load module when compile complete
//		s_pRuntimeObjectSystem->LoadCompiledModule();
//	}
//
//	if (!s_pRuntimeObjectSystem->GetIsCompiling())
//	{
//		s_pRuntimeObjectSystem->GetFileChangeNotifier()->Update(dt);
//	}
//}
