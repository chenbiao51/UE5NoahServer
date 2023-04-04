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


#include "NFJsPBModule.h"

bool NFJsPBModule::Awake()
{
	mSourceTree.MapPath("", "./../NFDataCfg");
	mSourceTree.MapPath("", "./../NFDataCfg/proto");
	m_pImporter = new google::protobuf::compiler::Importer(&mSourceTree, &mErrorCollector);
	m_pFactory = new google::protobuf::DynamicMessageFactory();

	mnTime = pPluginManager->GetNowTime();
   
	return true;
}

bool NFJsPBModule::Init()
{
	std::cout << "init123445.." << std::endl;

	m_pLogModule = this->pPluginManager->FindModule<NFILogModule>();


    return true;
}

bool NFJsPBModule::AfterInit()
{

    return true;
}

bool NFJsPBModule::Shut()
{
	delete m_pFactory;
	m_pFactory = nullptr;

	delete m_pImporter;
	m_pImporter = nullptr;

    return true;
}

bool NFJsPBModule::ReadyExecute()
{
	return true;
}

bool NFJsPBModule::Execute()
{
    return true;
}

bool NFJsPBModule::BeforeShut()
{
    return true;
}

void NFJsPBModule::ImportProtoFile(const std::string & strFile)
{
	const google::protobuf::FileDescriptor* pDesc = m_pImporter->Import(strFile);
	if (!pDesc)
	{
		m_pLogModule->LogError("unknow protocol  file to import struct name: " + strFile );
	};
}

void NFJsPBModule::SetIsolate(v8::Isolate* pIsolate)
{
	Isolate = pIsolate;
}

void NFJsPBModule::PrintMessage(const google::protobuf::Message & message, const bool bEncode)
{
	return;
	if (bEncode)
	{
		std::cout << "********begin encode***********" << std::endl;
		std::cout << message.DebugString() << std::endl;
		std::cout << "*********end encode************" << std::endl;
	}
	else
	{
		std::cout << "********begin decode***********" << std::endl;
		std::cout << message.DebugString() << std::endl;
		std::cout << "*********end decode************" << std::endl;
	}
}

v8::Local<v8::Value> NFJsPBModule::Decode(const std::string& strMsgTypeName, const std::string& strData)
{
	const google::protobuf::Descriptor* pDescriptor = m_pImporter->pool()->FindMessageTypeByName(strMsgTypeName);
    if (!pDescriptor)
    {
		//throw NFException("unknow message struct name: " + strMsgTypeName);
		return v8::Null(Isolate);
    }

    const google::protobuf::Message* pProtoType = m_pFactory->GetPrototype(pDescriptor);
    if (!pProtoType)
    {
		//throw NFException("cannot find the message body from factory: " + strMsgTypeName);

		return v8::Null(Isolate);
    }

    //GC
    std::shared_ptr<google::protobuf::Message> xMessageBody(pProtoType->New());

    if (xMessageBody->ParseFromString(strData))
    {
		PrintMessage(*xMessageBody, false);
		return MessageToObject(*xMessageBody);
    }

    return v8::Null(Isolate);
}

const std::string NFJsPBModule::Encode(const std::string& strMsgTypeName, const v8::Local<v8::Value>& v8Object)
{
    if (v8Object.IsEmpty() || v8Object->IsNull() || !v8Object->IsObject())
    {
        return NULL_STR;
    }

    const google::protobuf::Descriptor* pDesc = m_pImporter->pool()->FindMessageTypeByName(strMsgTypeName);
    if (!pDesc)
    {
        return NULL_STR;
    }

    const google::protobuf::Message* pProtoType = m_pFactory->GetPrototype(pDesc);
    if (!pProtoType)
    {
        return NULL_STR;
    }

    //GC
    std::shared_ptr<google::protobuf::Message> xMessageBody(pProtoType->New());

	if (ObjectToMessage(v8Object, *xMessageBody))
	{
		PrintMessage(*xMessageBody, true);

		return xMessageBody->SerializeAsString();
	}

	return NULL_STR;
}

v8::Local<v8::Value> NFJsPBModule::MessageToObject(const google::protobuf::Message& message) const
{
	const google::protobuf::Descriptor* pDesc = message.GetDescriptor();
	if (pDesc)
	{
		v8::Local<v8::Object> v8Object = v8::Object::New(Isolate);

		int nField = pDesc->field_count();
		for (int i = 0; i < nField; ++i)
		{
			const google::protobuf::FieldDescriptor* pField = pDesc->field(i);
			if (pField)
			{
				v8Object->Set(Context,v8::String::NewFromUtf8(Isolate,pField->name().data()).ToLocalChecked(),GetField(message, pField));

				if (pField->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE)
				{
					//std::cout << field->name() << " = " << pReflection->GetString(message, field) << std::endl;
					//std::cout << pField->name() << " = " << tbl[pField->name()].value<std::string>() << std::endl;
				}
			}
		}

		return v8Object;
	}


	return v8::Local<v8::Object>();
}

