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
#include "NFJsScriptModule.h"
#include "NFJsScriptPlugin.h"
#include "NFComm/NFPluginModule/NFIKernelModule.h"
#include "Dependencies/puerts/include/Binding.hpp" 
#include "Dependencies/puerts/include/CppObjectMapper.h"

#define TRY_RUN_GLOBAL_SCRIPT_FUN0(strFuncName)   try {LuaIntf::LuaRef func(mLuaContext, strFuncName);  func.call<LuaIntf::LuaRef>(); }   catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }
#define TRY_RUN_GLOBAL_SCRIPT_FUN1(strFuncName, arg1)  try {LuaIntf::LuaRef func(mLuaContext, strFuncName);  func.call<LuaIntf::LuaRef>(arg1); }catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }
#define TRY_RUN_GLOBAL_SCRIPT_FUN2(strFuncName, arg1, arg2)  try {LuaIntf::LuaRef func(mLuaContext, strFuncName);  func.call<LuaIntf::LuaRef>(arg1, arg2); }catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }

#define TRY_LOAD_SCRIPT_FLE(fileName)  try{mLuaContext.doFile(fileName);} catch (LuaIntf::LuaException& e) { cout << e.what() << endl; }

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
    m_pLogModule = pPluginManager->FindModule<NFILogModule>();

	

    Register();

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
	TRY_RUN_GLOBAL_SCRIPT_FUN0("module_shut");

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

void NFJsScriptModule::RegisterModule(const std::string & tableName, const LuaIntf::LuaRef & luaTable)
{
	mxTableName[tableName] = luaTable;
}

NFGUID NFJsScriptModule::CreateObject(const NFGUID & self, const int sceneID, const int groupID, const std::string & className, const std::string & objectIndex, const NFDataList & arg)
{
	NF_SHARE_PTR<NFIObject> xObject = m_pKernelModule->CreateObject(self, sceneID, groupID, className, objectIndex, arg);
	if (xObject)
	{
		return xObject->Self();

	}

	return NFGUID();
}

bool NFJsScriptModule::ExistObject(const NFGUID & self)
{
	return m_pKernelModule->ExistObject(self);
}

bool NFJsScriptModule::DestroyObject(const NFGUID & self)
{
	return m_pKernelModule->DestroyObject(self);
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

bool NFJsScriptModule::FindProperty(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->FindProperty(self, propertyName);
}

bool NFJsScriptModule::SetPropertyInt(const NFGUID & self, const std::string & propertyName, const NFINT64 propValue)
{
	return m_pKernelModule->SetPropertyInt(self, propertyName, propValue);
}

bool NFJsScriptModule::SetPropertyFloat(const NFGUID & self, const std::string & propertyName, const double propValue)
{
	return m_pKernelModule->SetPropertyFloat(self, propertyName, propValue);
}

bool NFJsScriptModule::SetPropertyString(const NFGUID & self, const std::string & propertyName, const std::string & propValue)
{
	return m_pKernelModule->SetPropertyString(self, propertyName, propValue);
}

bool NFJsScriptModule::SetPropertyObject(const NFGUID & self, const std::string & propertyName, const NFGUID & propValue)
{
	return m_pKernelModule->SetPropertyObject(self, propertyName, propValue);
}

bool NFJsScriptModule::SetPropertyVector2(const NFGUID & self, const std::string & propertyName, const NFVector2 & propValue)
{
	return m_pKernelModule->SetPropertyVector2(self, propertyName, propValue);
}

bool NFJsScriptModule::SetPropertyVector3(const NFGUID & self, const std::string & propertyName, const NFVector3 & propValue)
{
	return m_pKernelModule->SetPropertyVector3(self, propertyName, propValue);
}

NFINT64 NFJsScriptModule::GetPropertyInt(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyInt(self, propertyName);
}

int NFJsScriptModule::GetPropertyInt32(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyInt32(self, propertyName);
}

double NFJsScriptModule::GetPropertyFloat(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyFloat(self, propertyName);
}

std::string NFJsScriptModule::GetPropertyString(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyString(self, propertyName);
}

NFGUID NFJsScriptModule::GetPropertyObject(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyObject(self, propertyName);
}

NFVector2 NFJsScriptModule::GetPropertyVector2(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyVector2(self, propertyName);
}

NFVector3 NFJsScriptModule::GetPropertyVector3(const NFGUID & self, const std::string & propertyName)
{
	return m_pKernelModule->GetPropertyVector3(self, propertyName);
}

bool NFJsScriptModule::AddClassCallBack(std::string& className, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc)
{
	auto funcNameList = mxClassEventFuncMap.GetElement(className);
	if (!funcNameList)
	{
		funcNameList = new NFList<string>();
		mxClassEventFuncMap.AddElement(className, funcNameList);

		m_pKernelModule->AddClassCallBack(className, this, &NFJsScriptModule::OnClassEventCB);
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

int NFJsScriptModule::OnClassEventCB(const NFGUID& objectId, const std::string& className, const CLASS_OBJECT_EVENT classEvent, const NFDataList& var)
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

void NFJsScriptModule::OnScriptReload()
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

bool NFJsScriptModule::AddPropertyCallBack(const NFGUID& self, std::string& propertyName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaPropertyCallBackFuncMap, self, propertyName, luaFuncName))
		{
			m_pKernelModule->AddPropertyCallBack(self, propertyName, this, &NFJsScriptModule::OnLuaPropertyCB);
		}

		return true;
	}
    return false;
}

int NFJsScriptModule::OnLuaPropertyCB(const NFGUID& self, const std::string& propertyName, const NFData& oldVar, const NFData& newVar, const NFINT64 reason)
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

bool NFJsScriptModule::AddRecordCallBack(const NFGUID& self, std::string& recordName, const LuaIntf::LuaRef& luaTable, const LuaIntf::LuaRef& luaFunc)
{
	std::string luaFuncName = FindFuncName(luaTable, luaFunc);
	if (!luaFuncName.empty())
	{
		if (AddLuaFuncToMap(mxLuaRecordCallBackFuncMap, self, recordName, luaFuncName))
		{
			m_pKernelModule->AddRecordCallBack(self, recordName, this, &NFJsScriptModule::OnLuaRecordCB);
		}
		return true;
	}

	return false;
}

int NFJsScriptModule::OnLuaRecordCB(const NFGUID& self, const RECORD_EVENT_DATA& eventData, const NFData& oldVar, const NFData& newVar)
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
		if (AddLuaFuncToMap(mxLuaHeartBeatCallBackFuncMap, self, strHeartBeatName, luaFuncName))
		{
			m_pScheduleModule->AddSchedule(self, strHeartBeatName, this, &NFJsScriptModule::OnLuaHeartBeatCB, time, count);
		}

		return true;
	}

	return false;
}

int NFJsScriptModule::OnLuaHeartBeatCB(const NFGUID& self, const std::string& strHeartBeatName, const float time, const int count)
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

int NFJsScriptModule::AddRow(const NFGUID& self, std::string& recordName, const NFDataList& var)
{
    NF_SHARE_PTR<NFIRecord> pRecord = m_pKernelModule->FindRecord(self, recordName);
    if (nullptr == pRecord)
    {
        return -1;
    }

    return pRecord->AddRow(-1, var);
}

bool NFJsScriptModule::RemRow(const NFGUID & self, std::string & recordName, const int row)
{
    NF_SHARE_PTR<NFIRecord> pRecord = m_pKernelModule->FindRecord(self, recordName);
    if (nullptr == pRecord)
    {
        return false;
    }

    return pRecord->Remove(row);
}

bool NFJsScriptModule::SetRecordInt(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const NFINT64 value)
{
	return m_pKernelModule->SetRecordInt(self, recordName, row, colTag, value);
}

