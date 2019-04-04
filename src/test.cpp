#include "test.hpp"

namespace testcts {
    void test::testact( name user ) {
        require_auth( user );
    }
}