v8::Local<v8::Value> NFJsPBModule::GetField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field) const
{

	if (nullptr == field)
	{
		return v8::Local<v8::Object>();
	}

	if (field->is_repeated())
	{
		return GetRepeatedField(message, field);
	}

	const google::protobuf::Reflection* pReflection = message.GetReflection();
	if (nullptr == pReflection)
	{
		return v8::Local<v8::Object>();
	}

	google::protobuf::FieldDescriptor::CppType eCppType = field->cpp_type();
	switch (eCppType)
	{
		// Scalar field always has a default value.
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		return v8::Int32::New(Isolate, pReflection->GetInt32(message, field));
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		return v8::Integer::New(Isolate,  pReflection->GetInt64(message, field));
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		return v8::Uint32::New(Isolate, pReflection->GetUInt32(message, field));
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		return  v8::Integer::New(Isolate, pReflection->GetUInt64(message, field));
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		return v8::Number::New(Isolate, pReflection->GetDouble(message, field));
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		return v8::Number::New(Isolate, pReflection->GetFloat(message, field));
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		return v8::Boolean::New(Isolate, pReflection->GetBool(message, field));
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
		return v8::Int32::New(Isolate, pReflection->GetEnumValue(message, field));
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
		return v8::String::NewFromUtf8(Isolate, pReflection->GetString(message, field).data()).ToLocalChecked();
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		// For message field, the default value is null.
		if (pReflection->HasField(message, field))
		{
#if NF_PLATFORM == NF_PLATFORM_WIN
#pragma push_macro("GetMessage")
#undef GetMessage
			const google::protobuf::Message& subMsg = pReflection->GetMessage(message, field);
#pragma pop_macro("GetMessage")
#else
			const google::protobuf::Message& subMsg = pReflection->GetMessage(message, field);
#endif
			return MessageToObject(subMsg);
		}
		return  v8::Null(Isolate);
	}

	default:
		break;	
	}


	return v8::Local<v8::Value>();
}

v8::Local<v8::Value> NFJsPBModule::GetRepeatedField(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field) const
{
	if (!field->is_repeated())
	{
		return v8::Object::New(Isolate);
	}

	const google::protobuf::Reflection* pReflection = message.GetReflection();
	if (nullptr == pReflection)
	{
		return v8::Local<v8::Object>();
	}

	v8::Local<v8::Object> v8Object = v8::Object::New(Isolate);
	int nFldSize = pReflection->FieldSize(message, field);
	if (!field->is_map())
	{
		for (int index = 0; index < nFldSize; ++index)
		{
            v8Object->Set(Context,index,GetRepeatedFieldElement(message, field, index));
		}

		return v8Object;
	}

	// map
	if (field->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE)
	{
		return v8Object;
	}

	for (int i = 0; i < nFldSize; ++i)
	{
		const google::protobuf::Message& entryMsg = pReflection->GetRepeatedMessage(message, field, i);
		v8::Local<v8::Object> entryObj =v8::Local<v8::Object>::Cast(MessageToObject(message));
		const v8::Local<v8::Value> key = entryObj->Get(Context,v8::String::NewFromUtf8(Isolate,"key").ToLocalChecked()).ToLocalChecked();
		const v8::Local<v8::Value> value = entryObj->Get(Context,v8::String::NewFromUtf8(Isolate,"value").ToLocalChecked()).ToLocalChecked();
		v8Object->Set(Context,key,value);
	}

	return v8Object;
}

v8::Local<v8::Value> NFJsPBModule::GetRepeatedFieldElement(const google::protobuf::Message& message, const google::protobuf::FieldDescriptor* field, int index) const
{
	if (!field->is_repeated())
	{
		return v8::Local<v8::Value>();
	}

	if (index < 0)
	{
		return v8::Local<v8::Value>();
	}

	const google::protobuf::Reflection* pReflection = message.GetReflection();
	if (nullptr == pReflection)
	{
		return v8::Local<v8::Value>();
	}

	if (index >= pReflection->FieldSize(message, field))
	{
		return v8::Local<v8::Value>();
	}

	google::protobuf::FieldDescriptor::CppType eCppType = field->cpp_type();
	switch (eCppType)
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		return v8::Int32::New(Isolate, pReflection->GetRepeatedInt32(message, field, index));
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		return v8::Integer::New(Isolate, pReflection->GetRepeatedInt64(message, field, index));
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		return v8::Uint32::New(Isolate, pReflection->GetRepeatedUInt32(message, field, index));
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		return v8::Integer::New(Isolate, pReflection->GetRepeatedUInt64(message, field, index));
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		return v8::Number::New(Isolate, pReflection->GetRepeatedDouble(message, field, index));
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		return v8::Number::New(Isolate, pReflection->GetRepeatedFloat(message, field, index));
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		return v8::Boolean::New(Isolate, pReflection->GetRepeatedBool(message, field, index));
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
		return v8::Int32::New(Isolate, pReflection->GetRepeatedEnumValue(message, field, index));
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
		return v8::String::NewFromUtf8(Isolate, pReflection->GetRepeatedString(message, field, index).data()).ToLocalChecked();
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
	{
		const google::protobuf::Message& subMsg = pReflection->GetRepeatedMessage(message, field, index);
		return MessageToObject(subMsg);
	}
	default:
		break;
	}

	return v8::Local<v8::Object>();
}

