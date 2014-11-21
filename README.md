West Kingdom Website Repository
===============================

This repository holds the code for the regognized website for the
Kingdom of the West of the Society for Creative Anachronism, Inc.


Directory of Files
------------------

  htdocs         - Drupal root
  ssl-certs      - Backup copies of ssl certificates installed in web server
  private-files  - Drupal private upload files storage
  puppet         - Installation scripts
  Vagrantfile    - Can be used to start up a virtual environment for testing


Deploying
---------

tbd


Using the Vagrantfile
---------------------

This project contains a Vagrantfile and associated puppet manifests and
modules to sets up a postfix environment for testing the email forwarding
software.

The Vagrantfile defines two virtual machines:

  postfix         - The email server
  client          - A simple linux system to send test email to

Hiera is used to allow different machines to be set up with configuration
appropriate to their needs.  The hiera configuration files are located
in puppet/manifests/configuration.  Make particular note of the configuration
file named `postfix.postfix.local.yaml` -- this is the configuration file
for the test Vagrant postfix mail server.
