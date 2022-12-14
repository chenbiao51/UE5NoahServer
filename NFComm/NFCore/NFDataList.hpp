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



#ifndef NF_DATALIST_H
#define NF_DATALIST_H

#include <cstdarg>
#include <string>
#include <cstring>
#include <iostream>
#include <stdlib.h>
#include <math.h>
#include <list>
#include <vector>
#include <map>
#include <algorithm>
#include <assert.h>
#include <map>
#include <typeinfo>
#include <iomanip>
#include <functional>
#include <iostream>
#include <fstream>
#include <memory>
#include <variant>
#include "NFMemoryCounter.h"
#include "NFComm/NFPluginModule/NFGUID.h"
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFCore/NFVector2.hpp"
#include "NFComm/NFCore/NFVector3.hpp"



enum NFDATA_TYPE
{
    TDATA_UNKNOWN,
	TDATA_BOOLEAN,  
	TDATA_INT32,
    TDATA_INT,      
    TDATA_FLOAT,
	TDATA_DOUBLE,    
    TDATA_STRING,   
    TDATA_OBJECT,   
	TDATA_VECTOR2,
	TDATA_VECTOR3,
    TDATA_MAX,
};

const static std::string NULL_STR = "";
const static NFGUID NULL_OBJECT = NFGUID();
const static bool   NULL_BOOLEAN = false;
const static float  NULL_FLOAT = 0.0;
const static double NULL_DOUBLE = 0.0;
const static NFINT32 NULL_INT32 = 0;
const static NFINT64 NULL_INT = 0;
const static NFVector2 NULL_VECTOR2 = NFVector2();
const static NFVector3 NULL_VECTOR3 = NFVector3();

struct NFData
{
public:
	NFData()
	{
		type = TDATA_UNKNOWN;
	}

	NFData(NFDATA_TYPE eType)
	{
		bind = true;
		type = eType;
	}
	NFData(bool value)
	{
		bind = true;
		this->SetBool(value);
	}
	NFData(int32_t value)
	{
		bind = true;
		this->SetInt32(value);
	}
	
	NFData(int64_t value)
	{
		bind = true;
		this->SetInt(value);
	}

	NFData(float value)
	{
		bind = true;
		this->SetFloat(value);
	}

	NFData(double value)
	{
		bind = true;
		this->SetDouble(value);
	}

	NFData(NFGUID value)
	{
		bind = true;
		this->SetObject(value);
	}

	NFData(NFVector2 value)
	{
		bind = true;
		this->SetVector2(value);
	}

	NFData(NFVector3 value)
	{
		bind = true;
		this->SetVector3(value);
	}

	NFData(const NFData& value)
	{
		bind = value.bind;
		type = value.type;
		variantData = value.variantData;
	}

	~NFData()
	{
		type = TDATA_UNKNOWN;
	}

	inline bool operator==(const NFData& src) const
	{
		//&& src.variantData == variantData
		if (src.GetType() == GetType())
		{
			switch (GetType())
			{
				case TDATA_BOOLEAN:
			    {
					if (src.GetBool() == GetBool())
				    {
					    return true;
				    }
			    }
			    	break;
				case TDATA_INT32:
			    {
					if (src.GetInt32() == GetInt32())
				    {
					    return true;
				    }
			    }
			    case TDATA_INT:
			    {
				    if (src.GetInt() == GetInt())
				    {
					    return true;
				    }
			    }
			   	 	break;
				case TDATA_FLOAT:
			    {
				    float fValue = GetFloat() - src.GetFloat();
				    if (fValue < 0.001  && fValue > -0.001)
				    {
					    return true;
				    }
			    }
			    	break;
			    case TDATA_DOUBLE:
			    {
				    double fValue = GetDouble() - src.GetDouble();
				    if (fValue < 0.00001  && fValue > -0.00001)
				    {
					    return true;
				    }
			    }
			    	break;
			    case TDATA_STRING:
			    {
				    if (src.GetString() == GetString())
				    {
					    return true;
				    }
			    }
			    	break;
				case TDATA_OBJECT:
				{
					if (src.GetObject() == GetObject())
					{
						return true;
					}
				}
					break;
				case TDATA_VECTOR2:
				{
					if (src.GetVector2() == GetVector2())
					{
						return true;
					}
				}
					break;
				case TDATA_VECTOR3:
				{
					if (src.GetVector3() == GetVector3())
					{
						return true;
					}
				}
					break;
				default:
					break;
			}
		}
		return false;
	}

