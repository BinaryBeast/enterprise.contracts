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
    
    [[eosio::on_notify("gre111111111::issinflation")]]
    void inflation(name to, asset quantity);

    using issinflation_action = action_wrapper<"inflation"_n, &rewardspool::inflation>;
  
  private:

    TABLE state {
      unsigned long payable_accounts;
      unsigned long payable_actions;
    };
    typedef eosio::singleton<name("state"), state> s_state;
    s_state current_state;
    
    TABLE rewards_action {
      uint64_t id;
      name source;
      name owner;
      unsigned int current_pay_outs;
      asset rewards_paid;
      
      uint64_t primary_key() const { return id; }
      uint64_t get_secondary_source() const { return source.value; }
      uint64_t get_secondary_owner() const { return owner.value; }
    };
    typedef eosio::multi_index<name("rwdsacts"), rewards_action,
      indexed_by<name("source"), const_mem_fun<rewards_action, uint64_t, &rewards_action::get_secondary_source>>,
      indexed_by<name("owner"), const_mem_fun<rewards_action, uint64_t, &rewards_action::get_secondary_owner>>
    > rewards_actions;
    
    TABLE rewards_historical_action {
      uint64_t id;
      name source;
      name owner;
      asset rewards_paid;
      
      uint64_t primary_key() const { return id; }
      uint64_t get_secondary_source() const { return source.value; }
      uint64_t get_secondary_owner() const { return owner.value; }
    };
    typedef eosio::multi_index<name("rwdshistacts"), rewards_action,
      indexed_by<name("source"), const_mem_fun<rewards_action, uint64_t, &rewards_historical_action::get_secondary_source>>,
      indexed_by<name("owner"), const_mem_fun<rewards_action, uint64_t, &rewards_historical_action::get_secondary_owner>>
    > rewards_historical_actions;
    
    TABLE rewards_action_type {
      uint64_t key;
      
      uint64_t primary_key() const { return key; }
    };
    typedef eosio::multi_index<name("rwdsacttyps"), rewards_action_type> rewards_action_types;
    
    void increment_payable_actions();
    void increment_payable_accounts();
    void pay_rewards(asset inflation_asset);
    
    // Deprecated
    TABLE action_account {
      name account;
      int payable_actions;
  
      uint64_t primary_key() const { return account.value; }
    };
    typedef eosio::multi_index<name("actionaccs"), action_account> action_accounts;
};
