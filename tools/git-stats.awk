#!/usr/bin/awk -f

function printUsage() {
    print "Usage: git ls-files | ./tools/git-stats.awk"
    print "Options:"
    print "  --help      Print this help message"
}

BEGIN {
    # Set the field separator to whitespace
    FS = " "

    if (ARGC >= 1 && (ARGV[1] ~ /(-?-?help|-h)/)) {
        printUsage()
        skipEnd = 1
        exit
    }
}

# Process each file listed by 'git ls-files'
{
    file = $0
    if (file !~ /.*?(\.cpp|\.hpp)$/ || file ~ /.*?build.*/) {
        next
    }
    # Read the file and count lines, words, and characters
    while ((getline < file) > 0) {
        lines++
        words += NF
        chars += length + 1  # +1 for the newline character

        # Count the occurrences of each word
        for (i = 1; i <= NF; i++) {
            word = $i
            gsub(/\W/, "", word)
            if (length(word) > 3) {
                wordCount[word]++
            }
        }
    }
    close(file)
}

END {
    if (skipEnd) {
        exit
    }

    # Print the totals
    printf("Lines: %d\n", lines)
    printf("Words: %d\n", words)
    printf("Chars: %d\n", chars)

    asorti(wordCount, sortedWords, "@val_num_desc")

    # Print the most used words
    printf("\nMost Used Words:\n")
    for (i = 1; i <= 100 && i <= length(sortedWords); i++) {
        word = sortedWords[i]
        print wordCount[word], word
    }
}

