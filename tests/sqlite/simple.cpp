//
// Created by denn nevera on 2019-07-28.
//


#include "sqlite/sqlite_orm.h"
#include "gtest/gtest.h"
#include <list>
#include "dehancer/Log.h"
#include "dehancer/Common.h"

struct User {
    int id{};
    std::string firstName;
    std::string lastName;
    int birthDate{};
    std::unique_ptr<std::string> imageUrl;
    int typeId{};
};

struct UserType {
    int id;
    std::string name;
};

class OneUserCache: public dehancer::Singleton<User>{
public:
    OneUserCache() = default;
};

TEST(SQLITE, SimpleTest) {
  using namespace sqlite_orm;
  
  #if defined(DEHANCER_CONTROLLED_SINGLETON)
  OneUserCache::CreateInstance();
  #endif
  OneUserCache::Instance().id = 0;
  
  try {
    auto storage = make_storage("./test.db",
                                make_table("users",
                                           make_column("id", &User::id, autoincrement(), primary_key()),
                                           make_column("first_name", &User::firstName),
                                           make_column("last_name", &User::lastName),
                                           make_column("birth_date", &User::birthDate),
                                           make_column("image_url", &User::imageUrl),
                                           make_column("type_id", &User::typeId)),
                                make_table("user_types",
                                           make_column("id", &UserType::id, autoincrement(), primary_key()),
                                           make_column("name", &UserType::name, default_value("name_placeholder"))));
    
    storage.sync_schema();
    
    User user{-1, "Jonh", "Doe", (int)time(0), std::make_unique<std::string>("url_to_heaven"), 3 };
    
    auto insertedId = storage.insert(user);
    std::cout << "insertedId = " << insertedId << std::endl;      //  insertedId = 8
    user.id = insertedId;
    
    User secondUser{-1, "Alice", "Inwonder", (int)time(0), {} , 2};
    insertedId = storage.insert(secondUser);
    secondUser.id = insertedId;
    
    auto allUsersList = storage.get_all<User>();
    
    for(auto &user : allUsersList) {
      std::cout << storage.dump(user) << std::endl;
    }
    
    std::cout << "===" << std::endl;
    
    auto firstName = storage.get_all<User>(where(c(&User::firstName) == "Jonh" and c(&User::id) < 2 ));
    
    for(auto &user : firstName) {
      std::cout << storage.dump(user) << std::endl;
    }
    
    auto idsone = storage.get_all<User>(where(c(&User::id) == 1 ));
    
    if (!idsone.empty()) {
      auto user = idsone.begin();
      user->firstName += "#0";
      storage.update(*user);
    }
    dehancer::log::print("sqlite wrote %i objects", insertedId);
    
    dehancer::log::print(" OneUserCache::Instance().id  = %i",  OneUserCache::Instance().id );
    
    #if defined(DEHANCER_CONTROLLED_SINGLETON)
    OneUserCache::DestroyInstance();
    #endif
  }
  catch(std::system_error& e) {
    std::cerr << "System Error: " << e.what() << std::endl;
    dehancer::log::error(true, "sqlite error: %s", e.what());
  }
  catch (std::exception& e) {
    std::cerr << "Error: " << e.what() << std::endl;
    dehancer::log::error(true, "sqlite error: %s", e.what());
  }
}