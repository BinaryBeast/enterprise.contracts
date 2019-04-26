#include <eosio/eosio.hpp>
#include <eosio/transaction.hpp>
#include <eosio/print.hpp>
#include <eosio/time.hpp>
#include <eosio/crypto.hpp>

using namespace eosio;
using std::string;
using std::vector;

CONTRACT matching : public contract {
  public:
    using contract::contract;
    matching(eosio::name receiver, eosio::name code, datastream<const char*> ds):contract(receiver, code, ds) {}

    ACTION createmchtyp(string type, unsigned int max_opponents, string uuid_salt);
    ACTION updatemchtyp(checksum256 match_type_uuid, string type);
    ACTION createmch(checksum256 match_type_uuid, string title, name owner, time_point starts, string uuid_salt);
    ACTION updatemch(checksum256 match_uuid, string title, time_point starts);
    // Change Match Type
    // Publish Match
    ACTION createmchopp(string title, checksum256 match_uuid);
    // Update Match Opponent
    // Delete Match Opponent
    // Complete Match
    // Delete Match
    // Transfer Match Ownership
    // Create Validation Type
    // Validate Match
  
  private:
    TABLE match_type {
      uint64_t id;
      checksum256 uuid;
      string type;
      unsigned int max_opponents;
      
      uint64_t primary_key() const { return id; }
      checksum256 get_secondary_uuid() const { return uuid; }
    };
    typedef eosio::multi_index<name("matchtypes"), match_type,
      indexed_by<name("uuid"), const_mem_fun<match_type, checksum256, &match_type::get_secondary_uuid>>
    > match_types;
    
    struct match_opponent {
      checksum256 id;
      string title;
      double score = 0;
      bool winner = false;
    };
    
    TABLE match {
      uint64_t id;
      checksum256 uuid;
      checksum256 match_type_uuid;
      string title;
      name owner;
      time_point created;
      time_point starts;
      time_point completed;
      bool is_completed = false;
      bool is_live = false;
      double confidence = 0;
      vector<match_opponent> opponents;
      
      uint64_t primary_key() const { return id; }
      checksum256 get_secondary_uuid() const { return uuid; }
      checksum256 get_secondary_match_type_uuid() const { return match_type_uuid; }
      uint64_t get_secondary_owner() const { return owner.value; }
      uint64_t get_secondary_created() const { return created.elapsed.count(); }
      uint64_t get_secondary_created_live() const { return is_live ? created.elapsed.count() : std::numeric_limits<uint64_t>::max(); }
      uint64_t get_secondary_starts() const { return starts.elapsed.count(); }
      uint64_t get_secondary_completed() const { return completed.elapsed.count(); }
      uint64_t get_secondary_is_completed() const { return static_cast<uint64_t>(is_completed); }
      uint64_t get_secondary_is_live() const { return static_cast<uint64_t>(is_live); }
      double get_secondary_confidence() const { return confidence; }
    };
    typedef eosio::multi_index<name("matches"), match,
      indexed_by<name("uuid"), const_mem_fun<match, checksum256, &match::get_secondary_uuid>>,
      indexed_by<name("typeuuid"), const_mem_fun<match, checksum256, &match::get_secondary_match_type_uuid>>,
      indexed_by<name("owner"), const_mem_fun<match, uint64_t, &match::get_secondary_owner>>,
      indexed_by<name("created"), const_mem_fun<match, uint64_t, &match::get_secondary_created>>,
      indexed_by<name("createdlive"), const_mem_fun<match, uint64_t, &match::get_secondary_created_live>>,
      indexed_by<name("starts"), const_mem_fun<match, uint64_t, &match::get_secondary_starts>>,
      indexed_by<name("completed"), const_mem_fun<match, uint64_t, &match::get_secondary_completed>>,
      indexed_by<name("iscompleted"), const_mem_fun<match, uint64_t, &match::get_secondary_is_completed>>,
      indexed_by<name("islive"), const_mem_fun<match, uint64_t, &match::get_secondary_is_live>>,
      indexed_by<name("confidence"), const_mem_fun<match, double, &match::get_secondary_confidence>>
    > matches;
    
    
    // Validation_Type
    //    Id (uint64_t)
    //    UUID (checksum256) <- UUID_Salt
    //    Type (string)
    //    Weight (double)
    // Validation
    //    Id (uint64_t)
    //    UUID (checksum256) <- Validator + Match_UUID
    //    Validator (name)
    //    Match_UUID (checksum256)
    //    Validation_Type_UUID (checksum256)
  
    // Tables
    // Validation_Types (Validation_Type)
    // Validations (Validation)
};
