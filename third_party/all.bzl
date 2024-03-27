load("//third_party:repos/cisab.bzl", cisab_dl="download")
load("//third_party:repos/fastor.bzl", fastor_dl="download")
load("//third_party:repos/verum.bzl", verum_dl="download")

def validate(dep, excludes, force_includes):
    return dep not in excludes or dep in force_includes

def download(excludes = [], force_includes = []):
    ignores = native.existing_rules().keys() + excludes
    print("downloading cymbal dependencies. ignoring {}".format(ignores))

    if validate("com_github_mingkaic_cisab", ignores, force_includes):
        cisab_dl()

def download_test(excludes = [], force_includes = []):
    ignores = native.existing_rules().keys() + excludes
    print("downloading test cymbal dependencies. ignoring {}".format(ignores))

    if validate("com_github_mingkaic_verum", ignores, force_includes):
        verum_dl()

    if validate("com_github_romeric_Fastor", ignores, force_includes):
        fastor_dl()
