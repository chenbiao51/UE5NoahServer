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

#include "HelloWorld1.h"
#include <iostream>
#include <algorithm>
#include <mongocxx/client.hpp>
#include "mongocxx/instance.hpp"
#include "bsoncxx/types.hpp"
#include <bsoncxx/json.hpp>
#include <bsoncxx/builder/stream/document.hpp>


bool NFHelloWorld1::Init()
{
    m_pLogModule = pPluginManager->FindModule<NFILogModule>();

	std::cout << typeid(NFHelloWorld1).name() << std::endl;

    m_pLogModule->LogInfo("hahahahahhahahhahaxxxxx------------------");
    std::cout << "Hello, world1, Init" << std::endl;

    AfterInit();
    return true;
}

bool NFHelloWorld1::AfterInit()
{
    
    std::cout << "Hello, world1, AfterInit" << std::endl;
    m_pLogModule->LogInfo("ppppppppppppppp------------------");

	NFDataList dataList;
	dataList.Add("1");
	dataList.AddFloat(2.0f);
	dataList.AddObject(NFGUID(3,3));
	dataList.AddVector2(NFVector2(4.0f, 4.0f));

	for (int i = 0; i < dataList.GetCount(); ++i)
	{
		std::cout << dataList.ToString(i) << std::endl;
	}

    mongocxx::instance  inst{};
    mongocxx::client conn{mongocxx::uri{}};

    bsoncxx::builder::stream::document document{};

    m_pLogModule->LogInfo("xxxxxxx------------------");
    auto collection = conn["testdb"]["testcollection"];
    document <<"hello" <<"world";
	
    collection.insert_one(document.view());
    auto cursor = collection.find({});

    for (auto&& doc : cursor)
    {
        m_pLogModule->LogInfo("ooooooooooo------------------");
        m_pLogModule->LogInfo(bsoncxx::to_json(doc));
    }

    return true;
}

bool NFHelloWorld1::Execute()
{
    
    //std::cout << "Hello, world1, Execute" << std::endl;

    return true;
}

bool NFHelloWorld1::BeforeShut()
{
    std::cout << "Hello, world1, BeforeShut1111" << std::endl;
    // std::ostringstream stream;
	// stream << "Hello, world1, BeforeShut1111-------------------" << std::endl;

    system("PAUSE");

    
    std::cout << "Hello, world1, BeforeShut" << std::endl;

    return true;
}

bool NFHelloWorld1::Shut()
{
    
    std::cout << "Hello, world1, Shut" << std::endl;

    return true;
}
