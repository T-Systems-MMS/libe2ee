#include <iostream>

#define CATCH_CONFIG_RUNNER
#include <catch2/catch.hpp>

#if defined(__clang__) || !defined(__APPLE__)

#include <aixlog.hpp>

int main(int argc, char * argv[]) {
  AixLog::Log::init<AixLog::SinkCout>(
          AixLog::Severity::trace);
  return Catch::Session().run( argc, argv );
}

#else

int main(int argc, char * argv[]) {
  return Catch::Session().run( argc, argv );
}
#endif

#if USE_AIXLOG


#else



#endif

