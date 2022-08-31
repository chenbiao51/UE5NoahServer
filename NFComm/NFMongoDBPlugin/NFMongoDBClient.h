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

#ifndef NFMONGODBPLUGIN_NFMONGODBCLIENT_H
#define NFMONGODBPLUGIN_NFMONGODBCLIENT_H

#define GET_NAME(functionName)   (#functionName)

#include <string>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <set>
#include <ctime>
#include <sstream>
#include <iostream>
#include <random>
#include <thread>
#include <ostream>

#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/json.hpp>
#include <bsoncxx/types.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>
#include <mongocxx/uri.hpp>

#include "NFComm/NFPluginModule/NFIMongoDBModule.h"


using bsoncxx::builder::basic::kvp;

class NFMongoDBClient: public NFIMongoDBClient
{
public:
    NFMongoDBClient()
    {
        bEnable = false;
    }

    virtual mongocxx::pool::entry GetClient();
    virtual const std::string MakeMongoUrl(const ConnectInfo& xinfo);
    virtual const std::string& GetMongoUrl();
    virtual bool Connect(const std::string& strDBName, const std::string& strIPPort, const std::string& strUserName, const std::string&strPassword);

    virtual bool Aggregate(const std::string& strCollection, const mongocxx::pipeline& xPipeline, const mongocxx::options::aggregate& xOptions, std::list<bsoncxx::document::value>& listRusult);
    virtual bool Count(const std::string& strCollection, int64_t &nCount, const bsoncxx::document::view_or_value& xFilter, const mongocxx::options::count& xOptions);

    virtual bool Distinct(const std::string& strCollection, const bsoncxx::string::view_or_value& xDistinctField, const bsoncxx::document::view_or_value& xFilter, const  mongocxx::options::distinct& xOpts, std::list<bsoncxx::document::value>& listRusult);
    virtual void Drop(const std::string& strCollection);

    virtual bool BulkWrite(const std::string& strCollection, const mongocxx::options::bulk_write& xBulkWrite);

    virtual bool DeleteMany(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter, const mongocxx::options::delete_options& xOptions);
    virtual bool DeleteOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter, const mongocxx::options::delete_options& xOptions);

    virtual bool Query(const std::string& strCollection, const bsoncxx::builder::stream::document& xFilter, const mongocxx::options::find& xOpts, std::list<bsoncxx::document::value>& listRusult);

    virtual bool FindOneAndDelete(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                  const mongocxx::options::find_one_and_delete& xOptions, bsoncxx::document::value& xResult);

    virtual bool FindOneAndReplace(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                   const bsoncxx::document::view_or_value& xReplacement, const mongocxx::options::find_one_and_replace& xOptions, bsoncxx::document::value& xResult);

    virtual bool FindOneAndUpdate(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                                  const bsoncxx::document::view_or_value& xUpdate, const mongocxx::options::find_one_and_update& xOptions, bsoncxx::document::value& xResult);

    virtual bool InsertOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xDocument,
                           const mongocxx::options::insert& xOptions);
    virtual bool InsertMany(const std::string& strCollection, const std::list<bsoncxx::document::view_or_value> xDocuments, const mongocxx::options::insert& xOptions);
    virtual bool ReplaceOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                            const bsoncxx::document::view_or_value& xReplacement, const mongocxx::options::update& xOptions);

    virtual bool UpdateMany(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                            const bsoncxx::document::view_or_value& xUpdate, const mongocxx::options::update& xOptions);

    virtual bool UpdateOne(const std::string& strCollection, const bsoncxx::document::view_or_value& xFilter,
                           const bsoncxx::document::view_or_value& xUpdate, const mongocxx::options::update& xOptions);

    virtual bool RenameCollection(const std::string& strCollection, const bsoncxx::string::view_or_value& xNewName, bool bDropTargetBeforeRename = false);
    virtual bool SetReadPreference(const std::string& strCollection, const mongocxx::read_preference& xRP);
    virtual bool SetReadConcern(const std::string& strCollection, const mongocxx::read_concern& xRP);
    virtual bool SetWriteConcern(const std::string& strCollection, const mongocxx::write_concern& xWC);
private:
    bool bEnable;
    std::string m_strDBName;
    ConnectInfo mxConnectInfo;
    mongocxx::pool* m_xMongoPool;
};


#endif //NFMongoDBPLUGIN_NFMongoDBCLIENT_H
