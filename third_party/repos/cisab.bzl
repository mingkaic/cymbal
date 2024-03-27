load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

def download():
    COMMIT = "9907748007515f1339340e6cd0a8bbafe023a741"
    print("downloading cisab commit={} from cymbal".format(COMMIT))
    git_repository(
        name = "com_github_mingkaic_cisab",
        remote = "https://github.com/mingkaic/cisab",
        commit = COMMIT,
    )