const bool NFJsPBModule::ObjectToMessage(const v8::Local<v8::Value>& v8Object, google::protobuf::Message& message)
{
	if (!v8Object->IsObject())
	{
		return false;
	}

	v8::Local<v8::Object> obj ;
	if(!v8Object->ToObject(Context).ToLocal(&obj))
	{
		return;
	}
	// non-map
	v8::Local<v8::Array> propertyNames = obj->GetOwnPropertyNames(Context).ToLocalChecked();
    for(uint32_t i=0;i<propertyNames->Length();i++){
		v8::Local<v8::Value> propKey = propertyNames->Get(Context,i).ToLocalChecked();
		if(!propKey->IsString()){	continue;}
		v8::Local<v8::String> propName = propKey->ToString(Context).ToLocalChecked();
		v8::Local<v8::Value> propValue = obj->Get(Context,propName).ToLocalChecked();
		if(!propValue->IsObject())
		{	

		}
		SetField(message, *v8::String::Utf8Value(Isolate,propName), propValue);
	}
	return true;
}

void NFJsPBModule::SetField(google::protobuf::Message& message, const std::string & sField, const v8::Local<v8::Value>& v8Value)
{
	const google::protobuf::FieldDescriptor* pField = message.GetDescriptor()->FindFieldByName(sField);
	if (!pField)
	{
		return;
	}

	if (pField->is_repeated())
	{
		SetRepeatedField(message, pField, v8Value);
		return;
	}

	const google::protobuf::Reflection* pReflection = message.GetReflection();
	if (nullptr == pReflection)
	{
		return;
	}

	google::protobuf::FieldDescriptor::CppType eCppType = pField->cpp_type();
	switch (eCppType)
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		pReflection->SetInt32(&message, pField, v8Value->Int32Value(Context).FromMaybe(0));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		pReflection->SetInt64(&message, pField, v8Value->IntegerValue(Context).FromMaybe(0));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		pReflection->SetUInt32(&message, pField, v8Value->Uint32Value(Context).FromMaybe(0));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		pReflection->SetUInt64(&message, pField, v8Value->IntegerValue(Context).FromMaybe(0));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		pReflection->SetDouble(&message, pField,  v8Value->NumberValue(Context).FromMaybe(0.0));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		pReflection->AddFloat(&message, pField, static_cast<float>(v8Value->NumberValue(Context).FromMaybe(0.0)));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		pReflection->SetBool(&message, pField, v8Value->ToBoolean(Isolate)->Value());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
		// Support enum name.
		pReflection->SetEnumValue(&message, pField, GetEnumValue(message, v8Value, pField));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
		pReflection->SetString(&message, pField, *v8::String::Utf8Value(Isolate,v8Value));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
		if (v8Value->IsObject())
		{
			google::protobuf::Message* pSubMsg = pReflection->MutableMessage(&message, pField);
			if (pSubMsg)
			{
				ObjectToMessage(v8Value, *pSubMsg);
			}
		}
	default:
		break;
	}
}

void NFJsPBModule::SetRepeatedField(google::protobuf::Message& messag, const google::protobuf::FieldDescriptor * field, const v8::Local<v8::Value> & v8Object)
{
	if (!field->is_repeated())
	{
		return;
	}

	if (!v8Object->IsObject())
	{
		return;
	}

	if (field->is_map())
	{
		SetRepeatedMapField(messag, field, v8Object);
		return;
	}

	v8::Local<v8::Object> obj ;
	if(!v8Object->ToObject(Context).ToLocal(&obj))
	{
		return;
	}
	// non-map
	v8::Local<v8::Array> propertyNames = obj->GetOwnPropertyNames(Context).ToLocalChecked();
    for(uint32_t i=0;i<propertyNames->Length();i++){
		v8::Local<v8::String> propName = propertyNames->Get(Context,i).ToLocalChecked();
		v8::Local<v8::Value> propValue = obj->Get(Context,propName).ToLocalChecked();
		AddToRepeatedField(messag, field, propValue);
	}
}

