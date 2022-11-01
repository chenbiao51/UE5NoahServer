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
#include "NFTsScriptModule.h"
#include "NFTsScriptPlugin.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "Dependencies/puerts/include/Binding.hpp" 
#include "Dependencies/puerts/include/CppObjectMapper.h"

#define TRY_RUN_GLOBAL_SCRIPT_FUN0(strFuncName)   try {LuaIntf::LuaRef func(mLuaContext, strFuncName);  func.call<LuaIntf::LuaRef>(); }   catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }
#define TRY_RUN_GLOBAL_SCRIPT_FUN1(strFuncName, arg1)  try {LuaIntf::LuaRef func(mLuaContext, strFuncName);  func.call<LuaIntf::LuaRef>(arg1); }catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }
#define TRY_RUN_GLOBAL_SCRIPT_FUN2(strFuncName, arg1, arg2)  try {LuaIntf::LuaRef func(mLuaContext, strFuncName);  func.call<LuaIntf::LuaRef>(arg1, arg2); }catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }

#define TRY_LOAD_SCRIPT_FLE(fileName)  try{mLuaContext.doFile(fileName);} catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }

bool NFTsScriptModule::Awake()
{
	mnTime = pPluginManager->GetNowTime();

	m_pKernelModule = pPluginManager->FindModule<NFIKernelModule>();
	m_pClassModule = pPluginManager->FindModule<NFIClassModule>();
	m_pElementModule = pPluginManager->FindModule<NFIElementModule>();
	m_pEventModule = pPluginManager->FindModule<NFIEventModule>();
    m_pScheduleModule = pPluginManager->FindModule<NFIScheduleModule>();
    m_pNetClientModule = pPluginManager->FindModule<NFINetClientModule>();
    m_pNetModule = pPluginManager->FindModule<NFINetModule>();
    m_pLogModule = pPluginManager->FindModule<NFILogModule>();

	

    Register();

	std::string strRootFile = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/NFScriptSystem.lua";

	
    std::cout << "Hello, world1, BeforeShut1111" << std::endl;
	return true;
}

bool NFTsScriptModule::Init()
{
    

    return true;
}

bool NFTsScriptModule::AfterInit()
{
	

    return true;
}

bool NFTsScriptModule::Shut()
{
	TRY_RUN_GLOBAL_SCRIPT_FUN0("module_shut");

    return true;
}

bool NFTsScriptModule::ReadyExecute()
{
	
	return true;
}

bool NFTsScriptModule::Execute()
{
    if (pPluginManager->GetNowTime() - mnTime > 10)
    {
        mnTime = pPluginManager->GetNowTime();

        OnScriptReload();
    }

    return true;
}

bool NFTsScriptModule::BeforeShut()
{
    

    return true;
}

void NFTsScriptModule::RegisterModule(const std::string & tableName, const LuaIntf::LuaRef & luaTable)
{
	mxTableName[tableName] = luaTable;
}

NFGUID NFTsScriptModule::CreateObject(const NFGUID & self, const int sceneID, const int groupID, const std::string & className, const std::string & objectIndex, const NFDataList & arg)
{
	NF_SHARE_PTR<NFIObject> xObject = m_pKernelModule->CreateObject(self, sceneID, groupID, className, objectIndex, arg);
	if (xObject)
	{
		return xObject->Self();

	}

	return NFGUID();
}

bool NFTsScriptModule::ExistObject(const NFGUID & self)
{
	return m_pKernelModule->ExistObject(self);
}

bool NFTsScriptModule::DestroyObject(const NFGUID & self)
{
	return m_pKernelModule->DestroyObject(self);
}

bool NFTsScriptModule::EnterScene(const int sceneID, const int groupID)
{
	return false;
}

bool NFTsScriptModule::DoEvent(const NFGUID & self, const int eventID, const NFDataList & arg)
{
	m_pEventModule->DoEvent(self, (int)eventID, arg);

	return true;
}

bool NFTsScriptModule::FindProperty(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->FindProperty(self, propertyName);
}

