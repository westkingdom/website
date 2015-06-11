#!/bin/bash

SELF_DIRNAME="`dirname -- "$0"`"
ROOT_PATH="`cd -P -- "$SELF_DIRNAME/.." && pwd -P`"

# Set up our $PATH
export PATH="$TRAVIS_BUILD_DIR/bin:$HOME/bin:$PATH"

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
