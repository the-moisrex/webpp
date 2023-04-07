#!/bin/bash

function print_help {
  echo "Setup Development Tools for ease of use"
  echo "setup-repo --help         Print this page"
  echo "           --hooks        Setup Hooks"
}

function setup_hooks {
  git config --local core.hookspath "\$GIT_WORK_TREE/tools/git-hooks"
  echo "Git's hooks' path is now set to: '\$GIT_WORK_TREE/tools/git-hooks'"
}

function setup_repo {
  setup_hooks
}

case $1 in
  --help|help|-h)
    print_help;
    ;;
  --hooks)
    setup_hooks;
    ;;
  *)
    setup_repo;
    ;;
esac
