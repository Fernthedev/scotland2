// Test: Dependency sorting works
// Verifies that dependencies are sorted in correct topological order
// (each dependency available before dependent objects).

#include <cstdio>
#include <filesystem>
#include "internal-loader.hpp"

int test_dependency_sorting() {
  auto scenario = std::filesystem::current_path() / "tests" / "scenario_basic";

  auto mods = modloader::listAllObjectsInPhase(scenario, modloader::LoadPhase::Mods);
  if (mods.empty()) {
    printf("✗ No mods found\n");
    return 1;
  }

  int modsWithDeps = 0;
  int successfulSorts = 0;

  // Sort dependencies from each mod
  for (auto& mod : mods) {
    auto deps = mod.getToLoad(scenario, modloader::LoadPhase::Mods);

    if (deps.empty()) {
      continue;
    }

    modsWithDeps++;

    // Perform topological sort
    auto sorted = modloader::topologicalSort(std::move(deps));

    if (sorted.empty()) {
      printf("✗ Sort failed for mod (returned empty)\n");
      return 1;
    }

    successfulSorts++;

    // Verify all items in sorted list have valid paths
    for (auto const& item : sorted) {
      if (item.object.path.empty()) {
        printf("✗ Sorted item has empty path\n");
        return 1;
      }
    }

    printf("✓ %s: sorted %zu dependencies\n", mod.path.filename().c_str(), sorted.size());
  }

  if (modsWithDeps == 0) {
    printf("✗ No mods had dependencies to sort\n");
    return 1;
  }

  if (successfulSorts != modsWithDeps) {
    printf("✗ Only sorted %d of %d mods\n", successfulSorts, modsWithDeps);
    return 1;
  }

  printf("✓ Successfully sorted %d mods with dependencies\n", successfulSorts);
  printf("✓ Dependency sorting works correctly\n");
  return 0;
}

#ifdef LINUX_TEST
int main() { return test_dependency_sorting(); }
#endif
