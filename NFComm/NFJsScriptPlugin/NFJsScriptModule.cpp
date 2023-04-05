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
#include <node.h>
#include "uv.h"
#include <iostream>
#include <string>
#include <assert.h>
#include <filesystem>
#include "NFJsScriptModule.h"
#include "NFJsScriptPlugin.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "Dependencies/puerts/include/Binding.hpp" 
#include "Dependencies/puerts/include/CppObjectMapper.h"

namespace fs = std::filesystem;

namespace puerts
{



bool NFJsScriptModule::Awake()
{
	mnTime = pPluginManager->GetNowTime();

	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pEventModule = pPluginManager->FindModule<NFIEventModule>();
    m_pScheduleModule = pPluginManager->FindModule<NFIScheduleModule>();
    m_pNetClientModule = pPluginManager->FindModule<NFINetClientModule>();
    m_pNetModule = pPluginManager->FindModule<NFINetModule>();
    m_pLogModule  =  std::shared_ptr<NFILogModule>(pPluginManager->FindModule<NFILogModule>());

	

	std::string strRootFile = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/NFScriptSystem.lua";

	
    std::cout << "Hello, world1, BeforeShut1111" << std::endl;
	return true;
}

bool NFJsScriptModule::Init()
{
    return true;
}

bool NFJsScriptModule::AfterInit()
{
    return true;
}

bool NFJsScriptModule::Shut()
{
	
    return true;
}

bool NFJsScriptModule::ReadyExecute()
{
	return true;
}

bool NFJsScriptModule::Execute()
{
    if (pPluginManager->GetNowTime() - mnTime > 10)
    {
        mnTime = pPluginManager->GetNowTime();

        OnScriptReload();
    }

    return true;
}

bool NFJsScriptModule::BeforeShut()
{
    return true;
}



bool NFJsScriptModule::EnterScene(const int sceneID, const int groupID)
{
	return false;
}

bool NFJsScriptModule::DoEvent(const NFGUID & self, const int eventID, const NFDataList & arg)
{
	m_pEventModule->DoEvent(self, (int)eventID, arg);

	return true;
}




bool NFJsScriptModule::AddEventCallBack(const NFGUID& self, const int eventID, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaEventCallBackFuncMap, self, (int)eventID, luaFuncName))
		{
			m_pEventModule->AddEventCallBack(self, eventID, this, &NFJsScriptModule::OnLuaEventCB);
		}

		return true;
	}

	return false;
}

int NFJsScriptModule::OnLuaEventCB(const NFGUID& self, const int eventID, const NFDataList& argVar)
{

	auto funcList = mxLuaEventCallBackFuncMap.GetElement(eventID);
	if (funcList)
	{
		auto funcNameList = funcList->GetElement(self);
		if (funcNameList)
		{
			std::string funcName;
			auto Ret = funcNameList->First(funcName);
			while (Ret)
			{
				try
				{
					LuaIntf::LuaRef func(mLuaContext, funcName.c_str());
					func.call<LuaIntf::LuaRef>("", self, eventID, (NFDataList&)argVar);
				}
				catch (LuaIntf::LuaException& e)
				{
					cout << e.what() << endl;
				}
				catch (...)
				{
				}

				Ret = funcNameList->Next(funcName);
			}
		}
	}

    return 0;
}

bool NFJsScriptModule::AddModuleSchedule(std::string& strHeartBeatName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc, const float time, const int count)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaHeartBeatCallBackFuncMap, strHeartBeatName, luaFuncName))
		{
			return m_pScheduleModule->AddSchedule(NFGUID(), strHeartBeatName, this, &NFJsScriptModule::OnLuaHeartBeatCB, time, count);
		}
	}

	return false;
}

bool NFJsScriptModule::AddSchedule(const NFGUID& self, std::string& strHeartBeatName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc, const float time, const int count)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxJsHeartBeatCallBackFuncMap, self, strHeartBeatName, luaFuncName))
		{
			m_pScheduleModule->AddSchedule(self, strHeartBeatName, this, &NFJsScriptModule::OnTimeOutHeartBeatCB, time, count);
		}

		return true;
	}

	return false;
}

int NFJsScriptModule::OnTimeOutHeartBeatCB(const NFGUID& self, const std::string& strHeartBeatName, const float time, const int count)
{

    auto Iterator = std::find_if(TickerDelegateHandleMap.begin(), TickerDelegateHandleMap.end(), [&](auto& Pair) { return Pair.first == self; });
    if (Iterator != TickerDelegateHandleMap.end())
    {
        Iterator->second->IsCalling = true;
        v8::Isolate::Scope Isolatecvope(Iterator->second->Isolate);
        v8::HandleScope HandleScope(Iterator->second->Isolate);
        v8::Local<v8::Context> Context = v8::Local<v8::Context>::New(Iterator->second->Isolate,Iterator->second->DefaultContext);
        v8::Context::Scope ContextScope(Context);
        v8::Local<v8::Function> Function = v8::Local<v8::Function>::New(Iterator->second->Isolate,Iterator->second->DefaultFunction);

        v8::TryCatch TyCatch(Iterator->second->Isolate);
        Iterator->second->IsCalling = true;
        v8::MaybeLocal<v8::Value> Result = Function->Call(Context,Context->Global(),0,nullptr);
        Iterator->second->IsCalling = false;

        if (TyCatch.HasCaught())
        {
            Iterator->second->exceptionHandler(Iterator->second->Isolate,&TyCatch);
        }

        if(!Iterator->second->FunctionContinue)
        {
            m_pScheduleModule->RemoveSchedule(self);
        }
    }
    return 0;
}




NFINT64 NFJsScriptModule::GetNowTime()
{
	return pPluginManager->GetNowTime();
}

NFGUID NFJsScriptModule::CreateId()
{
	return m_pKernelModule->CreateGUID();
}

NFINT64 NFJsScriptModule::APPId()
{
	return pPluginManager->GetAppID();
}

