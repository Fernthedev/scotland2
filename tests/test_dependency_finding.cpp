// Test: Dependency finding works
// Verifies that dependencies can be extracted from ELF files and are properly
// identified as either resolved (found in available phases) or missing.

#include <cstdio>
#include <filesystem>
#include "internal-loader.hpp"

int test_dependency_finding() {
  auto scenario = std::filesystem::current_path() / "tests" / "scenario_basic";

  auto mods = modloader::listAllObjectsInPhase(scenario, modloader::LoadPhase::Mods);
  if (mods.empty()) {
    printf("✗ No mods found\n");
    return 1;
  }

  // Test each mod's dependencies
  int modsChecked = 0;
  int totalDeps = 0;
  int resolved = 0;
  int missing = 0;

  for (auto& mod : mods) {
    auto deps = mod.getToLoad(scenario, modloader::LoadPhase::Mods);
    modsChecked++;

    for (auto const& dep : deps) {
      totalDeps++;

      if (std::holds_alternative<modloader::Dependency>(dep)) {
        resolved++;
      } else if (std::holds_alternative<modloader::MissingDependency>(dep)) {
        missing++;
      }
    }
  }

  if (modsChecked != 5) {
    printf("✗ Expected to check 5 mods, checked %d\n", modsChecked);
    return 1;
  }
  printf("✓ Checked %d mods\n", modsChecked);

  if (totalDeps < 50) {
    printf("✗ Expected ~56 dependencies, found %d\n", totalDeps);
    return 1;
  }
  printf("✓ Found %d total dependencies\n", totalDeps);

  if (resolved < 9) {
    printf("✗ Expected at least 9 resolved dependencies, found %d\n", resolved);
    return 1;
  }
  printf("✓ Found %d resolved dependencies\n", resolved);

  if (missing < 5) {
    printf("✗ Expected at least 5 missing dependencies, found %d\n", missing);
    return 1;
  }
  printf("✓ Found %d missing dependencies\n", missing);

  printf("✓ Dependency finding works correctly\n");
  return 0;
}

#ifdef LINUX_TEST
int main() { return test_dependency_finding(); }
#endif
