#!/usr/bin/awk

# Usage:
#  awk -f gen-idna-tests.awk IdnaMappingTable.txt

BEGIN{
    FS = ";";
    OF = ";";

    for (i = 0; i < 10; i++) {
        decv[i] = i;
    }
    decv["a"] = 10; decv["b"] = 11; decv["c"] = 12;
    decv["d"] = 13; decv["e"] = 14; decv["f"] = 15;
    decv["A"] = 10; decv["B"] = 11; decv["C"] = 12;
    decv["D"] = 13; decv["E"] = 14; decv["F"] = 15;
}

function hex(x) {
    value = 0;
    n = length(x);
    for (i = 1; i <= n; i++) {
        value = value * 16 + decv[substr(x, i, 1)];
    }
    return value;
}



# Remove the comments:
!/(^# .*?$)|^$/ {
    status = $2
    repl = $3
    gsub(/\s+/, "", status)
    gsub(/#.*?$/, "", status)
    gsub(/#.*?$/, "", repl)
    gsub(/\s+/, "", $1)
    split($1, code_points, "\\.\\.")
    cp1 = code_points[1] != "" ? code_points[1] : $1
    cp2 = code_points[2] != "" ? code_points[2] : cp1
    start = hex(cp1) # Convert from hex
    end = hex(cp2)
    tenPercent = int((end - start) * 0.1)
    tenPercent = tenPercent <= 0 ? 1 : tenPercent
    # print "IMP " tenPercent " --- " start ", " end "--->" $1 "|---|" cp1 "|" cp2 "|"
    if (status != "") {
        for (code_point = start; code_point <= end; code_point += tenPercent) {
            if (status == "disallowed" || status == "valid") {
                print "    EXPECT_EQ(status_of(" code_point "), " status ");";
            } else if (status == "deviation") {
                print "    EXPECT_EQ(status_of(" code_point "), valid); // " status;
            } else {
                gsub(/\s+$/, "", repl);
                gsub(/\s+/, "\\x", repl);
                print "    EXPECT_EQ(mapped<std::u32string>(" code_point "), U\"" repl "\"); // " status;
            }
        }
    }
}

