<?php

/**
 * @file
 * API functions for Multiple Email module.
 */

/**
 * @addtogroup hooks
 * @{
 */

/**
 * Signal that an email address has been registered by Multiple Email.
 *
 * @param $email
 *   The fully-loaded email object created by Multiple Email.
 */
function hook_multiple_email_register($email) {
  drupal_set_message(t('Do not forget to confirm %email!', array('%email' => $email->mail)));
}

/**
 * Signal that an email address has been confirmed, or un-confirmed.
 *
 * Check $email->confirmed to determine if the email was confirmed or not.
 *
 * @param $email
 *   The fully-loaded email object created by Multiple Email.
 */
function hook_multiple_email_confirm($email) {
  drupal_set_message(t('Now that %email has been confirmed, you may make it your primary address.', array('%email' => $email->mail)));
}

/**
 * Signal that an email address has been deleted by Multiple Email.
 *
 * When an email address is deleted by multiple email, this hook is invoked to
 * notify other modules this has happened. Addresses might also be deleted in
 * multiple_email's implementation of hook_user(), without triggering this
 * hook.
 *
 * @param $eid
 *   Email Address ID.
 */
function hook_multiple_email_delete($eid) {
  db_query("DELETE FROM {mytable} WHERE eid = %d", $eid);
}

/**
 *@} End of "addtogroup hooks".
 */