	void Reset()
	{
		variantData = std::variant<bool,NFINT32,NFINT64,float, double, std::string, NFGUID, NFVector2, NFVector3>();
		type = TDATA_UNKNOWN;
		/*switch (GetType())
		{
			case TDATA_INT:
			{
				SetInt(0);
			}
				break;
			case TDATA_FLOAT:
			{
				SetFloat(0);
			}
				break;
			case TDATA_STRING:
			{
				SetString("");
			}
				break;
			case TDATA_OBJECT:
			{
				SetObject(NFGUID());
			}
				break;
			case TDATA_VECTOR2:
			{
				SetVector2(NFVector2());
			}
				break;
			case TDATA_VECTOR3:
			{
				SetVector3(NFVector3());
			}
				break;
			default:
				break;
		}*/
	}

	bool IsNullValue() const
	{
		bool bChanged = false;

		switch (GetType())
		{
			case TDATA_BOOLEAN:
			{
				bChanged = true;
			}
				break;
			case TDATA_INT32:
			{
				if (0 != GetInt32())
				{
					bChanged = true;
				}
			}
			case TDATA_INT:
			{
				if (0 != GetInt())
				{
					bChanged = true;
				}
			}
				break;
			case TDATA_FLOAT:
			{
				float fValue = GetFloat();
				if (fValue > 0.001 || fValue < -0.001)
				{
					bChanged = true;
				}
			}
				break;
			case TDATA_DOUBLE:
			{
				float fValue = GetDouble();
				if (fValue > 0.00001 || fValue < -0.00001)
				{
					bChanged = true;
				}
			}
				break;
			case TDATA_STRING:
			{
				const std::string& strData = GetString();
				if (!strData.empty())
				{
					bChanged = true;
				}
			}
				break;
			case TDATA_OBJECT:
			{
				if (!GetObject().IsNull())
				{
					bChanged = true;
				}
			}
				break;
			case TDATA_VECTOR2:
			{
				if (!GetVector2().IsZero())
				{
					bChanged = true;
				}
			}
				break;
			case TDATA_VECTOR3:
			{
				if (!GetVector3().IsZero())
				{
					bChanged = true;
				}
			}
				break;
			default:
				break;
		}

		return !bChanged;
	}

	NFDATA_TYPE GetType() const
	{
		return type;
	}

    void SetBool(const bool var)
	{
		if (type == TDATA_BOOLEAN || TDATA_UNKNOWN == type)
		{
			type = TDATA_BOOLEAN;
			variantData = (bool)var;
		}
	}

    void SetInt32(const NFINT32 var)
	{
		if (type == TDATA_INT32 || TDATA_UNKNOWN == type)
		{
			type = TDATA_INT32;
			variantData = (NFINT32)var;
		}
	}

	void SetInt(const NFINT64 var)
	{
		if (type == TDATA_INT || TDATA_UNKNOWN == type)
		{
			type = TDATA_INT;
			variantData = (NFINT64)var;
		}
	}

    void SetFloat(const float var)
	{
		if (type == TDATA_FLOAT || TDATA_UNKNOWN == type)
		{
			type = TDATA_FLOAT;
			variantData = (float)var;
		}
	}

	void SetDouble(const double var)
	{
		if (type == TDATA_DOUBLE || TDATA_UNKNOWN == type)
		{
			type = TDATA_DOUBLE;
			variantData = (double)var;
		}
	}

	void SetString(const std::string& var)
	{
		if (type == TDATA_STRING || TDATA_UNKNOWN == type)
		{
			type = TDATA_STRING;
			variantData = (std::string)var;
		}
	}