NFINT64 NFJsScriptModule::APPType()
{
    NF_SHARE_PTR<NFIClass> xLogicClass = m_pClassModule->GetElement(NFrame::Server::ThisName());
    if (xLogicClass)
    {
		const std::vector<std::string>& strIdList = xLogicClass->GetIDList();
		for (int i = 0; i < strIdList.size(); ++i)
		{
			const std::string& strId = strIdList[i];

            const int serverType = m_pElementModule->GetPropertyInt32(strId, NFrame::Server::Type());
            const int serverID = m_pElementModule->GetPropertyInt32(strId, NFrame::Server::ServerID());
            if (pPluginManager->GetAppID() == serverID)
            {
                return serverType;
            }
        }
    }
    
	return 0;
}

bool NFJsScriptModule::ExistElementObject(const std::string & configName)
{
	return m_pElementModule->ExistElement(configName);
}

std::vector<std::string> NFJsScriptModule::GetEleList(const std::string& className)
{
    NF_SHARE_PTR<NFIClass> xLogicClass = m_pClassModule->GetElement(NFrame::Server::ThisName());
	if (xLogicClass)
	{
		return xLogicClass->GetIDList();
    }

    return std::vector<std::string>();
}


void NFJsScriptModule::RemoveCallBackAsServer(const int msgID)
{
	m_pNetModule->RemoveReceiveCallBack(msgID);
}

void NFJsScriptModule::AddMsgCallBackAsServer(const int msgID, const LuaIntf::LuaRef & luaTable, const LuaIntf::LuaRef & luaFunc)
{
	auto funcNameList = mxNetMsgCallBackFuncMapAsServer.GetElement(msgID);
	if (!funcNameList)
	{
		funcNameList = new NFList<string>();
		mxNetMsgCallBackFuncMapAsServer.AddElement(msgID, funcNameList);

		m_pNetModule->AddReceiveCallBack(msgID, this, &NFJsScriptModule::OnNetMsgCallBackAsServer);
	}

	std::string funcName = FindFuncName(luaTable, luaFunc);
	if (!funcName.empty())
	{
		if (!funcNameList->Find(funcName))
		{
			funcNameList->Add(funcName);
		}
	}
}

void NFJsScriptModule::RemoveMsgCallBackAsClient(const NF_SERVER_TYPES serverType, const int msgID)
{
	m_pNetClientModule->RemoveReceiveCallBack(serverType, msgID);
}

void NFJsScriptModule::AddMsgCallBackAsClient(const NF_SERVER_TYPES serverType, const int msgID, const LuaIntf::LuaRef & luaTable, const LuaIntf::LuaRef & luaFunc)
{
	auto serverMap = mxNetMsgCallBackFuncMapAsClient.GetElement(serverType);
	if (!serverMap)
	{
		serverMap = new NFMap<int, NFList<std::string>>();
		mxNetMsgCallBackFuncMapAsClient.AddElement(serverType, serverMap);
	}

	auto funcNameList = serverMap->GetElement(msgID);
	if (!funcNameList)
	{
		funcNameList = new NFList<string>();
		serverMap->AddElement(msgID, funcNameList);

		switch (serverType)
		{
			case NF_SERVER_TYPES::NF_ST_MASTER:
				m_pNetClientModule->AddReceiveCallBack(serverType, msgID, this, &NFJsScriptModule::OnNetMsgCallBackAsClientForMasterServer);
				break;
			case NF_SERVER_TYPES::NF_ST_WORLD:
				m_pNetClientModule->AddReceiveCallBack(serverType, msgID, this, &NFJsScriptModule::OnNetMsgCallBackAsClientForWorldServer);
				break;
			case NF_SERVER_TYPES::NF_ST_GAME:
				m_pNetClientModule->AddReceiveCallBack(serverType, msgID, this, &NFJsScriptModule::OnNetMsgCallBackAsClientForGameServer);
				break;
			default:
				break;
		}

	}

	std::string funcName = FindFuncName(luaTable, luaFunc);
	if (!funcName.empty())
	{
		if (!funcNameList->Find(funcName))
		{
			funcNameList->Add(funcName);
		}
	}
}

void NFJsScriptModule::ImportProtoFile(const std::string& fileName)
{
	NFJsPBModule* p = (NFJsPBModule*)m_pJsPBModule;
	p->ImportProtoFile(fileName);
}

const std::string NFJsScriptModule::Encode(const std::string& msgTypeName, const v8::Local<v8::Value>& v8Value)
{
	NFJsPBModule* p = (NFJsPBModule*)m_pJsPBModule;
	return p->Encode(msgTypeName, v8Value);
}

v8::Local<v8::Value> NFJsScriptModule::Decode(const std::string& msgTypeName, const std::string& data)
{
	NFJsPBModule* p = (NFJsPBModule*)m_pJsPBModule;
	return p->Decode(msgTypeName, data);
}

void NFJsScriptModule::SendToServerByServerID(const int serverID, const uint16_t msgID, const std::string& data)
{
    if (pPluginManager->GetAppID() == serverID)
    {
        m_pLogModule->LogError("you can send message to yourself");
        return;
    }

	m_pNetClientModule->SendByServerID(serverID, msgID, data);
}

void NFJsScriptModule::SendToServerBySuit(const NF_SERVER_TYPES eType, const uint16_t msgID, const std::string & data, const std::string& hash)
{
	m_pNetClientModule->SendBySuitWithOutHead(eType, hash, msgID,data );
}

void NFJsScriptModule::SendToAllServerByServerType(const NF_SERVER_TYPES eType, const uint16_t msgID, const std::string &data)
{
	m_pNetClientModule->SendToAllServer(eType, msgID,data );
}

void NFJsScriptModule::SendMsgToClientByFD(const NFSOCK fd, const uint16_t msgID, const std::string &data)
{
	//for all servers
	m_pNetModule->SendMsgWithOutHead(msgID, data, fd);
}

void NFJsScriptModule::SendMsgToPlayer(const NFGUID player, const uint16_t msgID, const std::string& data)
{
    //the app must be the game server
	if (pPluginManager->GetAppType() == NF_SERVER_TYPES::NF_ST_GAME)
	{

	}
	else if (pPluginManager->GetAppType() == NF_SERVER_TYPES::NF_ST_WORLD)
	{

	}
	else if (pPluginManager->GetAppType() == NF_SERVER_TYPES::NF_ST_PROXY)
	{
	}
	else
	{
		m_pLogModule->LogError("you are not: NF_ST_GAME || NF_ST_WORLD");
	}
}

