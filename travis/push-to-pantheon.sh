#!/bin/bash

# Add vendor/bin and $HOME/bin to our $PATH
export PATH="$TRAVIS_BUILD_DIR/bin:$HOME/bin:$PATH"

# Do not attempt to push the site up to Pantheon unless the PPASS
# environment variable is set.  If we receive a PR from an external
# repository, then the secure environment variables will not be set;
# if we test PRs, we cannot send them to Pantheon in this instance.
if [ -n "$PPASS" ]
then
  # Dynamic hosts through Pantheon mean constantly checking interactively
  # that we mean to connect to an unknown host. We ignore those here.
  echo "StrictHostKeyChecking no" > ~/.ssh/config

  # Capture the commit message
  export TRAVIS_COMMIT_MSG="$(git log --format=%B --no-merges -n 1)"

  # Install Terminus
  curl https://github.com/pantheon-systems/cli/releases/download/0.5.5/terminus.phar -L -o $HOME/bin/terminus
  chmod +x $HOME/bin/terminus

  # Set up Terminus and aliases, and wake up the site
  echo "Log in to Pantheon via Terminus"
  # Redirect output when logging in.  Terminus prints the email address
  # used during login to stdout, which will end up in the log.  It's better
  # to conceal the email address used, to make it harder for a third party
  # to try to log in via a brute-force password-guessing attack.
  terminus auth login "$PEMAIL" --password="$PPASS" >/dev/null 2>&1
  if [ $? == 0 ]
  then
    echo "Login successful"
  else
    echo "Could not log in"
    exit 1
  fi
  echo "Fetch aliases via Terminus"
  mkdir -p $HOME/.drush
  terminus sites aliases
  echo "Wake up the site $PSITE"
  terminus site wake --site="$PSITE" --env="$PENV"
  echo "Look up site UUID"
  PUUID=$(terminus site info --site="$PSITE" --field=id)

  # Identify the automation user
  git config --global user.email "bot@westkingdom.org"
  git config --global user.name "West Kingdom Automation"

  # Clone the Pantheon repository into a separate directory, then
  # move the .git file to the location where we placed our composer targets
  cd $TRAVIS_BUILD_DIR
  git clone --depth 1 ssh://codeserver.dev.$PUUID@codeserver.dev.$PUUID.drush.in:2222/~/repository.git $HOME/pantheon
  mv $HOME/pantheon/.git $TRAVIS_BUILD_DIR/htdocs
  cd $TRAVIS_BUILD_DIR/htdocs

  # Output of the diff vs upstream.
  echo "Here's the status change!"
  git status

  # Make sure we are in git mode
  terminus site connection-mode --site="$PSITE" --env="$PENV" --set=git

  # Push our built files up to Pantheon
  git add --all
  git commit -a -m "Makefile build by CI: '$TRAVIS_COMMIT_MSG'"
  git push origin master
fi
