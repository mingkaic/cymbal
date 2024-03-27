workspace(name = "com_github_mingkaic_cymbal")

# === load local dependencies ===

load("//third_party:all.bzl", cymbal_dl="download")
cymbal_dl()

# === load test dependencies ===

load("//third_party:all.bzl", cymbal_dl_test="download_test")
cymbal_dl_test()

load("@com_github_mingkaic_verum//third_party:all.bzl", verum_deps="dependencies")
verum_deps()