void NFJsScriptModule::SendToGroupPlayer(const uint16_t msgID, const std::string& data)
{
    //the app must be the game server
	if (pPluginManager->GetAppType() == NF_SERVER_TYPES::NF_ST_GAME)
	{

	}
	else
	{
		m_pLogModule->LogError("you are not an game server");
	}
}

void NFJsScriptModule::SendToAllPlayer(const uint16_t msgID, const std::string& data)
{
	//if game server
	//if world server
	//if proxy server
	if (pPluginManager->GetAppType() == NF_SERVER_TYPES::NF_ST_GAME)
	{
	}
	else if (pPluginManager->GetAppType() == NF_SERVER_TYPES::NF_ST_WORLD)
	{
	}
	else if (pPluginManager->GetAppType() == NF_SERVER_TYPES::NF_ST_PROXY)
	{
		m_pNetModule->SendMsgToAllClientWithOutHead(msgID, data);
	}
	else
	{
		m_pLogModule->LogError("you are not an game server or world server");
	}
}



void NFJsScriptModule::SetVersionCode(const std::string& logData)
{
    strVersionCode = logData;
}

const std::string&  NFJsScriptModule::GetVersionCode()
{
    return strVersionCode;
}



std::string NFJsScriptModule::FindFuncName(const LuaIntf::LuaRef & luaTable, const LuaIntf::LuaRef & luaFunc)
{
	if (luaTable.isTable() && luaFunc.isFunction())
	{
		std::string strLuaTableName = "";
		std::map<std::string, LuaIntf::LuaRef>::iterator it = mxTableName.begin();
		for (it; it != mxTableName.end(); ++it)
		{
			if (it->second == luaTable)
			{
				strLuaTableName = it->first;
			}
		}
		
		if (!strLuaTableName.empty())
		{
			for (auto itr = luaTable.begin(); itr != luaTable.end(); ++itr)
			{
				const LuaIntf::LuaRef& key = itr.key();

				const std::string& sKey = key.toValue<std::string>();
				const LuaIntf::LuaRef& val = itr.value();
				if (val.isFunction() && luaFunc.isFunction() && val == luaFunc)
				{
					strLuaTableName.append(".");
					strLuaTableName.append(sKey);
					return strLuaTableName;
				}
			}
		}
	}

	return NULL_STR;
}

void NFJsScriptModule::OnNetMsgCallBackAsServer(const NFSOCK sockIndex, const int msgID, const char *msg, const uint32_t len)
{
	auto msgCallBack = mxNetMsgCallBackFuncMapAsServer.GetElement(msgID);
	if (msgCallBack)
	{
		std::string msgData(msg, len);
		std::string funcName;
		auto Ret = msgCallBack->First(funcName);
		while (Ret)
		{
			try
			{
				LuaIntf::LuaRef func(mLuaContext, funcName.c_str());
				func.call<LuaIntf::LuaRef>("", sockIndex, msgID, msgData);
			}
			catch (LuaIntf::LuaException& e)
			{
				cout << e.what() << endl;
			}
			catch (...)
			{
			}

			Ret = msgCallBack->Next(funcName);
		}
	}
}

void NFJsScriptModule::OnNetMsgCallBackAsClientForMasterServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len)
{
	auto serverData = mxNetMsgCallBackFuncMapAsClient.GetElement(NF_SERVER_TYPES::NF_ST_MASTER);
	if (serverData)
	{
		auto msgCallBack = serverData->GetElement(msgID);
		if (msgCallBack)
		{
			std::string msgData(msg, len);

			std::string funcName;
			auto Ret = msgCallBack->First(funcName);
			while (Ret)
			{
				try
				{
					LuaIntf::LuaRef func(mLuaContext, funcName.c_str());
					func.call<LuaIntf::LuaRef>("", sockIndex, msgID, msgData);
				}
				catch (LuaIntf::LuaException& e)
				{
					cout << e.what() << endl;
				}
				catch (...)
				{
				}

				Ret = msgCallBack->Next(funcName);
			}
		}
	}
}

void NFJsScriptModule::OnNetMsgCallBackAsClientForWorldServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len)
{
	auto serverData = mxNetMsgCallBackFuncMapAsClient.GetElement(NF_SERVER_TYPES::NF_ST_WORLD);
	if (serverData)
	{
		auto msgCallBack = serverData->GetElement(msgID);
		if (msgCallBack)
		{
			std::string msgData(msg, len);

			std::string funcName;
			auto Ret = msgCallBack->First(funcName);
			while (Ret)
			{
				try
				{
					LuaIntf::LuaRef func(mLuaContext, funcName.c_str());
					func.call<LuaIntf::LuaRef>("", sockIndex, msgID, msgData);
				}
				catch (LuaIntf::LuaException& e)
				{
					cout << e.what() << endl;
				}
				catch (...)
				{
				}

				Ret = msgCallBack->Next(funcName);
			}
		}
	}
}

void NFJsScriptModule::OnNetMsgCallBackAsClientForGameServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len)
{
	auto serverData = mxNetMsgCallBackFuncMapAsClient.GetElement(NF_SERVER_TYPES::NF_ST_GAME);
	if (serverData)
	{
		auto msgCallBack = serverData->GetElement(msgID);
		if (msgCallBack)
		{
			std::string msgData(msg, len);

			std::string funcName;
			auto Ret = msgCallBack->First(funcName);
			while (Ret)
			{
				try
				{
					LuaIntf::LuaRef func(mLuaContext, funcName.c_str());
					func.call<LuaIntf::LuaRef>("", sockIndex, msgID, msgData);
				}
				catch (LuaIntf::LuaException& e)
				{
					cout << e.what() << endl;
				}
				catch (...)
				{
				}

				Ret = msgCallBack->Next(funcName);
			}
		}
	}
}

NFJsScriptModule::NFJsScriptModule(const std::string& NFDataCfgPath,const std::string& ScriptRoot): NFJsScriptModule( std::make_shared<DefaultJSModuleLoader>(NFDataCfgPath,ScriptRoot), -1, nullptr, nullptr, nullptr)
{

}

