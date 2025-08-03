#include "Io_unit_test.hpp"

TEST(
    IoUnitTest,
    SimpleTypesYaml) {
    Parser                   io;
    ConfigSample_SimpleTypes config("simple_types.yaml");
    io.load_parameters_from_yaml(
        config.get_sample_path(), config.get_parsing_table());
    EXPECT_TRUE(config.correct_read());
}

TEST(
    IoUnitTest,
    ArrayTypesYaml) {
    Parser                  io;
    ConfigSample_ArrayTypes config("array_types.yaml");
    io.load_parameters_from_yaml(
        config.get_sample_path(), config.get_parsing_table());
    EXPECT_TRUE(config.correct_read());
}