bool NFJsScriptModule::SetRecordFloat(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const double value)
{
	return m_pKernelModule->SetRecordFloat(self, recordName, row, colTag, value);
}

bool NFJsScriptModule::SetRecordString(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const std::string & value)
{
	return m_pKernelModule->SetRecordString(self, recordName, row, colTag, value);
}

bool NFJsScriptModule::SetRecordObject(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const NFGUID & value)
{
	return m_pKernelModule->SetRecordObject(self, recordName, row, colTag, value);
}

bool NFJsScriptModule::SetRecordVector2(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const NFVector2 & value)
{
	return m_pKernelModule->SetRecordVector2(self, recordName, row, colTag, value);
}

bool NFJsScriptModule::SetRecordVector3(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag, const NFVector3 & value)
{
	return m_pKernelModule->SetRecordVector3(self, recordName, row, colTag, value);
}

NFINT64 NFJsScriptModule::GetRecordInt(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordInt(self, recordName, row, colTag);
}

double NFJsScriptModule::GetRecordFloat(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordFloat(self, recordName, row, colTag);
}

std::string NFJsScriptModule::GetRecordString(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordString(self, recordName, row, colTag);
}

NFGUID NFJsScriptModule::GetRecordObject(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordObject(self, recordName, row, colTag);
}

NFVector2 NFJsScriptModule::GetRecordVector2(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordVector2(self, recordName, row, colTag);
}

NFVector3 NFJsScriptModule::GetRecordVector3(const NFGUID & self, const std::string & recordName, const int row, const std::string & colTag)
{
	return m_pKernelModule->GetRecordVector3(self, recordName, row, colTag);
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

NFINT64 NFJsScriptModule::GetElePropertyInt(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyInt(configName, propertyName);
}

double NFJsScriptModule::GetElePropertyFloat(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyFloat(configName, propertyName);
}

std::string NFJsScriptModule::GetElePropertyString(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyString(configName, propertyName);
}

NFVector2 NFJsScriptModule::GetElePropertyVector2(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyVector2(configName, propertyName);
}

NFVector3 NFJsScriptModule::GetElePropertyVector3(const std::string & configName, const std::string & propertyName)
{
	return m_pElementModule->GetPropertyVector3(configName, propertyName);
}

template<typename T>
bool NFJsScriptModule::AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, const NFGUID& self, T key, string& luaFunc)
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
bool NFJsScriptModule::AddLuaFuncToMap(NFMap<T, NFMap<NFGUID, NFList<string>>>& funcMap, T key, string& luaFunc)
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

/*
void NFJsScriptModule::RemoveHttpCallBack(const std::string & path)
{
}

void NFJsScriptModule::AddHttpCallBack(const std::string & path, const int httpType, const LuaIntf::LuaRef & luaTable, const LuaIntf::LuaRef & luaFunc)
{
}
*/

void NFJsScriptModule::ImportProtoFile(const std::string& fileName)
{
	NFJsPBModule* p = (NFJsPBModule*)m_pJsPBModule;
	p->ImportProtoFile(fileName);
}

const std::string NFJsScriptModule::Encode(const std::string& msgTypeName, const LuaIntf::LuaRef& luaTable)
{
	NFJsPBModule* p = (NFJsPBModule*)m_pJsPBModule;
	return p->Encode(msgTypeName, luaTable);
}

LuaIntf::LuaRef NFJsScriptModule::Decode(const std::string& msgTypeName, const std::string& data)
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

void NFJsScriptModule::LogInfo(const std::string& logData)
{
	m_pLogModule->LogInfo(logData);
}

void NFJsScriptModule::LogError(const std::string& logData)
{
	m_pLogModule->LogError(logData);
}

void NFJsScriptModule::LogWarning(const std::string& logData)
{
	m_pLogModule->LogWarning(logData);
}

void NFJsScriptModule::LogDebug(const std::string& logData)
{
	m_pLogModule->LogDebug(logData);
}

void NFJsScriptModule::SetVersionCode(const std::string& logData)
{
    strVersionCode = logData;
}

const std::string&  NFJsScriptModule::GetVersionCode()
{
    return strVersionCode;
}

bool NFJsScriptModule::Register()
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

	LuaIntf::LuaBinding(mLuaContext).beginClass<NFJsScriptModule>("NFJsScriptModule")
		.addFunction("register_module", &NFJsScriptModule::RegisterModule)
		.addFunction("create_object", &NFJsScriptModule::CreateObject)
		.addFunction("exist_object", &NFJsScriptModule::ExistObject)
		.addFunction("destroy_object", &NFJsScriptModule::DestroyObject)
		.addFunction("enter_scene", &NFJsScriptModule::EnterScene)
		.addFunction("do_event", &NFJsScriptModule::DoEvent)

		.addFunction("set_prop_int", &NFJsScriptModule::SetPropertyInt)
		.addFunction("set_prop_float", &NFJsScriptModule::SetPropertyFloat)
		.addFunction("set_prop_string", &NFJsScriptModule::SetPropertyString)
		.addFunction("set_prop_object", &NFJsScriptModule::SetPropertyObject)
		.addFunction("set_prop_vector2", &NFJsScriptModule::SetPropertyVector2)
		.addFunction("set_prop_vector3", &NFJsScriptModule::SetPropertyVector3)


		.addFunction("get_prop_int", &NFJsScriptModule::GetPropertyInt)
		.addFunction("get_prop_float", &NFJsScriptModule::GetPropertyFloat)
		.addFunction("get_prop_string", &NFJsScriptModule::GetPropertyString)
		.addFunction("get_prop_object", &NFJsScriptModule::GetPropertyObject)
		.addFunction("get_prop_vector2", &NFJsScriptModule::GetPropertyVector2)
		.addFunction("get_prop_vector3", &NFJsScriptModule::GetPropertyVector3)

		.addFunction("set_record_int", &NFJsScriptModule::SetRecordInt)
		.addFunction("set_record_float",&NFJsScriptModule::SetRecordFloat)
		.addFunction("set_record_string", &NFJsScriptModule::SetRecordString)
		.addFunction("set_record_object", &NFJsScriptModule::SetRecordObject)
		.addFunction("set_record_vector2", &NFJsScriptModule::SetPropertyVector2)
		.addFunction("set_record_vector3", &NFJsScriptModule::SetPropertyVector3)

		.addFunction("get_record_int", &NFJsScriptModule::GetRecordInt)
		.addFunction("get_record_float", &NFJsScriptModule::GetRecordFloat)
		.addFunction("get_record_string", &NFJsScriptModule::GetRecordString)
		.addFunction("get_record_object", &NFJsScriptModule::GetRecordObject)
		.addFunction("get_record_vector2", &NFJsScriptModule::GetPropertyVector2)
		.addFunction("get_record_vector3", &NFJsScriptModule::GetPropertyVector3)

		.addFunction("add_prop_cb", &NFJsScriptModule::AddPropertyCallBack)
		.addFunction("add_record_cb", &NFJsScriptModule::AddRecordCallBack)
		.addFunction("add_event_cb", &NFJsScriptModule::AddEventCallBack)
		.addFunction("add_class_cb", &NFJsScriptModule::AddClassCallBack)
		.addFunction("add_schedule", &NFJsScriptModule::AddSchedule)
		.addFunction("add_module_schedule", &NFJsScriptModule::AddModuleSchedule)
		.addFunction("do_event", &NFJsScriptModule::DoEvent)
		.addFunction("add_row", &NFJsScriptModule::AddRow)
		.addFunction("rem_row", &NFJsScriptModule::RemRow)

		.addFunction("time", &NFJsScriptModule::GetNowTime)
		.addFunction("new_id", &NFJsScriptModule::CreateId)
		.addFunction("app_id", &NFJsScriptModule::APPId)
		.addFunction("app_type", &NFJsScriptModule::APPType)

		.addFunction("exist_ele", &NFJsScriptModule::ExistElementObject)
		.addFunction("get_ele_list", &NFJsScriptModule::GetEleList)
		.addFunction("get_ele_int", &NFJsScriptModule::GetElePropertyInt)
		.addFunction("get_ele_float", &NFJsScriptModule::GetElePropertyFloat)
		.addFunction("get_ele_string", &NFJsScriptModule::GetElePropertyString)
		.addFunction("get_ele_vector2", &NFJsScriptModule::GetElePropertyVector2)
		.addFunction("get_ele_vector3", &NFJsScriptModule::GetElePropertyVector3)

		.addFunction("remove_msg_cb_as_server", &NFJsScriptModule::RemoveCallBackAsServer)//as server
		.addFunction("add_msg_cb_as_server", &NFJsScriptModule::AddMsgCallBackAsServer)//as server
		.addFunction("remove_msg_cb_as_client", &NFJsScriptModule::RemoveMsgCallBackAsClient)//as client
		.addFunction("add_msg_cb_as_client", &NFJsScriptModule::AddMsgCallBackAsClient)//as client

		//.addFunction("remove_http_cb", &NFJsScriptModule::RemoveHttpCallBack)
		//.addFunction("add_http_cb", &NFJsScriptModule::AddHttpCallBack)

		.addFunction("send_to_server_by_id", &NFJsScriptModule::SendToServerByServerID)//as client
		.addFunction("send_to_all_server_by_type", &NFJsScriptModule::SendToAllServerByServerType)//as client
		.addFunction("send_to_server_by_suit", &NFJsScriptModule::SendToServerBySuit)//as client

		.addFunction("send_to_client_by_fd", &NFJsScriptModule::SendMsgToClientByFD)//as server

		.addFunction("send_to_player", &NFJsScriptModule::SendMsgToPlayer)//as game server
		.addFunction("send_to_group_player", &NFJsScriptModule::SendToGroupPlayer)//as game server
		.addFunction("send_to_all_player", &NFJsScriptModule::SendToAllPlayer)//as game server

		.addFunction("log_info", &NFJsScriptModule::LogInfo)
		.addFunction("log_error", &NFJsScriptModule::LogError)
		.addFunction("log_warning", &NFJsScriptModule::LogWarning)
		.addFunction("log_debug", &NFJsScriptModule::LogDebug)

		.addFunction("get_version_code", &NFJsScriptModule::GetVersionCode)
		.addFunction("set_version_code", &NFJsScriptModule::SetVersionCode)

		.addFunction("import_proto_file", &NFJsScriptModule::ImportProtoFile)
		.addFunction("encode", &NFJsScriptModule::Encode)
		.addFunction("decode", &NFJsScriptModule::Decode)

		.endClass();

    return true;
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