static void ToCString(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    auto Isolate = Info.GetIsolate();
    if (!Info[0]->IsString())
    {
        FV8Utils::ThrowException(Isolate, "expect a string");
        return;
    }
    v8::Local<v8::String> Str = Info[0]->ToString(Isolate->GetCurrentContext()).ToLocalChecked();

    const size_t Length = Str->Utf8Length(Isolate);
    v8::Local<v8::ArrayBuffer> Ab = v8::ArrayBuffer::New(Info.GetIsolate(), Length + 1);
#if defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
    char* Buff = static_cast<char*>(v8::ArrayBuffer_Get_Data(Ab));
#else
    char* Buff = static_cast<char*>(Ab->GetContents().Data());
#endif
    Str->WriteUtf8(Isolate, Buff);
    Buff[Length] = '\0';
    Info.GetReturnValue().Set(Ab);
}

static void ToCPtrArray(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    const size_t Length = sizeof(void*) * Info.Length();

    v8::Local<v8::ArrayBuffer> Ret = v8::ArrayBuffer::New(Info.GetIsolate(), Length);
#if defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
    void** Buff = static_cast<void**>(v8::ArrayBuffer_Get_Data(Ret));
#else
    void** Buff = static_cast<void**>(Ret->GetContents().Data());
#endif

    for (int i = 0; i < Info.Length(); i++)
    {
            auto Val = Info[i];
            void* Ptr = nullptr;
            if (Val->IsArrayBufferView())
            {
                v8::Local<v8::ArrayBufferView> BuffView = Val.As<v8::ArrayBufferView>();
                auto Ab = BuffView->Buffer();
    #if defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
                Ptr = static_cast<char*>(v8::ArrayBuffer_Get_Data(Ab)) + BuffView->ByteOffset();
    #else
                Ptr = static_cast<char*>(Ab->GetContents().Data()) + BuffView->ByteOffset();
    #endif
            }
            else if (Val->IsArrayBuffer())
            {
                auto Ab = v8::Local<v8::ArrayBuffer>::Cast(Val);
    #if defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
                Ptr = static_cast<char*>(v8::ArrayBuffer_Get_Data(Ab));
    #else
                Ptr = static_cast<char*>(Ab->GetContents().Data());
    #endif
            }
            Buff[i] = Ptr;
        }
        Info.GetReturnValue().Set(Ret);
    }

NFJsScriptModule::NFJsScriptModule(std::shared_ptr<IJSModuleLoader> InModuleLoader, int InDebugPort,std::function<void(const std::string&)> InOnSourceLoadedCallback, void* InExternalRuntime, void* InExternalContext)
{
    Started = false;
    Inspector = nullptr;
    InspectorChannel = nullptr;

    ModuleLoader = std::move(InModuleLoader);
    OnSourceLoadedCallback = InOnSourceLoadedCallback;

    int Argc = 1;
    const char* Argv[] = {"puerts"};
    std::vector<std::string> Args(Argv, Argv + Argc);
    std::vector<std::string> ExecArgs;
    std::vector<std::string> Errors;

  
    std::unique_ptr<node::MultiIsolatePlatform> platform =  node::MultiIsolatePlatform::Create(4);
    v8::V8::InitializePlatform(platform.get());
    v8::V8::Initialize();


    int exit_code = 0;
    std::vector<std::string> errors;
    std::unique_ptr<node::CommonEnvironmentSetup> setup = node::CommonEnvironmentSetup::Create(platform.get(), &errors, Args, ExecArgs);
    if (!setup) {
        for (const std::string& err : errors)
        fprintf(stderr, "%s: %s\n", Args[0].c_str(), err.c_str());
        return ;
    }

    MainIsolate = setup->isolate();
    NodeEnv = setup->env();

        
    v8::Locker locker(MainIsolate);
    v8::Isolate::Scope isolate_scope(MainIsolate);
    v8::HandleScope handle_scope(MainIsolate);
    v8::Context::Scope context_scope(setup->context());

    v8::MaybeLocal<v8::Value> LoadenvRet = node::LoadEnvironment(
                NodeEnv,
                "const publicRequire = require('module').createRequire(process.cwd() + '/');"
                "globalThis.require = publicRequire;"
                "globalThis.embedVars = { nön_ascıı: '🏳️‍🌈' };"
                "require('vm').runInThisContext(process.argv[1]);");

    if (LoadenvRet.IsEmpty())  // There has been a JS exception.
    {
        return ;
    }
        

exit_code = node::SpinEventLoop(NodeEnv).FromMaybe(1);


    
    auto Isolate = MainIsolate;
#ifdef THREAD_SAFE
    v8::Locker Locker(Isolate);
#endif

    Isolate->SetData(0, static_cast<ICppObjectMapper*>(this));    //直接传this会有问题，强转后地址会变


    v8::Local<v8::Context> Context = setup->context();
    DefaultContext.Reset(Isolate, Context);

    v8::Context::Scope context_scope(Context);
    // the same as raw v8
    Isolate->SetMicrotasksPolicy(v8::MicrotasksPolicy::kAuto);


    v8::Local<v8::Object> Global = Context->Global();

    v8::Local<v8::Object> PuertsObj = v8::Object::New(Isolate);
    Global->Set(Context, FV8Utils::InternalString(Isolate, "puerts"), PuertsObj).Check();

    auto This = v8::External::New(Isolate, this);

    MethodBindingHelper<&NFJsScriptModule::EvalScript>::Bind(Isolate, Context, Global, "__tgjsEvalScript", This);

    MethodBindingHelper<&NFJsScriptModule::Log>::Bind(Isolate, Context, Global, "__tgjsLog", This);

    MethodBindingHelper<&NFJsScriptModule::SearchModule>::Bind(Isolate, Context, Global, "__tgjsSearchModule", This);

    MethodBindingHelper<&NFJsScriptModule::LoadModule>::Bind(Isolate, Context, Global, "__tgjsLoadModule", This);

    MethodBindingHelper<&NFJsScriptModule::LoadCppType>::Bind(Isolate, Context, Global, "__tgjsLoadCDataType", This);

    MethodBindingHelper<&NFJsScriptModule::FindModule>::Bind(Isolate, Context, Global, "__tgjsFindModule", This);

    MethodBindingHelper<&NFJsScriptModule::SetInspectorCallback>::Bind(Isolate, Context, Global, "__tgjsSetInspectorCallback", This);

    MethodBindingHelper<&NFJsScriptModule::DispatchProtocolMessage>::Bind( Isolate, Context, Global, "__tgjsDispatchProtocolMessage", This);

    Isolate->SetPromiseRejectCallback(&PromiseRejectCallback<NFJsScriptModule>);
    Global->Set(Context, FV8Utils::ToV8String(Isolate, "__tgjsSetPromiseRejectCallback"),v8::FunctionTemplate::New(Isolate, &SetPromiseRejectCallback<NFJsScriptModule>)->GetFunction(Context).ToLocalChecked()).Check();

    MethodBindingHelper<&NFJsScriptModule::SetTimeout>::Bind(Isolate, Context, Global, "setTimeout", This);

    MethodBindingHelper<&NFJsScriptModule::ClearInterval>::Bind(Isolate, Context, Global, "clearTimeout", This);

    MethodBindingHelper<&NFJsScriptModule::SetInterval>::Bind(Isolate, Context, Global, "setInterval", This);

    MethodBindingHelper<&NFJsScriptModule::ClearInterval>::Bind(Isolate, Context, Global, "clearInterval", This);

    PuertsObj->Set(Context, FV8Utils::ToV8String(Isolate, "toCString"),
                v8::FunctionTemplate::New(Isolate, ToCString)->GetFunction(Context).ToLocalChecked())
            .Check();

    PuertsObj->Set(Context, FV8Utils::ToV8String(Isolate, "toCPtrArray"),
                v8::FunctionTemplate::New(Isolate, ToCPtrArray)->GetFunction(Context).ToLocalChecked())
            .Check();

    CppObjectMapper.Initialize(Isolate, Context);

    Inspector = CreateV8Inspector(InDebugPort, &Context);

    ExecuteModule("puerts/first_run.js");
    ExecuteModule("puerts/log.js");
    ExecuteModule("puerts/modular.js");
    ExecuteModule("puerts/uelazyload.js");
    ExecuteModule("puerts/events.js");
    ExecuteModule("puerts/promises.js");
    ExecuteModule("puerts/argv.js");
    ExecuteModule("puerts/jit_stub.js");
    ExecuteModule("puerts/hot_reload.js");

    Require.Reset(Isolate, PuertsObj->Get(Context, FV8Utils::ToV8String(Isolate, "__require")).ToLocalChecked().As<v8::Function>());

    ReloadJs.Reset(Isolate, PuertsObj->Get(Context, FV8Utils::ToV8String(Isolate, "__reload")).ToLocalChecked().As<v8::Function>());
}

