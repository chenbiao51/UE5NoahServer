#ifndef NF_JS_ENV_MODULE_H
#define NF_JS_ENV_MODULE_H


#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/dynamic_message.h>
#include "Dependencies/LuaIntf/LuaIntf/LuaIntf.h"
#include "Dependencies/LuaIntf/LuaIntf/LuaRef.h"
#include "NFComm/NFPluginModule/NFILogModule.h"


class NFIJsEnvModule : public NFIModule
{

};

class NFJsEnvModule : public NFIJsPBModule
{
public:
    NFJsEnvModule(NFIPluginManager* p)
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

protected:
	void SetLuaState(lua_State* pState);
	void PrintMessage(const google::protobuf::Message& messag, const bool bEncode);

	LuaIntf::LuaRef Decode(const std::string& strMsgTypeName, const std::string& strData);
	const std::string Encode(const std::string& strMsgTypeName, const LuaIntf::LuaRef& luaTable);

	friend class NFLuaScriptModule;

private:
	LuaIntf::LuaRef MessageToTbl(const google::protobuf::Message& message) const;

	LuaIntf::LuaRef GetField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field) const;
	LuaIntf::LuaRef GetRepeatedField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field) const;
	LuaIntf::LuaRef GetRepeatedFieldElement(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, int index) const;


	///////////////
	const bool TblToMessage(const LuaIntf::LuaRef& luaTable, google::protobuf::Message& message);
	
	void SetField(google::protobuf::Message& message, const std::string& sField, const LuaIntf::LuaRef& luaValue);
	void SetRepeatedField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const LuaIntf::LuaRef& luaTable);
	void SetRepeatedMapField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const LuaIntf::LuaRef& luaTable);
	void AddToRepeatedField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const LuaIntf::LuaRef& luaValue);
	void AddToMapField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, const LuaIntf::LuaRef& key, const LuaIntf::LuaRef& val);
	int GetEnumValue(google::protobuf::Message& message, const LuaIntf::LuaRef& luaValue, const google::protobuf::FieldDescriptor* field) const;

protected:
	NFILogModule* m_pLogModule;

    int64_t mnTime;
    std::string strVersionCode;
	lua_State* m_pLuaState;

	NFMultiFileErrorCollector mErrorCollector;
	google::protobuf::compiler::DiskSourceTree mSourceTree;
	google::protobuf::compiler::Importer* m_pImporter;
	google::protobuf::DynamicMessageFactory* m_pFactory;
};

#endif