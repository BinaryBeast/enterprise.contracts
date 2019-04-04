#include <eosiolib/eosio.hpp>
#include <eosiolib/asset.hpp>
#include <eosiolib/transaction.hpp>
#include <eosiolib/singleton.hpp>
#include <eosiolib/system.hpp>

#define DEBUG

using namespace eosio;
using std::string;

namespace enterprise {
   class [[eosio::contract]] token : public contract {
      public:
        token(name receiver, name code, datastream<const char*> ds) : contract(receiver, code, ds), s_inflator(_self, _self.value) { }

         [[eosio::action]]
         void create(name issuer, asset maximum_supply);

         [[eosio::action]]
         void issue(name to, asset quantity, string memo);

         [[eosio::action]]
         void burn(asset quantity, string memo);

         [[eosio::action]]
         void transfer(name from, name to, asset quantity, string memo);

         [[eosio::action]]
         void inflate(asset quantity);

         [[eosio::action]]
         void setinflation(asset quantity, double percentage, uint64_t frequency_seconds, bool can_inflate);

         [[eosio::action]]
         void stainflation(asset quantity);

         [[eosio::action]]
         void stoinflation();

         [[eosio::action]]
         void setinflpools(std::vector<name> pools, std::vector<double> distribution_percentages);

        using transfer_action = action_wrapper<name("transfer"), &token::transfer>;

      private:
         struct [[eosio::table]] inflator {
            asset quantity;
            uint64_t frequency_seconds;
            bool can_inflate;
            double inflation_percent;
         };
         typedef eosio::singleton<name("inflator"), inflator> sinflator;
         sinflator s_inflator;

         struct [[eosio::table]] account {
            asset balance;

            uint64_t primary_key() const { return balance.symbol.code().raw(); }
         };
         typedef eosio::multi_index<name("accounts"), account> accounts;

         struct [[eosio::table]] currency_stats {
            asset  supply;
            asset  max_supply;
            name issuer;

            uint64_t primary_key() const { return supply.symbol.code().raw(); }
         };
         typedef eosio::multi_index<name("stats"), currency_stats> stats;

         struct [[eosio::table]] inflation_pool {
            name account;
            double percentage;

            uint64_t primary_key() const { return account.value; }
         };
         typedef eosio::multi_index<name("inflationpls"), inflation_pool> inflation_pools;

         void sub_balance(name owner, asset value);
         void add_balance(name owner, asset value, name ram_payer);
         asset inflate_max_supply(asset quantity, double percentage);
         void distribute_inflation(asset inflation_asset);
   };
}