	void SetObject(const NFGUID var)
	{
		if (type == TDATA_OBJECT || TDATA_UNKNOWN == type)
		{
			type = TDATA_OBJECT;
			variantData = (NFGUID)var;
		}
	}

	void SetVector2(const NFVector2 var)
	{
		if (type == TDATA_VECTOR2 || TDATA_UNKNOWN == type)
		{
			type = TDATA_VECTOR2;
			variantData = (NFVector2)var;
		}
	}

	void SetVector3(const NFVector3 var)
	{
		if (type == TDATA_VECTOR3 || TDATA_UNKNOWN == type)
		{
			type = TDATA_VECTOR3;
			variantData = (NFVector3)var;
		}
	}

	bool GetBool() const
	{
		if (TDATA_BOOLEAN == type)
		{
			return std::get<bool>(variantData);
		}

		return NULL_BOOLEAN;
	}

    NFINT32 GetInt32() const
	{
		if (TDATA_INT32 == type)
		{
			return std::get<NFINT32>(variantData);
		}

		return NULL_INT32;
	}

    
	NFINT64 GetInt() const
	{
		if (TDATA_INT == type)
		{
			return std::get<NFINT64>(variantData);
		}

		return NULL_INT;
	}

    float GetFloat() const
	{
		if (TDATA_FLOAT == type)
		{
			return std::get<float>(variantData);
		}

		return NULL_FLOAT;
	}
	

	double GetDouble() const
	{
		if (TDATA_DOUBLE == type)
		{
			return std::get<double>(variantData);
		}

		return NULL_DOUBLE;
	}
	const std::string& GetString() const
	{
		if (TDATA_STRING == type)
		{
			return std::get<std::string>(variantData);
		}

		return NULL_STR;
	}

	const NFGUID& GetObject() const
	{
		if (TDATA_OBJECT == type)
		{
			return std::get<NFGUID>(variantData);
		}

		return NULL_OBJECT;
	}

	const NFVector2& GetVector2() const
	{
		if (TDATA_VECTOR2 == type)
		{
			return std::get<NFVector2>(variantData);
		}

		return NULL_VECTOR2;
	}

	const NFVector3& GetVector3() const
	{
		if (TDATA_VECTOR3 == type)
		{
			return std::get<NFVector3>(variantData);
		}

		return NULL_VECTOR3;
	}

	std::string ToString() const
	{
		std::string strData;

		switch (type)
		{
			case TDATA_BOOLEAN:
				strData = GetBool()?"true":"false";
				break;

			case TDATA_INT32:
				strData = lexical_cast<std::string> (GetInt32());
				break;

			case TDATA_INT:
				strData = lexical_cast<std::string> (GetInt());
				break;

			case TDATA_FLOAT:
				strData = lexical_cast<std::string> (GetFloat());
				break;

            case TDATA_DOUBLE:
				strData = lexical_cast<std::string> (GetDouble());
				break;

			case TDATA_STRING:
				strData = GetString();
				break;

			case TDATA_OBJECT:
				strData = GetObject().ToString();
				break;

			case TDATA_VECTOR2:
				strData = GetVector2().ToString();
				break;

			case TDATA_VECTOR3:
				strData = GetVector3().ToString();
				break;

			default:
				strData = NULL_STR;
				break;
		}
		return strData;
	}

	bool FromString(const std::string& strData)
	{
		try
		{
			switch (type)
			{
				case TDATA_BOOLEAN:
					SetBool(strData=="true"?true:false);
					break;

				case TDATA_INT32:
					SetInt32(lexical_cast<int32_t> (strData));
					break;

				case TDATA_INT:
					SetInt(lexical_cast<int64_t> (strData));
					break;

				case TDATA_FLOAT:
					SetFloat(lexical_cast<float> (strData));
					break;

				case TDATA_DOUBLE:
					SetDouble(lexical_cast<double> (strData));
					break;

				case TDATA_STRING:
					SetString(strData);
					break;

				case TDATA_OBJECT:
				{
					NFGUID xID;
					xID.FromString(strData);
					SetObject(xID);
				}
				break;

				case TDATA_VECTOR2:
				{
					NFVector2 v;
					v.FromString(strData);
					SetVector2(v);
				}
				break;

				case TDATA_VECTOR3:
				{
					NFVector3 v;
					v.FromString(strData);
					SetVector3(v);
				}
				break;

				default:
					break;
			}

			return true;
		}
		catch (...)
		{
			return false;
		}
		
		return false;
	}

private:

