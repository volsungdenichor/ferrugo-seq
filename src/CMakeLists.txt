set(TARGET_NAME ferrugo-seq)

add_executable (${TARGET_NAME} main.cpp)

include_directories(
    "${PROJECT_SOURCE_DIR}/include"
)

target_include_directories(
    ${TARGET_NAME}
    PUBLIC
    "${PROJECT_SOURCE_DIR}/include"
    "${ferrugo-core_SOURCE_DIR}/include")