NFJsScriptModule::FJsEnvImpl(const std::string& NFDataCfgPath,const std::string& ScriptRoot): 
                  FJsEnvImpl( std::make_shared<DefaultJSModuleLoader>(NFDataCfgPath,ScriptRoot), std::make_shared<NFLogModule>(), -1, nullptr, nullptr, nullptr){}

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

    NFJsScriptModule::FJsEnvImpl(std::shared_ptr<IJSModuleLoader> InModuleLoader, std::shared_ptr<NFILogModule> InLogger, int InDebugPort,std::function<void(const std::string&)> InOnSourceLoadedCallback, void* InExternalRuntime, void* InExternalContext)
    {
        Started = false;
        Inspector = nullptr;
        InspectorChannel = nullptr;

        ModuleLoader = std::move(InModuleLoader);
        Logger = InLogger;
        OnSourceLoadedCallback = InOnSourceLoadedCallback;

        int Argc = 1;
        const char* Argv[] = {"puerts"};
        std::vector<std::string> Args(Argv, Argv + Argc);
        std::vector<std::string> ExecArgs;
        std::vector<std::string> Errors;

  
        std::unique_ptr<MultiIsolatePlatform> platform =  MultiIsolatePlatform::Create(4);
        V8::InitializePlatform(platform.get());
        V8::Initialize();


       int exit_code = 0;
        std::vector<std::string> errors;
        std::unique_ptr<CommonEnvironmentSetup> setup = CommonEnvironmentSetup::Create(platform.get(), &errors, Args, ExecArgs);
        if (!setup) {
            for (const std::string& err : errors)
            fprintf(stderr, "%s: %s\n", args[0].c_str(), err.c_str());
            return ;
        }

        MainIsolate = setup->isolate();
        NodeEnv = setup->env();

        
        Locker locker(isolate);
        v8::Isolate::Scope isolate_scope(isolate);
        v8::HandleScope handle_scope(isolate);
        v8::Context::Scope context_scope(setup->context());

        MaybeLocal<Value> LoadenvRet = node::LoadEnvironment(
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

        UserObjectRetainer.Isolate = Isolate;
        SysObjectRetainer.Isolate = Isolate;
    #endif

        Isolate->SetData(0, static_cast<IObjectMapper*>(this));    //直接传this会有问题，强转后地址会变


        v8::Local<v8::Context> Context = setup->context();
        DefaultContext.Reset(Isolate, Context);

        v8::Context::Scope context_scope(Context);




        // the same as raw v8
        Isolate->SetMicrotasksPolicy(v8::MicrotasksPolicy::kAuto);


        v8::Local<v8::Object> Global = Context->Global();

        v8::Local<v8::Object> PuertsObj = v8::Object::New(Isolate);
        Global->Set(Context, FV8Utils::InternalString(Isolate, "puerts"), PuertsObj).Check();

        auto This = v8::External::New(Isolate, this);

        MethodBindingHelper<&FJsEnvImpl::EvalScript>::Bind(Isolate, Context, Global, "__tgjsEvalScript", This);

        MethodBindingHelper<&FJsEnvImpl::Log>::Bind(Isolate, Context, Global, "__tgjsLog", This);

        MethodBindingHelper<&FJsEnvImpl::SearchModule>::Bind(Isolate, Context, Global, "__tgjsSearchModule", This);

        MethodBindingHelper<&FJsEnvImpl::LoadModule>::Bind(Isolate, Context, Global, "__tgjsLoadModule", This);

        MethodBindingHelper<&FJsEnvImpl::LoadCppType>::Bind(Isolate, Context, Global, "__tgjsLoadCDataType", This);

        MethodBindingHelper<&FJsEnvImpl::FindModule>::Bind(Isolate, Context, Global, "__tgjsFindModule", This);

        MethodBindingHelper<&FJsEnvImpl::SetInspectorCallback>::Bind(Isolate, Context, Global, "__tgjsSetInspectorCallback", This);

        MethodBindingHelper<&FJsEnvImpl::DispatchProtocolMessage>::Bind( Isolate, Context, Global, "__tgjsDispatchProtocolMessage", This);

        Isolate->SetPromiseRejectCallback(&PromiseRejectCallback<FJsEnvImpl>);
        Global->Set(Context, FV8Utils::ToV8String(Isolate, "__tgjsSetPromiseRejectCallback"),v8::FunctionTemplate::New(Isolate, &SetPromiseRejectCallback<FJsEnvImpl>)->GetFunction(Context).ToLocalChecked()).Check();

        MethodBindingHelper<&FJsEnvImpl::SetTimeout>::Bind(Isolate, Context, Global, "setTimeout", This);

        MethodBindingHelper<&FJsEnvImpl::ClearInterval>::Bind(Isolate, Context, Global, "clearTimeout", This);

        MethodBindingHelper<&FJsEnvImpl::SetInterval>::Bind(Isolate, Context, Global, "setInterval", This);

        MethodBindingHelper<&FJsEnvImpl::ClearInterval>::Bind(Isolate, Context, Global, "clearInterval", This);

        PuertsObj->Set(Context, FV8Utils::ToV8String(Isolate, "toCString"),
                v8::FunctionTemplate::New(Isolate, ToCString)->GetFunction(Context).ToLocalChecked())
            .Check();

        PuertsObj->Set(Context, FV8Utils::ToV8String(Isolate, "toCPtrArray"),
                v8::FunctionTemplate::New(Isolate, ToCPtrArray)->GetFunction(Context).ToLocalChecked())
            .Check();

        MethodBindingHelper<&FJsEnvImpl::ReleaseManualReleaseDelegate>::Bind(Isolate, Context, PuertsObj, "releaseManualReleaseDelegate", This);

        CppObjectMapper.Initialize(Isolate, Context);

        DelegateTemplate = v8::UniquePersistent<v8::FunctionTemplate>(Isolate, FDelegateWrapper::ToFunctionTemplate(Isolate));

        MulticastDelegateTemplate =v8::UniquePersistent<v8::FunctionTemplate>(Isolate, FMulticastDelegateWrapper::ToFunctionTemplate(Isolate));

        SoftObjectPtrTemplate = v8::UniquePersistent<v8::FunctionTemplate>(Isolate, FSoftObjectWrapper::ToFunctionTemplate(Isolate));


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

        DelegateProxiesCheckerHandler =FTicker::GetCoreTicker().AddTicker(FTickerDelegate::CreateRaw(this, &FJsEnvImpl::CheckDelegateProxies), 1);

        ManualReleaseCallbackMap.Reset(Isolate, v8::Map::New(Isolate));

        UserObjectRetainer.SetName(TEXT("Puerts_UserObjectRetainer"));
        SysObjectRetainer.SetName(TEXT("Puerts_SysObjectRetainer"));

    }

    // #lizard forgives
    FJsEnvImpl::~FJsEnvImpl()
    {

        for (int i = 0; i < ManualReleaseCallbackList.size(); i++)
        {
            if (ManualReleaseCallbackList[i].IsValid())
            {
                ManualReleaseCallbackList[i].Get()->JsFunction.Reset();
            }
        }
        ManualReleaseCallbackMap.Reset();
        InspectorMessageHandler.Reset();
        Require.Reset();
        ReloadJs.Reset();
        JsPromiseRejectCallback.Reset();

        FTicker::GetCoreTicker().RemoveTicker(DelegateProxiesCheckerHandler);

        {
            auto Isolate = MainIsolate;
    #ifdef THREAD_SAFE
            v8::Locker Locker(Isolate);
    #endif
            v8::Isolate::Scope IsolateScope(Isolate);
            v8::HandleScope HandleScope(Isolate);

            ClassToTemplateMap.Empty();

            CppObjectMapper.UnInitialize(Isolate);

            ObjectMap.Empty();

            GeneratedObjectMap.Empty();

            StructCache.Empty();

            ContainerCache.Empty();

            for (auto Iter = DelegateMap.begin(); Iter != DelegateMap.end(); Iter++)
            {
                Iter->second.JSObject.Reset();
                if (Iter->second.Proxy.IsValid())
                {
                    Iter->second.Proxy->JsFunction.Reset();
                }
                for (auto ProxyIter = Iter->second.Proxys.CreateIterator(); ProxyIter; ++ProxyIter)
                {
                    if (!(*ProxyIter).IsValid())
                    {
                        continue;
                    }
                    (*ProxyIter)->JsFunction.Reset();
                }
                if (!Iter->second.PassByPointer)
                {
                    delete ((FScriptDelegate*) Iter->first);
                }
            }

            TsFunctionMap.Empty();
            MixinFunctionMap.Empty();

    #if !defined(ENGINE_INDEPENDENT_JSENV)
            TsDynamicInvoker.Reset();
            BindInfoMap.Empty();
    #endif

            for (auto& Pair : TickerDelegateHandleMap)
            {
                FTicker::GetCoreTicker().RemoveTicker(*(Pair.first));
                delete Pair.first;
                delete Pair.second;
            }
            TickerDelegateHandleMap.clear();

            node::EmitExit(NodeEnv);
            node::Stop(NodeEnv);
            node::FreeEnvironment(NodeEnv);
            v8::Dispose();
            v8::ShutdownPlatform();

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

            DynamicInvoker.Reset();
            MixinInvoker.Reset();

            SoftObjectPtrTemplate.Reset();
            MulticastDelegateTemplate.Reset();
            DelegateTemplate.Reset();
            FixSizeArrayTemplate.Reset();
            MapTemplate.Reset();
            SetTemplate.Reset();
            ArrayTemplate.Reset();
        }

        DefaultContext.Reset();
        MainIsolate->Dispose();
        MainIsolate = nullptr;

        // quickjs will call UnBind in vm dispose, so cleanup move to here
    #if !WITH_BACKING_STORE_AUTO_FREE && !defined(HAS_ARRAYBUFFER_NEW_WITHOUT_STL)
        for (auto& KV : ScriptStructFinalizeInfoMap)
        {
            FScriptStructWrapper::Free(KV.Value.Struct, KV.Value.Finalize, KV.Key);
        }
    #endif
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

        if (ModuleLoader->Search(,TEXT(""), ModuleName.ToString(), OutPath, OutDebugPath))
        {
            OutPath = FPaths::ConvertRelativePathToFull(OutPath);
            Logger->LogInfo("reload js module ["+OutPath+"]");
            v8::TryCatch TryCatch(Isolate);
            v8::Handle<v8::Value> Args[] = {
                FV8Utils::ToV8String(Isolate, ModuleName), 
                FV8Utils::ToV8String(Isolate, OutPath), 
                FV8Utils::ToV8String(Isolate, JsSource)};

            auto MaybeRet = LocalReloadJs->Call(Context, v8::Undefined(Isolate), 3, Args);

            if (TryCatch.HasCaught())
            {
                Logger->InfoError("reload module exception "+FV8Utils::TryCatchToString(Isolate, &TryCatch));
            }
        }
        else
        {
            Logger->InfoWarn("not find js module ["+ModuleName.ToString()+"]");
            return;
        }
    }

    void NFJsScriptModule::ReloadModule(std::string ModuleName, const std::string& JsSource)
    {

    #ifdef THREAD_SAFE
        v8::Locker Locker(MainIsolate);
    #endif
        // Logger->Info(FString::Printf(TEXT("start reload js module [%s]"), *ModuleName.ToString()));
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
        stream << "reload js [" << *Path <<"]";
        Logger->LogInfo(stream);
        v8::TryCatch TryCatch(Isolate);
        v8::Handle<v8::Value> Args[] = {v8::Undefined(Isolate), FV8Utils::ToV8String(Isolate, Path), FV8Utils::ToV8String(Isolate, JsSource.c_str())};

        auto MaybeRet = LocalReloadJs->Call(Context, v8::Undefined(Isolate), 3, Args);

        if (TryCatch.HasCaught())
        {
            std::ostringstream streama;
            streama << "reload module exception " << FV8Utils::TryCatchToString(Isolate, &TryCatch));
            Logger->LogError(streama);
        }
    }

    void NFJsScriptModule::OnSourceLoaded(std::function<void(const FString&)> Callback)
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

    v8::Local<v8::Value> NFJsScriptModule::FindOrAddDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, UObject* Owner, PropertyMacro* Property, void* DelegatePtr, bool PassByPointer)
    {
        check(DelegatePtr);    // must not null

        if (PassByPointer)
        {
            auto Iter = DelegateMap.find(DelegatePtr);
            if (Iter != DelegateMap.end())
            {
                if (Iter->second.Owner.IsValid())
                {
                    return Iter->second.JSObject.Get(Isolate);
                }
                else
                {
                    ClearDelegate(Isolate, Context, DelegatePtr);
                }
            }
        }
        else
        {
            if (CastFieldMacro<DelegatePropertyMacro>(Property))
            {
                auto NewDelegatePtr = new FScriptDelegate;
                *NewDelegatePtr = *static_cast<FScriptDelegate*>(DelegatePtr);
                DelegatePtr = NewDelegatePtr;
            }
            else    // do not support MulticastDelegate
            {
                return v8::Undefined(Isolate);
            }
        }

        {
            // UE_LOG(LogTemp, Warning, TEXT("FindOrAddDelegate -- new %s"), *Property->GetName());
            auto Constructor = (Property->IsA<DelegatePropertyMacro>() ? DelegateTemplate : MulticastDelegateTemplate)
                                .Get(Isolate)
                                ->GetFunction(Context)
                                .ToLocalChecked();
            auto JSObject = Constructor->NewInstance(Context).ToLocalChecked();
            DataTransfer::SetPointer(Isolate, JSObject, DelegatePtr, 0);
            auto ReturnVal = JSObject->Set(Context, 0, v8::Map::New(Isolate));
            UFunction* Function = nullptr;
            DelegatePropertyMacro* DelegateProperty = CastFieldMacro<DelegatePropertyMacro>(Property);
            MulticastDelegatePropertyMacro* MulticastDelegateProperty = CastFieldMacro<MulticastDelegatePropertyMacro>(Property);
            if (DelegateProperty)
            {
                Function = DelegateProperty->SignatureFunction;
            }
            else if (MulticastDelegateProperty)
            {
                Function = MulticastDelegateProperty->SignatureFunction;
            }
            DelegateMap[DelegatePtr] = {v8::UniquePersistent<v8::Object>(Isolate, JSObject), TWeakObjectPtr<UObject>(Owner),
                DelegateProperty, MulticastDelegateProperty, Function, PassByPointer, nullptr};
            return JSObject;
        }
    }

    v8::Local<v8::Value> NFJsScriptModule::CreateArray(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, FPropertyTranslator* Property, void* ArrayPtr)
    {
        auto Array = FixSizeArrayTemplate.Get(Isolate)->GetFunction(Context).ToLocalChecked()->NewInstance(Context).ToLocalChecked();
        DataTransfer::SetPointer(Isolate, Array, ArrayPtr, 0);
        DataTransfer::SetPointer(Isolate, Array, Property, 1);
        return Array;
    }

    void NFJsScriptModule::InvokeDelegateCallback(UDynamicDelegateProxy* Proxy, void* Params)
    {

        auto SignatureFunction = Proxy->SignatureFunction;
        auto Iter = JsCallbackPrototypeMap.find(SignatureFunction.Get());
        if (Iter == JsCallbackPrototypeMap.end())
        {
            if (!SignatureFunction.IsValid())
            {
                Logger->LogWarn("invalid SignatureFunction!");
                return;
            }
            JsCallbackPrototypeMap[SignatureFunction.Get()] = std::make_unique<FFunctionTranslator>(SignatureFunction.Get(), true);
            Iter = JsCallbackPrototypeMap.find(SignatureFunction.Get());
        }
        else
        {
            if (!SignatureFunction.IsValid())
            {
                JsCallbackPrototypeMap.erase(Iter);
                Logger->LogWarn("invalid SignatureFunction!");
                return;
            }

            // 非 Editor 模式，函数签名地址可能会变且内存可能复用，不检查可能会访问到旧的非法地址。
            if (!Iter->second->IsValid())
            {
                JsCallbackPrototypeMap[SignatureFunction.Get()] = std::make_unique<FFunctionTranslator>(SignatureFunction.Get(), true);
                Iter = JsCallbackPrototypeMap.find(SignatureFunction.Get());
            }
        }

        auto Isolate = MainIsolate;
        v8::Isolate::Scope IsolateScope(Isolate);
        v8::HandleScope HandleScope(Isolate);
        auto Context = DefaultContext.Get(Isolate);
        v8::Context::Scope ContextScope(Context);

        v8::TryCatch TryCatch(Isolate);

        Iter->second->CallJs(Isolate, Context, Proxy->JsFunction.Get(Isolate), Context->Global(), Params);

        if (TryCatch.HasCaught())
        {
            Logger->Error(FString::Printf(TEXT("js callback exception %s"), *FV8Utils::TryCatchToString(Isolate, &TryCatch)));
        }
    }



    void NFJsScriptModule::ExecuteDelegate( v8::Isolate* Isolate, v8::Local<v8::Context>& Context, const v8::FunctionCallbackInfo<v8::Value>& Info, void* DelegatePtr)
    {
        auto Iter = DelegateMap.find(DelegatePtr);
        if (Iter == DelegateMap.end())
        {
            FV8Utils::ThrowException(Isolate, "can not find the delegate!");
        }
        auto SignatureFunction = Iter->second.SignatureFunction;
        if (JsCallbackPrototypeMap.find(SignatureFunction) == JsCallbackPrototypeMap.end())
        {
            JsCallbackPrototypeMap[SignatureFunction] = std::make_unique<FFunctionTranslator>(SignatureFunction, true);
        }

        if (Iter->second.DelegateProperty)
        {
            JsCallbackPrototypeMap[SignatureFunction]->Call(Isolate, Context, Info,
                [ScriptDelegate = static_cast<FScriptDelegate*>(DelegatePtr)](void* Params)
                { ScriptDelegate->ProcessDelegate<UObject>(Params); });
        }
        else
        {
            JsCallbackPrototypeMap[SignatureFunction]->Call(Isolate, Context, Info,
                [MulticastScriptDelegate = static_cast<FMulticastScriptDelegate*>(DelegatePtr)](void* Params)
                { MulticastScriptDelegate->ProcessMulticastDelegate<UObject>(Params); });
        }
    }

    static string NAME_Fire("Fire");

    bool NFJsScriptModule::AddToDelegate( v8::Isolate* Isolate, v8::Local<v8::Context>& Context, void* DelegatePtr, v8::Local<v8::Function> JsFunction)
    {
        // UE_LOG(LogTemp, Warning, TEXT("add delegate proxy"));
        auto Iter = DelegateMap.find(DelegatePtr);
        if (Iter == DelegateMap.end())
        {
            return false;
        }
        if (!Iter->second.Owner.IsValid())
        {
            Logger->Warn("try to bind a delegate with invalid owner!");
            ClearDelegate(Isolate, Context, DelegatePtr);
            if (!Iter->second.PassByPointer)
            {
                delete ((FScriptDelegate*) Iter->first);
            }
            DelegateMap.erase(Iter);
            return false;
        }
        if (Iter->second.Proxy.IsValid())
        {
            ClearDelegate(Isolate, Context, DelegatePtr);
        }
        auto JSObject = Iter->second.JSObject.Get(Isolate);
        auto Map = v8::Local<v8::Map>::Cast(JSObject->Get(Context, 0).ToLocalChecked());
        auto MaybeProxy = Map->Get(Context, JsFunction);
        UDynamicDelegateProxy* DelegateProxy = nullptr;
        if (MaybeProxy.IsEmpty() || !MaybeProxy.ToLocalChecked()->IsExternal())
        {
            // UE_LOG(LogTemp, Warning, TEXT("new delegate proxy"));
            DelegateProxy = NewObject<UDynamicDelegateProxy>();
    #ifdef THREAD_SAFE
            DelegateProxy->Isolate = Isolate;
    #endif
            DelegateProxy->Owner = Iter->second.Owner;
            DelegateProxy->SignatureFunction = Iter->second.SignatureFunction;
            DelegateProxy->DynamicInvoker = DynamicInvoker;
            DelegateProxy->JsFunction = v8::UniquePersistent<v8::Function>(Isolate, JsFunction);

            SysObjectRetainer.Retain(DelegateProxy);
            auto ReturnVal = Map->Set(Context, JsFunction, v8::External::New(Context->GetIsolate(), DelegateProxy));
        }
        else
        {
            // UE_LOG(LogTemp, Warning, TEXT("find delegate proxy"));
            DelegateProxy =
                Cast<UDynamicDelegateProxy>(static_cast<UObject*>(v8::Local<v8::External>::Cast(MaybeProxy.ToLocalChecked())->Value()));
        }

        FScriptDelegate Delegate;
        Delegate.BindUFunction(DelegateProxy, NAME_Fire);

        if (Iter->second.DelegateProperty)
        {
            // UE_LOG(LogTemp, Warning, TEXT("bind to delegate"));
            Iter->second.Proxy = DelegateProxy;
            *(static_cast<FScriptDelegate*>(DelegatePtr)) = Delegate;
        }
        else if (Iter->second.MulticastDelegateProperty)
        {
            // UE_LOG(LogTemp, Warning, TEXT("add to multicast delegate, proxy: %p to:%p"), DelegateProxy, DelegatePtr);
            Iter->second.Proxys.Add(DelegateProxy);
    #if ENGINE_MINOR_VERSION >= 23 || ENGINE_MAJOR_VERSION > 4
            if (Iter->second.MulticastDelegateProperty->IsA<MulticastSparseDelegatePropertyMacro>())
            {
                Iter->second.MulticastDelegateProperty->AddDelegate(MoveTemp(Delegate), Iter->second.Owner.Get(), DelegatePtr);
            }
            else
    #endif
            {
                static_cast<FMulticastScriptDelegate*>(DelegatePtr)->AddUnique(Delegate);
            }
        }
        return true;
    }

    PropertyMacro* NFJsScriptModule::FindDelegateProperty(void* DelegatePtr)
    {
        auto Iter = DelegateMap.find(DelegatePtr);
        if (Iter == DelegateMap.end())
        {
            return nullptr;
        }
        return Iter->second.DelegateProperty ? (PropertyMacro*) Iter->second.DelegateProperty
                                            : (PropertyMacro*) Iter->second.MulticastDelegateProperty;
    }

    FScriptDelegate NFJsScriptModule::NewManualReleaseDelegate( v8::Isolate* Isolate, v8::Local<v8::Context>& Context, v8::Local<v8::Function> JsFunction, UFunction* SignatureFunction)
    {
        auto CallbacksMap = ManualReleaseCallbackMap.Get(Isolate);
        auto MaybeProxy = CallbacksMap->Get(Context, JsFunction);
        UDynamicDelegateProxy* DelegateProxy = nullptr;
        if (MaybeProxy.IsEmpty() || !MaybeProxy.ToLocalChecked()->IsExternal())
        {
            DelegateProxy = NewObject<UDynamicDelegateProxy>();
    #ifdef THREAD_SAFE
            DelegateProxy->Isolate = Isolate;
    #endif
            DelegateProxy->Owner = DelegateProxy;
            DelegateProxy->SignatureFunction = SignatureFunction;
            DelegateProxy->DynamicInvoker = DynamicInvoker;
            DelegateProxy->JsFunction = v8::UniquePersistent<v8::Function>(Isolate, JsFunction);

            SysObjectRetainer.Retain(DelegateProxy);
            __USE(CallbacksMap->Set(Context, JsFunction, v8::External::New(Context->GetIsolate(), DelegateProxy)));

            ManualReleaseCallbackList.push_back(DelegateProxy);
        }
        else
        {
            DelegateProxy =Cast<UDynamicDelegateProxy>(static_cast<UObject*>(v8::Local<v8::External>::Cast(MaybeProxy.ToLocalChecked())->Value()));
        }

        FScriptDelegate Delegate;
        Delegate.BindUFunction(DelegateProxy, NAME_Fire);
        return Delegate;
    }

    void NFJsScriptModule::ReleaseManualReleaseDelegate(const v8::FunctionCallbackInfo<v8::Value>& Info)
    {
        v8::Isolate* Isolate = Info.GetIsolate();
        v8::Isolate::Scope IsolateScope(Isolate);
        v8::HandleScope HandleScope(Isolate);
        v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
        v8::Context::Scope ContextScope(Context);

        CHECK_V8_ARGS(EArgFunction);

        auto CallbacksMap = ManualReleaseCallbackMap.Get(Isolate);
        auto MaybeProxy = CallbacksMap->Get(Context, Info[0]);
        if (!MaybeProxy.IsEmpty() && MaybeProxy.ToLocalChecked()->IsExternal())
        {
            __USE(CallbacksMap->Delete(Context, Info[0]));
            auto DelegateProxy =
                Cast<UDynamicDelegateProxy>(static_cast<UObject*>(v8::Local<v8::External>::Cast(MaybeProxy.ToLocalChecked())->Value()));
            for (auto it = ManualReleaseCallbackList.begin(); it != ManualReleaseCallbackList.end();)
            {
                if (!it->IsValid())
                {
                    it = ManualReleaseCallbackList.erase(it);
                }
                else if (it->Get() == DelegateProxy)
                {
                    DelegateProxy->JsFunction.Reset();
                    it = ManualReleaseCallbackList.erase(it);
                    SysObjectRetainer.Release(DelegateProxy);
                }
                else
                {
                    ++it;
                }
            }
        }
    }

    bool NFJsScriptModule::RemoveFromDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, void* DelegatePtr, v8::Local<v8::Function> JsFunction)
    {
        auto Iter = DelegateMap.find(DelegatePtr);
        if (Iter == DelegateMap.end())
        {
            return false;
        }

        FScriptDelegate Delegate;

        if (Iter->second.DelegateProperty)
        {
            return ClearDelegate(Isolate, Context, DelegatePtr);
        }
        else if (Iter->second.MulticastDelegateProperty)
        {
            auto JSObject = Iter->second.JSObject.Get(Isolate);
            auto Map = v8::Local<v8::Map>::Cast(JSObject->Get(Context, 0).ToLocalChecked());
            auto MaybeValue = Map->Get(Context, JsFunction);

            if (MaybeValue.IsEmpty())
            {
                return false;
            }

            auto MaybeProxy = MaybeValue.ToLocalChecked();
            if (!MaybeProxy->IsExternal())
            {
                return false;
            }

            auto DelegateProxy = Cast<UDynamicDelegateProxy>(static_cast<UObject*>(v8::Local<v8::External>::Cast(MaybeProxy)->Value()));

            Delegate.BindUFunction(DelegateProxy, NAME_Fire);

    #if ENGINE_MINOR_VERSION >= 23 || ENGINE_MAJOR_VERSION > 4
            if (Iter->second.MulticastDelegateProperty->IsA<MulticastSparseDelegatePropertyMacro>())
            {
                Iter->second.MulticastDelegateProperty->RemoveDelegate(Delegate, Iter->second.Owner.Get(), DelegatePtr);
            }
            else
    #endif
            {
                static_cast<FMulticastScriptDelegate*>(DelegatePtr)->Remove(Delegate);
            }

            auto ReturnVal = Map->Delete(Context, JsFunction);

            Iter->second.Proxys.Remove(DelegateProxy);
            SysObjectRetainer.Release(DelegateProxy);
            DelegateProxy->JsFunction.Reset();
        }

        return true;
    }

    bool NFJsScriptModule::ClearDelegate(v8::Isolate* Isolate, v8::Local<v8::Context>& Context, void* DelegatePtr)
    {
        auto Iter = DelegateMap.find(DelegatePtr);
        if (Iter == DelegateMap.end())
        {
            return false;
        }

        auto JSObject = Iter->second.JSObject.Get(Isolate);
        auto Map = v8::Local<v8::Map>::Cast(JSObject->Get(Context, 0).ToLocalChecked());
        Map->Clear();

        if (Iter->second.DelegateProperty)
        {
            if (Iter->second.Proxy.IsValid())
            {
                if (Iter->second.Owner.IsValid())
                {
                    FScriptDelegate Delegate;
                    *(static_cast<FScriptDelegate*>(DelegatePtr)) = Delegate;
                }

                SysObjectRetainer.Release(Iter->second.Proxy.Get());
                Iter->second.Proxy->JsFunction.Reset();
                Iter->second.Proxy.Reset();
            }
        }
        else if (Iter->second.MulticastDelegateProperty)
        {
            if (Iter->second.Owner.IsValid())
            {
    #if ENGINE_MINOR_VERSION >= 23 || ENGINE_MAJOR_VERSION > 4
                if (Iter->second.MulticastDelegateProperty->IsA<MulticastSparseDelegatePropertyMacro>())
                {
                    Iter->second.MulticastDelegateProperty->ClearDelegate(Iter->second.Owner.Get(), DelegatePtr);
                }
                else
    #endif
                {
                    static_cast<FMulticastScriptDelegate*>(DelegatePtr)->Clear();
                }
            }

            for (auto ProxyIter = Iter->second.Proxys.CreateIterator(); ProxyIter; ++ProxyIter)
            {
                if (!(*ProxyIter).IsValid())
                {
                    continue;
                }
                (*ProxyIter)->JsFunction.Reset();
                SysObjectRetainer.Release((*ProxyIter).Get());
            }
            Iter->second.Proxys.Empty();
        }

        return true;
    }

    bool NFJsScriptModule::CheckDelegateProxies(float Tick)
    {

        auto Isolate = MainIsolate;
    #ifdef THREAD_SAFE
        v8::Locker Locker(Isolate);
    #endif

        std::vector<void*> PendingToRemove;
        for (auto& KV : DelegateMap)
        {
            if (!KV.second.Owner.IsValid())
            {
                PendingToRemove.push_back(KV.first);
            }
        }

        if (PendingToRemove.size() > 0)
        {
            v8::Isolate::Scope IsolateScope(Isolate);
            v8::HandleScope HandleScope(Isolate);
            v8::Local<v8::Context> Context = DefaultContext.Get(Isolate);
            v8::Context::Scope ContextScope(Context);
            for (int i = 0; i < PendingToRemove.size(); ++i)
            {
                ClearDelegate(Isolate, Context, PendingToRemove[i]);
                if (!DelegateMap[PendingToRemove[i]].PassByPointer)
                {
                    delete ((FScriptDelegate*) PendingToRemove[i]);
                }
                DelegateMap.erase(PendingToRemove[i]);
            }
        }

        // Collecting invalid function translators to remove.
        std::vector<UFunction*> PendingToRemoveJsCallbacks;
        for (auto& KV : JsCallbackPrototypeMap)
        {
            if ((nullptr == KV.first) || (!KV.second->IsValid()))
            {
                PendingToRemoveJsCallbacks.push_back(KV.first);
            }
        }

        if (PendingToRemoveJsCallbacks.size() > 0)
        {
            for (int32 i = 0; i < PendingToRemoveJsCallbacks.size(); i++)
            {
                JsCallbackPrototypeMap.erase(PendingToRemoveJsCallbacks[i]);
            }
        }

        return true;
    }

