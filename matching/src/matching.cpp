#include <matching.hpp>

ACTION matching::createmchtyp(string type, string uuid_salt) {
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