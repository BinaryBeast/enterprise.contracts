#pragma once

#include <eosiolib/eosio.hpp>

using namespace eosio;

namespace testcts {
    class [[eosio::contract("testct")]] test : public contract {
        public:
            test(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds) { }

            ACTION testact( name user );
    };
}