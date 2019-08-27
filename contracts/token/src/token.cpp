#include <token.hpp>

ACTION token::create(name issuer, asset maximum_supply) {
  require_auth(_self);

  auto sym = maximum_supply.symbol;
  check(sym.is_valid(), "invalid symbol name");
  check(maximum_supply.is_valid(), "invalid supply");
  check(maximum_supply.amount > 0, "max-supply must be positive");

  // check for already existing symbol
  stats statstable(_self, sym.code().raw());
  auto existing = statstable.find(sym.code().raw());
  check(existing == statstable.end(), "token with symbol already exists");

  // add new currency in the stats table
  statstable.emplace(_self, [&](auto& s) {
     s.supply.symbol = maximum_supply.symbol;
     s.max_supply  = maximum_supply;
     s.issuer  = issuer;
  });
}

ACTION token::issue(name to, asset quantity, string memo) {
  // check if symbol is valid and memo does not exceeds 256 bytes
  auto sym = quantity.symbol;
  check(sym.is_valid(), "invalid symbol name");
  check(memo.size() <= 256, "memo has more than 256 bytes");

  // check if token with this symbol name exists, and get iterator_
  stats statstable(_self, sym.code().raw());
  auto existing = statstable.find(sym.code().raw());
  check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
  const auto& st = *existing;

  // check for the authority of issuer and valid quantity_require_auth( st.issuer );
  require_auth(st.issuer);
  check(quantity.is_valid(), "invalid quantity");
  auto greaterZeroErrorMessage = ("must issue positive quantity " + std::to_string(quantity.amount)).c_str();
  check(quantity.amount > 0, greaterZeroErrorMessage);
  check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
  check(quantity.amount <= st.max_supply.amount - st.supply.amount, "quantity exceeds available supply");

  // modify records
  statstable.modify(st, same_payer, [&](auto& s) {
    s.supply += quantity;
  });

  add_balance(st.issuer, quantity, st.issuer);

  if (to != st.issuer) {
    token::transfer_action action(_self, { st.issuer, name("active") });
    action.send(st.issuer, to, quantity, memo);
  }
}

ACTION token::burn(asset quantity, string memo) {
  auto sym = quantity.symbol;
  check( sym.is_valid(), "invalid symbol name" );
  check( memo.size() <= 256, "memo has more than 256 bytes" );

  stats statstable( _self, sym.code().raw() );auto existing = statstable.find( sym.code().raw() );
  check( existing != statstable.end(), "token with symbol does not exist" );
  const  auto& st = *existing;

  require_auth( st.issuer );
  check( quantity.is_valid(), "invalid quantity" );
  check( quantity.amount > 0, "must burn positive quantity" );
  check( quantity.symbol == st.supply.symbol, "symbol precision mismatch" );

  statstable.modify( st, same_payer, [&]( auto& s ) {
    s.supply -= quantity;
  });

  sub_balance(st.issuer, quantity);
}

ACTION token::transfer(name from, name to, asset quantity, string memo) {
  check(from != to, "cannot transfer to self");
  require_auth(from);
  check(is_account(to), "to account does not exist");
  auto sym = quantity.symbol.code();
  stats statstable(_self, sym.raw());
  const  auto& st = statstable.get(sym.raw());

  require_recipient(from);
  require_recipient(to);

  check(quantity.is_valid(), "invalid quantity");
  check(quantity.amount > 0, "must transfer positive quantity");
  check(quantity.symbol == st.supply.symbol, "symbol precision mismatch");
  check(memo.size() <= 256, "memo has more than 256 bytes");

  auto payer = has_auth(to) ? to : from;

  sub_balance(from, quantity);
  add_balance(to, quantity, payer);
}

ACTION token::inflate(asset quantity) {
  require_auth(_self);
  auto current_inflator = s_inflator.get();

  auto inflation_asset = inflate_max_supply(quantity, current_inflator.inflation_percent);

  distribute_inflation(inflation_asset);
}

ACTION token::setinflation(asset quantity, double percentage, uint64_t frequency_seconds, bool can_inflate) {
  require_auth(_self);
  
  s_inflator.set(inflator { 
    quantity,
    frequency_seconds,
    can_inflate,
    percentage
  }, _self);
}

