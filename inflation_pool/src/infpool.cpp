#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/singleton.hpp>

#define DEBUG

using namespace eosio;
using std::string;

namespace enterprise {
   class [[eosio::contract]] infpool : public contract {
      public:
         infpool(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds) { }

         [[eosio::action]]
         void test() {

         }

      private:
         
   };
}

EOSIO_DISPATCH(enterprise::infpool, (test))