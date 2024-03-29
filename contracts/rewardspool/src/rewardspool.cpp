#include <rewardspool.hpp>

ACTION rewardspool::createacttyp(string type, asset max_reward, unsigned int max_pay_outs) {
  rewards_action_types actions_types(_self, _self.value);
  
  actions_types.emplace(_self, [&](auto& at) {
    at.id = actions_types.available_primary_key();
    at.type = type;
    at.max_reward = max_reward;
    at.max_pay_outs = max_pay_outs;
    at.max_rpp = max_reward / max_pay_outs;
  });
}

ACTION rewardspool::createact(name source, name owner, uint64_t type_id, string uuid_salt) {
  rewards_action_types actions_types(_self, _self.value);
  auto action_type = actions_types.find(type_id);
  check(action_type != actions_types.end(), "Action type does not exist");
  
  auto uuid_raw = source.to_string() + owner.to_string() + std::to_string(type_id) + uuid_salt;
  checksum256 uuid_hash = sha256(const_cast<char*>(uuid_raw.c_str()), uuid_raw.size() * sizeof(char));
  
  rewards_actions actions(_self, type_id);
  auto indexed_by_uuid_actions = actions.template get_index<name("uuid")>();
  auto existing_action = indexed_by_uuid_actions.find(uuid_hash);
  check(existing_action == indexed_by_uuid_actions.end(), "Action with UUID already exists");
  
  rewards_historical_actions historical_actions(_self, type_id);
  auto indexed_by_uuid_historical_actions = historical_actions.template get_index<name("uuid")>();
  auto existing_historical_action = indexed_by_uuid_historical_actions.find(uuid_hash);
  check(existing_historical_action == indexed_by_uuid_historical_actions.end(), "Hisotrical action with UUID already exists");
  
  actions.emplace(_self, [&](auto& a) {
    a.id = actions.available_primary_key();
    a.source = source;
    a.owner = owner;
    a.uuid_salt = uuid_salt;
    a.uuid = uuid_hash;
    a.current_pay_outs = 0;
    a.rewards_paid = asset(0, action_type->max_reward.symbol);
    a.created = current_time_point();
  });
  
  increment_payable_actions();
}

void rewardspool::inflation (name to, asset quantity) {
  auto self = get_self();

  if (to == self) {
    print("Paying inflation from ", name(to), " totalling ", quantity.to_string(), "\n");
    pay_rewards(quantity);
  }
}

void rewardspool::increment_payable_actions() {
  auto c_state = current_state.get_or_create(_self);

  c_state.payable_actions = c_state.payable_actions + 1;

  current_state.set(c_state, _self);
}

void rewardspool::decrement_payable_actions() {
  auto c_state = current_state.get_or_create(_self);

  c_state.payable_actions = c_state.payable_actions - 1;

  current_state.set(c_state, _self);
}

void rewardspool::pay_rewards(asset inflation_asset) {
  auto c_state = current_state.get();
  auto total_rewards_paid = asset(0, inflation_asset.symbol);
  
  if (c_state.payable_actions == 0) {
    print("> Processing Inflation | No Payable Action");
    c_state.last_pay_out_amount = total_rewards_paid;
    c_state.last_pay_out_time = current_time_point();
  
    current_state.set(c_state, _self);
    
    return;
  }
  
  auto current_balance = get_current_balance(inflation_asset);
  auto payable_per_action = (inflation_asset + current_balance) / c_state.payable_actions;
  print("> Processing Inflation | Current Balance: ", current_balance, " | Inflation: ", inflation_asset, " | Payable Per Action: ", payable_per_action, "\n");
  
  rewards_action_types action_types(_self, _self.value);
  for (auto &action_type : action_types) {
    auto payable_per_action_type = ((payable_per_action > action_type.max_rpp) ? action_type.max_rpp : payable_per_action);
    print(">> Processing Action Type: ", action_type.type," | Inflation Per Action Type: ", payable_per_action_type, "\n");
    
    rewards_actions actions(_self, action_type.id);
    rewards_historical_actions historical_actions(_self, action_type.id);
    
    auto action_itr = actions.begin();
    while (action_itr != actions.end()) {
      auto& action = *action_itr;
      print(">>> Processing Action: ", action.id, " | Owner: ", action.owner, " | Current Pay Outs: ", action.current_pay_outs, " | Rewards Paid: ", action.rewards_paid, "\n");
      auto rewards_to_pay = payable_per_action_type;
      
      print(">>>> Paying Reward (", rewards_to_pay.to_string(), ") to account ", name(action.owner), "\n");
      token::transfer_action transfer(name("gre111111111"), {get_self(), name("active")});
      transfer.send(_self, action.owner, rewards_to_pay, "Rewards");
      
      total_rewards_paid = total_rewards_paid + rewards_to_pay;
      auto rewards_paid = action.rewards_paid + rewards_to_pay;
      
      if (action.current_pay_outs == action_type.max_pay_outs - 1) {
        print(">>>> Converting to historical action\n");
        historical_actions.emplace(_self, [&](auto& ha) {
          ha.id = historical_actions.available_primary_key();
          ha.source = action.source;
          ha.owner = action.owner;
          ha.uuid_salt = action.uuid_salt;
          ha.uuid = action.uuid;
          ha.rewards_paid = rewards_paid;
          ha.created = action.created;
          ha.completed = current_time_point();
        });
        action_itr = actions.erase(action_itr);
        decrement_payable_actions();
      } else {
        print(">>>> Updating existing action\n");
        actions.modify(action_itr, same_payer, [&](auto& a) {
          a.rewards_paid = rewards_paid;
          a.current_pay_outs = a.current_pay_outs + 1;
        });
        action_itr++;
      }
    }
  }
  
  c_state.last_pay_out_amount = total_rewards_paid;
  c_state.last_pay_out_time = current_time_point();

  current_state.set(c_state, _self);
}

asset rewardspool::get_current_balance(asset balance_asset) {
  token::accounts token_accounts(name("gre111111111"), _self.value);
  auto existing_token = token_accounts.find(balance_asset.symbol.code().raw());
  check(existing_token != token_accounts.end(), "Token does not exist for this account");
  
  return existing_token->balance;
}
  
// Deprecated / Testing
ACTION rewardspool::payrewards(asset inflation_asset) {
  require_auth(_self);
  pay_rewards(inflation_asset);
}

ACTION rewardspool::reset() {
  require_auth(_self);
  current_state.remove();
  
  rewards_action_types action_types(_self, _self.value);
  auto action_type_itr = action_types.begin();
  
  while (action_type_itr != action_types.end()) {
    rewards_actions actions(_self, action_type_itr->id);
    auto action_itr = actions.begin();
    
    while (action_itr != actions.end()) {
      action_itr = actions.erase(action_itr);
    }
    
    rewards_historical_actions historical_actions(_self, action_type_itr->id);
    auto historical_action_itr = historical_actions.begin();
    
    while (historical_action_itr != historical_actions.end()) {
      historical_action_itr = historical_actions.erase(historical_action_itr);
    }
    
    action_type_itr = action_types.erase(action_type_itr);
  }
}