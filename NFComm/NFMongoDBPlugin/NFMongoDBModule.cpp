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

#include <iostream>
#include <algorithm>
#include "NFMongoDBModule.h"
#include "NFMongoDBClient.h"
#include "bsoncxx/types.hpp"
#include "NFComm/NFMessageDefine/NFProtocolDefine.hpp"


std::unique_ptr<mongocxx::instance> NFMongoDBModule::m_pMongoInstance = nullptr;

NFMongoDBModule::NFMongoDBModule(NFIPluginManager* p)
{
    pPluginManager = p;

    if(!m_pMongoInstance.get())
    {
        m_pMongoInstance = std::make_unique<mongocxx::instance>();
    }

    m_pMongoDBClient = new NFMongoDBClient();
}

NFMongoDBModule::~NFMongoDBModule()
{
    delete(m_pMongoDBClient);
}

bool NFMongoDBModule::Init()
{
    return true;
}

bool NFMongoDBModule::Shut()
{
    return true;
}

bool NFMongoDBModule::Execute()
{
    return true;
}

bool NFMongoDBModule::AfterInit()
{
    return true;
}

NFIMongoDBClient* NFMongoDBModule::GetMongoDBClient()
{
    return m_pMongoDBClient;
}

bool NFMongoDBModule::AddMongoDBInfo(const std::string& strDBName, const std::string& strIpPort, const std::string& strUserName, const std::string&strPassword)
{
    if(m_pMongoDBClient->Connect(strDBName, strIpPort, strUserName, strPassword))
    {
        const std::string strURl = m_pMongoDBClient->GetMongoUrl();
        std::cout << "connect uri: " << strURl << " succes " << std::endl;
        return true;
    }
    else
    {
        const std::string strURl = m_pMongoDBClient->GetMongoUrl();
        std::cout << "connect uri: " << strURl << " fail" << std::endl;
        return false;
    }

    return true;
}

bool NFMongoDBModule::FindOne(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, std::map<std::string, NFData>& xFiledValue)
{
    if(xFiledValue.size() == 0)
    {
        std::cout << "field size = 0" << std::endl;
        return true;
    }

    mongocxx::options::find xOpts{};
    const NFData xValue = xKeyValue.second;
    bsoncxx::builder::stream::document xOptsDocument;
    bsoncxx::builder::stream::document xFilter;

    if(!FillValueToDoc(xFilter, xKeyValue.first, xValue))
    {
        std::cout << "FillValueToDoc error" << std::endl;
        return false;
    }

    bool bHaveID = false;
    for(auto &it : xFiledValue)
    {
        xOptsDocument << it.first << 1;
    }

    if(xFiledValue.find("_id") == xFiledValue.end())
    {
        xOptsDocument << "_id" << 0;
    }

    xOpts.projection(xOptsDocument.view());
    //opts.max_time(); 

    std::list<bsoncxx::document::value> listRusult;
    if(!m_pMongoDBClient->Query(strCollection, xFilter, xOpts, listRusult))
    {
        std::cout << "mongo Query error" << std::endl;
        return false;
    }

    for(auto &it : listRusult)
    {
        std::cout << bsoncxx::to_json(it.view()) << std::endl;
        for(auto &itField : xFiledValue)
        {
            if(!GetValueFromBsonDoc(it.view(), itField.first, itField.second))
            {
                std::cout << "GetValueFromBsonDoc error" << std::endl;
                return false;
            }
        }

        //peek one
        break;
    }

    return true;
}

bool NFMongoDBModule::Insert(const std::string& strCollection, const std::list<std::pair<std::string, NFData>>& listFieldValue)
{
    bsoncxx::builder::stream::document xDocument{};
    for(auto &it : listFieldValue)
    {
        if(!FillValueToDoc(xDocument, it.first, it.second))
        {
            std::cout << "FillValueToDoc error" << std::endl;
            return false;
        }
    }
    return m_pMongoDBClient->InsertOne(strCollection, xDocument.view(), mongocxx::options::insert());
    //return true;
}

bool NFMongoDBModule::UnInsertSet(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, const std::list<std::pair<std::string, NFData>>& listFieldValue)
{
    bsoncxx::builder::stream::document xFilter;
    if(!FillValueToDoc(xFilter, xKeyValue.first, xKeyValue.second))
    {
        std::cout << "FillValueToDoc error" << std::endl;
        return false;
    }

    bsoncxx::builder::stream::document xDocument;
    xDocument << "$set" << bsoncxx::builder::stream::open_document;
    for(auto &it : listFieldValue)
    {
        if(!FillValueToDoc(xDocument, it.first, it.second))
        {
            std::cout << "FillValueToDoc error" << std::endl;
            return false;
        }
    }
    xDocument << bsoncxx::builder::stream::close_document;

    mongocxx::options::update xOptions;
    xOptions.upsert(false);

    return m_pMongoDBClient->UpdateOne(strCollection, xFilter.view(), xDocument.view(), xOptions);
}

