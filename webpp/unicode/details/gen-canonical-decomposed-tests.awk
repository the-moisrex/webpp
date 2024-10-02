#!/usr/bin/awk

BEGIN{
    FS=";";
    OF=";";
}

# Remove the comments:
!/^\s*#|</ {if ($6 != "") { 
    rec[$1] = $6; }
} 

END { 
    # Recursively (kinda) calculate the compositions:
    for (repl in rec) {
        split(rec[repl], cps, " ");
        while (cps[1] in rec && rec[cps[1]] != "") {
            split(rec[cps[1]], repcp, " ");
            cps[1] = repcp[1];
            for (cp in repcp) {
                if (cp != 1) {
                    cps[2] = repcp[cp] " " cps[2];
                }
            }
            /* print "Replaced: \'" cps[1] "\' " rec[cps[1]]; */ 
        }

        # join:
        cpstr = "";
        for (cp in cps) {
            cpstr = cpstr " " cps[cp];
        }

        rec[repl] = cpstr;
    }

    # Print the codes
    for (repl in rec) {
        gsub(/\s$/, "", rec[repl]);
        gsub(/\s+/, "\\x", rec[repl]);
        print "    EXPECT_EQ(canonical_decomposed<u32string>(U'\\x" repl "'), U\"" rec[repl] "\") << desc_decomp_of(U'\\x" repl "');";
    }
}
