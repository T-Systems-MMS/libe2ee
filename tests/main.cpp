#include <iostream>

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

#if defined(__GNUC__) && defined(__APPLE__)

int main(int argc, char * argv[]) {
  Catch::Session().run( argc, argv );
}

#else

#include <aixlog.hpp>
int main(int argc, char * argv[]) {
  AixLog::Log::init<AixLog::SinkCout>(
          AixLog::Severity::trace,
          AixLog::Type::normal);
  Catch::Session().run( argc, argv );
}

#endif

