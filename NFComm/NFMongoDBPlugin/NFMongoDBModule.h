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

#ifndef NF_DATAMONGODB_MODULE_H
#define NF_DATAMONGODB_MODULE_H

#include <memory>
#include "mongocxx/instance.hpp"
#include "NFComm/NFCore/NFDataList.hpp"
#include "NFMongoDBClient.h"
#include <bsoncxx/document/value.hpp>
#include <bsoncxx/document/view_or_value.hpp>
#include "NFComm/NFPluginModule/NFPlatform.h"
#include "NFComm/NFPluginModule/NFIPluginManager.h"
#include "NFComm/NFPluginModule/NFIMongoDBModule.h"
#include "NFComm/NFPluginModule/NFIClassModule.h"
#include "NFComm/NFPluginModule/NFIElementModule.h"
#include "NFComm/NFPluginModule/NFILogModule.h"


class NFMongoDBModule : public NFIMongoDBModule
{
public:
public:
    NFMongoDBModule(NFIPluginManager* p);
    virtual ~NFMongoDBModule();
    virtual bool Init();
    virtual bool Shut();
    virtual bool Execute();

    virtual bool AfterInit();

    virtual NFIMongoDBClient* GetMongoDBClient();
    virtual bool AddMongoDBInfo(const std::string& strDBName, const std::string& strIpPort, const std::string& strUserName, const std::string&strPassword);
    virtual bool FindOne(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, std::map<std::string, NFData>& xFiledValue);
    virtual bool Insert(const std::string& strCollection, const std::list<std::pair<std::string, NFData>>& listFieldValue);
    virtual bool UnInsertSet(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, const std::list<std::pair<std::string, NFData>>& listFieldValue);
    virtual bool InsertSet(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, const std::list<std::pair<std::string, NFData>>& listFieldValue);
    virtual bool Delete(const std::string& strCollection, const std::pair<std::string, NFData>& xKeyValue, bool bDelOne = true);

private:
    bool GetValueFromBsonDoc(const bsoncxx::document::view& xDoc, const std::string& strField, NFData& xValue);
    bool FillValueToDoc(bsoncxx::builder::stream::document& xDoc, const std::string& strField, const NFData& xValue);

private:
    NFIMongoDBClient* m_pMongoDBClient;
private:
    static std::unique_ptr<mongocxx::instance> m_pMongoInstance;
};


#endif