#include <eosio/eosio.hpp>
#include <eosio/print.hpp>

using namespace eosio;

CONTRACT matches : public contract {
  public:
    using contract::contract;
    matches(eosio::name receiver, eosio::name code, datastream<const char*> ds):contract(receiver, code, ds) {}

    ACTION hi(name user);
    // Actions
    // Create Match Type
    // Create Validate Type
    // Create Match
    // Update Match
    // Create Match Opponent
    // Update Match Opponent
    // Validate Match
    // Complete Match
  
  private:
    // Structs
    // Match_Type
    //    Id (uint64_t)
    //    UUID (checksum256) <- UUID_Salt
    //    Type (string)
    // Match
    //    Id (uint64_t)
    //    UUID (checksum256) <- UUID_Salt
    //    Title (string)
    //    Owner (name)
    //    Opponents (Vector<Match_Opponent>)
    //    Created_DateTime (time_point)
    //    Start_DateTime (time_point)
    //    Completed_DateTime (time_point)
    //    Completed (boolean)
    //    Confidence (double)
    // Match_Opponent
    //    Id (checksum256) <- Name (unique inside a match)
    //    Name (string)
    //    Score (double)
    //    Winner (boolean)
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
    // Match_Types (Match_Type)
    // Matches (Match)
    // Validation_Types (Validation_Type)
    // Validations (Validation)
  
    TABLE tableStruct {
      name key;
      std::string name;
    };
    typedef eosio::multi_index<"table"_n, tableStruct> table;
};