void NFJsPBModule::SetRepeatedMapField(google::protobuf::Message& messag, const google::protobuf::FieldDescriptor * field, const v8::Local<v8::Value>& v8Object)
{
	if (!field->is_repeated())
	{
		return;
	}

	if (!field->is_map())
	{
		return;
	}

	if (!v8Object->IsObject())
	{
		return;
	}
	v8::Local<v8::Object> obj ;
	if(!v8Object->ToObject(Context).ToLocal(&obj))
	{
		return;
	}
	
	v8::Local<v8::Array> propertyNames = obj->GetOwnPropertyNames(Context).ToLocalChecked();
    for(uint32_t i=0;i<propertyNames->Length();i++){
		v8::Local<v8::String> propName = propertyNames->Get(Context,i).ToLocalChecked().As<v8::String>();
		v8::Local<v8::Value> propValue = obj->Get(Context,propName).ToLocalChecked();

		AddToMapField(messag, field, propName, propValue);
	}
}

void NFJsPBModule::AddToRepeatedField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor * field, const v8::Local<v8::Value> & v8Value)
{
	if (!field->is_repeated())
	{
		return;
	}

	if (field->is_map())
	{
		return;
	}

	const google::protobuf::Reflection* pReflection = message.GetReflection();
	if (nullptr == pReflection)
	{
		return;
	}

	google::protobuf::FieldDescriptor::CppType eCppType = field->cpp_type();
	switch (eCppType)
	{
	case google::protobuf::FieldDescriptor::CPPTYPE_INT32:
		pReflection->AddInt32(&message, field, v8Value->Int32Value(Context).FromMaybe(0));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_INT64:
		pReflection->AddInt64(&message, field, v8Value->ToNumber(Context).ToLocalChecked()->Value());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT32:
		pReflection->AddUInt32(&message, field, v8Value->ToUint32(Context).ToLocalChecked()->Value());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_UINT64:
		pReflection->AddUInt64(&message, field, v8Value->IntegerValue(Context).FromMaybe(0));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_DOUBLE:
		pReflection->AddDouble(&message, field,  v8Value->ToNumber(Context).ToLocalChecked()->Value());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_FLOAT:
		pReflection->AddFloat(&message, field, static_cast<float>(v8Value->NumberValue(Context).FromMaybe(0.0)));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_BOOL:
		pReflection->AddBool(&message, field, v8Value->ToBoolean(Isolate)->Value());
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
		// Support enum name.
		pReflection->AddEnumValue(&message, field, GetEnumValue(message, v8Value, field));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
		pReflection->AddString(&message, field, *v8::String::Utf8Value(Isolate,v8Value));
		break;
	case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
		if (v8Value->IsObject())
		{
			google::protobuf::Message* pSubMsg = pReflection->AddMessage(&message, field);
			if (pSubMsg)
			{
				ObjectToMessage(v8Value, *pSubMsg);
			}
		}
		break;
	default:
		break;
	}
}

void NFJsPBModule::AddToMapField(google::protobuf::Message& message, const google::protobuf::FieldDescriptor * field, const v8::Local<v8::String> & key, const v8::Local<v8::Value> & val)
{
	if (!field->is_repeated())
	{
		return;
	}

	if (!field->is_map())
	{
		return;
	}

	if (field->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE)
	{
		return;
	}

	const google::protobuf::Reflection* pReflection = message.GetReflection();
	if (nullptr == pReflection)
	{
		return;
	}

	google::protobuf::Message* pMapEntry = pReflection->AddMessage(&message, field);
	if (!pMapEntry)
	{
		return;
	}

	const google::protobuf::Reflection* pMapReflection = pMapEntry->GetReflection();
	if (nullptr == pMapReflection)
	{
		return;
	}
	v8::String::Utf8Value utf8Value(Isolate,key);
	std::string strKey(*utf8Value);

	SetField(*pMapEntry,strKey, val);
}

int NFJsPBModule::GetEnumValue(google::protobuf::Message& messag, const v8::Local<v8::Value> & v8Value, const google::protobuf::FieldDescriptor * field) const
{
	if (!v8Value->IsString())
	{
		return v8Value->ToInt32(Context).ToLocalChecked()->Value();
	}

	std::string sEnum(*v8::String::Utf8Value(Isolate,v8Value));
	if (field->cpp_type() != google::protobuf::FieldDescriptor::CPPTYPE_ENUM)
	{
		return 0;
	}

	const google::protobuf::EnumDescriptor* pEnum = field->enum_type();
	if (!pEnum)
	{
		return 0;
	}

	const google::protobuf::EnumValueDescriptor* pEnumVal = pEnum->FindValueByName(sEnum);
	if (pEnumVal)
	{
		return pEnumVal->number();
	}

	return v8Value->ToInt32(Context).ToLocalChecked()->Value(); // "123" -> 123
}
