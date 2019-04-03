#include <eosiolib/contract.hpp>
#include <eosiolib/transaction.hpp>

using namespace eosio;

namespace test {
    class [[eosio::contract]] test : public contract {
        public:
            test(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds) { }

            ACTION testact( name user ) {
                require_auth( user );
            }
    };
}