#!/bin/bash

SELF_DIRNAME="`dirname -- "$0"`"
ROOT_PATH="`cd -P -- "$SELF_DIRNAME/.." && pwd -P`"

# Set up our $PATH
export PATH="$TRAVIS_BUILD_DIR/bin:$HOME/bin:$PATH"

# Fix bug in our custom installer
if [ -d $ROOT_PATH/htdocs/sites/sites/default ]
then
  mv $ROOT_PATH/htdocs/sites/sites/default $ROOT_PATH/htdocs/sites/default
fi

if [ ! -f $ROOT_PATH/htdocs/sites/default/default.settings.php ]
then
  echo "No default.settings.php file; did you run composer install?"
  exit 1
fi

# Do the settings.php shuffle for an empty settings.php
# This prevents permissions issues with the sites/default directory
cp $ROOT_PATH/htdocs/sites/default/default.settings.php $ROOT_PATH/htdocs/sites/default/settings.php

# Use Drush to install Drupal and spin up PHP's built-in webserver
(
  cd $ROOT_PATH/htdocs
  drush site-install -y standard --site-name="$SITE_NAME Travis Test Site" --db-url=mysql://root@localhost/drupal --account-name=admin --account-pass=admin
  drush runserver --server=builtin 8088 --strict=0 &
)

# Wait for a little while to let the webserver spin up
echo "Waiting for the web server to finish spinning up."
until netstat -an 2>/dev/null | grep '8088.*LISTEN'; do sleep 0.2; done
echo "Got a response from our webserver; continuing."