#include <iostream>

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#if defined(__clang__) || !defined(__APPLE__)

#include <aixlog.hpp>

int main(int argc, char * argv[]) {
  AixLog::Log::init<AixLog::SinkCout>(
          AixLog::Severity::trace);
  Catch::Session().run( argc, argv );
}

#else

int main(int argc, char * argv[]) {
  Catch::Session().run( argc, argv );
}
#endif

#if USE_AIXLOG


#else



#endif

