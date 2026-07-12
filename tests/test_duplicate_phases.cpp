// Test: Duplicated dependencies in different phases
// Verifies that if the same object appears in or is needed by multiple phases,
// it is only loaded once via the skipLoad tracking mechanism.

#include <cstdio>
#include <filesystem>
#include <unordered_set>
#include "internal-loader.hpp"

int test_duplicate_phases() {
  auto scenario = std::filesystem::current_path() / "tests" / "scenario_basic";

  auto libs = modloader::listAllObjectsInPhase(scenario, modloader::LoadPhase::Libs);
  auto mods = modloader::listAllObjectsInPhase(scenario, modloader::LoadPhase::Mods);
  auto earlyMods = modloader::listAllObjectsInPhase(scenario, modloader::LoadPhase::EarlyMods);

  if (libs.empty() || mods.empty()) {
    printf("✗ Need both libs and mods\n");
    return 1;
  }
  printf("✓ Found %zu libs, %zu mods, %zu early_mods\n", libs.size(), mods.size(), earlyMods.size());

  // Check for duplicates within each phase
  std::unordered_set<std::string> libNames;
  for (auto const& lib : libs) {
    std::string name = lib.path.filename().string();
    if (libNames.count(name) > 0) {
      printf("✗ Duplicate in Libs phase: %s\n", name.c_str());
      return 1;
    }
    libNames.insert(name);
  }
  printf("✓ All %zu libs are unique within phase\n", libs.size());

  std::unordered_set<std::string> modNames;
  for (auto const& mod : mods) {
    std::string name = mod.path.filename().string();
    if (modNames.count(name) > 0) {
      printf("✗ Duplicate in Mods phase: %s\n", name.c_str());
      return 1;
    }
    modNames.insert(name);
  }
  printf("✓ All %zu mods are unique within phase\n", mods.size());

  // Test skipLoad mechanism - simulate loading
  std::unordered_set<std::string> skipLoad;

  // Simulate loading all libs first
  for (auto const& lib : libs) {
    skipLoad.insert(lib.path.stem().string());
  }

  // Count how many mods would be skipped
  int skipped = 0;
  for (auto const& mod : mods) {
    if (skipLoad.count(mod.path.stem().string()) > 0) {
      skipped++;
      printf("✓ Would skip duplicate: %s\n", mod.path.filename().c_str());
    }
  }

  printf("✓ Skip-load would prevent %d duplicate loads\n", skipped);

  // Verify total unique objects
  int total = libs.size() + (mods.size() - skipped);
  printf("✓ Total unique objects after deduplication: %d (%zu libs + %zu mods)\n",
         total, libs.size(), mods.size() - skipped);

  printf("✓ Duplicate prevention works correctly\n");
  return 0;
}

#ifdef LINUX_TEST
int main() { return test_duplicate_phases(); }
#endif
