// Test: Missing dependency error handling
// Verifies that missing dependencies (system libraries not in available phases)
// are correctly detected and reported without causing failures.

#include <cstdio>
#include <filesystem>
#include "internal-loader.hpp"

int test_missing_dependency() {
  auto scenario = std::filesystem::current_path() / "tests" / "scenario_basic";

  auto mods = modloader::listAllObjectsInPhase(scenario, modloader::LoadPhase::Mods);
  if (mods.empty()) {
    printf("✗ No mods found\n");
    return 1;
  }

  // Check for missing dependencies
  int modsChecked = 0;
  int missingFound = 0;
  bool foundLibc = false;
  bool foundLibm = false;
  bool foundLibdl = false;

  for (auto& mod : mods) {
    auto deps = mod.getToLoad(scenario, modloader::LoadPhase::Mods);
    modsChecked++;

    for (auto const& dep : deps) {
      if (std::holds_alternative<modloader::MissingDependency>(dep)) {
        auto& missing = std::get<modloader::MissingDependency>(dep);
        std::string name = missing.path.string();

        missingFound++;

        if (name == "libc.so") foundLibc = true;
        if (name == "libm.so") foundLibm = true;
        if (name == "libdl.so") foundLibdl = true;
      }
    }
  }

  if (modsChecked != 5) {
    printf("✗ Expected to check 5 mods, checked %d\n", modsChecked);
    return 1;
  }
  printf("✓ Checked %d mods for missing dependencies\n", modsChecked);

  if (missingFound == 0) {
    printf("✗ No missing dependencies found (expected system libraries)\n");
    return 1;
  }
  printf("✓ Found %d missing dependencies\n", missingFound);

  // Verify expected system libraries are missing
  int systemLibsFound = 0;
  if (foundLibc) systemLibsFound++;
  if (foundLibm) systemLibsFound++;
  if (foundLibdl) systemLibsFound++;

  if (systemLibsFound < 2) {
    printf("✗ Expected libc.so, libm.so, libdl.so to be missing, found %d\n", systemLibsFound);
    return 1;
  }
  printf("✓ Correctly identified system libraries as missing (found %d/3)\n", systemLibsFound);

  printf("✓ Missing dependency error handling works correctly\n");
  return 0;
}

#ifdef LINUX_TEST
int main() { return test_missing_dependency(); }
#endif
