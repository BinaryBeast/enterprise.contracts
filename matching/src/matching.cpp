#include <matching.hpp>

ACTION matching::createmchtyp(string type, string uuid_salt) {
  require_auth(_self);
  match_types mts(_self, _self.value);
  checksum256 uuid_hash = sha256(const_cast<char*>(uuid_salt.c_str()), uuid_salt.size() * sizeof(char));
  
  auto indexed_by_uuid = mts.template get_index<name("uuid")>();
  auto existing_mt = indexed_by_uuid.find(uuid_hash);
  check(existing_mt == indexed_by_uuid.end(), "Match Type with UUID already exists");
  
  mts.emplace(_self, [&](auto& mt) {
    mt.id = mts.available_primary_key();
    mt.uuid = uuid_hash;
    mt.type = type;
  });
}

ACTION matching::createmch(checksum256 match_type_uuid, string title, name owner, time_point starts, string uuid_salt) {
  check(title.size() <= 64, "Title must be 64 characters or less");
  require_auth(owner);
  
  match_types mts(_self, _self.value);
  
  auto mts_indexed_by_uuid = mts.template get_index<name("uuid")>();
  auto existing_mt = mts_indexed_by_uuid.find(match_type_uuid);
  check(existing_mt != mts_indexed_by_uuid.end(), "Match Type with UUID does not exist");
  
  checksum256 uuid_hash = sha256(const_cast<char*>(uuid_salt.c_str()), uuid_salt.size() * sizeof(char));
  
  matches mchs(_self, _self.value);
  auto mchs_indexed_by_uuid = mchs.template get_index<name("uuid")>();
  auto existing_mch = mchs_indexed_by_uuid.find(uuid_hash);
  check(existing_mch == mchs_indexed_by_uuid.end(), "Match with UUID already exists");
  
  mchs.emplace(_self, [&](auto& mch) {
    mch.id = mchs.available_primary_key();
    mch.uuid = uuid_hash;
    mch.title = title;
    mch.owner = owner;
    mch.created = current_time_point();
    mch.starts = starts;
  });
}