bool NFTsScriptModule::SetPropertyInt(const NFGUID & self, const std::string & propertyName, const NFINT64 propValue)
{
	return m_pKernelModule->SetPropertyInt(self, propertyName, propValue);
}

bool NFTsScriptModule::SetPropertyFloat(const NFGUID & self, const std::string & propertyName, const double propValue)
{
	return m_pKernelModule->SetPropertyFloat(self, propertyName, propValue);
}

bool NFTsScriptModule::SetPropertyString(const NFGUID & self, const std::string & propertyName, const std::string & propValue)
{
	return m_pKernelModule->SetPropertyString(self, propertyName, propValue);
}

bool NFTsScriptModule::SetPropertyObject(const NFGUID & self, const std::string & propertyName, const NFGUID & propValue)
{
	return m_pKernelModule->SetPropertyObject(self, propertyName, propValue);
}

bool NFTsScriptModule::SetPropertyVector2(const NFGUID & self, const std::string & propertyName, const NFVector2 & propValue)
{
	return m_pKernelModule->SetPropertyVector2(self, propertyName, propValue);
}

bool NFTsScriptModule::SetPropertyVector3(const NFGUID & self, const std::string & propertyName, const NFVector3 & propValue)
{
	return m_pKernelModule->SetPropertyVector3(self, propertyName, propValue);
}

NFINT64 NFTsScriptModule::GetPropertyInt(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyInt(self, propertyName);
}

int NFTsScriptModule::GetPropertyInt32(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyInt32(self, propertyName);
}

double NFTsScriptModule::GetPropertyFloat(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyFloat(self, propertyName);
}

std::string NFTsScriptModule::GetPropertyString(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyString(self, propertyName);
}

NFGUID NFTsScriptModule::GetPropertyObject(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyObject(self, propertyName);
}

NFVector2 NFTsScriptModule::GetPropertyVector2(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyVector2(self, propertyName);
}

NFVector3 NFTsScriptModule::GetPropertyVector3(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyVector3(self, propertyName);
}

bool NFTsScriptModule::AddClassCallBack(std::string& className, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc)
{
	auto funcNameList = mxClassEventFuncMap.GetElement(className);
	if (!funcNameList)
	{
		funcNameList = new NFList<string>();
		mxClassEventFuncMap.AddElement(className, funcNameList);

		m_pKernelModule->AddClassCallBack(className, this, &NFTsScriptModule::OnClassEventCB);
	}
	
	std::string strfuncName = FindFuncName(luaTable, luaFunc);
	if (!strfuncName.empty())
	{
		if (!funcNameList->Find(strfuncName))
		{
			funcNameList->Add(strfuncName);

			return true;
		}
	}

	return false;
}

int NFTsScriptModule::OnClassEventCB(const NFGUID& objectId, const std::string& className, const CLASS_OBJECT_EVENT classEvent, const NFDataList& var)
{
    auto funcNameList = mxClassEventFuncMap.GetElement(className);
    if (funcNameList)
    {
		std::string strFuncNme;
		bool ret = funcNameList->First(strFuncNme);
		while (ret)
		{
			try
			{
				LuaIntf::LuaRef func(mLuaContext, strFuncNme.c_str());
				func.call("", objectId, className, (int)classEvent, (NFDataList)var);
			}
			catch (LuaIntf::LuaException& e)
			{
				cout << e.what() << endl;
				return 0;
			}
			catch (...)
			{
				return 0;
			}

			ret = funcNameList->Next(strFuncNme);
		}
    }

	return -1;
}