FPropertyTranslator* NFJsScriptModule::GetContainerPropertyTranslator(PropertyMacro* Property)
{
    auto Iter = ContainerPropertyMap.find(Property);
    // TODO: 如果脚本一直持有蓝图里头的Map，还是有可能有问题的，需要统筹考虑一套机制解决这类问题
    if (Iter == ContainerPropertyMap.end() || !Iter->second.PropertyWeakPtr.IsValid())
    {
        ContainerPropertyInfo Temp{Property, FPropertyTranslator::Create(Property)};
            ContainerPropertyMap[Property] = std::move(Temp);
#if ENGINE_MINOR_VERSION < 25 && ENGINE_MAJOR_VERSION < 5
        if (!Property->IsNative())
        {
            SysObjectRetainer.Retain(Property);
        }
 #endif
        return ContainerPropertyMap[Property].PropertyTranslator.get();
    }
    else
    {
        return Iter->second.PropertyTranslator.get();
    }
}

v8::Local<v8::Value> NFJsScriptModule::FindOrAddContainer(v8::Isolate* Isolate, v8::Local<v8::Context>& Context,v8::Local<v8::Function> Constructor, PropertyMacro* Property1, PropertyMacro* Property2, void* Ptr, bool PassByPointer)
{
    check(Ptr);    // must not null

    auto PersistentValuePtr = ContainerCache.Find(Ptr);
    if (PersistentValuePtr)
    {
        return v8::Local<v8::Value>::New(Isolate, *PersistentValuePtr);
    }

    auto BindTo = v8::External::New(Context->GetIsolate(), Ptr);
    v8::Handle<v8::Value> Args[] = {BindTo, v8::Boolean::New(Isolate, PassByPointer)};
    auto Result = Constructor->NewInstance(Context, 2, Args).ToLocalChecked();
    DataTransfer::SetPointer(Isolate, Result, GetContainerPropertyTranslator(Property1), 1);
    if (Property2)
    {
        DataTransfer::SetPointer(Isolate, Result, GetContainerPropertyTranslator(Property2), 2);
    } 
    return Result;
}