	NFDATA_TYPE type;
	bool bind = false;

public:
	std::variant<bool,NFINT32,NFINT64,float, double, std::string, NFGUID, NFVector2, NFVector3> variantData;
	//mapbox::util::variant<NFINT64, double, std::string, NFGUID, NFVector2, NFVector3> variantData;
};

class NFDataList :public NFMemoryCounter
{
public:
    NFDataList() : NFMemoryCounter(GET_CLASS_NAME(NFDataList))
    {
        mnUseSize = 0;
        mvList.reserve(STACK_SIZE);
        for (int i = 0; i < STACK_SIZE; ++i)
        {
            mvList.push_back(NF_SHARE_PTR<NFData>(NF_NEW NFData()));
        }
    }

	NFDataList(const std::string& data, const std::string& strSplit) : NFMemoryCounter(GET_CLASS_NAME(NFDataList))
	{
		mnUseSize = 0;
		mvList.reserve(STACK_SIZE);
		for (int i = 0; i < STACK_SIZE; ++i)
		{
			mvList.push_back(NF_SHARE_PTR<NFData>(NF_NEW NFData()));
		}

		this->Split(data, strSplit);
	}

	virtual ~NFDataList()
	{
	}

	static const NFDataList& Empty()
	{
		static NFDataList data;
		return data;
	}

	virtual std::string ToString() const
	{
		std::stringstream os;
		for (int i = 0; i < GetCount(); ++i)
		{
			os << ToString(i);
			if (i < GetCount() - 1)
			{
				os << "|";
			}
		}

		return os.str();
	}

	virtual void ToMemoryCounterString(std::string& data)
	{
		
	}

	virtual std::string ToString(const int index) const
	{
		std::string strData;

		if (ValidIndex(index))
		{
			const NFDATA_TYPE eType = Type(index);
			switch (eType)
			{
				case TDATA_BOOLEAN:
					strData = Bool(index)?"true":"false";
					break;
				case TDATA_INT32:
					strData = lexical_cast<std::string> (Int32(index));
					break;
				case TDATA_INT:
					strData = lexical_cast<std::string> (Int(index));
					break;

				case TDATA_FLOAT:
					strData = lexical_cast<std::string> (Float(index));
					break;
				case TDATA_DOUBLE:
					strData = lexical_cast<std::string> (Double(index));
					break;

				case TDATA_STRING:
					strData = String(index);
					break;

				case TDATA_OBJECT:
					strData = Object(index).ToString();
					break;

				case TDATA_VECTOR2:
					strData = Vector2(index).ToString();
					break;

				case TDATA_VECTOR3:
					strData = Vector3(index).ToString();
					break;

				default:
					strData = NULL_STR;
					break;
			}
		}

		return strData;
	}

	virtual bool ToString(std::string& str, const std::string& strSplit) const
	{
		for (int i = 0; i < GetCount(); ++i)
		{
			std::string strVal = ToString(i);
			str += strVal;
			str += strSplit;
		}

		std::string strTempSplit(strSplit);
		std::string::size_type nPos = str.rfind(strSplit);
		if (nPos == str.length() - strTempSplit.length())
		{
			str = str.substr(0, nPos);
		}

		return true;
	}

public:

	virtual const NF_SHARE_PTR<NFData> GetStack(const int index) const
	{
		if (index < (int)mvList.size())
		{
			return mvList[index];
		}

		return NF_SHARE_PTR<NFData>();
	}

	virtual bool Concat(const NFDataList& src)
	{
		InnerAppendEx(src, 0, src.GetCount());
		return true;
	}
    
	virtual bool Append(const NFDataList& src)
	{
		return Append(src, 0, src.GetCount());
	}