void NFTsScriptModule::OnScriptReload()
{
    NFINT64 nAppType = APPType();
    std::string strRootFile = "";
    switch ((NF_SERVER_TYPES)(nAppType))
    {
        case NF_SERVER_TYPES::NF_ST_GAME:
        {
			strRootFile = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/game/game_script_reload.lua";
        }
        break;
        case NF_SERVER_TYPES::NF_ST_LOGIN:
        {
			strRootFile = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/login/login_script_reload.lua";
        }
        break;
        case NF_SERVER_TYPES::NF_ST_WORLD:
        {
			strRootFile = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/world/world_script_reload.lua";
        }
        break;
        case NF_SERVER_TYPES::NF_ST_PROXY:
        {
			strRootFile = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/proxy/proxy_script_reload.lua";
        }
        break;
        case NF_SERVER_TYPES::NF_ST_MASTER:
        {
			strRootFile = pPluginManager->GetConfigPath() + "NFDataCfg/ScriptModule/master/master_script_reload.lua";
        }
        break;
        default:
        break;
    }
    
    if (!strRootFile.empty())
    {
		TRY_LOAD_SCRIPT_FLE(strRootFile.c_str());
    }
}

bool NFTsScriptModule::AddPropertyCallBack(const NFGUID& self, std::string& propertyName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaPropertyCallBackFuncMap, self, propertyName, luaFuncName))
		{
			m_pKernelModule->AddPropertyCallBack(self, propertyName, this, &NFTsScriptModule::OnLuaPropertyCB);
		}

		return true;
	}
    return false;
}

int NFTsScriptModule::OnLuaPropertyCB(const NFGUID& self, const std::string& propertyName, const NFData& oldVar, const NFData& newVar, const NFINT64 reason)
{
	auto funcList = mxLuaPropertyCallBackFuncMap.GetElement(propertyName);
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
					func.call("", self, propertyName, oldVar, newVar);
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

bool NFTsScriptModule::AddRecordCallBack(const NFGUID& self, std::string& recordName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaRecordCallBackFuncMap, self, recordName, luaFuncName))
		{
			m_pKernelModule->AddRecordCallBack(self, recordName, this, &NFTsScriptModule::OnLuaRecordCB);
		}
		return true;
	}

	return false;
}

int NFTsScriptModule::OnLuaRecordCB(const NFGUID& self, const RECORD_EVENT_DATA& eventData, const NFData& oldVar, const NFData& newVar)
{
	auto funcList = mxLuaRecordCallBackFuncMap.GetElement(eventData.recordName);
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
					func.call<LuaIntf::LuaRef>("", self, eventData.recordName, eventData.nOpType, eventData.row, eventData.col, oldVar, newVar);
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

bool NFTsScriptModule::AddEventCallBack(const NFGUID& self, const int eventID, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaEventCallBackFuncMap, self, (int)eventID, luaFuncName))
		{
			m_pEventModule->AddEventCallBack(self, eventID, this, &NFTsScriptModule::OnLuaEventCB);
		}

		return true;
	}

	return false;
}

int NFTsScriptModule::OnLuaEventCB(const NFGUID& self, const int eventID, const NFDataList& argVar)
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

bool NFTsScriptModule::AddModuleSchedule(std::string& strHeartBeatName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc, const float time, const int count)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaHeartBeatCallBackFuncMap, strHeartBeatName, luaFuncName))
		{
			return m_pScheduleModule->AddSchedule(NFGUID(), strHeartBeatName, this, &NFTsScriptModule::OnLuaHeartBeatCB, time, count);
		}
	}

	return false;
}

bool NFTsScriptModule::AddSchedule(const NFGUID& self, std::string& strHeartBeatName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc, const float time, const int count)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaHeartBeatCallBackFuncMap, self, strHeartBeatName, luaFuncName))
		{
			m_pScheduleModule->AddSchedule(self, strHeartBeatName, this, &NFTsScriptModule::OnLuaHeartBeatCB, time, count);
		}

		return true;
	}

	return false;
}

