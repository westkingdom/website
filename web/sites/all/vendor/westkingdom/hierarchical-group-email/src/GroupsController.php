<?php

namespace Westkingdom\HierarchicalGroupEmail;

use Westkingdom\HierarchicalGroupEmail\Internal\Operation;

interface GroupsController {
  function begin();
  function insertBranch(Operation $op, $branch);
  function deleteBranch(Operation $op, $branch);
  function verifyBranch(Operation $op, $branch);
  function insertMember(Operation $op, $branch, $officename, $group_id, $memberEmailAddress);
  function removeMember(Operation $op, $branch, $officename, $group_id, $memberEmailAddress);
  function verifyMember(Operation $op, $branch, $officename, $group_id, $memberEmailAddress);
  function insertGroupAlternateAddress(Operation $op, $branch, $officename, $group_id, $alternateAddress);
  function removeGroupAlternateAddress(Operation $op, $branch, $officename, $group_id, $alternateAddress);
  function verifyGroupAlternateAddress(Operation $op, $branch, $officename, $group_id, $alternateAddress);
  function insertOffice(Operation $op, $branch, $officename, $properties);
  function configureOffice(Operation $op, $branch, $officename, $properties);
  function deleteOffice(Operation $op, $branch, $officename, $properties);
  function verifyOffice(Operation $op, $branch, $officename, $properties);
  function verifyOfficeConfiguration(Operation $op, $branch, $officename, $properties);
  function complete();
}
