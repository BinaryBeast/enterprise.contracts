#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/transaction.hpp>
#include <eosio/singleton.hpp>
#include <eosio/print.hpp>

#include "token.hpp"

using namespace eosio;
using std::string;

CONTRACT rewardspool : public contract {
  public:
    using contract::contract;
    rewardspool(eosio::name receiver, eosio::name code, datastream<const char*> ds):contract(receiver, code, ds), current_state(_self, _self.value) {}
    
    ACTION addaccaction(name account);
    ACTION setpayables(unsigned long payable_accounts, unsigned long payable_actions);
    ACTION incpayables();
    ACTION clearstate();
    ACTION payrewards(asset inflation_asset);
    
    //[[eosio::on_notify("gre111111111::issinflation")]]
    //void on_inflation(name to, asset quantity);
  
  private:

    TABLE state {
      unsigned long payable_accounts;
      unsigned long payable_actions;
    };
    typedef eosio::singleton<name("state"), state> s_state;
    s_state current_state;
  
    TABLE action_account {
      name account;
      int payable_actions;
  
      uint64_t primary_key() const { return account.value; }
    };
    typedef eosio::multi_index<name("actionaccs"), action_account> action_accounts;
    
    void increment_payable_actions();
    void increment_payable_accounts();
    void pay_rewards(asset inflation_asset);
};