void NFJsScriptModule::BindCppObject(v8::Isolate* InIsolate, JSClassDefinition* ClassDefinition, void* Ptr, v8::Local<v8::Object> JSObject, bool PassByPointer)
{
    CppObjectMapper.BindCppObject(InIsolate, ClassDefinition, Ptr, JSObject, PassByPointer);
}



void NFJsScriptModule::UnBindCppObject(JSClassDefinition* ClassDefinition, void* Ptr)
{
    CppObjectMapper.UnBindCppObject(ClassDefinition, Ptr);
}

void NFJsScriptModule::BindContainer(void* Ptr, v8::Local<v8::Object> JSObject, void (*Callback)(const v8::WeakCallbackInfo<void>& data))
{
    DataTransfer::SetPointer(MainIsolate, JSObject, Ptr, 0);
    ContainerCache.Emplace(Ptr, v8::UniquePersistent<v8::Value>(MainIsolate, JSObject));
    ContainerCache[Ptr].SetWeak<void>(nullptr, Callback, v8::WeakCallbackType::kInternalFields);
}

void NFJsScriptModule::UnBindContainer(void* Ptr)
{
    ContainerCache.Remove(Ptr);
}

bool NFJsScriptModule::IsInstanceOfCppObject(const void* TypeId, v8::Local<v8::Object> JsObject)
{
    return CppObjectMapper.IsInstanceOfCppObject(TypeId, JsObject);
}

