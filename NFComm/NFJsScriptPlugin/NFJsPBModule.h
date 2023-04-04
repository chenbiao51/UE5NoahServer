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


#ifndef NF_JS_PB_MODULE_H
#define NF_JS_PB_MODULE_H

#define LUAINTF_LINK_LUA_COMPILED_IN_CXX 0
#include "v8.h"


#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include "Dependencies/LuaIntf/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NFILogModule.h"

#if NF_PLATFORM != NF_PLATFORM_WIN
#include "NFComm/NFCore/NFException.hpp"
#endif

class NFMultiFileErrorCollector : public google::protobuf::compiler::MultiFileErrorCollector
{
public:
	NFMultiFileErrorCollector() {}
	virtual ~NFMultiFileErrorCollector() {};

	// Line and column numbers are zero-based.  A line number of -1 indicates
	// an error with the entire file (e.g. "not found").
	virtual void AddError(const string& filename, int line, int column, const string& message)
	{
		std::cout << filename << " line:" << line << " column:" << column  << " message:" << message  << std::endl;
	}
};


class NFIJsPBModule : public NFIModule
{
public:
	virtual void ImportProtoFile(const std::string& strFile) = 0;
};

class NFJsPBModule : public NFIJsPBModule
{
public:
    NFJsPBModule(NFIPluginManager* p)
    {
        pPluginManager = p;
    }

	virtual bool Awake();
	virtual bool Init();
    virtual bool Shut();
	virtual bool ReadyExecute();
	virtual bool Execute();

    virtual bool AfterInit();
    virtual bool BeforeShut();

	virtual void ImportProtoFile(const std::string& strFile);


    v8::Local<v8::Value> Decode(const std::string& strMsgTypeName, const std::string& strData);
	const std::string Encode(const std::string& strMsgTypeName, const v8::Local<v8::Value>& v8Value);
protected:
	void SetIsolate(v8::Isolate* pState);
	void PrintMessage(const google::protobuf::Message& messag, const bool bEncode);

	

	friend class NFLuaScriptModule;

private:
	v8::Local<v8::Value> MessageToObject(const google::protobuf::Message& message) const;

	v8::Local<v8::Value> GetField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field) const;
	v8::Local<v8::Value> GetRepeatedField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field) const;
	v8::Local<v8::Value> GetRepeatedFieldElement(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, int index) const;


	///////////////
	const bool ObjectToMessage(const v8::Local<v8::Value>& v8Value, google::protobuf::Message& message);
	
	void SetField(google::protobuf::Message& message, const std::string& sField, const v8::Local<v8::Value>& luaValue);
	void SetRepeatedField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const v8::Local<v8::Value>& v8Value);
	void SetRepeatedMapField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const v8::Local<v8::Value>& v8Value);
	void AddToRepeatedField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const v8::Local<v8::Value>& luaValue);
	void AddToMapField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const v8::Local<v8::String>& key, const v8::Local<v8::Value>& val);
	int GetEnumValue(google::protobuf::Message& message, const v8::Local<v8::Value>& luaValue, const google::protobuf::FieldDescriptor* field) const;

protected:
	NFILogModule* m_pLogModule;

    int64_t mnTime;
    std::string strVersionCode;
	v8::Isolate* Isolate;
    v8::Local<v8::Context> Context;

	NFMultiFileErrorCollector mErrorCollector;
	google::protobuf::compiler::DiskSourceTree mSourceTree;
	google::protobuf::compiler::Importer* m_pImporter;
	google::protobuf::DynamicMessageFactory* m_pFactory;
};

#endif
