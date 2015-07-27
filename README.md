West Kingdom Website Repository
===============================

This repository holds the code for the regognized website for the
Kingdom of the West of the Society for Creative Anachronism, Inc.

This project is based on the pattern shown in the [pantheon-systems/example-drupal7-composer](https://github.com/pantheon-systems/example-drupal7-composer) project.


Directory of Files
------------------

-  **drupal:** Drupal root
-  **features:** Behat tests
-  **travis:** Public/Private key pair for Travis testing
-  **scripts:** Define configuration and environment for tests
-  **bin:** Scripts provided from Composer dependencies.


Configuration
-------------

Uses drush_ctex_bonus to export configuration, which is imported again via the [install-configuration](https://github.com/westkingdom/website/blob/master/scripts/install-configuration) script.


Deploying
---------

tbd


Testing Locally
---------------

Run:

./bin/local-test

The tests are also run via Travis automatically on every push to the central GitHub repository.
