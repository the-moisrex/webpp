#!/usr/bin/awk -f

# Usage:
#  awk -f gen-joiners-tests.awk DerivedJoiningType.txt
#  ./gen-joiners-tests.awk DerivedJoiningType.txt

# AWK script to generate C++ GTest EXPECT_EQ lines from DerivedJoiningType.txt
# It generates tests for explicitly listed types and adds sample tests
# for the default 'non_joining' type.
# Usage: awk -f generate_gtest_with_default.awk DerivedJoiningType.txt > joining_type_test.cpp

BEGIN {
    # Map the Joining_Type abbreviation to the C++ enum name
    jtmap["C"] = "join_causing"
    jtmap["D"] = "dual_joining"
    jtmap["L"] = "left_joining"
    jtmap["R"] = "right_joining"
    jtmap["T"] = "transparent"
    jtmap["U"] = "non_joining"; # Added for completeness, used in END block

    # Set Field Separator to semicolon, surrounded by optional whitespace
    FS = "[[:space:]]*;[[:space:]]*"

}

# Skip comment lines and blank lines
/^#/ { next }
/^[[:space:]]*$/ { next }

# Process data lines (should have at least 2 fields after splitting by ;)
NF >= 2 {
    # Field 1: Code point(s) (e.g., "0640" or "0883..0885")
    # Field 2: Joining Type Abbreviation and the rest (e.g., "C # Lm  ARABIC TATWEEL")

    codepoints = $1
    sub(/[[:space:]]+$/, "", codepoints); # Trim trailing whitespace from codepoints field

    # Extract the joining type abbreviation (the first non-space character of field 2)
    jt_abbr = $2
    sub(/^[[:space:]]+/, "", jt_abbr); # Trim leading whitespace
    jt_abbr = substr(jt_abbr, 1, 1);   # Get the first character

    # Get the corresponding C++ enum name from the map
    enum_name = jtmap[jt_abbr]

    if (enum_name == "" || enum_name == "non_joining") {
        # Type U (non_joining) should not be explicitly listed in this file.
        # If it is, or if the type is unknown, print a warning and skip.
        print "// Warning: Skipping line with unexpected or unknown Joining_Type abbreviation '" jt_abbr "': " $0 > "/dev/stderr"
        next
    }

    # Check if it's a range or a single code point
    if (index(codepoints, "..")) {
        # It's a range
        split(codepoints, range, "\\.\\."); # Split by ".."
        start_hex = "0x" range[1]
        end_hex   = "0x" range[2]

        # Convert hex to decimal for iteration
        start_dec = strtonum(start_hex)
        end_dec   = strtonum(end_hex)

        # Iterate through the range (inclusive)
        for (cp = start_dec; cp <= end_dec; cp++) {
            # Format the code point back to hex for the C++ output
            hex_cp = sprintf("%X", cp)
            # Print the GTest line
            printf("    EXPECT_EQ(joiner_type_of(U'\\x%s'), joiner_type::%s); // %s\n", hex_cp, enum_name, jt_abbr)
        }
    } else {
        # It's a single code point
        hex_cp = codepoints
        # Print the GTest line
        printf("    EXPECT_EQ(joiner_type_of(U'\\x%s'), joiner_type::%s); // %s\n", hex_cp, enum_name, jt_abbr)
    }
}

