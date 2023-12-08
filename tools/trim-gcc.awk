#!/usr/bin/awk -f

function printUsage() {
    print "Usage: ninja test-... | ../tools/trim-gcc.awk"
    print "Options:"
    print "  --help      Print this help message"
}

# Check if a file is a directory
function isDir(path) {
  cmd_dir = "[ -d \"" path "\" ]"
  res = system(cmd_dir)
  close(cmd_dir)
  return res == 0
}

# Find the root of the git project
function findProjectRoot() {
  gitroot = ".git";
  while (1) {
    cmd = "realpath \"" gitroot  "\" 2>/dev/null"
    cmd | getline path
    print cmd
    if (path == "/.git" || path == "/" || isDir(path)) {
      break
    }
    gitroot = "../" gitroot
    close(cmd)
  }

  cmd = "realpath \"" gitroot "/.." "\" 2>/dev/null"
  cmd | getline proj_dir
  close(cmd)
  return proj_dir
}

BEGIN {
  FS = " "

  if (ARGC >= 1 && (ARGV[1] ~ /(-?-?help|-h)/)) {
    printUsage()
    skipEnd = 1
    exit
  }

  proj_dir = findProjectRoot()
  print "Project Root: " proj_dir
}

# Process each line
{
  trimmed = $0

  # Remove namespaces
  gsub(/webpp::/, "", trimmed)
  gsub(/http::/, "", trimmed)
  gsub(/std::/, "", trimmed)
  gsub(/stl::/, "", trimmed)

  # Remove project root paths
  gsub(proj_dir, "", trimmed);

  # Clang-Format the strings
  match(trimmed, /‘(.*?)’/)
  type_str = substr(trimmed, RSTART + 1, RLENGTH - 2)
  clangformat_cmd = "clang-format <<ENDOFCLANGFORMAT\n" type_str "\nENDOFCLANGFORMAT"

  print substr(trimmed, 0, RSTART)
  while ((clangformat_cmd | getline line)) {
    print line
  }
  close(clangformat_cmd)
  print substr(trimmed, RSTART + 1 + RLENGTH + 1)
}

END {
  if (skipEnd) {
    exit
  }
}
