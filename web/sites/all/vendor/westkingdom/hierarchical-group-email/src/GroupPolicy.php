<?php

namespace Westkingdom\HierarchicalGroupEmail;

/**
 * The GroupPolicy object is responsible for what groups
 * are called, and how they are supposed to behave.  For
 * standard behavior, use a StandardGroupPolicy.
 */
interface GroupPolicy {
  /**
   * Convert from a branch name and an office name
   * to a group ID.  Standard behavior is to use the
   * group email address as its group id.
   */
  function getGroupId($branch, $officename);

  /**
   * Convert from a branch name and an office name
   * to the group's primary email address.  Standard
   * behavior is branch-office@domain.  The domain
   * is stored in the StandardGroupPolicy object.
   */
  function getGroupEmail($branch, $officename);

  /**
   * Look up the name of the group based on its
   * branch and office.  Standard behaivor is
   * to use the 'group-name' property, if it is
   * set, or "Branch Office" if it is not.
   */
  function getGroupName($branch, $officename, $properties);

  /**
   * Return the domain name associated with these Google groups.
   */
  function getDomain();

  /**
   * Normalize an email address
   */
  function normalizeEmail($email);

  /**
   * Generate 'parentage' elements from 'subgroups' elements.
   */
  function generateParentage($memberships);

  /**
   * Normalize the entire state record
   */
  function normalize($state);
}
