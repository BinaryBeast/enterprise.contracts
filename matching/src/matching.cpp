#include <matching.hpp>

ACTION matching::mchtypcreate(checksum256 match_type_uuid, string type, unsigned int max_opponents) {
  check(type.size() < 24 && type.size() > 0, "Type must be 24 characters or less");
  check(max_opponents > 1, "A minimum of 2 opponents is required");
  require_auth(_self);
  
  match_types mts(_self, _self.value);
  auto indexed_by_uuid = mts.template get_index<name("uuid")>();
  auto existing_mt = indexed_by_uuid.find(match_type_uuid);
  check(existing_mt == indexed_by_uuid.end(), "Match Type with UUID already exists");
  
  mts.emplace(_self, [&](auto& mt) {
    mt.id = mts.available_primary_key();
    mt.uuid = match_type_uuid;
    mt.type = type;
    mt.max_opponents = max_opponents;
  });
}

ACTION matching::mchtypupdate(checksum256 match_type_uuid, string type) {
  check(type.size() < 24 && type.size() > 0, "Type must be 24 characters or less");
  
  match_types mts(_self, _self.value);
  auto mts_indexed_by_uuid = mts.template get_index<name("uuid")>();
  auto existing_mt = mts_indexed_by_uuid.find(match_type_uuid);
  check(existing_mt != mts_indexed_by_uuid.end(), "Match Type with UUID does not exist");
  
  mts_indexed_by_uuid.modify(existing_mt, same_payer, [&](auto& mt) {
     mt.type = type;
  });
}

ACTION matching::mchcreate(checksum256 match_uuid, checksum256 match_type_uuid, string title, name owner, time_point starts) {
  check(title.size() <= 64 && title.size() > 0, "Title must be 64 characters or less");
  //check(starts in future)
  require_auth(owner);
  
  match_types mts(_self, _self.value);
  auto mts_indexed_by_uuid = mts.template get_index<name("uuid")>();
  auto existing_mt = mts_indexed_by_uuid.find(match_type_uuid);
  check(existing_mt != mts_indexed_by_uuid.end(), "Match Type with UUID does not exist");
  
  matches mchs(_self, _self.value);
  auto mchs_indexed_by_uuid = mchs.template get_index<name("uuid")>();
  auto existing_mch = mchs_indexed_by_uuid.find(match_uuid);
  check(existing_mch == mchs_indexed_by_uuid.end(), "Match with UUID already exists");
  
  mchs.emplace(_self, [&](auto& mch) {
    mch.id = mchs.available_primary_key();
    mch.uuid = match_uuid;
    mch.match_type_uuid = match_type_uuid;
    mch.title = title;
    mch.owner = owner;
    mch.created = current_time_point();
    mch.starts = starts;
  });
}

ACTION matching::mchupdate(checksum256 match_uuid, string title, time_point starts) {
  check(title.size() <= 32 && title.size() > 0, "Title must be 32 characters or less");
  //check(starts in future)
  
  matches mchs(_self, _self.value);
  auto mchs_indexed_by_uuid = mchs.template get_index<name("uuid")>();
  auto existing_mch = mchs_indexed_by_uuid.find(match_uuid);
  check(existing_mch != mchs_indexed_by_uuid.end(), "Match with UUID does not exist");
  require_auth(existing_mch->owner);
  
  mchs_indexed_by_uuid.modify(existing_mch, same_payer, [&](auto& mch) {
     mch.title = title;
     mch.starts = starts;
  });
}

ACTION matching::mchsetstatus(checksum256 match_uuid, uint64_t status) {
  matches mchs(_self, _self.value);
  auto mchs_indexed_by_uuid = mchs.template get_index<name("uuid")>();
  auto existing_mch = mchs_indexed_by_uuid.find(match_uuid);
  check(existing_mch != mchs_indexed_by_uuid.end(), "Match with UUID does not exist");
  require_auth(existing_mch->owner);
  
  mchs_indexed_by_uuid.modify(existing_mch, same_payer, [&](auto& mch) {
     mch.status = status;
  });
}

ACTION matching::mchcreateopp(string title, checksum256 match_uuid) {
  check(title.size() <= 32 && title.size() > 0, "Title must be 32 characters or less");
  
  matches mchs(_self, _self.value);
  auto mchs_indexed_by_uuid = mchs.template get_index<name("uuid")>();
  auto existing_mch = mchs_indexed_by_uuid.find(match_uuid);
  check(existing_mch != mchs_indexed_by_uuid.end(), "Match with UUID does not exist");
  require_auth(existing_mch->owner);
  
  auto opponents = existing_mch->opponents;
  
  match_types mts(_self, _self.value);
  auto mts_indexed_by_uuid = mts.template get_index<name("uuid")>();
  auto existing_mt = mts_indexed_by_uuid.find(existing_mch->match_type_uuid);
  check(opponents.size() < existing_mt->max_opponents, "Maximum number of opponents for match already reached");
  
  checksum256 id_hash = sha256(const_cast<char*>(title.c_str()), title.size() * sizeof(char));
  auto existing_opp = std::find_if(opponents.begin(), opponents.end(), [&](auto& opp) { return opp.id == id_hash; });
  check(existing_opp == opponents.end(), "Opponent with that title already exists");
  
  opponents.push_back({id_hash, title});
  
  mchs_indexed_by_uuid.modify(existing_mch, same_payer, [&](auto& mch) {
     mch.opponents = opponents;
  });
}

ACTION matching::mchdeleteopp(checksum256 id, checksum256 match_uuid) {
  matches mchs(_self, _self.value);
  auto mchs_indexed_by_uuid = mchs.template get_index<name("uuid")>();
  auto existing_mch = mchs_indexed_by_uuid.find(match_uuid);
  check(existing_mch != mchs_indexed_by_uuid.end(), "Match with UUID does not exist");
  require_auth(existing_mch->owner);
  
  auto opponents = existing_mch->opponents;
  auto existing_opp = std::find_if(opponents.begin(), opponents.end(), [&](auto& opp) { return opp.id == id; });
  check(existing_opp != opponents.end(), "Opponent with that title does not exist");
  
  opponents.erase(existing_opp);
  
  mchs_indexed_by_uuid.modify(existing_mch, same_payer, [&](auto& mch) {
     mch.opponents = opponents;
  });
}

// Testing
ACTION matching::reset() {
  require_auth(_self);
  
  matches mchs(_self, _self.value);
  auto match_itr = mchs.begin();
  while (match_itr != mchs.end()) {
    match_itr = mchs.erase(match_itr);
  }
  
  match_types mts(_self, _self.value);
  auto match_type_itr = mts.begin();
  while (match_type_itr != mts.end()) {
    match_type_itr = mts.erase(match_type_itr);
  }
}