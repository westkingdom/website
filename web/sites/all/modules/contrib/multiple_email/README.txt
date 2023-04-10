Module: Multiple E-mail Addresses
Original Author: Joshua Benner <joshbenner@gmail.com>
Sponsor of Drupal 5 version: Rock River Star (www.rockriverstar.com)

Drupal 6 version maintainer: Shawn DeArmond (http://drupal.org/user/77785)

The Multiple E-mail module allows users to register additional emails for their
user accounts. Only one e-mail address is considered to be the "primary" email
address, and will continue to behave as normal. Non-primary accounts are
mostly functionally meaningless, except that during user registration any
e-mail address registered to a user cannot be used to create a new account.

Users may select any confirmed e-mail address to become their primary email
address. This means that the user account edit page's e-mail address field will
not change the user's e-mail address. The default settings for the module will
actually hide the e-mail address field on the user account edit page.

Once the module is installed, administration settings are available under Site
Configuration -> Multiple E-mail Settings. The configuration options are rather
straight-forward at this point and are documented in the field descriptions.

The module will create a menu item in the Navigation menu called 'E-Mail
Addresses' that links to the user's e-mail management page.

Hooks
-----
hook_multiple_email_register($email)
  - $email is the e-mail object that has just been registered
  - Use this hook to perform actions when a user registers an e-mail address
    (but isn't confirmed yet)

hook_multiple_email_confirm($email)
  - $email is the e-mail object that has just been registered
  - Use this hook to perform actions when a user confirms an e-mail address

hook_multiple_email_delete($eid)
  - $eid is the e-mail object ID that has just been deleted
  - Use this hook to perform actions when a user deletes an e-mail address