void NFJsScriptModule::LoadCppType(const v8::FunctionCallbackInfo<v8::Value>& Info)
{
    CppObjectMapper.LoadCppType(Info);
}

bool NFJsScriptModule::GetContainerTypeProperty(v8::Local<v8::Context> Context, v8::Local<v8::Value> Value, PropertyMacro** PropertyPtr)
{
    if (Value->IsInt32())
    {
        int Type = Value->Int32Value(Context).ToChecked();
        if (Type >= MaxBuiltinType)
        {
            *PropertyPtr = nullptr;
            return false;
        }
        *PropertyPtr = ContainerMeta.GetBuiltinProperty((BuiltinType) Type);
        return true;
    }
    else if (auto Field = Cast<UField>(FV8Utils::GetUObject(Context, Value)))
    {
        *PropertyPtr = ContainerMeta.GetObjectProperty(Field);
        return *PropertyPtr != nullptr;
    }
    else
    {
        *PropertyPtr = nullptr;
        return false;
    }
}



void NFJsScriptModule::Start(const std::string& ModuleNameOrScript, const std::vector<std::pair<std::string, NFIModule*>>& Arguments, bool IsScript)
{

    if (Started)
    {
        Logger->Error("Started yet!");
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
        Logger->Error("global.puerts not found!");
        return;
    }

    auto TGJS = MaybeTGameTGJS.ToLocalChecked()->ToObject(Context).ToLocalChecked();

    auto MaybeArgv = TGJS->Get(Context, FV8Utils::ToV8String(Isolate, "argv"));

    if (MaybeArgv.IsEmpty() || !MaybeArgv.ToLocalChecked()->IsObject())
    {
        Logger->Error("global.puerts.argv not found!");
        return;
    }

    auto Argv = MaybeArgv.ToLocalChecked()->ToObject(Context).ToLocalChecked();

    auto MaybeArgvAdd = Argv->Get(Context, FV8Utils::ToV8String(Isolate, "add"));

    if (MaybeArgvAdd.IsEmpty() || !MaybeArgvAdd.ToLocalChecked()->IsFunction())
    {
        Logger->Error("global.puerts.argv.add not found!");
        return;
    }

    auto ArgvAdd = MaybeArgvAdd.ToLocalChecked().As<v8::Function>();

    for (int i = 0; i < Arguments.size(); i++)
    {
        auto Object = Arguments[i].Value;
        v8::Local<v8::Value> Args[2] = {FV8Utils::ToV8String(Isolate, Arguments[i].Key), FindOrAdd(Isolate, Context, Object->GetClass(), Object)};
        auto Result = ArgvAdd->Call(Context, Argv, 2, Args);
    }

    if (IsScript)
    {
        v8::ScriptOrigin Origin(FV8Utils::ToV8String(Isolate, "chunk"));
        v8::Local<v8::String> Source = FV8Utils::ToV8String(Isolate, ModuleNameOrScript);
        v8::TryCatch TryCatch(Isolate);

        auto CompiledScript = v8::Script::Compile(Context, Source, &Origin);
        if (CompiledScript.IsEmpty())
        {
            Logger->Error(FV8Utils::TryCatchToString(Isolate, &TryCatch));
            return;
        }
        auto ReturnVal = CompiledScript.ToLocalChecked()->Run(Context);
        if (TryCatch.HasCaught())
        {
            Logger->Error(FV8Utils::TryCatchToString(Isolate, &TryCatch));
        }
    }
    else
    {
        v8::TryCatch TryCatch(Isolate);
        v8::Local<v8::Value> Args[] = {FV8Utils::ToV8String(Isolate, ModuleNameOrScript)};
        __USE(Require.Get(Isolate)->Call(Context, v8::Undefined(Isolate), 1, Args));
        if (TryCatch.HasCaught())
        {
            Logger->Error(FV8Utils::TryCatchToString(Isolate, &TryCatch));
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
            stream <<"can not load ["<< *ModuleName <<"]";
            ErrInfo = stream.str();
            //Logger->LogInfo(stream.str());
            return false;
        }
    }
    else
    {
        std::ostringstream stream;
        stream <<"can not load ["<< *ModuleName <<"]";
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
            Logger->LogError(ErrInfo);
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
            Logger->LogError(FV8Utils::TryCatchToString(Isolate, &TryCatch));
            return;
        }
    auto ReturnVal = CompiledScript.ToLocalChecked()->Run(Context);
    if (TryCatch.HasCaught())
    {
        Logger->LogError(FV8Utils::TryCatchToString(Isolate, &TryCatch));
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
            Logger->LogInfo(Msg);
            break;
        case 2:
            Logger->LogWarn(Msg);
            break;
        case 3:
            Logger->LogError(Msg);
            break;
        default:
            Logger->LogInfo(Msg);
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
    std::function<void(const JSError*, std::shared_ptr<ILogger>&)> ExceptionLog =
        [](const JSError* Exception, std::shared_ptr<ILogger>& InLogger)
    {
        FString Message = FString::Printf(TEXT("JS Execution Exception: %s"), *(Exception->Message));
        InLogger->Warn(Message);
    };
    std::function<void(const JSError*)> ExceptionLogWrapper = std::bind(ExceptionLog, _1, Logger);
    std::function<void(v8::Isolate*, v8::TryCatch*)> ExecutionExceptionHandler =
        std::bind(&FJsEnvImpl::ReportExecutionException, this, _1, _2, ExceptionLogWrapper);
    std::function<void(FDelegateHandle*)> DelegateHandleCleaner = std::bind(&FJsEnvImpl::RemoveFTickerDelegateHandle, this, _1);

    FTickerDelegateWrapper* DelegateWrapper = new FTickerDelegateWrapper(Continue);
    DelegateWrapper->Init(Info, ExecutionExceptionHandler, DelegateHandleCleaner);

    FTickerDelegate Delegate = FTickerDelegate::CreateRaw(DelegateWrapper, &FTickerDelegateWrapper::CallFunction);

    v8::Isolate* Isolate = Info.GetIsolate();
    v8::Isolate::Scope IsolateScope(Isolate);
    v8::HandleScope HandleScope(Isolate);
    v8::Local<v8::Context> Context = Isolate->GetCurrentContext();
    float Millisecond = Info[1]->NumberValue(Context).ToChecked();
    float Delay = Millisecond / 1000.f;

    // TODO - 如果实现多线程，这里应该加锁阻止定时回调的执行，直到DelegateWrapper设置好handle
    FDelegateHandle* DelegateHandle = new FDelegateHandle(FTicker::GetCoreTicker().AddTicker(Delegate, Delay));
    DelegateWrapper->SetDelegateHandle(DelegateHandle);
    TickerDelegateHandleMap[DelegateHandle] = DelegateWrapper;

    Info.GetReturnValue().Set(v8::External::New(Info.GetIsolate(), DelegateHandle));
}

void NFJsScriptModule::ReportExecutionException(v8::Isolate* Isolate, v8::TryCatch* TryCatch, std::function<void(const JSError*)> CompletionHandler)
{
    const JSError Error(FV8Utils::TryCatchToString(Isolate, TryCatch));
    if (CompletionHandler)
    {
        CompletionHandler(&Error);
    }
}

void NFJsScriptModule::RemoveFTickerDelegateHandle(FDelegateHandle* Handle)
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
        FTicker::GetCoreTicker().RemoveTicker(*(Iterator->first));
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
        Logger->Warn(TEXT("Calling ClearInterval with 0 argument."));
    }
    else if (Info[0]->IsNullOrUndefined())
    {
        // 屏蔽这条只在mocha中出现的警告
        // Logger->Warn(TEXT("Calling ClearInterval with a Null or Undefined."));
    }
    else
    {
        CHECK_V8_ARGS(EArgExternal);
        v8::Local<v8::External> Arg = v8::Local<v8::External>::Cast(Info[0]);
        FDelegateHandle* Handle = static_cast<FDelegateHandle*>(Arg->Value());
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