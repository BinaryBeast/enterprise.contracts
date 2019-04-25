#include <matches.hpp>

ACTION matches::hi(name user) {
  require_auth(user);
  print("Hello, ", name{user});
}