	virtual bool Append(const NFDataList& src, const int start, const int count)
	{
		if (start >= src.GetCount())
		{
			return false;
		}

		int end = start + count;

		if (end > src.GetCount())
		{
			return false;
		}

		InnerAppendEx(src, start, end);

		return true;
	}
    
	virtual bool Append(const NFData& xData)
	{
		if (xData.GetType() <= TDATA_UNKNOWN
			|| xData.GetType() >= TDATA_MAX)
		{
			return false;
		}

		switch (xData.GetType())
		{
			case TDATA_BOOLEAN:
				AddBool(xData.GetBool());
				break;
			case TDATA_INT32:
				AddInt32(xData.GetInt32());
				break;
			case TDATA_INT:
				AddInt(xData.GetInt());
				break;
			case TDATA_FLOAT:
				AddFloat(xData.GetFloat());
				break;
			case TDATA_DOUBLE:
				AddDouble(xData.GetDouble());
				break;
			case TDATA_OBJECT:
				AddObject(xData.GetObject());
				break;
			case TDATA_STRING:
				AddString(xData.GetString());
				break;
			case TDATA_VECTOR2:
				AddVector2(xData.GetVector2());
				break;
			case TDATA_VECTOR3:
				AddVector3(xData.GetVector3());
				break;
			default:
				break;
		}

		return true;
	}
    
	virtual void Clear()
	{
		mnUseSize = 0;

		if (mvList.size() > STACK_SIZE)
		{
			mvList.erase(mvList.begin() + STACK_SIZE, mvList.end());
		}
		for (int i = 0; i < STACK_SIZE; ++i)
		{
			mvList[i]->Reset();
		}
	}
    
	virtual bool IsEmpty() const
	{
		return (0 == mnUseSize);
	}
    
	virtual int GetCount() const
	{
		return mnUseSize;
	}
    
	virtual NFDATA_TYPE Type(const int index) const
	{
		if (!ValidIndex(index))
		{
			return TDATA_UNKNOWN;
		}

		if (index < STACK_SIZE)
		{
			return mvList[index]->GetType();
		}
		else
		{
			const NF_SHARE_PTR<NFData> pData = GetStack(index);
			if (pData)
			{
				return pData->GetType();
			}
		}

		return TDATA_UNKNOWN;
	}
    
	virtual bool TypeEx(const  int type, ...) const
	{

		bool bRet = true;

		if (TDATA_UNKNOWN == type)
		{
			bRet = false;
			return bRet;
		}

		NFDATA_TYPE pareType = (NFDATA_TYPE)type;
		va_list arg_ptr;
		va_start(arg_ptr, type);
		int index = 0;

		while (pareType != TDATA_UNKNOWN)
		{

			NFDATA_TYPE varType = Type(index);
			if (varType != pareType)
			{
				bRet = false;
				break;
			}

			++index;
			pareType = (NFDATA_TYPE)va_arg(arg_ptr, int);
		}

		va_end(arg_ptr);

		return bRet;
	}
    
	virtual bool Split(const std::string& str, const std::string& strSplit)
	{
		Clear();

		std::string strData(str);
		if (strData.empty())
		{
			return true;
		}

		std::string temstrSplit(strSplit);
		std::string::size_type pos;
		strData += temstrSplit;
		std::string::size_type size = strData.length();

		for (std::string::size_type i = 0; i < size; i++)
		{
			pos = int(strData.find(temstrSplit, i));
			if (pos < size)
			{
				std::string strSub = strData.substr(i, pos - i);
				Add(strSub.c_str());

				i = pos + temstrSplit.size() - 1;
			}
		}

		return true;
	}