int NFTsScriptModule::OnLuaHeartBeatCB(const NFGUID& self, const std::string& strHeartBeatName, const float time, const int count)
{

	auto funcList = mxLuaHeartBeatCallBackFuncMap.GetElement(strHeartBeatName);
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
					if (self.IsNull())
					{
						func.call<LuaIntf::LuaRef>("", strHeartBeatName, time, count);
					}
					else
					{
						func.call<LuaIntf::LuaRef>("", self, strHeartBeatName, time, count);
					}
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

int NFTsScriptModule::AddRow(const NFGUID& self, std::string& recordName, const NFDataList& var)
{
    NF_SHARE_PTR<NFIRecord> pRecord = m_pKernelModule->FindRecord(self, recordName);
    if (nullptr == pRecord)
    {
        return -1;
    }

    return pRecord->AddRow(-1, var);
}

bool NFTsScriptModule::RemRow(const NFGUID & self, std::string & recordName, const int row)
{
    NF_SHARE_PTR<NFIRecord> pRecord = m_pKernelModule->FindRecord(self, recordName);
    if (nullptr == pRecord)
    {
        return false;
    }

    return pRecord->Remove(row);
}

bool NFTsScriptModule::SetRecordInt(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const NFINT64 value)
{
	return m_pKernelModule->SetRecordInt(self, recordName, row, colTag, value);
}

bool NFTsScriptModule::SetRecordFloat(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const double value)
{
	return m_pKernelModule->SetRecordFloat(self, recordName, row, colTag, value);
}

bool NFTsScriptModule::SetRecordString(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const std::string & value)
{
	return m_pKernelModule->SetRecordString(self, recordName, row, colTag, value);
}

bool NFTsScriptModule::SetRecordObject(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const NFGUID & value)
{
	return m_pKernelModule->SetRecordObject(self, recordName, row, colTag, value);
}

bool NFTsScriptModule::SetRecordVector2(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const NFVector2 & value)
{
	return m_pKernelModule->SetRecordVector2(self, recordName, row, colTag, value);
}

bool NFTsScriptModule::SetRecordVector3(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const NFVector3 & value)
{
	return m_pKernelModule->SetRecordVector3(self, recordName, row, colTag, value);
}

NFINT64 NFTsScriptModule::GetRecordInt(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordInt(self, recordName, row, colTag);
}

double NFTsScriptModule::GetRecordFloat(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordFloat(self, recordName, row, colTag);
}

std::string NFTsScriptModule::GetRecordString(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordString(self, recordName, row, colTag);
}

NFGUID NFTsScriptModule::GetRecordObject(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordObject(self, recordName, row, colTag);
}

NFVector2 NFTsScriptModule::GetRecordVector2(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordVector2(self, recordName, row, colTag);
}

NFVector3 NFTsScriptModule::GetRecordVector3(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordVector3(self, recordName, row, colTag);
}

NFINT64 NFTsScriptModule::GetNowTime()
{
	return pPluginManager->GetNowTime();
}

NFGUID NFTsScriptModule::CreateId()
{
	return m_pKernelModule->CreateGUID();
}

NFINT64 NFTsScriptModule::APPId()
{
	return pPluginManager->GetAppID();
}

NFINT64 NFTsScriptModule::APPType()
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

bool NFTsScriptModule::ExistElementObject(const std::string & configName)
{
	return m_pElementModule->ExistElement(configName);
}

std::vector<std::string> NFTsScriptModule::GetEleList(const std::string& className)
{
    NF_SHARE_PTR<NFIClass> xLogicClass = m_pClassModule->GetElement(NFrame::Server::ThisName());
	if (xLogicClass)
	{
		return xLogicClass->GetIDList();
    }

    return std::vector<std::string>();
}

NFINT64 NFTsScriptModule::GetElePropertyInt(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyInt(configName, propertyName);
}

double NFTsScriptModule::GetElePropertyFloat(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyFloat(configName, propertyName);
}

std::string NFTsScriptModule::GetElePropertyString(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyString(configName, propertyName);
}

NFVector2 NFTsScriptModule::GetElePropertyVector2(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyVector2(configName, propertyName);
}

NFVector3 NFTsScriptModule::GetElePropertyVector3(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyVector3(configName, propertyName);
}

template<typename T>
bool NFTsScriptModule::AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, const NFGUID& self, T key, string& luaFunc)
{
    auto funcList = funcMap.GetElement(key);
    if (!funcList)
    {
        NFList<string>* funcNameList = new NFList<string>;
        funcNameList->Add(luaFunc);
        funcList = new NFMap<NFGUID, NFList<string>>;
        funcList->AddElement(self, funcNameList);
        funcMap.AddElement(key, funcList);
        return true;
    }

    if (!funcList->GetElement(self))
    {
        NFList<string>* funcNameList = new NFList<string>;
        funcNameList->Add(luaFunc);
        funcList->AddElement(self, funcNameList);
        return true;
    }
    else
    {
        auto funcNameList = funcList->GetElement(self);
        if (!funcNameList->Find(luaFunc))
        {
            funcNameList->Add(luaFunc);
            return true;
        }
        else
        {
            return false;
        }
    }

}


