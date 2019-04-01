#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/singleton.hpp>

#define DEBUG

using namespace eosio;
using std::string;

namespace enterprise {
   class [[eosio::contract]] rewardspool : public contract {
      public:
         rewardspool(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds) { }

         [[eosio::action]]
         void test() {

         }

      private:
         struct [[eosio::table]] reward_action {

         }
   };
}

EOSIO_DISPATCH(enterprise::rewardspool, (test))