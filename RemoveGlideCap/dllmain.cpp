// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"
#include <Spore\Simulator\cCreatureGameData.h>

class CrGSetup 
	: public App::IMessageListener
	, public DefaultRefCounted
{
public:
	static const uint32_t TYPE = id("CrGSetup");
	CrGSetup::CrGSetup() {}
	CrGSetup::~CrGSetup() {}

	// For internal use, do not modify.
	int CrGSetup::AddRef() override {
		return DefaultRefCounted::AddRef();
	}

	// For internal use, do not modify.
	int CrGSetup::Release() override {
		return DefaultRefCounted::Release();
	}

	// The method that receives the message. The first thing you should do is checking what ID sent this message...
	bool CrGSetup::HandleMessage(uint32_t messageID, void* message) {
		if (messageID == App::kMsgOnModeEnter) {
			if (Simulator::IsCreatureGame())
				CreatureGameData.mNumFlapsAllowed = INT_MAX;
		}
		return true;
	}
};

void Initialize()
{
	MessageManager.AddListener(new CrGSetup(), App::kMsgOnModeEnter);
}

void Dispose()
{
}

static_detour(AfterGlideFinish__detour, void()) {
	void detoured() {
		original_function();
		CreatureGameData.mNumFlapsAllowed = INT_MAX;
	}
};

void AttachDetours()
{
	AfterGlideFinish__detour::attach(GetAddress(Simulator::cCreatureGameData, AfterGlideFinish));
}


// Generally, you don't need to touch any code here
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		ModAPI::AddPostInitFunction(Initialize);
		ModAPI::AddDisposeFunction(Dispose);

		PrepareDetours(hModule);
		AttachDetours();
		CommitDetours();
		break;

	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
		break;
	}
	return TRUE;
}