template<typename T>
bool NFTsScriptModule::AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, T key, string& luaFunc)
{
    auto funcList = funcMap.GetElement(key);
    if (!funcList)
    {
        NFList<string>* funcNameList = new NFList<string>;
        funcNameList->Add(luaFunc);
        funcList = new NFMap<NFGUID, NFList<string>>;
        funcList->AddElement(NFGUID(), funcNameList);
        funcMap.AddElement(key, funcList);
        return true;
    }

    if (!funcList->GetElement(NFGUID()))
    {
        NFList<string>* funcNameList = new NFList<string>;
        funcNameList->Add(luaFunc);
        funcList->AddElement(NFGUID(), funcNameList);
        return true;
    }
    else
    {
        auto funcNameList = funcList->GetElement(NFGUID());
        if (!funcNameList->Find(luaFunc))
        {
            funcNameList->Add(luaFunc);
            return true;
        }
        else
        {
            return false;
        }
    }

}

void NFTsScriptModule::RemoveCallBackAsServer(const int msgID)
{
	m_pNetModule->RemoveReceiveCallBack(msgID);
}

void NFTsScriptModule::AddMsgCallBackAsServer(const int msgID, const LuaIntf::LuaRef & luaTable, const LuaIntf::LuaRef & luaFunc)
{
	auto funcNameList = mxNetMsgCallBackFuncMapAsServer.GetElement(msgID);
	if (!funcNameList)
	{
		funcNameList = new NFList<string>();
		mxNetMsgCallBackFuncMapAsServer.AddElement(msgID, funcNameList);

		m_pNetModule->AddReceiveCallBack(msgID, this, &NFTsScriptModule::OnNetMsgCallBackAsServer);
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

void NFTsScriptModule::RemoveMsgCallBackAsClient(const NF_SERVER_TYPES serverType, const int msgID)
{
	m_pNetClientModule->RemoveReceiveCallBack(serverType, msgID);
}

void NFTsScriptModule::AddMsgCallBackAsClient(const NF_SERVER_TYPES serverType, const int msgID, const LuaIntf::LuaRef & luaTable, const LuaIntf::LuaRef & luaFunc)
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
				m_pNetClientModule->AddReceiveCallBack(serverType, msgID, this, &NFTsScriptModule::OnNetMsgCallBackAsClientForMasterServer);
				break;
			case NF_SERVER_TYPES::NF_ST_WORLD:
				m_pNetClientModule->AddReceiveCallBack(serverType, msgID, this, &NFTsScriptModule::OnNetMsgCallBackAsClientForWorldServer);
				break;
			case NF_SERVER_TYPES::NF_ST_GAME:
				m_pNetClientModule->AddReceiveCallBack(serverType, msgID, this, &NFTsScriptModule::OnNetMsgCallBackAsClientForGameServer);
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

/*
void NFTsScriptModule::RemoveHttpCallBack(const std::string & path)
{
}

void NFTsScriptModule::AddHttpCallBack(const std::string & path, const int httpType, const LuaIntf::LuaRef & luaTable, const LuaIntf::LuaRef & luaFunc)
{
}
*/

void NFTsScriptModule::ImportProtoFile(const std::string& fileName)
{
	NFLuaPBModule* p = (NFLuaPBModule*)m_pLuaPBModule;
	p->ImportProtoFile(fileName);
}

const std::string NFTsScriptModule::Encode(const std::string& msgTypeName, const LuaIntf::LuaRef& luaTable)
{
	NFLuaPBModule* p = (NFLuaPBModule*)m_pLuaPBModule;
	return p->Encode(msgTypeName, luaTable);
}

LuaIntf::LuaRef NFTsScriptModule::Decode(const std::string& msgTypeName, const std::string& data)
{
	NFLuaPBModule* p = (NFLuaPBModule*)m_pLuaPBModule;
	return p->Decode(msgTypeName, data);
}

void NFTsScriptModule::SendToServerByServerID(const int serverID, const uint16_t msgID, const std::string& data)
{
    if (pPluginManager->GetAppID() == serverID)
    {
        m_pLogModule->LogError("you can send message to yourself");
        return;
    }

	m_pNetClientModule->SendByServerID(serverID, msgID, data);
}

void NFTsScriptModule::SendToServerBySuit(const NF_SERVER_TYPES eType, const uint16_t msgID, const std::string & data, const std::string& hash)
{
	m_pNetClientModule->SendBySuitWithOutHead(eType, hash, msgID,data );
}

void NFTsScriptModule::SendToAllServerByServerType(const NF_SERVER_TYPES eType, const uint16_t msgID, const std::string &data)
{
	m_pNetClientModule->SendToAllServer(eType, msgID,data );
}

void NFTsScriptModule::SendMsgToClientByFD(const NFSOCK fd, const uint16_t msgID, const std::string &data)
{
	//for all servers
	m_pNetModule->SendMsgWithOutHead(msgID, data, fd);
}

void NFTsScriptModule::SendMsgToPlayer(const NFGUID player, const uint16_t msgID, const std::string& data)
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

void NFTsScriptModule::SendToGroupPlayer(const uint16_t msgID, const std::string& data)
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

void NFTsScriptModule::SendToAllPlayer(const uint16_t msgID, const std::string& data)
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

void NFTsScriptModule::LogInfo(const std::string& logData)
{
	m_pLogModule->LogInfo(logData);
}

void NFTsScriptModule::LogError(const std::string& logData)
{
	m_pLogModule->LogError(logData);
}

void NFTsScriptModule::LogWarning(const std::string& logData)
{
	m_pLogModule->LogWarning(logData);
}

void NFTsScriptModule::LogDebug(const std::string& logData)
{
	m_pLogModule->LogDebug(logData);
}

void NFTsScriptModule::SetVersionCode(const std::string& logData)
{
    strVersionCode = logData;
}

const std::string&  NFTsScriptModule::GetVersionCode()
{
    return strVersionCode;
}

bool NFTsScriptModule::Register()
{

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFGUID>("NFGUID")
		.addConstructor(LUA_ARGS())
		.addProperty("data", &NFGUID::GetData, &NFGUID::SetData)
		.addProperty("head", &NFGUID::GetHead, &NFGUID::SetHead)
		.addFunction("tostring", &NFGUID::ToString)
		.addFunction("fromstring", &NFGUID::FromString)
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFDataList>("NFDataList")
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginExtendClass<NFDataList, NFDataList>("NFDataList")
		.addConstructor(LUA_ARGS())
		.addFunction("empty", &NFDataList::IsEmpty)
		.addFunction("count", &NFDataList::GetCount)
		.addFunction("tye", &NFDataList::Type)

		.addFunction("add_int", &NFDataList::AddInt)
		.addFunction("add_float", &NFDataList::AddFloat)
		.addFunction("add_string", &NFDataList::AddStringFromChar)
		.addFunction("add_object", &NFDataList::AddObject)
		.addFunction("add_vector2", &NFDataList::AddVector2)
		.addFunction("add_vector3", &NFDataList::AddVector3)

		.addFunction("set_int", &NFDataList::SetInt)
		.addFunction("set_float", &NFDataList::SetFloat)
		.addFunction("set_string", &NFDataList::SetString)
		.addFunction("set_object", &NFDataList::SetObject)
		.addFunction("set_vector2", &NFDataList::SetVector2)
		.addFunction("set_vector3", &NFDataList::SetVector3)

		.addFunction("int", &NFDataList::Int)
		.addFunction("float", &NFDataList::Float)
		.addFunction("string", &NFDataList::String)
		.addFunction("object", &NFDataList::Object)
		.addFunction("vector2", &NFDataList::Vector2)
		.addFunction("vector3", &NFDataList::Vector3)
		.endClass();

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFData>("TData")
		.addConstructor(LUA_ARGS())
		.addFunction("float", &NFData::GetFloat)
		.addFunction("int", &NFData::GetInt)
		.addFunction("object", &NFData::GetObject)
		.addFunction("string", &NFData::GetString)
		.addFunction("vector2", &NFData::GetVector2)
		.addFunction("vector3", &NFData::GetVector3)

		.addFunction("type", &NFData::GetType)
		.addFunction("is_null", &NFData::IsNullValue)

		.addFunction("set_float", &NFData::SetFloat)
		.addFunction("set_int", &NFData::SetInt)
		.addFunction("set_object", &NFData::SetObject)
		.addFunction("set_string", &NFData::SetString)
		.addFunction("set_vector2", &NFData::SetVector2)
		.addFunction("set_vector3", &NFData::SetVector3)
		.endClass();
	//for kernel module

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFTsScriptModule>("NFTsScriptModule")
		.addFunction("register_module", &NFTsScriptModule::RegisterModule)
		.addFunction("create_object", &NFTsScriptModule::CreateObject)
		.addFunction("exist_object", &NFTsScriptModule::ExistObject)
		.addFunction("destroy_object", &NFTsScriptModule::DestroyObject)
		.addFunction("enter_scene", &NFTsScriptModule::EnterScene)
		.addFunction("do_event", &NFTsScriptModule::DoEvent)

		.addFunction("set_prop_int", &NFTsScriptModule::SetPropertyInt)
		.addFunction("set_prop_float", &NFTsScriptModule::SetPropertyFloat)
		.addFunction("set_prop_string", &NFTsScriptModule::SetPropertyString)
		.addFunction("set_prop_object", &NFTsScriptModule::SetPropertyObject)
		.addFunction("set_prop_vector2", &NFTsScriptModule::SetPropertyVector2)
		.addFunction("set_prop_vector3", &NFTsScriptModule::SetPropertyVector3)


		.addFunction("get_prop_int", &NFTsScriptModule::GetPropertyInt)
		.addFunction("get_prop_float", &NFTsScriptModule::GetPropertyFloat)
		.addFunction("get_prop_string", &NFTsScriptModule::GetPropertyString)
		.addFunction("get_prop_object", &NFTsScriptModule::GetPropertyObject)
		.addFunction("get_prop_vector2", &NFTsScriptModule::GetPropertyVector2)
		.addFunction("get_prop_vector3", &NFTsScriptModule::GetPropertyVector3)

		.addFunction("set_record_int", &NFTsScriptModule::SetRecordInt)
		.addFunction("set_record_float",&NFTsScriptModule::SetRecordFloat)
		.addFunction("set_record_string", &NFTsScriptModule::SetRecordString)
		.addFunction("set_record_object", &NFTsScriptModule::SetRecordObject)
		.addFunction("set_record_vector2", &NFTsScriptModule::SetPropertyVector2)
		.addFunction("set_record_vector3", &NFTsScriptModule::SetPropertyVector3)

		.addFunction("get_record_int", &NFTsScriptModule::GetRecordInt)
		.addFunction("get_record_float", &NFTsScriptModule::GetRecordFloat)
		.addFunction("get_record_string", &NFTsScriptModule::GetRecordString)
		.addFunction("get_record_object", &NFTsScriptModule::GetRecordObject)
		.addFunction("get_record_vector2", &NFTsScriptModule::GetPropertyVector2)
		.addFunction("get_record_vector3", &NFTsScriptModule::GetPropertyVector3)

		.addFunction("add_prop_cb", &NFTsScriptModule::AddPropertyCallBack)
		.addFunction("add_record_cb", &NFTsScriptModule::AddRecordCallBack)
		.addFunction("add_event_cb", &NFTsScriptModule::AddEventCallBack)
		.addFunction("add_class_cb", &NFTsScriptModule::AddClassCallBack)
		.addFunction("add_schedule", &NFTsScriptModule::AddSchedule)
		.addFunction("add_module_schedule", &NFTsScriptModule::AddModuleSchedule)
		.addFunction("do_event", &NFTsScriptModule::DoEvent)
		.addFunction("add_row", &NFTsScriptModule::AddRow)
		.addFunction("rem_row", &NFTsScriptModule::RemRow)

		.addFunction("time", &NFTsScriptModule::GetNowTime)
		.addFunction("new_id", &NFTsScriptModule::CreateId)
		.addFunction("app_id", &NFTsScriptModule::APPId)
		.addFunction("app_type", &NFTsScriptModule::APPType)

		.addFunction("exist_ele", &NFTsScriptModule::ExistElementObject)
		.addFunction("get_ele_list", &NFTsScriptModule::GetEleList)
		.addFunction("get_ele_int", &NFTsScriptModule::GetElePropertyInt)
		.addFunction("get_ele_float", &NFTsScriptModule::GetElePropertyFloat)
		.addFunction("get_ele_string", &NFTsScriptModule::GetElePropertyString)
		.addFunction("get_ele_vector2", &NFTsScriptModule::GetElePropertyVector2)
		.addFunction("get_ele_vector3", &NFTsScriptModule::GetElePropertyVector3)

		.addFunction("remove_msg_cb_as_server", &NFTsScriptModule::RemoveCallBackAsServer)//as server
		.addFunction("add_msg_cb_as_server", &NFTsScriptModule::AddMsgCallBackAsServer)//as server
		.addFunction("remove_msg_cb_as_client", &NFTsScriptModule::RemoveMsgCallBackAsClient)//as client
		.addFunction("add_msg_cb_as_client", &NFTsScriptModule::AddMsgCallBackAsClient)//as client

		//.addFunction("remove_http_cb", &NFTsScriptModule::RemoveHttpCallBack)
		//.addFunction("add_http_cb", &NFTsScriptModule::AddHttpCallBack)

		.addFunction("send_to_server_by_id", &NFTsScriptModule::SendToServerByServerID)//as client
		.addFunction("send_to_all_server_by_type", &NFTsScriptModule::SendToAllServerByServerType)//as client
		.addFunction("send_to_server_by_suit", &NFTsScriptModule::SendToServerBySuit)//as client

		.addFunction("send_to_client_by_fd", &NFTsScriptModule::SendMsgToClientByFD)//as server

		.addFunction("send_to_player", &NFTsScriptModule::SendMsgToPlayer)//as game server
		.addFunction("send_to_group_player", &NFTsScriptModule::SendToGroupPlayer)//as game server
		.addFunction("send_to_all_player", &NFTsScriptModule::SendToAllPlayer)//as game server

		.addFunction("log_info", &NFTsScriptModule::LogInfo)
		.addFunction("log_error", &NFTsScriptModule::LogError)
		.addFunction("log_warning", &NFTsScriptModule::LogWarning)
		.addFunction("log_debug", &NFTsScriptModule::LogDebug)

		.addFunction("get_version_code", &NFTsScriptModule::GetVersionCode)
		.addFunction("set_version_code", &NFTsScriptModule::SetVersionCode)

		.addFunction("import_proto_file", &NFTsScriptModule::ImportProtoFile)
		.addFunction("encode", &NFTsScriptModule::Encode)
		.addFunction("decode", &NFTsScriptModule::Decode)

		.endClass();

    return true;
}

std::string NFTsScriptModule::FindFuncName(const LuaIntf::LuaRef & luaTable, const LuaIntf::LuaRef & luaFunc)
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

void NFTsScriptModule::OnNetMsgCallBackAsServer(const NFSOCK sockIndex, const int msgID, const char *msg, const uint32_t len)
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

void NFTsScriptModule::OnNetMsgCallBackAsClientForMasterServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len)
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

void NFTsScriptModule::OnNetMsgCallBackAsClientForWorldServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len)
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

void NFTsScriptModule::OnNetMsgCallBackAsClientForGameServer(const NFSOCK sockIndex, const int msgID, const char* msg, const uint32_t len)
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