	virtual bool Add(const bool value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		NF_SHARE_PTR<NFData> var = GetStack(GetCount());
		if (var)
		{
			var->SetBool(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool Add(const NFINT32 value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		NF_SHARE_PTR<NFData> var = GetStack(GetCount());
		if (var)
		{
			var->SetInt32(value);
			mnUseSize++;

			return true;
		}

		return false;
	}
    
	virtual bool Add(const NFINT64 value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		NF_SHARE_PTR<NFData> var = GetStack(GetCount());
		if (var)
		{
			var->SetInt(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool Add(const float value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		NF_SHARE_PTR<NFData> var = GetStack(GetCount());
		if (var)
		{
			var->SetFloat(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool Add(const double value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		NF_SHARE_PTR<NFData> var = GetStack(GetCount());
		if (var)
		{
			var->SetDouble(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool Add(const std::string& value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		NF_SHARE_PTR<NFData> var = GetStack(GetCount());
		if (var)
		{
			var->SetString(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool Add(const NFGUID& value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		NF_SHARE_PTR<NFData> var = GetStack(GetCount());
		if (var)
		{
			var->SetObject(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool Add(const NFVector2& value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		NF_SHARE_PTR<NFData> var = GetStack(GetCount());
		if (var)
		{
			var->SetVector2(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool Add(const NFVector3& value)
	{
		if (GetCount() == (int)(mvList.size()))
		{
			AddStatck();
		}

		NF_SHARE_PTR<NFData> var = GetStack(GetCount());
		if (var)
		{
			var->SetVector3(value);
			mnUseSize++;

			return true;
		}

		return false;
	}

	virtual bool Set(const int index, const bool value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_BOOLEAN)
		{
			NF_SHARE_PTR<NFData> var = GetStack(index);
			if (var)
			{
				var->SetBool(value);
				return true;
			}
		}
		return false;
	}

	virtual bool Set(const int index, const NFINT32 value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_INT32)
		{
			NF_SHARE_PTR<NFData> var = GetStack(index);
			if (var)
			{
				var->SetInt32(value);
				return true;
			}
		}
		return false;
	}

	virtual bool Set(const int index, const NFINT64 value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_INT)
		{
			NF_SHARE_PTR<NFData> var = GetStack(index);
			if (var)
			{
				var->SetInt(value);

				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const float value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_FLOAT)
		{
			NF_SHARE_PTR<NFData> var = GetStack(index);
			if (var)
			{
				var->SetFloat(value);
				return true;
			}
		}
		return false;
	}

	virtual bool Set(const int index, const double value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_DOUBLE)
		{
			NF_SHARE_PTR<NFData> var = GetStack(index);
			if (var)
			{
				var->SetDouble(value);
				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const std::string& value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_STRING)
		{
			NF_SHARE_PTR<NFData> var = GetStack(index);
			if (var)
			{
				var->SetString(value);

				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const NFGUID& value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_OBJECT)
		{
			NF_SHARE_PTR<NFData> var = GetStack(index);
			if (var)
			{
				var->SetObject(value);

				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const NFVector2& value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_VECTOR2)
		{
			NF_SHARE_PTR<NFData> var = GetStack(index);
			if (var)
			{
				var->SetVector2(value);

				return true;
			}
		}

		return false;
	}

	virtual bool Set(const int index, const NFVector3& value)
	{
		if (ValidIndex(index) && Type(index) == TDATA_VECTOR3)
		{
			NF_SHARE_PTR<NFData> var = GetStack(index);
			if (var)
			{
				var->SetVector3(value);

				return true;
			}
		}

		return false;
	}

	virtual bool Bool(const int index) const
	{
		if (ValidIndex(index))
		{
			if (Type(index) == TDATA_BOOLEAN)
			{
				const NF_SHARE_PTR<NFData> var = GetStack(index);
				return var->GetBool();
			}
		}
		return 0;
	}

	virtual NFINT32 Int32(const int index) const
	{
		if (ValidIndex(index))
		{
			if (Type(index) == TDATA_INT32)
			{
				const NF_SHARE_PTR<NFData> var = GetStack(index);
				return var->GetInt32();
			}
		}
		return 0;
	}
    
	virtual NFINT64 Int(const int index) const
	{
		if (ValidIndex(index))
		{
			if (Type(index) == TDATA_INT)
			{
				const NF_SHARE_PTR<NFData> var = GetStack(index);
				return var->GetInt();
			}
		}

		return 0;
	}

	

	virtual float Float(const int index) const
	{
		if (ValidIndex(index))
		{
			const NF_SHARE_PTR<NFData> var = mvList[index];
			if (var && TDATA_FLOAT == var->GetType())
			{
				return var->GetFloat();
			}
		}

		return 0.0f;
	}

	virtual double Double(const int index) const
	{
		if (ValidIndex(index))
		{
			const NF_SHARE_PTR<NFData> var = mvList[index];
			if (var && TDATA_DOUBLE == var->GetType())
			{
				return var->GetDouble();
			}
		}

		return 0.0f;
	}

	virtual const std::string& String(const int index) const
	{
		if (ValidIndex(index))
		{
			const NF_SHARE_PTR<NFData> var = mvList[index];
			if (var && TDATA_STRING == var->GetType())
			{
				return var->GetString();
			}
		}

		return NULL_STR;
	}

	virtual const NFGUID& Object(const int index) const
	{
		if (ValidIndex(index))
		{
			NFDATA_TYPE type = Type(index);
			if (TDATA_OBJECT == type)
			{
				NF_SHARE_PTR<NFData> var = GetStack(index);
				if (var)
				{
					return var->GetObject();
				}
			}
		}

		return NULL_OBJECT;
	}

	virtual const NFVector2& Vector2(const int index) const
	{
		if (ValidIndex(index))
		{
			NFDATA_TYPE type = Type(index);
			if (TDATA_VECTOR2 == type)
			{
				NF_SHARE_PTR<NFData> var = GetStack(index);
				if (var)
				{
					return var->GetVector2();
				}
			}
		}

		return NULL_VECTOR2;
	}

	virtual const NFVector3& Vector3(const int index) const
	{
		if (ValidIndex(index))
		{
			NFDATA_TYPE type = Type(index);
			if (TDATA_VECTOR3 == type)
			{
				NF_SHARE_PTR<NFData> var = GetStack(index);
				if (var)
				{
					return var->GetVector3();
				}
			}
		}

		return NULL_VECTOR3;
	}

    bool AddBool(const bool value)
    {
        return Add(value);
    }
 	bool AddInt32(const NFINT32 value)
    {
        return Add(value);
    }
    bool AddInt(const NFINT64 value)
    {
        return Add(value);
    }
    bool AddFloat(const float value)
    {
        return Add(value);
    }
	bool AddDouble(const double value)
    {
        return Add(value);
    }
    bool AddString(const std::string& value)
    {
        return Add(value);
    }
    bool AddStringFromChar(const char* value)
    {
        return Add(value);
    }
    bool AddObject(const NFGUID& value)
    {
        return Add(value);
    }
	bool AddVector2(const NFVector2& value)
	{
		return Add(value);
	}
	bool AddVector3(const NFVector3& value)
	{
		return Add(value);
	}

  	bool SetBool(const int index, const bool value)
    {
        return Set(index, value);
    }
  	bool SetInt32(const int index, const NFINT32 value)
    {
        return Set(index, value);
    }
    bool SetInt(const int index, const NFINT64 value)
    {
        return Set(index, value);
    }
    bool SetFloat(const int index, const float value)
    {
        return Set(index, value);
    }
	bool SetDouble(const int index, const double value)
    {
        return Set(index, value);
    }
    bool SetString(const int index, const std::string& value)
    {
        return Set(index, value);
    }
    bool SetObject(const int index, const NFGUID& value)
    {
        return Set(index, value);
    }
	bool SetVector2(const int index, const NFVector2& value)
	{
		return Set(index, value);
	}
	bool SetVector3(const int index, const NFVector3& value)
	{
		return Set(index, value);
	}



    inline bool Compare(const int nPos, const NFDataList& src) const
    {
        if (src.GetCount() > nPos
            && GetCount() > nPos
            && src.Type(nPos) == Type(nPos))
        {
            switch (src.Type(nPos))
            {
				case TDATA_BOOLEAN:
                    return Bool(nPos) == src.Bool(nPos);
                    break;

               case TDATA_INT32:
                    return Int32(nPos) == src.Int32(nPos);
                    break;

                case TDATA_INT:
                    return Int(nPos) == src.Int(nPos);
                    break;

                case TDATA_FLOAT:
                    return fabs(Float(nPos) - src.Float(nPos)) < 0.001f;
                    break;

				case TDATA_DOUBLE:
                    return fabs(Double(nPos) - src.Double(nPos)) < 0.00001f;
                    break;

                case TDATA_STRING:
                    return String(nPos) == src.String(nPos);
                    break;

                case TDATA_OBJECT:
                    return Object(nPos) == src.Object(nPos);
                    break;

				case TDATA_VECTOR2:
					return Vector2(nPos) == src.Vector2(nPos);
					break;

				case TDATA_VECTOR3:
					return Vector3(nPos) == src.Vector3(nPos);
					break;

                default:
                    return false;
                    break;
            }
        }

        return false;
    }


    inline bool operator==(const NFDataList& src) const
    {
        if (src.GetCount() == GetCount())
        {
            for (int i = 0; i < GetCount(); i++)
            {
                if (!Compare(i, src))
                {
                    return false;
                }
            }

            return true;
        }

        return false;
    }

    inline bool operator!=(const NFDataList& src)
    {
        return !(*this == src);
    }
	inline NFDataList& operator<<(const bool value)
    {
        Add(value);
        return *this;
    }
	inline NFDataList& operator<<(const float value)
    {
        Add(value);
        return *this;
    }
    inline NFDataList& operator<<(const double value)
    {
        Add(value);
        return *this;
    }
    inline NFDataList& operator<<(const char* value)
    {
        Add(value);
        return *this;
    }
    inline NFDataList& operator<<(const std::string& value)
    {
        Add(value);
        return *this;
    }

    inline NFDataList& operator<<(const NFINT64& value)
    {
        Add(value);
        return *this;
    }
    inline NFDataList& operator<<(const NFINT32& value)
    {
        Add((NFINT32)value);
        return *this;
    }
    inline NFDataList& operator<<(const NFGUID& value)
    {
        Add(value);
        return *this;
    }
	inline NFDataList& operator<<(const NFVector2& value)
	{
		Add(value);
		return *this;
	}
	inline NFDataList& operator<<(const NFVector3& value)
	{
		Add(value);
		return *this;
	}
    inline NFDataList& operator<<(const NFDataList& value)
    {
        Concat(value);
        return *this;
    }
    enum { STACK_SIZE = 8 };

protected:

	bool ValidIndex(int index) const
	{
		return (index < GetCount()) && (index >= 0);
	}

	void AddStatck()
	{
		for (int i = 0; i < STACK_SIZE; ++i)
		{
			NF_SHARE_PTR<NFData> pData(NF_NEW  NFData());
			mvList.push_back(pData);
		}
	}

	void InnerAppendEx(const NFDataList& src, const int start, const int end)
	{
		for (int i = start; i < end; ++i)
		{
			NFDATA_TYPE vType = src.Type(i);
			switch (vType)
			{
				case TDATA_BOOLEAN:
					AddBool(src.Bool(i));
					break;
				case TDATA_INT32:
					AddInt32(src.Int32(i));
					break;
				case TDATA_INT:
					AddInt(src.Int(i));
					break;
				case TDATA_FLOAT:
					AddFloat(src.Float(i));
					break;
				case TDATA_DOUBLE:
					AddDouble(src.Double(i));
					break;
				case TDATA_STRING:
					AddString(src.String(i));
					break;
				case TDATA_OBJECT:
					AddObject(src.Object(i));
					break;
				case TDATA_VECTOR2:
					AddVector2(src.Vector2(i));
					break;
				case TDATA_VECTOR3:
					AddVector3(src.Vector3(i));
					break;
				default:
					break;
			}
		}
	}
protected:

    int mnUseSize;
    std::vector< NF_SHARE_PTR<NFData> > mvList;
    std::map<std::string, NF_SHARE_PTR<NFData> > mxMap;
};

const static NFData NULL_TDATA = NFData();

#endif