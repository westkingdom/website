<?php

namespace Westkingdom\HierarchicalGroupEmail;

class LegacyGroups {

  static function parseLegacyDreamHostGroups($dreamhostGroups, $blacklist = array()) {
    $legacy = array();
    foreach (explode("\n", $dreamhostGroups) as $group) {
      // Replace all runs of spaces with a single space
      $group = trim(preg_replace('/  */', ' ', $group));
      if (!empty($group) && ($group[0] == '!')) {
        $excludedAddress = trim(substr($group, 1));
        if (!empty($excludedAddress)) {
          $blacklist[] = $excludedAddress;
        }
      }
      elseif (!empty($group) && ($group[0] != '#')) {
        list($emailAddress, $members) = explode(' ', $group, 2);
        if (!in_array($emailAddress, $blacklist)) {
          $members = array_unique(array_diff(array_map('trim', explode(',', $members)), $blacklist));
          sort($members);
          if (!empty($members)) {
            $legacy[$emailAddress] = $members;
          }
        }
      }
    }
    return $legacy;
  }

  static function listAllOffices($memberships) {
    $offices = array();

    foreach ($memberships as $group => $info) {
      if (isset($info['lists'])) {
        foreach ($info['lists'] as $office => $officeInfo) {
          $offices[$office] = TRUE;
        }
      }
    }
    return array_keys($offices);
  }

  static function applyLegacyGroups($memberships, $legacy, $policy) {
    $tld = $policy->getDomain();
    $allOffices = LegacyGroups::listAllOffices($memberships);
    foreach ($legacy as $legacyGroup => $members) {
      $matchedExisting = LegacyGroups::applyLegacyGroup($memberships, $legacyGroup, $members);
      if (!$matchedExisting) {
        $legacyOfficename = preg_replace('/@.*/', '', $legacyGroup);
        $legacyDomain = preg_replace('/.*@/', '', $legacyGroup);
        $legacySubdomain = preg_replace('/\.' . $tld . '/', '', $legacyDomain);
        if (($legacySubdomain == $tld) || !in_array($legacyOfficename, $allOffices)) {
          if ($legacySubdomain != $tld) {
            $legacyOfficename = $legacySubdomain . '-' . $legacyOfficename;
          }
          $legacySubdomain = '_legacy';
        }
        $memberships[$legacySubdomain]['lists'][$legacyOfficename] = array(
          'members' => $members,
          'properties' => array(
            'group-name' => ucfirst(strtr($legacyOfficename, '-', ' ')),
            'alternate-addresses' => array($legacyGroup),
          ),
        );
      }
      // If the current domain in the $legacyGroup is a subdomain,
      // then add all of the members of this legacy group to the
      // group "legacy@subdomain.domain.org".
      $branch = $policy->branchFromEmail($legacyGroup);
      if ($policy->isSubdomain($branch) || ($branch == $policy->getProperty('top-level-group'))) {
        if (!isset($branch, $memberships['_legacy']['lists'][$branch])) {
          $memberships['_legacy']['lists'][$branch] = array(
            'members' => array(),
            'properties' => array(
              'group-name' => ucfirst($branch) . ' Legacy Group Members',
              'alternate-addresses' => array("legacy@$branch.$tld"),
            ),
          );
        }
        $memberships['_legacy']['lists'][$branch]['members'] = array_unique(array_merge($memberships['_legacy']['lists'][$branch]['members'], $members));
      }
    }
    return $memberships;
  }

  static protected function applyLegacyGroup(&$memberships, $legacyGroup, $members) {
    foreach ($memberships as $branch => $officesLists) {
      if ($branch[0] != '#') {
        $offices = $officesLists['lists'];
        foreach ($offices as $officename => $officeData) {
          if (LegacyGroups::legacyGroupMatches($legacyGroup, $officename, $officeData)) {
            foreach ($members as $member) {
              $member = trim(strtolower($member));
              if (!in_array($member, $officeData['members'])) {
                $memberships[$branch]['lists'][$officename]['members'][] = $member;
              }
            }
            return TRUE;
          }
        }
      }
    }
    return FALSE;
  }

  static protected function legacyGroupMatches($legacyGroup, $officename, $officeData) {
    if (isset($officeData['properties']['group-email']) && ($legacyGroup == $officeData['properties']['group-email'])) {
      return TRUE;
    }
    return isset($officeData['properties']['alternate-addresses']) && in_array($legacyGroup, $officeData['properties']['alternate-addresses']);
  }
}
