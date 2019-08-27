#include <eosio/eosio.hpp>
#include <eosio/print.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using std::string;

CONTRACT token : public contract {
  public:
  
    using contract::contract;
    token(eosio::name receiver, eosio::name code, datastream<const char*> ds):contract(receiver, code, ds), s_inflator(_self, _self.value) {}

    ACTION create(name issuer, asset maximum_supply);
    ACTION issue(name to, asset quantity, string memo);
    ACTION burn(asset quantity, string memo);
    ACTION transfer(name from, name to, asset quantity, string memo);
    ACTION inflate(asset quantity);
    ACTION setinflation(asset quantity, double percentage, uint64_t frequency_seconds, bool can_inflate);
    ACTION stainflation(asset quantity);
    ACTION stoinflation();
    ACTION issinflation(name to, asset quantity);
    ACTION setinflpools(std::vector<name> pools, std::vector<double> distribution_percentages);
    
    using issue_action = action_wrapper<name("issue"), &token::issue>;
    using transfer_action = action_wrapper<name("transfer"), &token::transfer>;
    using inflate_action = action_wrapper<name("inflate"), &token::inflate>;
    using issinflation_action = action_wrapper<name("issinflation"), &token::issinflation>;
  
    TABLE account {
      asset balance;
  
      uint64_t primary_key() const { return balance.symbol.code().raw(); }
    };
    typedef eosio::multi_index<name("accounts"), account> accounts;
  
  private:
  
    TABLE inflator {
      asset quantity;
      uint64_t frequency_seconds;
      bool can_inflate;
      double inflation_percent;
    };
    typedef eosio::singleton<name("inflator"), inflator> sinflator;
    sinflator s_inflator;
    
    
    TABLE currency_stats {
      asset  supply;
      asset  max_supply;
      name issuer;

      uint64_t primary_key() const { return supply.symbol.code().raw(); }
    };
    typedef eosio::multi_index<name("stats"), currency_stats> stats;
    
    TABLE inflation_pool {
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
