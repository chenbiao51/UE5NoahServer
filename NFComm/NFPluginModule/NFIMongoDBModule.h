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


#ifndef NFI_MONGODB_MODULE_H
#define NFI_MONGODB_MODULE_H

#include <string>
#include <memory>
#include <list>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <mongocxx/pool.hpp>
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include "NFComm/NFPluginModule/NFIModule.h"



class NFIMongoDBClient
{
	public:
    enum ClientType
    {
        EClientType_Single = 0,
        EClientType_Pool = 1,
    };

    struct ConnectInfo
    {
        ConnectInfo() : nMinPoolSize(0), nMaxPoolSize(0), nClientType(0), nWriteTimeOut(0)
        {

        }
        int nMinPoolSize;
        int nMaxPoolSize;
        int nClientType;
        int64_t nWriteTimeOut;

        std::string strIPPort;
        std::string strUserName;
        std::string strPassword;
        std::string strDBName;
        std::string strAuthMechanism;
        std::string strUrl;
    };

public:
    virtual mongocxx::pool::entry GetClient() = 0;
    virtual const std::string& GetMongoUrl() = 0;
    virtual bool Connect(const std::string& strDBName, const std::string& strIPPort, const std::string& strUserName, const std::string&strPassword) = 0;
    //
    virtual bool Aggregate(const std::string& strCollection, const mongocxx::pipeline& xPipeline,
                           const mongocxx::options::aggregate& xOptions, std::list<bsoncxx::document::value>& listRusult) = 0;

    virtual bool Count(const std::string& strCollection, int64_t &nCount, const bsoncxx::document::view_or_value& xFilter, const mongocxx::options::count& xOptions) = 0;
    //virtual bool CreateIndex(const std::string& strCollection, const bsoncxx::document::view_or_value& xKeys, const mongocxx::options::index& xOptions, bsoncxx::document::value& xResult) = 0;

    virtual bool Distinct(const std::string& strCollection, const bsoncxx::string::view_or_value& xDistinctField,
                          const bsoncxx::document::view_or_value& xFilter, const  mongocxx::options::distinct& xOpts,
                          std::list<bsoncxx::document::value>& listRusult) = 0;

    virtual void Drop(const std::string& strCollection) = 0;

    virtual bool BulkWrite(const std::string& strCollection, const mongocxx::options::bulk_write& xBulkWrite) = 0;

    virtual bool DeleteMany(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter, const mongocxx::options::delete_options& xOptions) = 0;
    virtual bool DeleteOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter, const mongocxx::options::delete_options& xOptions) = 0;

    virtual bool Query(const std::string& strCollection, const bsoncxx::builder::stream::document& xFilter, const mongocxx::options::find& xOpts, std::list<bsoncxx::document::value>& listRusult) = 0;

    virtual bool FindOneAndDelete(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                  const mongocxx::options::find_one_and_delete& xOptions, bsoncxx::document::value& xResult) = 0;

    virtual bool FindOneAndReplace(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                   const bsoncxx::document::view_or_value& xReplacement, const mongocxx::options::find_one_and_replace& xOptions, bsoncxx::document::value& xResult) = 0;

    virtual bool FindOneAndUpdate(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                  const bsoncxx::document::view_or_value& xUpdate, const mongocxx::options::find_one_and_update& xOptions, bsoncxx::document::value& xResult) = 0;

    virtual bool InsertOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xDocument,
                           const mongocxx::options::insert& xOptions) = 0;

    virtual bool InsertMany(const std::string& strCollection, const std::list<bsoncxx::document::view_or_value> xDocuments, const mongocxx::options::insert& xOptions) = 0;

    virtual bool ReplaceOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                            const bsoncxx::document::view_or_value& xReplacement, const mongocxx::options::update& xOptions) = 0;

    virtual bool UpdateMany(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                            const bsoncxx::document::view_or_value& xUpdate, const mongocxx::options::update& xOptions) = 0;

    virtual bool UpdateOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                           const bsoncxx::document::view_or_value& xUpdate, const mongocxx::options::update& xOptions) = 0;

    virtual bool RenameCollection(const std::string& strCollection, const bsoncxx::string::view_or_value& xNewName, bool bDropTargetBeforeRename = false) = 0;
    virtual bool SetReadPreference(const std::string& strCollection, const mongocxx::read_preference& xRP) = 0;
    virtual bool SetReadConcern(const std::string& strCollection, const mongocxx::read_concern& xRP) = 0;
    virtual bool SetWriteConcern(const std::string& strCollection, const mongocxx::write_concern& xWC) = 0;
};

class NFIMongoDBModule: public NFIModule
{
public:
    virtual bool Init() = 0;
    virtual NFIMongoDBClient* GetMongoDBClient() = 0;
    virtual bool AddMongoDBInfo(const std::string& strDBName, const std::string& strIpPort, const std::string& strUserName, const std::string&strPassword) = 0;
    virtual bool FindOne(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, std::map<std::string, NFData>& xFiledValue) = 0;
    virtual bool Insert(const std::string& strCollection, const std::list<std::pair<std::string, NFData>>& listFieldValue) = 0;
    virtual bool UnInsertSet(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, const std::list<std::pair<std::string, NFData> >& listFieldValue) = 0;
    virtual bool InsertSet(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, const std::list<std::pair<std::string, NFData> >& listFieldValue) = 0;
    virtual bool Delete(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, bool bDelOne = true) = 0;
};

#endif