load("@bazel_tools//tools/build_defs/repo:git.bzl", "new_git_repository")

def download():
    COMMIT = "21c7747ca7c0b5298e5bbf7469a1d9e4880a8482"
    print("downloading fastor commit={} from cymbal".format(COMMIT))
    new_git_repository(
        name = "com_github_romeric_Fastor",
        remote = "https://github.com/mingkaic/Fastor.git",
        commit = COMMIT,
        build_file = "@com_github_mingkaic_cymbal//third_party:fastor.BUILD",
    )