// #lizard forgives
NFJsScriptModule::~NFJsScriptModule()
{

    InspectorMessageHandler.Reset();
    Require.Reset();
    ReloadJs.Reset();
    JsPromiseRejectCallback.Reset();
    {
        auto Isolate = MainIsolate;
#ifdef THREAD_SAFE
        v8::Locker Locker(Isolate);
#endif
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);

    CppObjectMapper.UnInitialize(Isolate);


    for (auto& Pair : TickerDelegateHandleMap)
    {
        m_pScheduleModule->RemoveSchedule(*(Pair.first));
        delete Pair.first;
        delete Pair.second;
    }
    TickerDelegateHandleMap.clear();

    node::EmitExit(NodeEnv);
    node::Stop(NodeEnv);
    node::FreeEnvironment(NodeEnv);
    node::FreeIsolateData(NodeIsolateData);

    if (InspectorChannel)
    {
        delete InspectorChannel;
        InspectorChannel = nullptr;
    }

    if (Inspector)
    {
        delete Inspector;
        Inspector = nullptr;
    }
    
    }
    DefaultContext.Reset();
    MainIsolate->Dispose();
    MainIsolate = nullptr;

    v8::V8::Dispose();
    v8::V8::ShutdownPlatform();
}


bool NFJsScriptModule::IdleNotificationDeadline(double DeadlineInSeconds)
{
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif
    return MainIsolate->IdleNotificationDeadline(DeadlineInSeconds);
}

void NFJsScriptModule::LowMemoryNotification()
{
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif
    MainIsolate->LowMemoryNotification();
}

void NFJsScriptModule::RequestMinorGarbageCollectionForTesting()
{
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif
    MainIsolate->RequestGarbageCollectionForTesting(v8::Isolate::kMinorGarbageCollection);
}

void NFJsScriptModule::RequestFullGarbageCollectionForTesting()
{
#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif

    MainIsolate->RequestGarbageCollectionForTesting(v8::Isolate::kFullGarbageCollection);

}


void NFJsScriptModule::JsHotReload(std::string ModuleName, const std::string& JsSource)
{

    auto Isolate = MainIsolate;
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    auto Context = DefaultContext.Get(Isolate);
    v8::Context::Scope ContextScope(Context);
    auto LocalReloadJs = ReloadJs.Get(Isolate);

    std::string OutPath, OutDebugPath;

    if (ModuleLoader->Search("", ModuleName, OutPath, OutDebugPath))
    {
        fs::path  FullPath = fs::absolute(OutPath);
        m_pLogModule->LogInfo("reload js module ["+FullPath.string()+"]");
        v8::TryCatch TryCatch(Isolate);
        v8::Handle<v8::Value> Args[] = {
            FV8Utils::ToV8String(Isolate, ModuleName), 
            FV8Utils::ToV8String(Isolate, FullPath.string()), 
            FV8Utils::ToV8String(Isolate, JsSource)};

        auto MaybeRet = LocalReloadJs->Call(Context, v8::Undefined(Isolate), 3, Args);

        if (TryCatch.HasCaught())
        {
            m_pLogModule->LogError("reload module exception "+FV8Utils::TryCatchToString(Isolate, &TryCatch));
        }
    }
    else
    {
        m_pLogModule->LogWarning("not find js module ["+ModuleName+"]");
        return;
    }
}

void NFJsScriptModule::ReloadModule(std::string ModuleName, const std::string& JsSource)
{

#ifdef THREAD_SAFE
    v8::Locker Locker(MainIsolate);
#endif
    // m_pLogModule->Info(FString::Printf(TEXT("start reload js module [%s]"), *ModuleName.ToString()));
    JsHotReload(ModuleName, JsSource);
}

