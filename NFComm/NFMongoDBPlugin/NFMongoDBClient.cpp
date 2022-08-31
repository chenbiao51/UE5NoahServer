/*
            This file is part of: 
                NoahFrame
            https://github.com/ketoo/NoahGameFrame

   Copyright 2009 - 2021 NoahFrame(NoahGameFrame)

   File creator: lvsheng.huang
   
   NoahFrame is open-source software and you can MongoDBtribute it and/or modify
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

#include "NFMongoDBClient.h"
#include <iostream>
#include <ostream>
#include <sstream>
#include <thread>
#include <vector>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/bulk_write.hpp>
#include <mongocxx/exception/exception.hpp>
#include <mongocxx/exception/query_exception.hpp>
#include <mongocxx/exception/bulk_write_exception.hpp>
#include <mongocxx/exception/logic_error.hpp>
#include <mongocxx/exception/write_exception.hpp>

#include <NFComm/NFPluginModule/NFPlatform.h>
#include <NFComm/NFCore/NFDataList.hpp>
#define  MONGOCATCH(function, line)     catch(mongocxx::query_exception er)\
    {\
        std::cout<< "Mongo query Error:"<< er.what() << " Function:" << function << " Line:" << line << std::endl;\
        return false;\
    }\
    catch(mongocxx::bulk_write_exception er)\
    {\
        std::cout<< "Mongo bulk_write Error:"<< er.what() << " Function:" << function << " Line:" << line << std::endl;\
        return false;\
    }\
    catch(mongocxx::logic_error er)\
    {\
        std::cout<< "Mongo logic Error:"<< er.what() << " Function:" << function << " Line:" << line << std::endl;\
        return false;\
    }\
    catch(mongocxx::write_exception er)\
    {\
        std::cout<< "Mongo write Error:"<< er.what() << " Function:" << function << " Line:" << line << std::endl;\
        return false;\
    }\
    catch(mongocxx::operation_exception er)\
    {\
        bEnable = false;\
        std::cout<< "Mongo operation Error:"<< er.what() << " Function:" << function << " Line:" << line << std::endl;\
        return false;\
    }\
    catch(mongocxx::exception er)\
    {\
        bEnable = false;\
        std::cout<< "Mongo Error:"<< er.what() << " Function:" << function << " Line:" << line << std::endl;\
        return false;\
    }\
    catch (...)\
    {\
        std::cout<< "Mongo Error: Unknow Exception" << " Function:" << function << " Line:" << line << std::endl;\
        return false;\
    }

bool NFMongoDBClient::Connect(const std::string& strDBName, const std::string& strIPPort, const std::string& strUserName, const std::string&strPassword)
{
    if(bEnable == true)
    {
        return true;
    }
    try
    {
        m_strDBName = strDBName;

        mxConnectInfo.strIPPort = strIPPort;
        mxConnectInfo.strUserName = strUserName;
        mxConnectInfo.strPassword = strPassword;
        mxConnectInfo.strAuthMechanism;
        mxConnectInfo.nMinPoolSize = 1;
        mxConnectInfo.nMaxPoolSize = 4;
        mxConnectInfo.nClientType = EClientType_Pool;

        mxConnectInfo.strUrl = MakeMongoUrl(mxConnectInfo);
        m_xMongoPool = new mongocxx::pool{ mongocxx::uri(mxConnectInfo.strUrl) };
    }
    MONGOCATCH(__FUNCTION__, __LINE__);

    bEnable = true;
    return true;
}
//
bool NFMongoDBClient::Aggregate(const std::string& strCollection, const mongocxx::pipeline& xPipeline, const mongocxx::options::aggregate& xOptions, std::list<bsoncxx::document::value>& listRusult)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        auto xCursor = xClient->database(m_strDBName)[strCollection].aggregate(xPipeline, xOptions);
        for(auto && doc : xCursor)
        {
            listRusult.push_back(bsoncxx::document::value(doc));
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);

    return true;
}
bool NFMongoDBClient::Count(const std::string& strCollection, int64_t &nCount, const bsoncxx::document::view_or_value& xFilter, const mongocxx::options::count& xOptions)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        //nCount = xClient->database(m_strDBName)[strCollection].count(xFilter, xOptions);
    }
    MONGOCATCH(__FUNCTION__, __LINE__);

    return true;
}
/*
bool NFMongoDBClient::CreateIndex(const std::string& strCollection, const bsoncxx::document::view_or_value& xKeys, const mongocxx::options::index& xOptions, bsoncxx::document::value& xResult)
{

}
*/
bool NFMongoDBClient::Distinct(const std::string& strCollection, const bsoncxx::string::view_or_value& xDistinctField, const bsoncxx::document::view_or_value& xFilter, const  mongocxx::options::distinct& xOpts, std::list<bsoncxx::document::value>& listRusult)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        auto xCursor = xClient->database(m_strDBName)[strCollection].distinct(xDistinctField, xFilter, xOpts);
        for(auto && doc : xCursor)
        {
            listRusult.push_back(bsoncxx::document::value(doc));
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);

    return true;
}
void NFMongoDBClient::Drop(const std::string& strCollection)
{
    if(!bEnable)
    {
        return;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        xClient->database(m_strDBName)[strCollection].drop();
    }
    catch(mongocxx::operation_exception er)
    {
        std::cout << "Mongo Operation Error:" << er.what() << " Function:" << __FUNCTION__ << " Line:" << __LINE__ << std::endl;
        return;
    }
    return;
}