ACTION token::stainflation(asset quantity) {
  require_auth(_self);

  auto current_inflator = s_inflator.get();

  if (!current_inflator.can_inflate) { return; }

  eosio::transaction tx;

  tx.actions.emplace_back(
    permission_level { _self, name("active") },
    _self,
    name("stainflation"),
    std::make_tuple(quantity)
  );

  tx.delay_sec = current_inflator.frequency_seconds;
  tx.send(current_time_point().sec_since_epoch(), _self);
  
  token::inflate_action action(_self, { _self, name("active") });
  action.send(quantity);
}

ACTION token::stoinflation() {
  require_auth(_self);
  auto current_inflator = s_inflator.get();

  current_inflator.can_inflate = false;

  s_inflator.set(current_inflator, _self);
}

ACTION token::issinflation(name to, asset quantity) {
  require_recipient(to);
  token::issue_action action(_self, { _self, name("active") });
  action.send(to, quantity, "Inflation Distribution");
}

ACTION token::setinflpools(std::vector<name> pools, std::vector<double> distribution_percentages) {
  require_auth(_self);

  int poolsSize = pools.size();
  int percentagesSize = distribution_percentages.size();
  const char* poolsErrorMessage = ("Must be equal numbers of pools & distribution percentages | Pools: " + std::to_string(poolsSize) + " | Percentages: " + std::to_string(percentagesSize)).c_str();
  check(poolsSize == percentagesSize, poolsErrorMessage);

  double totalPercentage = 0;
  for (auto &percentage : distribution_percentages) {
     totalPercentage += percentage;
  }
  const char* percentageErrorMessage = ("Distrbution percentages must add up to 100 percent | Percentage: " + std::to_string(totalPercentage)).c_str();
  check(totalPercentage == 100, percentageErrorMessage);

  inflation_pools infpools(_self, _self.value);
  auto oldpool = infpools.begin();
  while (oldpool != infpools.end()) {
     oldpool = infpools.erase(oldpool);
  }

  for (std::vector<name>::size_type i = 0; i != pools.size(); i++) {
     auto pool = pools[i];
     auto percentage = distribution_percentages[i];
     infpools.emplace(_self, [&](auto& p) {
        p.account = pool;
        p.percentage = percentage;
     });
  }
}

void token::sub_balance(name owner, asset value) {
  accounts from_acnts(_self, owner.value);

  const  auto& from = from_acnts.get(value.symbol.code().raw(), "no balance object found");
  check(from.balance.amount >= value.amount, "overdrawn balance");

  from_acnts.modify(from, owner, [&](auto& a) {
     a.balance -= value;
  });
}

void token::add_balance(name owner, asset value, name ram_payer) {
  accounts to_acnts(_self, owner.value);
  auto to = to_acnts.find(value.symbol.code().raw());

  if(to == to_acnts.end()) {
     to_acnts.emplace(ram_payer, [&](auto& a) {
        a.balance = value;
     });
  }
  else {
     to_acnts.modify(to, same_payer, [&](auto& a) {
        a.balance += value;
     });
  }
}

asset token::inflate_max_supply(asset quantity, double percentage) {
  auto sym = quantity.symbol;
  check(sym.is_valid(), "invalid symbol name");

  stats statstable(_self, sym.code().raw());
  auto existing = statstable.find(sym.code().raw());
  check(existing != statstable.end(), "token with symbol does not exist, create token before issue");
  const auto& st = *existing;

  require_auth(st.issuer);

  auto inflation = st.max_supply.amount * percentage;
  auto new_max_supply = st.max_supply.amount + inflation;

  statstable.modify(st, same_payer, [&](auto& s) {
     s.max_supply.amount = new_max_supply;
  });

  asset inflation_asset(inflation, quantity.symbol);

  return inflation_asset;
}

void token::distribute_inflation(asset inflation_asset) {
  inflation_pools pools(_self, _self.value);

  for (auto &pool : pools) {
     auto inflation = inflation_asset.amount * (pool.percentage / 100);
     asset split_asset(inflation, inflation_asset.symbol);

    token::issinflation_action action(_self, { _self, name("active") });
    action.send(pool.account, split_asset);
  }
}