void NFJsScriptModule::ReloadSource(const std::string& Path, const std::string& JsSource)
{

    auto Isolate = MainIsolate;
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    auto Context = DefaultContext.Get(Isolate);
    v8::Context::Scope ContextScope(Context);
    auto LocalReloadJs = ReloadJs.Get(Isolate);

    std::ostringstream stream;
    std::string copyPath = Path;
    stream << "reload js [" << copyPath <<"]";
    m_pLogModule->LogInfo(stream);
    v8::TryCatch TryCatch(Isolate);
    v8::Handle<v8::Value> Args[] = {v8::Undefined(Isolate), FV8Utils::ToV8String(Isolate, Path), FV8Utils::ToV8String(Isolate, JsSource.c_str())};

        auto MaybeRet = LocalReloadJs->Call(Context, v8::Undefined(Isolate), 3, Args);

    if (TryCatch.HasCaught())
    {
        std::ostringstream streama;
        streama << "reload module exception " << FV8Utils::TryCatchToString(Isolate, &TryCatch);
        m_pLogModule->LogError(streama);
    }
}

void NFJsScriptModule::OnSourceLoaded(std::function<void(const std::string&)> Callback)
{
    OnSourceLoadedCallback = Callback;
}

std::string NFJsScriptModule::CurrentStackTrace()
{
    return "";
}


v8::Local<v8::Value> NFJsScriptModule::FindOrAddCppObject(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, const void* TypeId, void* Ptr, bool PassByPointer)
{
    return CppObjectMapper.FindOrAddCppObject(Isolate, Context, TypeId, Ptr, PassByPointer);
}

   

void NFJsScriptModule::BindCppObject(v8::Isolate* InIsolate, JSClassDefinition* ClassDefinition, void* Ptr, v8::Local<v8::Object> JSObject, bool PassByPointer)
{
    CppObjectMapper.BindCppObject(InIsolate, ClassDefinition, Ptr, JSObject, PassByPointer);
}



void NFJsScriptModule::UnBindCppObject(JSClassDefinition* ClassDefinition, void* Ptr)
{
    CppObjectMapper.UnBindCppObject(ClassDefinition, Ptr);
}


bool NFJsScriptModule::IsInstanceOfCppObject(const void* TypeId, v8::Local<v8::Object> JsObject)
{
    return CppObjectMapper.IsInstanceOfCppObject(TypeId, JsObject);
}

void NFJsScriptModule::LoadCppType(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    CppObjectMapper.LoadCppType(Info);
}



void NFJsScriptModule::Start(const std::string& ModuleNameOrScript,  bool IsScript)
{

    if (Started)
    {
        m_pLogModule->LogError("Started yet!");
        return;
    }

    auto Isolate = MainIsolate;
#ifdef THREAD_SAFE
    v8::Locker Locker(Isolate);
#endif
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    auto Context = v8::Local<v8::Context>::New(Isolate, DefaultContext);
    v8::Context::Scope ContextScope(Context);

    auto MaybeTGameTGJS = Context->Global()->Get(Context, FV8Utils::ToV8String(Isolate, "puerts"));

    if (MaybeTGameTGJS.IsEmpty() || !MaybeTGameTGJS.ToLocalChecked()->IsObject())
    {
        m_pLogModule->LogError("global.puerts not found!");
        return;
    }

    auto TGJS = MaybeTGameTGJS.ToLocalChecked()->ToObject(Context).ToLocalChecked();

    auto MaybeArgv = TGJS->Get(Context, FV8Utils::ToV8String(Isolate, "argv"));

    if (MaybeArgv.IsEmpty() || !MaybeArgv.ToLocalChecked()->IsObject())
    {
        m_pLogModule->LogError("global.puerts.argv not found!");
        return;
    }

    auto Argv = MaybeArgv.ToLocalChecked()->ToObject(Context).ToLocalChecked();

    auto MaybeArgvAdd = Argv->Get(Context, FV8Utils::ToV8String(Isolate, "add"));

    if (MaybeArgvAdd.IsEmpty() || !MaybeArgvAdd.ToLocalChecked()->IsFunction())
    {
        m_pLogModule->LogError("global.puerts.argv.add not found!");
        return;
    }

    auto ArgvAdd = MaybeArgvAdd.ToLocalChecked().As<v8::Function>();
    if (IsScript)
    {
        v8::ScriptOrigin Origin(FV8Utils::ToV8String(Isolate, "chunk"));
        v8::Local<v8::String> Source = FV8Utils::ToV8String(Isolate, ModuleNameOrScript);
        v8::TryCatch TryCatch(Isolate);

        auto CompiledScript = v8::Script::Compile(Context, Source, &Origin);
        if (CompiledScript.IsEmpty())
        {
            m_pLogModule->LogError(FV8Utils::TryCatchToString(Isolate, &TryCatch));
            return;
        }
        auto ReturnVal = CompiledScript.ToLocalChecked()->Run(Context);
        if (TryCatch.HasCaught())
        {
            m_pLogModule->LogError(FV8Utils::TryCatchToString(Isolate, &TryCatch));
        }
    }
    else
    {
        v8::TryCatch TryCatch(Isolate);
        v8::Local<v8::Value> Args[] = {FV8Utils::ToV8String(Isolate, ModuleNameOrScript)};
        __USE(Require.Get(Isolate)->Call(Context, v8::Undefined(Isolate), 1, Args));
        if (TryCatch.HasCaught())
        {
            m_pLogModule->LogError(FV8Utils::TryCatchToString(Isolate, &TryCatch));
        }
    }
    Started = true;
}

bool NFJsScriptModule::LoadFile(const std::string& RequiringDir, const std::string& ModuleName, std::string& OutPath, std::string& OutDebugPath,std::string& Data, std::string& ErrInfo)
{
    if (ModuleLoader->Search(RequiringDir, ModuleName, OutPath, OutDebugPath))
    {
        if (!ModuleLoader->Load(OutPath, Data))
        {
            std::ostringstream stream;
            stream <<"can not load ["<< ModuleName <<"]";
            ErrInfo = stream.str();
            //m_pLogModule->LogInfo(stream.str());
            return false;
        }
    }
    else
    {
        std::ostringstream stream;
        stream <<"can not load ["<< ModuleName <<"]";
        ErrInfo = stream.str();
        //ErrInfo = stream.str();
        return false;
    }
    return true;
}