bool NFMongoDBClient::BulkWrite(const std::string& strCollection, const mongocxx::options::bulk_write& xBulkWrite)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        // mongocxx::stdx::optional<mongocxx::result::bulk_write> xResult = xClient->database(m_strDBName)[strCollection].bulk_write(xBulkWrite);
        // if(!xResult)
        // {
        //     std::cout << "Mongo bulk_write failed, Function:" << __FUNCTION__ << " Line:" << __LINE__ << std::endl;
        //     return false;
        // }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);

    return true;
}

bool NFMongoDBClient::DeleteMany(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter, const mongocxx::options::delete_options& xOptions)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        mongocxx::stdx::optional<mongocxx::result::delete_result> xResult = xClient->database(m_strDBName)[strCollection].delete_many(xFilter, xOptions);
        if(!xResult)
        {
            std::cout << "Mongo delete_many failed, Function:" << __FUNCTION__ << " Line:" << __LINE__ << std::endl;
            return false;
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}
bool NFMongoDBClient::DeleteOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter, const mongocxx::options::delete_options& xOptions)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        mongocxx::stdx::optional<mongocxx::result::delete_result> xResult = xClient->database(m_strDBName)[strCollection].delete_one(xFilter, xOptions);
        if(!xResult)
        {
            std::cout << "Mongo delete_one failed, Function:" << __FUNCTION__ << " Line:" << __LINE__ << std::endl;
            return false;
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::Query(const std::string& strCollection, const bsoncxx::builder::stream::document& xFilter, const mongocxx::options::find& xOpts, std::list<bsoncxx::document::value>& listRusult)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        auto xCursor = xClient->database(m_strDBName)[strCollection].find(xFilter.view(), xOpts);
        for(auto && doc : xCursor)
        {
            listRusult.push_back(bsoncxx::document::value(doc));
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::FindOneAndDelete(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                      const mongocxx::options::find_one_and_delete& xOptions, bsoncxx::document::value& xResult)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        mongocxx::stdx::optional<bsoncxx::document::value> xValue = xClient->database(m_strDBName)[strCollection].find_one_and_delete(xFilter, xOptions);
        if(xValue)
        {
            //xResult = xValue.get();
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::FindOneAndReplace(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                       const bsoncxx::document::view_or_value& xReplacement, const mongocxx::options::find_one_and_replace& xOptions, bsoncxx::document::value& xResult)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        mongocxx::stdx::optional<bsoncxx::document::value> xValue = xClient->database(m_strDBName)[strCollection].find_one_and_replace(xFilter, xReplacement, xOptions);
        if(xValue)
        {
            //xResult = xValue.get();
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::FindOneAndUpdate(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                      const bsoncxx::document::view_or_value& xUpdate, const mongocxx::options::find_one_and_update& xOptions, bsoncxx::document::value& xResult)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        mongocxx::stdx::optional<bsoncxx::document::value> xValue = xClient->database(m_strDBName)[strCollection].find_one_and_update(xFilter, xUpdate, xOptions);
        if(xValue)
        {
            //xResult = xValue.get();
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::InsertOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xDocument,
                               const mongocxx::options::insert& xOptions)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        mongocxx::stdx::optional<mongocxx::result::insert_one> xValue = xClient->database(m_strDBName)[strCollection].insert_one(xDocument, xOptions);
        if(!xValue)
        {
            std::cout << "insert one failed, Function:" << __FUNCTION__ << " Line:" << __LINE__ << std::endl;
            return false;
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::InsertMany(const std::string& strCollection, const std::list<bsoncxx::document::view_or_value> xDocuments, const mongocxx::options::insert& xOptions)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        mongocxx::stdx::optional<mongocxx::result::insert_many> xValue = xClient->database(m_strDBName)[strCollection].insert_many(xDocuments, xOptions);
        if(!xValue)
        {
            std::cout << "insert_many failed, Function:" << __FUNCTION__ << " Line:" << __LINE__ << std::endl;
            return false;
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::ReplaceOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                const bsoncxx::document::view_or_value& xReplacement, const mongocxx::options::update& xOptions)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        mongocxx::stdx::optional<mongocxx::result::update> xValue = xClient->database(m_strDBName)[strCollection].update_one(xFilter, xReplacement, xOptions);
        if(!xValue)
        {
            std::cout << "replace one failed, Function:" << __FUNCTION__ << " Line:" << __LINE__ << std::endl;
            return false;
        }
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::UpdateMany(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                const bsoncxx::document::view_or_value& xUpdate, const mongocxx::options::update& xOptions)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        xClient->database(m_strDBName)[strCollection].update_many(xFilter, xUpdate, xOptions);
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::UpdateOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter, const bsoncxx::document::view_or_value& xUpdate, const mongocxx::options::update& xOptions)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        xClient->database(m_strDBName)[strCollection].update_one(xFilter, xUpdate, xOptions);
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}

bool NFMongoDBClient::RenameCollection(const std::string& strCollection, const bsoncxx::string::view_or_value& xNewName, bool bDropTargetBeforeRename/* = false*/)
{
    if(!bEnable)
    {
        return false;
    }
    try
    {
        auto xClient = m_xMongoPool->acquire();
        xClient->database(m_strDBName)[strCollection].rename(xNewName, bDropTargetBeforeRename);
    }
    MONGOCATCH(__FUNCTION__, __LINE__);
    return true;
}
bool NFMongoDBClient::SetReadPreference(const std::string& strCollection, const mongocxx::read_preference& xRP)
{
    if(!bEnable)
    {
        return false;
    }

    auto xClient = m_xMongoPool->acquire();
    xClient->database(m_strDBName)[strCollection].read_preference(xRP);
    return true;
}
bool NFMongoDBClient::SetReadConcern(const std::string& strCollection, const mongocxx::read_concern& xRP)
{
    if(!bEnable)
    {
        return false;
    }

    auto xClient = m_xMongoPool->acquire();
    xClient->database(m_strDBName)[strCollection].read_concern(xRP);
    return true;
}
bool NFMongoDBClient::SetWriteConcern(const std::string& strCollection, const mongocxx::write_concern& xWC)
{
    if(!bEnable)
    {
        return false;
    }

    auto xClient = m_xMongoPool->acquire();
    xClient->database(m_strDBName)[strCollection].write_concern(xWC);
    return true;
}

mongocxx::pool::entry NFMongoDBClient::GetClient()
{
    return m_xMongoPool->acquire();
}

const std::string NFMongoDBClient::MakeMongoUrl(const ConnectInfo& xinfo)
{
    std::string strUri;
    strUri += "mongodb://";
    if(!xinfo.strUserName.empty() && !xinfo.strPassword.empty())
    {
        strUri = strUri + xinfo.strUserName + ":" + xinfo.strPassword + "@";
    }

    strUri += xinfo.strIPPort;

    bool bFirstOption = true;

    if(!xinfo.strUserName.empty() && !xinfo.strPassword.empty())
    {
        strUri = strUri + "/?authSource=" + xinfo.strDBName;
        bFirstOption = false;
    }

    if(xinfo.nClientType == EClientType_Pool)
    {
        std::string strMinPoolSize = std::to_string(xinfo.nMinPoolSize);
        std::string strMaxPoolSize = std::to_string(xinfo.nMaxPoolSize);
        strUri = strUri + (bFirstOption ? "/?" : "&") + "minPoolSize=" + strMinPoolSize + "&maxPoolSize=" + strMaxPoolSize;
    }

    return strUri;
}

const std::string& NFMongoDBClient::GetMongoUrl()
{
    return mxConnectInfo.strUrl;
}
