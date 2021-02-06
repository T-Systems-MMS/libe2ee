#include <iostream>

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#if !(defined(__GNUC__) && defined(__APPLE__))
#include <aixlog.hpp>
#endif

int main(int argc, char * argv[]) {
#if !(defined(__GNUC__) && defined(__APPLE__))
  AixLog::Log::init<AixLog::SinkCout>(
          AixLog::Severity::trace,
          AixLog::Type::normal);
#endif
  Catch::Session().run( argc, argv );
}
