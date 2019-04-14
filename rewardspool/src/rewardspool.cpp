#include <rewardspool.hpp>

ACTION rewardspool::createacttyp(string type, asset max_reward, unsigned int max_pay_outs) {
  rewards_action_types actions_types(_self, _self.value);
  
  actions_types.emplace(_self, [&](auto& at) {
    at.id = actions_types.available_primary_key();
    at.type = type;
    at.max_reward = max_reward;
    at.max_pay_outs = max_pay_outs;
  });
}

ACTION rewardspool::createact(name source, name owner, uint64_t type_id) {
  rewards_action_types actions_types(_self, _self.value);
  auto action_type = actions_types.find(type_id);
  check(action_type != actions_types.end(), "Action type does not exist");
  const auto& at = *action_type;

  rewards_actions actions(_self, type_id);
  
  actions.emplace(_self, [&](auto& a) {
    a.id = actions.available_primary_key();
    a.source = source;
    a.owner = owner;
    a.current_pay_outs = 0;
    a.rewards_paid = asset(0, at.max_reward.symbol);
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
  auto c_state = current_state.get_or_create(_self, state { 0 });

  c_state.payable_actions = c_state.payable_actions + 1;

  current_state.set(c_state, _self);
}

void rewardspool::decrement_payable_actions() {
  auto c_state = current_state.get_or_create(_self, state { 0 });

  c_state.payable_actions = c_state.payable_actions - 1;

  current_state.set(c_state, _self);
}

void rewardspool::pay_rewards(asset inflation_asset) {
  auto c_state = current_state.get();
  
  if (c_state.payable_actions == 0) {
    print("> Processing Inflation | No Payable Action");
    return;
  }
  
  auto inflation_per_action = inflation_asset / c_state.payable_actions;
  print("> Processing Inflation | Inflation Per Action: ", inflation_per_action, "\n");
  
  rewards_action_types action_types(_self, _self.value);
  for (auto &action_type : action_types) {
    auto inflation_per_action_type = ((inflation_per_action > action_type.max_reward) ? action_type.max_reward : inflation_per_action);
    print(">> Processing Action Type: ", action_type.type," | Inflation Per Action Type: ", inflation_per_action_type, "\n");
    
    rewards_actions actions(_self, action_type.id);
    rewards_historical_actions historical_actions(_self, action_type.id);
    
    auto action_itr = actions.begin();
    while (action_itr != actions.end()) {
      auto& action = *action_itr;
      print(">>> Processing Action: ", action.id, " | Owner: ", action.owner, " | Current Pay Outs: ", action.current_pay_outs, " | Rewards Paid: ", action.rewards_paid, "\n");
      auto rewards_to_pay = inflation_per_action_type; // + reserve distribution
      
      // Send funds
      print(">>>> Paying Reward (", rewards_to_pay.to_string(), ") to account ", name(action.owner), "\n");
      token::transfer_action transfer(name("gre111111111"), {get_self(), name("active")});
      transfer.send(_self, action.owner, rewards_to_pay, "Rewards");
      
      auto rewards_paid = action.rewards_paid + rewards_to_pay;
      
      if (action.current_pay_outs == action_type.max_pay_outs - 1) {
        print(">>>> Converting to historical action\n");
        historical_actions.emplace(_self, [&](auto& ha) {
          ha.id = historical_actions.available_primary_key();
          ha.source = action.source;
          ha.owner = action.owner;
          ha.rewards_paid = rewards_paid;
          ha.created = action.created;
          ha.completed = current_time_point();
        });
        action_itr = actions.erase(action_itr);
        decrement_payable_actions();
      } else {
        print(">>>> Updating existing action\n");
        actions.modify(action_itr, _self, [&](auto& a) {
          a.rewards_paid = rewards_paid;
          a.current_pay_outs = a.current_pay_outs + 1;
          a.last_paid = current_time_point();
        });
        action_itr++;
      }
    }
  }
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