void NFJsScriptModule::ExecuteModule(const std::string& ModuleName, std::function<std::string(const std::string&, const std::string&)> Preprocessor)
{
        std::string OutPath;
        std::string DebugPath;
        std::string Data;

        std::string ErrInfo;
        if (!LoadFile("", ModuleName, OutPath, DebugPath, Data, ErrInfo))
        {
            m_pLogModule->LogError(ErrInfo);
            return;
        }

        auto Isolate = MainIsolate;
        v8::Isolate::Scope IsolateScope(Isolate);
        v8::HandleScope HandleScope(Isolate);
        auto Context = v8::Local<v8::Context>::New(Isolate, DefaultContext);
        v8::Context::Scope ContextScope(Context);
        
        v8::Local<v8::String> Source;
        if (Preprocessor)
        {
            std::string Script = Data;
            if (Preprocessor)
            {
                Script = Preprocessor(Script, OutPath);
            }
                
            Source = FV8Utils::ToV8String(Isolate, Script);
        }
        else
        {
            Source = FV8Utils::ToV8String(Isolate, Data);
        }
        std::string FormattedScriptUrl = DebugPath;
        v8::Local<v8::String> Name = FV8Utils::ToV8String(Isolate, FormattedScriptUrl);
        v8::ScriptOrigin Origin(Name);
        v8::TryCatch TryCatch(Isolate);

        auto CompiledScript = v8::Script::Compile(Context, Source, &Origin);
        if (CompiledScript.IsEmpty())
        {
            m_pLogModule->LogError(FV8Utils::TryCatchToString(Isolate, &TryCatch));
            return;
        }
    auto ReturnVal = CompiledScript.ToLocalChecked()->Run(Context);
    if (TryCatch.HasCaught())
    {
        m_pLogModule->LogError(FV8Utils::TryCatchToString(Isolate, &TryCatch));
        return;
    }
}

void NFJsScriptModule::EvalScript(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgString, EArgString);

    v8::Local<v8::String> Source = Info[0]->ToString(Context).ToLocalChecked();

    v8::String::Utf8Value UrlArg(Isolate, Info[1]);
    std::string ScriptUrl = std::string(*UrlArg);

    std::string FormattedScriptUrl = ScriptUrl;
    v8::Local<v8::String> Name = FV8Utils::ToV8String(Isolate, FormattedScriptUrl);
    v8::ScriptOrigin Origin(Name);
    auto Script = v8::Script::Compile(Context, Source, &Origin);
    if (Script.IsEmpty())
    {
        return;
    }
    auto Result = Script.ToLocalChecked()->Run(Context);
    if (Result.IsEmpty())
    {
        return;
    }
    Info.GetReturnValue().Set(Result.ToLocalChecked());

    if (OnSourceLoadedCallback)
    {
        OnSourceLoadedCallback(FormattedScriptUrl);
    }
}

void NFJsScriptModule::Log(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgInt32, EArgString);

    auto Level = Info[0]->Int32Value(Context).ToChecked();

    std::string Msg = FV8Utils::ToFString(Isolate, Info[1]);
    switch (Level)
    {
        case 1:
            m_pLogModule->LogInfo(Msg);
            break;
        case 2:
            m_pLogModule->LogWarning(Msg);
            break;
        case 3:
            m_pLogModule->LogError(Msg);
            break;
        default:
            m_pLogModule->LogInfo(Msg);
            break;
    }
}

void NFJsScriptModule::SearchModule(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();

    CHECK_V8_ARGS(EArgString, EArgString);

    std::string ModuleName = FV8Utils::ToFString(Isolate, Info[0]);
    std::string RequiringDir = FV8Utils::ToFString(Isolate, Info[1]);
    std::string OutPath;
    std::string OutDebugPath;

    if (ModuleLoader->Search(RequiringDir, ModuleName, OutPath, OutDebugPath))
    {
        auto Result = v8::Array::New(Isolate);
        Result->Set(Context, 0, FV8Utils::ToV8String(Isolate, OutPath)).Check();
        Result->Set(Context, 1, FV8Utils::ToV8String(Isolate, OutDebugPath)).Check();
        Info.GetReturnValue().Set(Result);
    }
}

void NFJsScriptModule::LoadModule(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgString);

    std::string Path = FV8Utils::ToFString(Isolate, Info[0]);
    std::string Data;
    if (!ModuleLoader->Load(Path, Data))
    {
        FV8Utils::ThrowException(Isolate, "can not load module");
        return;
    }

    Info.GetReturnValue().Set(FV8Utils::ToV8String(Isolate, Data));
}

void NFJsScriptModule::SetTimeout(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgFunction, EArgNumber);

    SetFTickerDelegate(Info, false);
}

void NFJsScriptModule::SetFTickerDelegate(const v8::FunctionCallbackInfo<v8::Value>& Info, bool Continue)
{
    using std::placeholders::_1;
    using std::placeholders::_2;
    std::function<void(const JSError*, std::shared_ptr<NFILogModule>&)> ExceptionLog =[](const JSError* Exception, std::shared_ptr<NFILogModule>& InLogger)
    {
        InLogger->LogWarning("JS Execution Exception: "+((Exception->Message)));
    };
    std::function<void(const JSError*)> ExceptionLogWrapper = std::bind(ExceptionLog, _1, m_pLogModule);
    std::function<void(v8::Isolate*, v8::TryCatch*)> ExecutionExceptionHandler =
    std::bind(&NFJsScriptModule::ReportExecutionException, this, _1, _2, ExceptionLogWrapper);
    std::function<void(NFGUID*)> DelegateHandleCleaner = std::bind(&NFJsScriptModule::RemoveFTickerDelegateHandle, this, _1);

    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    float Millisecond = Info[1]->NumberValue(Context).ToChecked();
    float Delay = Millisecond;
    //float Delay = Millisecond / 1000.f;
    TickDelegateInfo* tickDelegateInfo = new TickDelegateInfo();
    tickDelegateInfo->FunctionContinue  = Continue;
    tickDelegateInfo->exceptionHandler = ExecutionExceptionHandler;
    tickDelegateInfo->DelegateHandleCleaner = DelegateHandleCleaner;

    NFGUID* tdhandle = new NFGUID();
    m_pScheduleModule->AddSchedule(*tdhandle, "JsTimeOutTick", this, &NFJsScriptModule::OnTimeOutHeartBeatCB, Delay, 1);

    // TODO - 如果实现多线程，这里应该加锁阻止定时回调的执行，直到DelegateWrapper设置好handle
    TickerDelegateHandleMap[tdhandle] = tickDelegateInfo;

    Info.GetReturnValue().Set(v8::External::New(Info.GetIsolate(), tdhandle));
}

