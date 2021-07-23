#include <ekutils/log.hpp>

#include "test.hpp"

void log_this() {
    log_debug("DEBUG");
    log_verbose("VERBOSE");
    log_info("INFO");
    log_warning("WARNING");
    log_error("ERROR");
    assert_fails_with(handtruth::ekutils::fatal_error, {
        log_fatal("FATAL");
    });
}

test {
    using namespace handtruth::ekutils;
    log->set_log_level(log_level::debug);
    log_this();
    
    log = new file_log("rdGFdvrgdyn.log", log_level::debug);
    log_this();
    delete log;

    log = new empty_log();
    log_this();
    delete log;
}
