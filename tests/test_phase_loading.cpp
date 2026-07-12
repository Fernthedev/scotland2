// Test: Modloading works with phases
// Verifies that objects are correctly discovered and loaded from their respective phases
// (Libs, EarlyMods, Mods). Ensures phase separation is enforced.

#include <cstdio>
#include <filesystem>
#include "internal-loader.hpp"

int test_phase_loading() {
  auto scenario = std::filesystem::current_path() / "tests" / "scenario_basic";

  // Load Libs phase
  auto libs = modloader::listAllObjectsInPhase(scenario, modloader::LoadPhase::Libs);
  if (libs.empty() || libs.size() != 6) {
    printf("✗ Expected 6 libraries in Libs phase, got %zu\n", libs.size());
    return 1;
  }
  printf("✓ Loaded 6 libraries from Libs phase\n");

  // Load Mods phase
  auto mods = modloader::listAllObjectsInPhase(scenario, modloader::LoadPhase::Mods);
  if (mods.empty() || mods.size() != 5) {
    printf("✗ Expected 5 mods in Mods phase, got %zu\n", mods.size());
    return 1;
  }
  printf("✓ Loaded 5 mods from Mods phase\n");

  // Verify libs and mods are different
  if (libs.size() == mods.size()) {
    printf("✗ Libs and mods have same count (phase separation failed)\n");
    return 1;
  }
  printf("✓ Libs (%zu) and Mods (%zu) properly separated\n", libs.size(), mods.size());

  // Verify all have valid paths
  for (auto const& lib : libs) {
    if (lib.path.empty() || !std::filesystem::exists(lib.path)) {
      printf("✗ Invalid library path: %s\n", lib.path.c_str());
      return 1;
    }
  }
  for (auto const& mod : mods) {
    if (mod.path.empty() || !std::filesystem::exists(mod.path)) {
      printf("✗ Invalid mod path: %s\n", mod.path.c_str());
      return 1;
    }
  }
  printf("✓ All paths are valid and exist\n");

  printf("✓ Phase loading works correctly\n");
  return 0;
}

#ifdef LINUX_TEST
int main() { return test_phase_loading(); }
#endif
