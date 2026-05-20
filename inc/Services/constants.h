#pragma once

#include <string_view>

inline constexpr std::string_view TXT_FILE_EXTENSION{".txt"};
inline constexpr std::string_view CSV_FILE_EXTENSION{".csv"};

inline constexpr std::string_view FILE_NAME_INITIAL_INDIVIDUAL{"initial_individual.txt"};

inline constexpr std::string_view FILE_NAME_INDIVIDUAL{"final_individual.txt"};
inline constexpr std::string_view FILE_NAME_INDIVIDUAL_VALUES{"individual_values.csv"};

inline constexpr std::string_view FILE_NAME_ALGORITHM_SETTINGS{"algorithm_settings.txt"};

inline constexpr int LOWER_BOUND{0};
inline constexpr int UPPER_BOUND{1};

inline constexpr double EPSILON{0.0000001};
inline constexpr double EPSILON_STRESS{10.0};

inline constexpr double MINIM_INDIVIDUAL_VALUE{1.0};