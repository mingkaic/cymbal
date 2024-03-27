package(
	default_visibility = ["//visibility:public"],
)

filegroup(
    name = "hdrs",
    srcs = glob(
        include = [
            "Fastor/**/*.h",
        ],
        exclude = ["**/CMakeLists.txt"],
    ),
)

cc_library(
    name = "fastor",
    hdrs = [":hdrs"],
	defines = ["DISABLE_ASM_INTERACTION"],
)