void NFJsScriptModule::ReportExecutionException(v8::Isolate* Isolate, v8::TryCatch* TryCatch, std::function<void(const JSError*)> CompletionHandler)
{
    const JSError Error(FV8Utils::TryCatchToString(Isolate, TryCatch));
    if (CompletionHandler)
    {
        CompletionHandler(&Error);
    }
}

void NFJsScriptModule::RemoveFTickerDelegateHandle(NFGUID* Handle)
{
    // TODO - 如果实现多线程，FTicker所在主线程和当前线程释放handle可能有竞争
    auto Iterator = std::find_if(TickerDelegateHandleMap.begin(), TickerDelegateHandleMap.end(), [&](auto& Pair) { return Pair.first == Handle; });
    if (Iterator != TickerDelegateHandleMap.end())
    {
        // call clearTimeout in setTimeout callback
        if (Iterator->second->IsCalling)
        {
            Iterator->second->FunctionContinue = false;
            return;
        }
        m_pScheduleModule->RemoveSchedule(*Handle);
        //m_pScheduleModule->RemoveSchedule(*Handle,"JsTimeOutTick");
        delete Iterator->first;
        delete Iterator->second;
        TickerDelegateHandleMap.erase(Iterator);
    }
}

void NFJsScriptModule::ClearInterval(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    // todo - mocha 7.0.1，当reporter为JSON，调用clearTimeout时，可能不传值，或传Null、Undefined过来。暂将其忽略
    if (Info.Length() == 0)
    {
        m_pLogModule->LogWarning("Calling ClearInterval with 0 argument.");
    }
    else if (Info[0]->IsNullOrUndefined())
    {
        // 屏蔽这条只在mocha中出现的警告
        // m_pLogModule->Warn(TEXT("Calling ClearInterval with a Null or Undefined."));
    }
    else
    {
        CHECK_V8_ARGS(EArgExternal);
        v8::Local<v8::External> Arg = v8::Local<v8::External>::Cast(Info[0]);
        NFGUID* Handle = static_cast<NFGUID*>(Arg->Value());
        RemoveFTickerDelegateHandle(Handle);
    }
}

void NFJsScriptModule::SetInterval(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgFunction, EArgNumber);

    SetFTickerDelegate(Info, true);
}


void NFJsScriptModule::FindModule(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope Isolatescope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgString);
    std::string Name = *(v8::String::Utf8Value(Isolate, Info[0]));
    auto Func = FindAddonRegisterFunc(Name);
    if (Func)
    {
        auto Exports = v8::Object::New(Isolate);
        Func(Context, Exports);
        Info.GetReturnValue().Set(Exports);
    }
}

void NFJsScriptModule::SetInspectorCallback(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope Isolatescope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    if (!Inspector)
        return;

    CHECK_V8_ARGS(EArgFunction);

    if (!InspectorChannel)
    {
        InspectorChannel = Inspector->CreateV8InspectorChannel();
        InspectorChannel->OnMessage(
            [this](std::string Message)
            {
                // UE_LOG(LogTemp, Warning, TEXT("<-- %s"), UTF8_TO_TCHAR(Message.c_str()));
                v8::Isolate::Scope IsolatescopeObject(MainIsolate);
                v8::HandleScope HandleScopeObject(MainIsolate);
                v8::Local<v8::Context> ContextInner = DefaultContext.Get(MainIsolate);
                v8::Context::Scope ContextScopeObject(ContextInner);

                auto Handler = InspectorMessageHandler.Get(MainIsolate);

                v8::Local<v8::Value> Args[] = {FV8Utils::ToV8String(MainIsolate, Message.c_str())};

                v8::TryCatch TryCatch(MainIsolate);
                __USE(Handler->Call(ContextInner, ContextInner->Global(), 1, Args));
                if (TryCatch.HasCaught())
                {
                    std::stringstream  ss;
                    ss  <<"inspector callback exception"<<FV8Utils::TryCatchToString(MainIsolate, &TryCatch);
                    m_pLogModule->LogError(ss.str());
                }
            });
    }

    InspectorMessageHandler.Reset(Isolate, v8::Local<v8::Function>::Cast(Info[0]));
}

void NFJsScriptModule::DispatchProtocolMessage(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope Isolatescope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    CHECK_V8_ARGS(EArgString);

    if (InspectorChannel)
    {
        std::string Message = FV8Utils::ToFString(Isolate, Info[0]);
        // UE_LOG(LogTemp, Warning, TEXT("--> %s"), *Message);
        InspectorChannel->DispatchProtocolMessage(Message);
    }
}

void NFJsScriptModule::DumpStatisticsLog(const v8::FunctionCallbackInfo<v8::Value>& Info)
{

    v8::HeapStatistics Statistics;

    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    v8::Context::Scope ContextScope(Context);

    Isolate->GetHeapStatistics(&Statistics);
    std::stringstream  ss;
    ss  <<"------------------------\n"
        <<"Dump Statistics of V8:\n"
        <<"total_heap_size: "<< Statistics.total_heap_size() <<"\n"
        <<"total_heap_size_executable:"<< Statistics.total_heap_size_executable() <<"\n"
        << "total_physical_size: "<< Statistics.total_physical_size() <<"\n"
        << "total_available_size: "<< Statistics.total_available_size() <<"\n"
        << "used_heap_size: "<< Statistics.used_heap_size() <<"\n"
        << "heap_size_limit: "<< Statistics.heap_size_limit() <<"\n"
        << "malloced_memory: "<< Statistics.malloced_memory() <<"\n"
        << "external_memory: "<< Statistics.external_memory() <<"\n"
        << "peak_malloced_memory: "<< Statistics.peak_malloced_memory() <<"\n"
        << "number_of_native_contexts: "<< Statistics.number_of_native_contexts() <<"\n"
        << "number_of_detached_contexts: "<< Statistics.number_of_detached_contexts() <<"\n"
        << "does_zap_garbage: "<< Statistics.does_zap_garbage() <<"\n"
        << "------------------------\n";
    std::string StatisticsLog = ss.str();
    m_pLogModule->LogInfo(StatisticsLog);

}
}