/*
            This file is part of: 
                NoahFrame
            https://github.com/ketoo/NoahGameFrame

   Copyright 2009 - 2021 NoahFrame(NoahGameFrame)

   File creator: lvsheng.huang
   
   NoahFrame is open-source software and you can redistribute it and/or modify
   it under the terms of the License; besides, anyone who use this file/software must include this copyright announcement.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

//#include "Dependencies/googleLuban-release-1.8.0/googleLuban/include/gtest/gtest.h"
#include "NFLubanModule.h"

bool NFLubanModule::Awake()
{
	int argc = 0;
	char* c = new char[1];
	//::Lubaning::GLuban_FLAG(output) = "xml:hello.xml";
	//::Lubaning::InitGoogleLuban(&argc, &c);

	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->Awake();
	}

	return true;
}

bool NFLubanModule::Init()
{
	//find all plugins and all modules, then check whether they have a Lubaner
	//if any module have't a Lubaner for it then  can not start the application
	//this is a rule for NF's world to keep high quality code under TDD

	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->Init();
	}

    return true;
}

bool NFLubanModule::AfterInit()
{
	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->AfterInit();
	}

	return true;
}

bool NFLubanModule::CheckConfig()
{
	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->CheckConfig();
	}

	return true;
}

bool NFLubanModule::ReadyExecute()
{
	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->ReadyExecute();
	}

	return true;
}

bool NFLubanModule::Execute()
{
	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->Execute();
	}

    return true;
}

bool NFLubanModule::BeforeShut()
{
	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->BeforeShut();
	}

	return true;
}

bool NFLubanModule::Shut()
{
	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->Shut();
	}

	return true;
}

bool NFLubanModule::Finalize()
{
	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->Finalize();
	}

	return true;
}

bool NFLubanModule::OnReloadPlugin()
{
	std::list<NFIModule*> xModules = pPluginManager->Modules();
	for (auto it : xModules)
	{
		NFIModule* pModule = it;
		NFIModule* pLubanModule = pPluginManager->FindModule(pModule->name);
		pLubanModule->OnReloadPlugin();
	}

	return true;
}

int NFLubanModule::Factorial(int n)
{
	if (1 == n)
	{
		return 1;
	}

	return n*Factorial(n-1);
}

// 测试用例
/*
Luban(FactorialLuban, HandlesPositiveInput) {
    EXPECT_EQ(1, this->Factorial(1));
    EXPECT_EQ(2, this->Factorial(2));
    EXPECT_EQ(6, this->Factorial(3));
    EXPECT_EQ(40320, this->Factorial(8));
}
*/