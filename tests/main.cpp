#include <iostream>

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>
#include <aixlog.hpp>

int main(int argc, char * argv[]) {
  AixLog::Log::init<AixLog::SinkCout>(
          AixLog::Severity::info,
          AixLog::Type::normal);
  Catch::Session().run( argc, argv );
}
