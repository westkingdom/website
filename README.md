OGM / POSTFIX ENVIRONMENT
=========================

This project sets up a postfix environment for use with the og_mailinglist
Drupal module.  For testing purposes, a Vagrantfile is also provided; it
defines a postfix mail server called 'postfix', and a secondary clinet
machine called the 'client' for testing email delivery from another machine.

Hiera is used to allow different machines to be set up with configuration
appropriate to their needs.  The hiera configuration files are located
in puppet/manifests/configuration.  Make particular note of the configuration
file named `postfix.postfix.local.yaml` -- this is the configuration file
for the test Vagrant server.

The following puppet modules are used to create this environment:

  - apache
  - composer
  - concat
  - dns
  - drupal
  - drush
  - mysql
  - ogm
  - php
  - postfix
  - procmail
  - stdlib
  - wget

These modules are cloned here for convenience.  Submit any improvements to
the original module's project page.
