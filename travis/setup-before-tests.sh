#!/bin/bash

# Set up our $PATH
export PATH="$TRAVIS_BUILD_DIR/htdocs/sites/all/vendor/bin:$HOME/bin:$PATH"

# Set the 'sendmail_path' to point to the program 'true'; this
# will cause the system to essentially swallow any emails Drupal
# tries to send.
echo "sendmail_path='true'" >> `php --ini | grep "Loaded Configuration" | awk '{print $4}'`

# Use Drush to install Drupal and spin up PHP's built-in webserver
cd htdocs
drush site-install -y standard --site-name="$SITE_NAME" --db-url=mysql://root@localhost/drupal --account-name=admin --account-pass=admin
drush runserver --server=builtin 8080 --strict=0 &
cd ..

# Wait for a little while to let the webserver spin up
until netstat -an 2>/dev/null | grep '8080.*LISTEN'; do sleep 0.2; done