bool NFMongoDBModule::InsertSet(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, const std::list<std::pair<std::string, NFData>>& listFieldValue)
{
    bsoncxx::builder::stream::document xFilter;
    if(!FillValueToDoc(xFilter, xKeyValue.first, xKeyValue.second))
    {
        std::cout << "FillValueToDoc error" << std::endl;
        return false;
    }

    bsoncxx::builder::stream::document xDocument;
    xDocument << "$set" << bsoncxx::builder::stream::open_document;
    for(auto &it : listFieldValue)
    {
        if(!FillValueToDoc(xDocument, it.first, it.second))
        {
            std::cout << "FillValueToDoc error" << std::endl;
            return false;
        }
    }
    xDocument << bsoncxx::builder::stream::close_document;

    mongocxx::options::update xOptions;
    xOptions.upsert(true);

    return m_pMongoDBClient->UpdateOne(strCollection, xFilter.view(), xDocument.view(), xOptions);
}

bool NFMongoDBModule::Delete(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, bool bDelOne/* = true*/)
{
    bsoncxx::builder::stream::document xFilter;
    if(!FillValueToDoc(xFilter, xKeyValue.first, xKeyValue.second))
    {
        std::cout << "FillValueToDoc error" << std::endl;
        return false;
    }
    mongocxx::options::delete_options xOptions;

    if(bDelOne)
    {
        if(!m_pMongoDBClient->DeleteOne(strCollection, xFilter.view(), xOptions))
        {
            std::cout << "delete one failed" << std::endl;
            return false;
        }
    }
    else
    {
        if(!m_pMongoDBClient->DeleteMany(strCollection, xFilter.view(), xOptions))
        {
            std::cout << "delete many failed" << std::endl;
            return false;
        }
    }
    return true;
}

bool NFMongoDBModule::GetValueFromBsonDoc(const bsoncxx::document::view& xDoc, const std::string& strField, NFData& xValue)
{
    auto xDataType = xDoc[strField].type();
    switch(xDataType)
    {

    case bsoncxx::type::k_bool:
        xValue.SetBool(xDoc[strField].get_bool().value);
        break;
    case bsoncxx::type::k_timestamp:
        xValue.SetInt((int64_t)(xDoc[strField].get_timestamp().timestamp));
        break;
    case bsoncxx::type::k_int64:
        {
            if(xValue.GetType() == TDATA_OBJECT)
            {
                //xValue.SetObject(xDoc[strField].get_int64().value);
            }
            else
            {
                xValue.SetInt(xDoc[strField].get_int64().value);
            }
        }
        break;
    case bsoncxx::type::k_int32:
        xValue.SetInt32(xDoc[strField].get_int32().value);
        break;
    case bsoncxx::type::k_double:
        xValue.SetDouble(xDoc[strField].get_double().value);
        break;
    case bsoncxx::type::k_utf8:
        xValue.SetString(xDoc[strField].get_utf8().value.to_string().c_str());
        break;
    //case bsoncxx::type::k_binary:
    //    xDoc[strField].get_binary();
    //    break;
    default:
        std::cout << "unsupport value type :" << (int64_t)xDataType <<  std::endl;
        return false;
    }
    return true;
}

bool NFMongoDBModule::FillValueToDoc(bsoncxx::builder::stream::document& xDoc, const std::string& strField, const NFData& xValue)
{
    switch(xValue.GetType())
    {
    case NFDATA_TYPE::TDATA_BOOLEAN:
        {
            xDoc << strField << xValue.GetBool();
        }
        break;
    case NFDATA_TYPE::TDATA_INT32:
        {
            xDoc << strField << (int32_t)xValue.GetInt32();
        }
        break;
    case NFDATA_TYPE::TDATA_INT:
        {
            xDoc << strField << xValue.GetInt();
        }
        break;
    case NFDATA_TYPE::TDATA_FLOAT:
        {
            xDoc << strField << xValue.GetFloat();
        }
        break;
    case NFDATA_TYPE::TDATA_DOUBLE:
        {
            xDoc << strField << xValue.GetDouble();
        }
        break;
    case NFDATA_TYPE::TDATA_STRING:
        {
            xDoc << strField << xValue.GetString();
        }
        break;
    case NFDATA_TYPE::TDATA_OBJECT:
        {
            xDoc << strField << (int64_t)xValue.GetObject().nData64;
        }
        break;
    case NFDATA_TYPE::TDATA_VECTOR2:
    case NFDATA_TYPE::TDATA_VECTOR3:
    case NFDATA_TYPE::TDATA_UNKNOWN:
        {
            std::cout << "Cannot save key value type " << strField  << std::endl;
            return false;
        }
    default:
        {
            std::cout << "Unknow key value type " << strField << std::endl;
            return false;
        }
        break;
    }
    return true;
}