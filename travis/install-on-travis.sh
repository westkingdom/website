#!/bin/bash

# Add a 'bin' directory
mkdir -p $HOME/bin

# Run composer install to install Drupal (pantheon-systems/drops-7),
# all of our modules, and other components we need
composer install
if [ "$?" != "0" ]
then
  echo "Composer install failed." >&2
  exit 1
fi

# Fix bug in our custom installer
mv $TRAVIS_BUILD_DIR/htdocs/sites/sites/default $TRAVIS_BUILD_DIR/htdocs/sites/default

# Do the settings.php shuffle for an empty settings.php
# This prevents permissions issues with the sites/default directory
cp $TRAVIS_BUILD_DIR/htdocs/sites/default/default.settings.php $TRAVIS_BUILD_DIR/htdocs/sites/default/settings.php

# Add vendor/bin and $HOME/bin to our $PATH
export PATH="$TRAVIS_BUILD_DIR/htdocs/sites/all/vendor/bin:$HOME/bin:$PATH"
